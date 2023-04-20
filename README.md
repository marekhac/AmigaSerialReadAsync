# AmigaSerialReadAsync
Example of async reading of data from serial port on Amiga. 

### What we will do:

* Allocate 32 bytes of memory to store portion of data received on the serial port
* Setup baud-rate for 9600 bps and few other transmission parameters 
* Use termination character (0x00) to find out when to stop receiveing portion of data 
* Send "wake up" string to the device on the other side of serial cable (some devices will not be able to setup connection without it)
* Allow exit the program by pressing CTRL+D on the Amiga keyboard

Thanks to *y and Hexmage960 from PPA.pl for their help and good advices :) 