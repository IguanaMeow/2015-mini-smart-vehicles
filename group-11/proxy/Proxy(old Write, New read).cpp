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

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"
#include "core/data/environment/VehicleData.h"

#include "OpenCVCamera.h"

#include "GeneratedHeaders_Data.h"

#include "Proxy.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
 #include <sstream>

namespace msv {

    int fd, angle, counter=0;
    // Declare sensor variables
    int FUS, FRUS, IRT, IRB, IRR;  
 
    const char* z = "";  

    struct termios toptions;

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::environment;
    using namespace tools::recorder;

    Proxy::Proxy(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "proxy"),
        m_recorder(NULL),
        m_camera(NULL)
    {}

    Proxy::~Proxy() {
    }

    void Proxy::setUp() {
        // This method will be call automatically _before_ running body().
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

    int openSerial() {
 

       fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
       /* wait for the Arduino to reboot */
        usleep(3500000);
  
      /* get current serial port settings */
      tcgetattr(fd, &toptions);
      /* set 9600 baud both ways */
      cfsetispeed(&toptions, B115200);
      cfsetospeed(&toptions, B115200);
      /* 8 bits, no parity, no stop bits */
      toptions.c_cflag &= ~PARENB;
      toptions.c_cflag &= ~CSTOPB;
      toptions.c_cflag &= ~CSIZE;
      toptions.c_cflag |= CS8;
      /* Canonical mode */
      toptions.c_lflag |= ICANON;
      /* commit the serial port settings */
      tcsetattr(fd, TCSANOW, &toptions);

       return fd; 
     } 

    void writeByte(const char* inByte, int port) {


	// Go right
      if (*inByte == 'a') {
        write(port, "0", 1);
	// Go left
      } else if (*inByte == 'b') {
        write(port, "1", 1);
      } else {
	// Go forwertz
	write(port, "2", 1);
      }


    }     
    /* Receives and handles data from LLB */
    void readByte(int port) {
	
        /* Send byte to trigger Arduino to send string back */
        write(port, "0", 1);
        int  n = 0, cSensor = 0;
        char buf[64];

          /* Receive string from Arduino */
          n = read(fd, buf, 64);  

          /* insert terminating zero in the string */
          buf[n] = 0;

          // Save to temporary char array
          char sensorTemp[64];

         
          /* The main exe. area */
            int s = 0, k = 0;

         
            // While not stop-zero (space):
            while(buf[k] != '\0')
            {
                
                // Copy strings elements:
                sensorTemp[s] = buf[k];
                
                    if(sensorTemp[s]  == ' ')
                    {

                        // Add a stop zero:
                        sensorTemp[s] = '\0';

                        // Reset s value used indexing:
                        s = -1;

                        // Controlling correct sensor:
                        if(cSensor > 4)
                        {
                            cSensor = 0;
                        }

                        // Adressing the correct sensor:
                        if(cSensor == 0)
                        {   // Front US
                            sscanf(sensorTemp, "%d", &FUS); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 1)
                        {   // Front-right US
                            sscanf(sensorTemp, "%d", &FRUS); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 2)
                        {   // IR-top
                            sscanf(sensorTemp, "%d", &IRT); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 3)
                        {   // IR-bottom
                            sscanf(sensorTemp, "%d", &IRB); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 4)
                        {   // IR-rear
                            sscanf(sensorTemp, "%d", &IRR); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }   

                    }
                            
                // incrementing:
                s++; k++;
            }

            /*// Making sure that each sensor attained the correct value by printing them.:
            printf("FUS: %d\n", FUS); printf("FRUS: %d\n", FRUS);
            printf("IRT: %d\n", IRT); printf("IRV: %d\n", IRB);
            printf("IRR: %d\n", IRR); */
		

            // Vacate buffer for new incoming values:
            fflush(stdin);
            fflush(stdout);
        
    }

    
    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() 
    {
    	openSerial();
        uint32_t captureCounter = 0;
        while (getModuleState() == ModuleState::RUNNING) 
        {
            // Capture frame.
            if (m_camera != NULL) 
            {
                core::data::image::SharedImage si = m_camera->capture();

                Container c(Container::SHARED_IMAGE, si);
                distribute(c);
                captureCounter++;
            }

	    /* Reading Data from LLB */
	    readByte(fd);
	    SensorBoardData sbd;
        
	     // Map out real sensorboard data
	     sbd.putTo_MapOfDistances(0, (int)IRT);
             sbd.putTo_MapOfDistances(1, (int)IRR);
             sbd.putTo_MapOfDistances(2, (int)IRB);
             sbd.putTo_MapOfDistances(3, (int)FUS);
             sbd.putTo_MapOfDistances(4, (int)FRUS);

            // Distribute sensorBoardData through UDP
	    Container contSBD(Container::USER_DATA_0, sbd);
            distribute(contSBD); 
	    /* End of Reading Data from LLB */


	    /* Sending Data to LLB */
	    Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
 	    SteeringData sd = containerSteeringData.getData<SteeringData> ();

            angle = sd.getExampleData();

            if (angle > 0) {
		z = "a";
 	    } else if (angle < 0) {
		z = "b";
	    } else {
		z = "c";
 	    }


          /*  if (counter == 0) {
		    openSerial();
		    counter++;
            } */
         
            writeByte(z, fd);  
            /* End of Sending Data to LLB */


            cout << "Proxy: Captured " << captureCounter << " frames." << endl;

            
        }

        return ModuleState::OKAY;

    } 


}



