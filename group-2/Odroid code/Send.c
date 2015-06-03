#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <sys/shm.h>
#include <ctype.h>

#include "Send.h"

#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

char* serialport = "/dev/ttyACM0";
int baudrate = 115200;  // default
int oldVal;

int main(int argc, char *argv[])    {
  int fd;
  fd = serialport_init(serialport,baudrate);
  char write[256];
  int loop = 0;
  while(1){
    readMemory(fd);
    usleep(1);
  }
  return 0;
}
void readMemory(int fd){
  key_t key;
  int shmid;
  char *data, *ptr;
  int mode;
  char* shared_memory[6];
  int *p;
  /* make the key: */
  if ((key = 456456) == -1) {
      perror("ftok");
      exit(1);
  }
  /* connect to (and possibly create) the segment: */
  if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
      perror("shmget");
      exit(1);
  }
  /* attach to the segment to get a pointer to it: */
  data = shmat(shmid, (void *)0, 0);
  if (data == (char *)(-1)) {
      perror("shmat");
      exit(1);
  }

    p = (int *)data;
    ptr = data + 12;
    shared_memory[0] = ptr;
    ptr += *p++;
    shared_memory[1] = ptr;
    ptr += *p;
    shared_memory[2] = ptr;

    /*
    printf("shared_memory[0] %s\n",shared_memory[0]);
    printf("shared_memory[1] %s\n",shared_memory[1]);
    printf("shared_memory[2] %s\n",shared_memory[2]);
    */
    //printf("asdqwe1!\n");
    if(atoi(shared_memory[2]) != oldVal){
      char* toSend = (char*)malloc(4);
      sprintf(toSend,"!%s%s0",shared_memory[0],shared_memory[1]);
      printf("toSend %s\n",toSend);
      serialport_write(fd,toSend);
      oldVal = atoi(shared_memory[2]);

    }

  /* detach from the segment: */
  if (shmdt(data) == -1) {
      perror("shmdt");
      exit(1);
  }
}

int serialport_write(int fd, const char* str)
{
    //fd = serialport_init(serialport,baudrate);
    usleep(10);
    int len = strlen(str);
    int n = write(fd, str, len);
    if( n!=len )
        return -1;
    return 0;
}

int serialport_init(char* serialport, int baud){

    printf("serialport.. %s\n",serialport);
    printf("baud.. %i\n",baud);

    struct termios toptions;
    int fd;

    //fprintf(stderr,"init_serialport: opening port %s @ %d bps\n",
    //        serialport,baud);

    fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)  {
        printf("init_serialport: Unable to open port ");
        return -1;
    }

    if (tcgetattr(fd, &toptions) < 0) {
        printf("init_serialport: Couldn't get term attributes");
        return -1;
    }
    speed_t brate = baud; // let you override switch below if needed
    switch(baud) {
    case 4800:   brate=B4800;   break;
    case 9600:   brate=B9600;   break;
#ifdef B14400
    case 14400:  brate=B14400;  break;
#endif
    case 19200:  brate=B19200;  break;
#ifdef B28800
    case 28800:  brate=B28800;  break;
#endif
    case 38400:  brate=B38400;  break;
    case 57600:  brate=B57600;  break;
    case 115200: brate=B115200; break;
    }
    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 20;

    if( tcsetattr(fd, TCSANOW, &toptions) < 0) {
        printf("init_serialport: Couldn't set term attributes");
        return -1;
    }

    return fd;
}

