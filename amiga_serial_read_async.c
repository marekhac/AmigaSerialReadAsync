/*
 * Amiga_Serial_Read_Async.c
 *
 * Example of async reading of data from serial port on Amiga.
 * Read data in the loop, until CTRL-D is pressed
 *
 * based on complex tricky example of serial.device usage
 *
 * Compile with GCC
 * Run from CLI only
 */

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/io.h>
#include <exec/devices.h>
#include <exec/libraries.h>
#include <dos/dos.h>
#include <devices/serial.h>
#include <stdio.h>

// serial port params

#define READ_BUFFER_SIZE 32

struct IOExtSer *SerialIO;   /* pointer to I/O request */
struct MsgPort  *SerialMP;   /* pointer to Message Port */

struct IOTArray Terminators =
{
	0x00  /* EOF character */
};

UBYTE serialReadBuffer[READ_BUFFER_SIZE]; /* reserve 32 bytes storage */

void sendWriteCommand()
{
    SerialIO->IOSer.io_Command  = CMD_WRITE;
    SerialIO->IOSer.io_Length = -1;
    SerialIO->IOSer.io_Data = (APTR)"WakeUp";
    if (DoIO(SerialIO))
    {
        printf("Error while sending CMD_WRITE command\n");
    }
}

void setupReadCommand()
{
    SerialIO->IOSer.io_Command = CMD_READ;
    SerialIO->IOSer.io_Length = READ_BUFFER_SIZE;
    SerialIO->IOSer.io_Data = &serialReadBuffer;
}

void setupCustomSerialParams()
{
    // update I/O request

    SerialIO->io_RBufLen = 512;
    SerialIO->io_Baud = 9600;
    SerialIO->io_ReadLen = 8;
    SerialIO->io_WriteLen = 8;
    SerialIO->io_StopBits = 1;
    SerialIO->io_SerFlags = SERF_EOFMODE;
    SerialIO->io_TermArray = Terminators;

    // update serial parameters using SDCMD_SETPARAMS command

    SerialIO->IOSer.io_Command = SDCMD_SETPARAMS;

    if (DoIO(SerialIO))
    {
        printf("Error while setting serial parameters!\n");
    }
}

int main(void)
{
    ULONG signal;

    /* create message port */

    if (SerialMP = (struct MsgPort *) CreatePort(0,0) )
    {
     /* create I/O request */

        if (SerialIO=(struct IOExtSer *)
            CreateExtIO(SerialMP,sizeof(struct IOExtSer)) )
        {
            if (OpenDevice(SERIALNAME,0L,SerialIO,0) )
                printf("%s did not open\n",SERIALNAME);
            else
            {
                /* device is open */

                setupCustomSerialParams();
                sendWriteCommand();
                setupReadCommand();

                /* Initiate I/O command and not wait for it to complete */

                SendIO(SerialIO);

                printf("Sleeping until CTRL-D or serial input\n");

                do
                {
                    signal = Wait(1L << SerialMP -> mp_SigBit | SIGBREAKF_CTRL_D);

                    if (CheckIO(SerialIO) ) /* If request is complete... */
                    {
                        /* wait for specific I/O request */

                        WaitIO(SerialIO);

                        printf("%ld bytes received. ",SerialIO->IOSer.io_Actual);
                        printf("Data: %s\n", serialReadBuffer);

                        SendIO(SerialIO); /* restart I/O request */
                    }
                }
                while (!(signal & SIGBREAKF_CTRL_D));

                AbortIO(SerialIO);  /* ask device to abort request, if pending */
                WaitIO(SerialIO);   /* wait for abort, then clean up */

                CloseDevice(SerialIO); /* close serial device */
                DeletePort(SerialMP);
            }

            DeleteExtIO(SerialIO); /* delete I/O request */
        }
        else
        {   printf("Unable to create IORequest\n");
            DeletePort(SerialMP); /* delete message port */
        }
    }
    else
    {
        printf("Unable to create message port\n");
    }

    return 0;
}
