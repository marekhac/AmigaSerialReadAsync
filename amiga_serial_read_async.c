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

// size of serial read buffer

#define READ_BUFFER_SIZE 32

// serial port params

#define INPUT_BUFFER_SIZE 512
#define READ_LENGTH 8
#define WRITE_LENGTH 8
#define STOP_BITS 1
#define BAUD_RATE 9600 //  number of bits transferred per second

struct IOExtSer *SerialIO;   /* pointer to I/O request */
struct MsgPort  *SerialMP;   /* pointer to Message Port*/

int main(void)
{
ULONG signal;
BYTE serialReadBuffer[READ_BUFFER_SIZE]; /* reserve 32 bytes storage */

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

            SerialIO->IOSer.io_Command  = CMD_READ;
            SerialIO->IOSer.io_Length   = -1;
            SerialIO->IOSer.io_Data     = &serialReadBuffer;

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
            }

        DeleteExtIO(SerialIO); /* delete I/O request */
        }
    else
        printf("Unable to create IORequest\n");

    DeletePort(SerialMP); /* delete message port */
    }
else
    printf("Unable to create message port\n");

return 0;
}

void setupCustomSerialParams()
{
    // update I/O request

    SerialIO->io_RBufLen = INPUT_BUFFER_SIZE;
    SerialIO->io_Baud = BAUD_RATE;
    SerialIO->io_ReadLen = READ_LENGTH;
    SerialIO->io_WriteLen = WRITE_LENGTH;
    SerialIO->io_StopBits = STOP_BITS;
    SerialIO->io_SerFlags &= ~SERF_PARTY_ON; // set parity off
    SerialIO->io_SerFlags |= SERF_XDISABLED; // set xON/xOFF disabled

    // update serial parameters using SDCMD_SETPARAMS command

    SerialIO->IOSer.io_Command = SDCMD_SETPARAMS;

    if (DoIO(SerialIO))
    {
        printf("Error setting serial parameters!\n");
    }
}
