int serialport_read_until(int fd, char ** buf, char until);
int serialport_init(char* serialport, int baud);
void writeToMem(char * sendData);
void allocateMemory();
char * cleanConnectionData(char * clean);

