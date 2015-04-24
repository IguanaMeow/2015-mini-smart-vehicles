#include <iostream>
using namespace std;
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */


#define USB_PORT "/dev/tty.ttyACM0"


int open_port() {
    int fd; //Descriptor for the port
    fd = open(USB_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        cout << "Unable to open port. \n";
    }
    else {
        cout << "Port opened.\n";
    }
    cout << "Descriptor in open:";
    cout << fd;
    cout << "\n";
    return fd;
}

int configure_port (int fd) {
    struct termios options;

    tcgetattr(fd, &options);
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);

    cout << "Port configured.\n";

    return (fd);
}

void read_data(int fd) {


    cout << "Reading data from: ";
    cout << fd;
    cout << "\n";

    char buffer;
    buffer = fcntl(fd, F_SETFL, FNDELAY);
    cout << "Buffer: ";
    cout << buffer;
    cout << "\n";


}

int main (int argc, char * const argv[]) {

    int fd; //Descriptor for the port

    fd = open_port();
    configure_port(fd);
    cout << "Descriptor: ";
    cout << fd;
    cout<< "\n";
    read_data(fd);
    cout << "Data read.";

    return 0;
}
