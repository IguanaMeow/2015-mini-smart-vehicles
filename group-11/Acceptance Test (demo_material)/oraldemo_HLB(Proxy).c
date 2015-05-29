#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DEBUG 1
  
int main(int argc, char *argv[])
{
  int fd, c;

  struct termios toptions;

  /* open serial port for arduino UNO*/
  fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
  printf("fd opened as %i\n", fd);
  
  /* wait for the Arduino to reboot */
  usleep(3500000);
  
  /* get current serial port settings */
  tcgetattr(fd, &toptions);
  /* set 9600 baud both ways */
  cfsetispeed(&toptions, B9600);
  cfsetospeed(&toptions, B9600);
  /* 8 bits, no parity, no stop bits */
  toptions.c_cflag &= ~PARENB;
  toptions.c_cflag &= ~CSTOPB;
  toptions.c_cflag &= ~CSIZE;
  toptions.c_cflag |= CS8;
  /* Canonical mode */
  toptions.c_lflag |= ICANON;
  /* commit the serial port settings */
  tcsetattr(fd, TCSANOW, &toptions);


  /* Program used for Demonstrational Purposes */
  printf("Type 1, 2, 3, 4, 5 and 6 for controlling ESC & SERVO, or 0 to exit \n\n");
  while((c = getchar()) != '0'){
    // Keyboard integer input: 
  	if (c == '1'){
   	     // a = 97 in ASCII (dec), .ino file (LLB) executes a certain 
    	// command depending on which ASCII value it gets:
		write(fd, "a", 1);	
	}

	else if (c == '2'){
		write(fd, "b", 1);
	}

	else if (c == '3'){
		write(fd, "c", 1);
	}

	else if (c == '4'){
		write(fd, "d", 1);
	}

	else if (c == '5'){
		write(fd, "e", 1);
	}

	else if (c == '6'){
		write(fd, "f", 1);
	}


	else{

		printf("Type 1, 2, 3, 4, 5 and 6 for controlling ESC & SERVO, or 0 to exit\n\n");
	}

	//Flush the buffer:
	fflush(stdin);
	fflush(stdout);
}
 printf("/nSystem exits...");
  return 0;
}

