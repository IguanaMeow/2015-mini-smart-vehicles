// Authors:
//   Giorgi Dundua
//   Saimonas Sileikis

#include <string>
#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <unistd.h>

#include <pb_encode.h>
#include <pb_decode.h>

#include "serial/serial.h"
//#include "simple.pb.h"
#include "message.pb.h"

using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

void my_sleep(unsigned long milliseconds) {
    usleep(milliseconds*1000);
}

int run(int argc, char **argv)
{
  string port("/dev/ttyACM0");
  unsigned long baud = 9600;


  // port, baudrate, timeout in milliseconds
  serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));

  cout << "Is the serial port open?";
  if(my_serial.isOpen())
    cout << " Yes." << endl;
  else
    cout << " No." << endl;

  // Get the Test string

  string test_string;
  if (argc == 2) {
    test_string = argv[1];
  } else {
    test_string = "Testing.";
  }


uint8_t bufferIn[64];
ArduinoToOdroid msg;
int numOfErrors = 0;
  while (true) {
    string result = my_serial.read(27);

    for(int i = 0; i < result.size(); i++){
        bufferIn[i] = result[i];
    }
    pb_istream_t streamIn = pb_istream_from_buffer(bufferIn, 27);
    bool status = pb_decode(&streamIn, ArduinoToOdroid_fields, &msg);

    if(!status){
        numOfErrors++;
        cout << "Error number: " << numOfErrors << endl;
    }
    else{
        cout << "Ultrasound centre: " << msg.usCentre << endl;
        cout << "Ultrasound right: " << msg.usRight << endl;
        cout << "Infrared right 1: " << msg.irRight_one << endl;
        cout << "Infrared right 2: " << msg.irRight_two << endl;
        cout << "Infrared rear: " << msg.irRear << endl;
        cout << "Wheel ticks: " << msg.wheelTicks << endl;
        cout << endl << endl;
    }

    //cout << result << endl;

    if(numOfErrors > 200){
        printf("Decoding failed: %s\n", PB_GET_ERROR(&streamIn));
        return 1;
    }
  }

  return 0;
}

int main(int argc, char **argv) {

    // uint8_t buffer[128];
    // size_t message_length;
    // bool status;
    
 
    // {     
    //     SimpleMessage message;
    //     pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    //     message.lucky_number = 13;
    //     status = pb_encode(&stream, SimpleMessage_fields, &message);
    //     message_length = stream.bytes_written;

    //     if (!status)
    //     {
    //         printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    //         return 1;
    //     }
    // }
    
    // {

    //     SimpleMessage message;
    //     pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);
    //     status = pb_decode(&stream, SimpleMessage_fields, &message);
        
    //     if (!status)
    //     {
    //         printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    //         return 1;
    //     }
    //     printf("Your lucky number was %d!\n", message.lucky_number);
    // }
    

  try {
    return run(argc, argv);
  } catch (exception &e) {
    cerr << "Unhandled Exception: " << e.what() << endl;
  }
}
