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

#include "Connection.h"
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */

char* serialport = "/dev/ttyACM1";
int baudrate = 115200;  // default
key_t key;
int shmid;
char * data;
int mode;

int main(int argc, char *argv[])    {
  allocateMemory();
  int fd = serialport_init(serialport,baudrate);
  while(1){
      char * buf;
      buf = (char*)malloc(30);
      int n = serialport_read_until(fd, &buf, '\n');
      //printf("buf0 %s", buf);
      if(buf){
        char * sendData = cleanConnectionData(buf);
        //The cleanConnectionData method will return NULL if it has any errors
        //Then we just disregard the string and add nothing
        //printf("clean buf %s\n", sendData);
        if(sendData){
	  printf("sending %s\n", sendData);
          writeToMem(sendData);
        };
      }
  }
  return 0;
}

char * cleanConnectionData(char * clean){
  #pragma clang diagnostic ignored "-Wunused-value"
  //printf("reading %s",clean);
  char * sendData = (char *)malloc(128);
  //printf("starting; %c\n",*clean);
  if(*clean == '!'){
    *clean ++;
    char * temp = (char*)malloc(3);
    while (*clean) {
      //printf("checking; %c\n",*clean++);
      if(*clean == ',' || *clean == '\n'){
        if(strlen(temp) == 3){
          strcpy(sendData+strlen(sendData),temp);
        }
        else if(strlen(temp) == 2){
          //to make sure the string is allways the same length
          sprintf(temp,"0%d",atoi(temp));
          strcpy(sendData+strlen(sendData),temp);
        }
        else if(strlen(temp) == 1){
          //to make sure the string is allways the same length
          sprintf(temp,"00%d",atoi(temp));
          strcpy(sendData+strlen(sendData),temp);
        }
        else{
          //If the char length is of an unexpected length we send back an empty response
          return NULL;
        }
        free((char *)temp);
        temp = (char *)malloc(3);
         *clean ++;
         *temp = '\0';
       }
       //Make sure we only send digits in to the temp char
       else if(isdigit(*clean)){

        sprintf(temp,"%s%c",temp,clean[0])+1;
        *clean ++;
        }
      else{
        *clean ++;
      }
    }
    if(strlen(sendData) == 15){
      return sendData;
    }
    else{
      return NULL;
    }
  }
  return NULL;
}
void writeToMem(char * sendData){
//<<<<<<< Updated upstream
//  printf("writeToMem %s\n",sendData);
//=======
  //printf("writeToMem %s\n",sendData);
//>>>>>>> Stashed changes

  /* attach to the segment to get a pointer to it: */
  data = (char*) shmat(shmid, (void *)0, 0);
  if (data == (char *)(-1)) {
      perror("shmat");
      exit(1);
  }
  char* inData;
  int next[6];
  inData = data + sizeof (next);
  next[0] = sprintf(inData,"%c%c%c",sendData[0],sendData[1],sendData[2])+1;
  inData += next[0];
  next[1] = sprintf(inData,"%c%c%c",sendData[3],sendData[4],sendData[5])+1;
  inData += next[1];
  next[2] = sprintf(inData,"%c%c%c",sendData[6],sendData[7],sendData[8])+1;
  inData += next[2];
  next[3] = sprintf(inData,"%c%c%c",sendData[9],sendData[10],sendData[11])+1;
  inData += next[3];
  next[4] = sprintf(inData,"%c%c%c",sendData[12],sendData[13],sendData[14])+1;
  inData += next[4];
  /*
  next[5] = sprintf(inData,"%c%c%c",sendData[15],sendData[16],sendData[17])+1;
  inData += next[5];
  */
  /* read or modify the segment, based on the command line: */
  memcpy(data, sendData, sizeof (sendData)*4);
  printf("inData %d \n", &next);


  /* detach from the segment: */
  if (shmdt(data) == -1) {
      perror("shmdt");
      exit(1);
  }
};

void allocateMemory(){
  /* make the key: */
  if ((key = 123123) == -1) {
      perror("ftok");
  }

  /* connect to (and possibly create) the segment: */
  if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
      perror("shmget");
  }
}

int serialport_read_until(int fd, char** buf, char until){
    usleep( 10 * 1000 );
    //printf("read_until starting\n");
	char b[1];
    char * temp;
    temp = (char*)malloc(30);
    int i=0;
    do {
        int n = read(fd, b, 1);  // read a char at a time
        if( n==-1) return -1;    // couldn't read
        if( n==0 ) {
            usleep( 10 * 1000 ); // wait 10 msec try again
            continue;
        }
        printf("Banan %c\n", b[0]);
        temp[i] = b[0]; i++;
    } while( b[0] != until );

    temp[i] = 0;  // null terminate the string
    *buf = temp;
//    printf("BUF INSIDE %s\n", &buf);
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

