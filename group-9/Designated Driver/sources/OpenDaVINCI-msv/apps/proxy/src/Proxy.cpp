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
#include <unistd.h>
#include <fcntl.h> 
#include <termios.h> 
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define USB "/dev/ttyACM0"

namespace msv {
    int port;

    int open_port(std::string adr);
    void port_config(int x);
    void write_int(int y);
    std::string readString(int z);
    void readSerial(int q); 

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
        port = msv::open_port(USB); // Connect to arduino
        msv::port_config(port);
        // This method will be called automatically _before_ running body().
        if (getFrequency() < 20) {
            cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
        }

        // Get configuration data.
        KeyValueConfiguration kv = getKeyValueConfiguration();

        // Create built-in recorder.
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
		    //int port = open_port();
    		//int portOptions = port_config(port);
    		//printf("Port: %d\n", portOptions);
    		//write_int(port);
    		//std::string read = readString(port);
   		    //cout << "received string: " << read << endl;
		//so far i dont know the string format, so I cant move on for decoding the string

            // Test ***************************
            char fromInt[2];
            char fromInt2[2];
            char fromDouble[4];
            char sentence[11];               
            double speed = 0.8;
            double steeringAngle = vc.getSteeringWheelAngle();
            steeringAngle = steeringAngle * 57.2957795;
            std::cout << steeringAngle << std::endl;
            if (steeringAngle < 0) {
               steeringAngle = (steeringAngle * (-1));
            } else if (steeringAngle >= 0) {
                steeringAngle = steeringAngle + 25;
            }
            int newAngle = (double)steeringAngle + 1;
            std::cout << "Angle: " << newAngle << std::endl;
            
            if (newAngle < 10) {
                sprintf(fromInt2, "%d", newAngle);
                strcpy(fromInt, "0");
                strcat(fromInt, fromInt2);    
                snprintf(fromDouble, sizeof(fromDouble), "%g", speed);    
            } else {
                sprintf(fromInt, "%d", newAngle);
                snprintf(fromDouble, sizeof(fromDouble), "%g", speed);
            }
            // "{4:0.8,15}"
            strcpy(sentence, "{4:");
            strcat(sentence, fromDouble);
            strcat(sentence, ",");
            strcat(sentence, fromInt);
            strcat(sentence, "}");
            
            write(port, sentence, 13);
            std::cout << "Wrote: "<< sentence << std::endl;
         //  msv::readString(port);
            /*************************
            Test this read
                        Instead of readString
            *************************/
          // msv::readSerial(port);

        }

        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }

int open_port(std::string adr) {
    USB_PORT = adr.c_str();
    /*
    O_RDWR = Posix read write
    O_NOCTTY = Not a controlling terminal 
    O_NDELAY = Ignore dcd signal state
    */
    port = open(USB_PORT, O_RDWR | O_NOCTTY);
    // Check open
    
    if (port < 0) {
        std::cerr << "Unable to open port: " << port << std::endl;
        return (port);
    } else {
        fcntl (port, F_SETFL, 0);       
    }
    return (port);
    
    /*
    if (port != 0) {
        port_config(port);
    }
    */
}

void port_config(int x) {
    // Setup control structure
    struct termios options;
    struct termios oldOptions;
    memset (&options, 0, sizeof(options));
    
    // Get currently set options
    // Error handling
    if (tcgetattr(x, &options) != 0) {
        std::cout << "Error: " << errno << "from tcgetattr" << strerror(errno) << std::endl;
    }
    // Save old options
    oldOptions = options;
    // Set read and write speed to 9600 baud
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
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
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;
    // Commit
    tcflush(x, TCIFLUSH);
    tcsetattr(x, TCSANOW, &options);
}

// Need to fix vc import
void write_int(int y) {
    double speed = 0.8;
    double steeringAngle = -20.1; // vc.getExampleData(); 
    if (steeringAngle < 0) {
        steeringAngle = (steeringAngle * (-1));
    } else {
        steeringAngle = (steeringAngle + 25);
    }

    std::cout << "Angle: " << steeringAngle << std::endl;
    char toSend[10];
    snprintf(toSend, sizeof(toSend), "%g,%g", speed, steeringAngle);
    
    char sentence[13];
    strcpy(sentence, "{4:");
    strcat(sentence, toSend);
    strcat(sentence, "}");
    //puts(sentence);
    
    // "{4:0.8,15.2}"
    write(y, sentence, 13);
   
    std::cout << "Wrote: "<< sentence << std::endl;
}
 
std::string readString(int z) {
    char start[1] = "";
    char next[1] = "";
    char svar[10] = ""; 
    
    std::string result = ""; 
    std::cout << "Serial available: " << port << std::endl;
    
        //for (int x = 0; x < 6; x++) {
        read(z, start, 1);
        std::cout << "Start: " << start << std::endl;
        if (start[0] != '{') {
            std::cout << "No start" << std::endl;
            //readString(z);
            return svar;
        } else {
            result += start;
            //delete[] start;
            std::cout << "Result start: " << result << std::endl;
            for (int x = 0; x < 8; x++) {
                read(z, next, 1);
                result += next;
                //std::cout << "Result: " << result <<std::endl;
        }
        std::cout << "Efter loopen: " << result << std::endl;

        //svar = new char[result.size() + 1];
        memcpy(svar, result.c_str(), result.size() + 1);
       
        //svar = (char *)malloc(result.size() + 1);
        //memcpy(svar, result.c_str(), result.size() + 1);
        
        // strncpy(svar1, result.c_str(), sizeof(svar1));
        // sizeof(svar1) - 1 för att null terminatea
        
        svar[9] = '\0';
        //svar[sizeof(svar) - 1] = '\0';
        std::cout << "Svar kopia: " << svar << std::endl;        

        if (svar[0] == '{' && isdigit(svar[1])) {
            //size = svar[1];
            //speed = svar[2] + svar[3]; 
            std::cout << "Svar: " << svar[0] << std::endl;
            std::cout << "Svar: " << svar[1] << std::endl;
            std::cout << "Svar: " << svar[2] << std::endl;
            std::cout << "Svar: " << svar[3] << std::endl;
            std::cout << "Svar: " << svar[4] << std::endl;
            std::cout << "Svar: " << svar[5] << std::endl;
            std::cout << "Svar: " << svar[6] << std::endl;
            std::cout << "Svar: " << svar[7] << std::endl;
            std::cout << "Svar: " << svar[8] << std::endl;
            if (svar[8] == '}') {
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

void readSerial(int q) {
    char buf[15];
    // Read
    int n = read(q, buf, 15);
    // Null terminatea, alternativt, kommentera ut buf[n] = 0;
    buf[n] = 0;
    // Check read
    if (n > 0) {
        std::cout << "Bytes read: " << n << std::endl;
        std::cout << "In buf: " << buf << std::endl;
    } else {
        std::cout << "Got nothing!" << std::endl;
    }
}

} // msv

