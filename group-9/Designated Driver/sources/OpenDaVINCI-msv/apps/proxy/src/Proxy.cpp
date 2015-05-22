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

#include "core/data/control/VehicleControl.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"

#include "OpenCVCamera.h"

#include "GeneratedHeaders_Data.h"

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

// Markus Erlach
#define USB "/dev/ttyACM0"

namespace msv { 
    int port;
    void open_port(std::string adr);
    void port_config();
    //void write_int();
    //std::string readString();
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

            // Test ***************************
            // Markus Erlach
            char checkSum[3];
            char fromIntAngle[3];
            char fromIntSpeed[3];
            char padSum[4];
            char padAngle[4];
            char padSpeed[4];
            char sentence[15];               
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
            // Create angle
            int intAngle = (int)steeringAngle;
            if (intAngle < 100) {
                sprintf(fromIntAngle, "%d", intAngle);
                strcpy(padAngle, "0");
                strcat(padAngle, fromIntAngle);    
            } else {
                sprintf(fromIntAngle, "%d", intAngle);
                strcpy(padAngle, fromIntAngle);
            }
            // Create speed
            // Simon Lobo Roca
            int intSpeed = (int)speed;
            if (intSpeed < 10) {
                sprintf(fromIntSpeed, "%d", intSpeed);
                strcpy(padSpeed, "0");
                strcat(padSpeed, fromIntSpeed);    
            } else {
                sprintf(fromIntSpeed, "%d", intSpeed);
                strcpy(padSpeed, fromIntSpeed);
            }
            // Create checksum
            //  Markus Erlach
            int check = intAngle + intSpeed; 
            if (check < 100) {
                sprintf(checkSum, "%d", check);
                strcpy(padSum, "0");
                strcat(padSum, checkSum);      
            } else {
                sprintf(checkSum, "%d", check);
                strcpy(padSum, checkSum);
            }
            //12308115
            strcpy(sentence, padSum);
            strcat(sentence, padSpeed);     
            strcat(sentence, padAngle);   
            
            write(port, sentence, 10);
            std::cout << "Wrote: "<< sentence << std::endl;
            msv::readSerial();
            
            /*************************
            Test this read
            msv::readString();
            *************************/
        }
        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }

// Markus Erlach
void open_port(std::string adr) {
    USB_PORT = adr.c_str();
    /*
    O_RDWR = Posix read write
    O_NOCTTY = Not a controlling terminal 
    O_NDELAY = Ignore dcd signal state
    */
    port = open(USB_PORT, O_RDWR | O_NOCTTY);

    if (port != 0) {
        port_config();
    } 
    
}

// Markus Erlach
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
    // wait for min 1 chars and wait time min
    options.c_cc[VMIN]  = 1;
    options.c_cc[VTIME] = 0;
    // Commit
    tcflush(port, TCIFLUSH);
    tcsetattr(port, TCSANOW, &options);
}

// Markus Erlach
// <16:100:2020202020>
// XXX:IR1IR2IR3US1US2 Example: 100:2020202020
// All sensors should equal XXX (checksum) Like above.
/*
std::string readString() {
    char start[1] = "";
    char length[1] = "";
    char sum[1] = "";
    char next[1] = "";
    char svar[18] = ""; 
    string result = ""; 
    string csum = "";
    string stringlength = "";
    cout << "Serial available: " << port << endl;
    // <17:200:
    read(port, start, 1);
    cout << "Start: " << start << endl;
    if (start[0] != '<') {
        cout << "No start" << endl;
        //readString();
        return svar;
    } else {
        result += start;
        for (int o = 0; o < 3; o++) {
            read(port, length, 1);
            stringlength += length;
        }
        int len = atoi(stringlength.c_str());
        cout << "Len: " << len  << endl;
        for (int p = 0; p < 3; p++) {
            read(port, sum, 1);
            csum += sum;
        }
        int cs = atoi(csum.c_str());
        cout << "CS: " << cs << endl;
        for (int q = 0; q < len; q++) {
            read(port, next, 1);
            result += next;
        }
        cout << "Efter loopen: " << result << " Csum: " << cs << endl;
        memcpy(svar, result.c_str(), result.size() + 1);
       
        //svar[9] = '\0';
        svar[17] = '\0';
        cout << "Svar kopia: " << svar << endl;        

        if (svar[0] == '<' && isdigit(svar[1])) {
            cout << "Svar: " << svar << endl;
            if (svar[16] == '>') {
                //checksum
                // Set to containers
                cout << "Done: " << svar << endl;
                return svar;
            } else {
                cout << "Rerunning, no closing bracket" << endl;
                readString();
                //return svar;
            }
            //return svar;
        } else {
                cout << "Rerunning!" << svar << endl;
                readString();
                //return svar;
            }
        } 
    return svar;
}
*/

// Markus Erlach
std::string readSerial() {
    char start[255] = "";
    
    std::cout << "Serial available: " << port << std::endl;
    
    int n = read(port, start, 255);
    start[n] = 0;
    std::cout << "N: " << n << std::endl;
    std::string received(start);
    return received;
}

} // msv

