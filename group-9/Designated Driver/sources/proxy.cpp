/**
 * proxy - Sample application to encapsulate HW/SW interfacing with embedded systems.
 * Copyright (C) 2012 - 2015 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ctype.h>
#include <cstring>
#include <cmath>

//#include "core/data/control/VehicleControl.h"
//#include "core/base/KeyValueConfiguration.h"
//#include "core/data/Container.h"
//#include "core/data/TimeStamp.h"

//#include "OpenCVCamera.h"

//#include "GeneratedHeaders_Data.h"

#include "Proxy.h"
#include <fstream>
#include <iostream> 
#include <ctime>
#include <unistd.h>
#include <fcntl.h> 
#include <termios.h> 
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h> 

#define USB "/dev/ttyACM1"

namespace msv { 
    double getCheckSum(char *data);
    int port;
    std::clock_t startTimer;

    void open_port(std::string adr);
    void port_config();
    void write_int();
    std::string readString();
    //void readSerial(int q);
    std::string readSerial(); 

    const char *USB_PORT;

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace tools::recorder;
    using namespace core::data::control;
    
    Proxy::Proxy(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "proxy"),
        m_recorder(NULL),
        m_camera(NULL)
    {}

    Proxy::~Proxy() {
    }

    void Proxy::setUp() {

        
        startTimer = std::clock();
        msv::open_port(USB); // Connect to arduino
        msv::port_config();
        // This method will be called automatically _before_ running body().
        if (getFrequency() < 20) {
            cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
        }

        // Get configuration data.
        KeyValueConfiguration kv = getKeyValueConfiguration();

        // Create built-in recorder.
        /*
        const bool useRecorder = kv.getValue<uint32_t>("proxy.useRecorder") == 1;
        if (useRecorder) {
            // URL for storing containers.
            stringstream recordingURL;
            recordingURL << "file://" << "proxy_" << TimeStamp().getYYYYMMDD_HHMMSS() << ".rec";
            // Size of memory segments.
            const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
            // Number of memory segments.
            const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
            // Run recorder in asynchronous mode to allow real-time recording in background.
            const bool THREADING = true;

            m_recorder = new Recorder(recordingURL.str(), MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING);
        }
*/
        // Create the camera grabber.
        const string NAME = getKeyValueConfiguration().getValue<string>("proxy.camera.name");
        string TYPE = getKeyValueConfiguration().getValue<string>("proxy.camera.type");
        std::transform(TYPE.begin(), TYPE.end(), TYPE.begin(), ::tolower);
        const uint32_t ID = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.id");
        const uint32_t WIDTH = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.width");
        const uint32_t HEIGHT = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.height");
        const uint32_t BPP = getKeyValueConfiguration().getValue<uint32_t>("proxy.camera.bpp");

        if (TYPE.compare("opencv") == 0) {
            m_camera = new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP);
        }

        if (m_camera == NULL) {
            cerr << "No valid camera type defined." << endl;
        }
    }

    void Proxy::tearDown() {
        // This method will be call automatically _after_ return from body().
        OPENDAVINCI_CORE_DELETE_POINTER(m_recorder);
        OPENDAVINCI_CORE_DELETE_POINTER(m_camera);
    }

    void Proxy::distribute(Container c) {
        // Store data to recorder.
        if (m_recorder != NULL) {
            // Time stamp data before storing.
            c.setReceivedTimeStamp(TimeStamp());
            m_recorder->store(c);
        }

        // Share data.
        getConference().send(c);
        startTimer = 0;
    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() {
        uint32_t captureCounter = 0;
        while (getModuleState() == ModuleState::RUNNING) {
            // Capture frame.
            if (m_camera != NULL) {
                core::data::image::SharedImage si = m_camera->capture();

                Container c(Container::SHARED_IMAGE, si);
                distribute(c);
                captureCounter++;
            }
            Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
                VehicleControl vc = containerVehicleControl.getData<VehicleControl>();
                cerr << "Speed: '" << vc.getSpeed() << "'" << endl;
                cerr << "Angle: '" << vc.getSteeringWheelAngle() << "'" << endl;
               
            // TODO: Here, you need to implement the data links to the embedded system
            // to read data from IR/US.

distribute()
sbd.putTo_MapOfDistances(0,1) // id of sensor,value

            // Test ***************************
            char fromInt[3];
            //char checkSum[3];
            char paddSpeed[4];
            char padded[4];
            char fromIntSpeed[4];
            char sentence[10];               
            double speed = vc.getSpeed()*10;
            double steeringAngle = vc.getSteeringWheelAngle();
            steeringAngle = steeringAngle * 57.3;
            std::cout << "After calc: " << steeringAngle << std::endl;
            
            if (steeringAngle < -1) {
                steeringAngle = 90 - (steeringAngle * -1);
            } else if (steeringAngle > 1) {
                steeringAngle = 90 + steeringAngle;
            } else {
                steeringAngle = 90;
            }
            std::cout << "Int angle: " << steeringAngle << std::endl;
            if (steeringAngle > 115) {
                steeringAngle = 115;
            }
            if (steeringAngle < 65) {
                steeringAngle = 65;
            }
            
            int intAngle = (int)steeringAngle;
            if (intAngle < 100) {
                sprintf(fromInt, "%d", intAngle);
                strcpy(padded, "0");
                strcat(padded, fromInt);    
                    
            } else {
                sprintf(fromInt, "%d", intAngle);
                strcpy(padded, fromInt);
            }

            int intSpeed = (int)speed;
            if (intAngle < 100) {
                sprintf(fromIntSpeed, "%d", intSpeed);
                strcpy(paddSpeed, "0");
                strcat(paddSpeed, fromIntSpeed);    
                    
            } else {
                sprintf(fromIntSpeed, "%d", intSpeed);
                strcpy(paddSpeed, fromIntSpeed);
            }
            /*
            int check = (intAngle + 8) / 2; // + speed;
            if (check < 10) {
                sprintf(checkSum, "%d", check);
                strcpy(padSum, "0");
                strcat(padSum, checkSum);      
            } else {
                sprintf(checkSum, "%d", check);
                strcpy(padded, checkSum);
            }
            */
            //0408115
            //sprintf(checkSum, "%d", check);
           // snprintf(fromDouble, sizeof(fromDouble), "%g", speed);
            strcpy(sentence, "04");
            strcat(sentence, paddSpeed);     // fromDouble
            strcat(sentence, padded);   
            //strcat(sentence, "}");
            
            //char random[3];
            //strcpy(random, "<>");
            //write(port, random, 3);
            write(port, sentence, 10);
            std::cout << "Wrote: "<< sentence << std::endl;
            msv::readSerial();
            
            /*************************
            Test this read
            msv::readSerial(port);
            Instead of readString
            *************************/
            // Peili Ge
            Container c = Container(Container::USER_DATA_0, sensorBoardData);
            distribute(c);
 
            /*int IR1Data = sensorBoardData.getValueForKey_MapOfDistances(0);
            cout << "SBD IR1: " << IR1Data << endl;
            */
            //flushes the input queue, which contains data that have been received but not yet read.
            tcflush(port, TCIFLUSH); 
            
            std::cout << "Time: " << (std::clock() - startTimer) / (double)(CLOCKS_PER_SEC / 1000) << "ms"  << std::endl;
        }
        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }

void open_port(std::string adr) {
    USB_PORT = adr.c_str();
    /*
    O_RDWR = Posix read write
    O_NOCTTY = Not a controlling terminal 
    O_NDELAY = Ignore dcd signal state
    */
    port = open(USB_PORT, O_RDWR | O_NOCTTY);
    // Check open
    
    /*
    if (port <= 0) {
        std::cerr << "Unable to open port: " << port << std::endl;
        return false;
    } else {
        fcntl (port, F_SETFL, 0);       
    }
    return (port);
    
    */
    if (port != 0) {
        port_config();
    } 
    
}

void port_config() {
    // Setup control structure
    struct termios options;
    struct termios oldOptions;
    memset (&options, 0, sizeof(options));
    
    // Get currently set options
    // Error handling
    if (tcgetattr(port, &options) != 0) {
        std::cout << "Error: " << errno << "from tcgetattr" << strerror(errno) << std::endl;
    }
    // Save old options
    oldOptions = options;
    // Set read and write speed to 9600, 19200 or 384000 baud
    cfsetispeed(&options, B38400);
    cfsetospeed(&options, B38400);
    // 8bits, no parity, no stop bits
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    /*
    No hw flow control and ignore status lines
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= CREAD | CLOCAL
    */
    // turn off s/w flow ctrl and make raw
    options.c_iflag &= ~(IXON | IXOFF | IXANY); 
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    options.c_oflag &= ~OPOST; 
    // wait for min 0 chars and wait time min
    options.c_cc[VMIN]  = 1;
    options.c_cc[VTIME] = 0;
    // Commit
    tcflush(port, TCIFLUSH);
    tcsetattr(port, TCSANOW, &options);
}

// XXX::IR1IR2IR3US1US2 Example: 100:2020202020
// All sensors should equal XXX (checksum) Like above.
std::string readString() {
    char start[1] = "";
    char next[1] = "";
    char svar[10] = ""; 
    
    std::string result = ""; 
    std::cout << "Serial available: " << port << std::endl;
    
        //for (int x = 0; x < 6; x++) {
    read(port, start, 1);
    std::cout << "Start: " << start << std::endl;
    if (start[0] != '1') {
        std::cout << "No start" << std::endl;
        //readString(z);
        return svar;
    } else {
        result += start;
        //delete[] start;
        std::cout << "Result start: " << result << std::endl;
        for (int x = 0; x < 8; x++) {
            read(port, next, 1);
            result += next;
            //std::cout << "Result: " << result <<std::endl;
        }
        std::cout << "Efter loopen: " << result << std::endl;
        //svar = new char[result.size() + 1];
        memcpy(svar, result.c_str(), result.size() + 1);
       
        svar[9] = '\0';
        //svar[sizeof(svar) - 1] = '\0';
        std::cout << "Svar kopia: " << svar << std::endl;        

        if (svar[0] == '{' && isdigit(svar[1]))
         {
            std::cout << "Svar: " << svar << std::endl;
            if (svar[8] == '}') {
                // Set to containers
                std::cout << "Done: " << svar << std::endl;
                return svar;
            } else {
                
                std::cout << "Rerunning, no closing bracket" << std::endl;
                //readString(z);
                return svar;
            }
            //return svar;
        } else {
                std::cout << "Rerunning!" << svar << std::endl;
                //readString(z);
                return svar;
            }
        } 
    return svar;
}

//Peili Ge
std::string readSerial() {
    char start[1] = "";
    char next[1] = "";
    char end[16] = "";
    char length[2] = "";

        string s = ""; // example string s = "<16:100:2020202020>";
       read (port, start,1);
    if(start[0] != '<' ){
        std::cout << "Wrong data received " << std::endl;
        return 0;
       } else {
        std::cout << "recived data: " << std::endl;
     for(int i=0; i<2; i++){   
        read(port, length, 2);
    }
        s += start;
     for(int j =0; j< 16; j++ ){
       read(port, next, 1);
       s += next;
 }
       size_t findT = s.find(s);
       findT =s.find( ':');
     if (findT == string::npos){
        std::cout << "Wrong data received, with no ':' " << std::endl;
        return 0;
} 
     if (svar[0] == '<' && isdigit(end[1])) {
            if (end[16] == '>') {
                std::cout << "Done: " << end << std::endl;
                return end;
            } else {            
                std::cout << "Wrong data received, no ending" << std::endl;
                return end;
            }
         {
        char chars[] = "()<>"; //remove whatever <>
        for (unsigned int i = 0; i < strlen(chars); ++i) {
        s.erase (std::remove(s.begin(), s.end(), chars[i]), s.end());
        } //"16:100:2020202020"
        for (int z = 3; z < 16; z++){
            read(port, next, 3);
            s += next;
        } // "100:2020202020"
     
    int i = 0; //test the last step if it has any letters
    char tab2[1024];
    strncpy(tab2, s.c_str(), sizeof(tab2));
    //tab2[sizeof(tab2) - 1] = 0;
    while (tab2[i]){
    if ((tab2[i] <= 'z' && tab2[i] >= 'a') || (tab2[i] <= 'Z' && tab2[i] >= 'A')){
       std::cout << "Wrong data recived, it shouldn't be letters" << std::endl;
        return 0;
     }

    const char *inData;    
    inData = s.c_str();
     cout << "check sum data: " << inData << endl;
   // char inData[15];
    char sens1[2],sens2[2],sens3[2],sens4[2], sens5[2];
    double s1,s2,s3,s4,s5, totalSens;
    char sum[3];
    int i = 0;
    while (i < 3) {
        sum[i] = inData[i];
        i++;
    }
    double result = atoi(sum);

    double checkSum = result;
    if (checkSum > 99){
        sens1[0] = inData[5];
        sens1[1] = inData[6];
        sens2[0] = inData[7];
        sens2[1] = inData[8];
        sens3[0] = inData[9];
        sens3[1] = inData[10];
        sens4[0] = inData[11];
        sens4[1] = inData[12];
        sens5[0] = inData[13];
        sens5[1] = inData[14];
    } else {
        sens1[0] = inData[4];
        sens1[1] = inData[5];
        sens2[0] = inData[6];
        sens2[1] = inData[7];
        sens3[0] = inData[8];
        sens3[1] = inData[9];
        sens4[0] = inData[10];
        sens4[1] = inData[11];
        sens5[0] = inData[12];
        sens5[1] = inData[13];
    }
    s1 = atoi(sens1);
    s2 = atoi(sens2);
    s3 = atoi(sens3);
    s4 = atoi(sens4);
    s5 = atoi(sens5);
    totalSens = s1;
    totalSens = totalSens + s2;
    totalSens = totalSens + s3;
    totalSens = totalSens + s4;
    totalSens = totalSens + s5;
    if (totalSens == checkSum) {
        cout << "good match, total: " << totalSens << endl;
        cout << "check:" << checkSum << endl;

     Container containerSensorBoardData = getKeyValueDataStore().get(Container::SENSORBOARD);
     SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
        sbd.putTo_MapOfDistances(0, sense1);
        sbd.putTo_MapOfDistances(1, sense2);
        sbd.putTo_MapOfDistances(2, sense3);
        sbd.putTo_MapOfDistances(3, sense4);
        sbd.putTo_MapOfDistances(4, sense5);
        getConference().send(containerSensorBoardData);
    }
    return x;
}
    }
    else{
         cout << "bad match, total: " << totalSens << endl;
         cout << "check:" << checkSum << endl;
    }


 // msv
}
}
}

