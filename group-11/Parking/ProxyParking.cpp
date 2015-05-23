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

#include "core/io/ContainerConference.h"
#include "core/data/Constants.h"



#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"
#include "core/data/environment/VehicleData.h"
#include "core/data/control/VehicleControl.h"

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

    int forwardAngle = 70;
    double multiplier = 0.65;
    int adjustedAngle = 0;   
    char vehicleValue [10];

    int sensorPort, vehiclePort;

    // Declare sensor variables
    int FUS, FRUS, IRT, IRB, IRR;
    double distance;
    struct termios toptions;

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::environment;
    using namespace core::data::control;

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

    int openSerial(const char* port) {
 
      int fd;

      fd = open(port, O_RDWR | O_NOCTTY);
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

      toptions.c_cflag     &=  ~CRTSCTS;           //flow control
      toptions.c_cc[VMIN]   =  1;                  // read doesn't block
      toptions.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
      toptions.c_cflag     |=  CREAD | CLOCAL;
      /* Canonical mode */
      toptions.c_lflag |= ICANON;
      /* commit the serial port settings */
      tcsetattr(fd, TCSANOW, &toptions);

      return fd; 
     } 

    void writeVehicleValues(const char* data, int port, int length) 
    {	
		write(port, data, length);
	}    

    void readSensorValues(int port) 
    {
        /* Send byte to trigger Arduino to send string back */
        write(port, "0", 1);
        int  n = 0, cSensor = 0;
        char buf[64];
        
        /* Receive string from Arduino */
        n = read(port, buf, 64);  

        /* insert terminating zero in the string */
        buf[n] = 0;

        //printf("buffer: %s\n", buf);
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
                        if(cSensor > 5)
                        {
                            cSensor = 0;
                        }

                        // Adressing the correct sensor:
                        if(cSensor == 0)
                        {
                            sscanf(sensorTemp, "%d", &FUS); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 1)
                        {
                            sscanf(sensorTemp, "%d", &FRUS); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 2)
                        {
                            sscanf(sensorTemp, "%d", &IRT); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 3)
                        {
                            sscanf(sensorTemp, "%d", &IRB); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 4)
                        {
                            sscanf(sensorTemp, "%d", &IRR); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }
                        else if(cSensor == 5)
                        {
                            sscanf(sensorTemp, "%lf", &distance); 
                            memset(&sensorTemp[0], 0, sizeof(sensorTemp));
                            cSensor++;
                        }

                    }
                            
                // incrementing:
                s++; k++;
            }

            // Making sure that each sensor attained the correct value by printing them.:
          	printf("FUS: %d\n", FUS); printf("FRUS: %d\n", FRUS);
            printf("IRT: %d\n", IRT); printf("IRV: %d\n", IRB);
            printf("IRR: %d\n", IRR); printf("Distance traveled: %f\n", distance); 
            //printf("test: %d\n", n );

                
            // Vacate buffer for new incoming values:
            fflush(stdin);
            fflush(stdout);
        
    }

    
    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() 
    {
        uint32_t captureCounter = 0;
        int angle;

        /* Open usb ports, specify which port is for sensors and which port is for vehicle */
        sensorPort = openSerial("/dev/ttyACM1");
        vehiclePort = openSerial("/dev/ttyACM0");

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

            // USER_DATA_0 for sensors, USER_DATA_1 for vehicle data, USER_DATA_2 for steering data, USER_DATA_3 for vehicle control

            /* Reading Data from LLB */

          
            readSensorValues(sensorPort);
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

            // Set distance traveled for parking

            VehicleData vd;
            vd.setAbsTraveledPath(distance);

			cout << "Getabs is =" << vd.getAbsTraveledPath() << endl;

            Container contVD(Container::VEHICLEDATA, vd);
            distribute(contVD);

            // Send steering data to LLB

            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
			SteeringData sd = containerSteeringData.getData<SteeringData> ();

			Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);

			VehicleControl vc = containerVehicleControl.getData<VehicleControl> (); 

			double speed = sd.getSpeedData(); 
            cout  << "Proxy: SpeedData = " << sd.getSpeedData() << endl;
			char escValue [10];



			// Reverse car

			if (speed < 0.0)
			{
				strcpy(escValue, "");
				strcpy(escValue, " r");
			}

			// Drive car

			else if (speed > 0.0)
			{
				strcpy(escValue, "");
				strcpy(escValue, " f");

                
			}

			// Stop car

			else
			{
				strcpy(escValue, "");
				strcpy(escValue, " s");
			}


			char str1[16];
        	char str2[16];

			
			angle = sd.getExampleData();

            cout  << "Proxy: ExampleData = " << sd.getExampleData() << endl;
			
			//sd.setExampleData(angle);
			
			printf("Angle is:  %d\n", angle);
			
			if (angle > 0) 
			{
				strcpy(vehicleValue, "");
				strcpy(vehicleValue, ",r");
			} 
			
			else if (angle < 0) 
			{
				strcpy(vehicleValue, "");
				strcpy(vehicleValue, ",l");
			} 

			else 
			{
				strcpy(vehicleValue, "");
				strcpy(vehicleValue, ",f");
			}

			strcpy(str1, vehicleValue);
        	strcpy(str2, escValue);
        	//strcat(str1, str2);

			writeVehicleValues(strcat(str1, str2), vehiclePort, 4); 

            

        //  Send data to Arduino
        
     /*   Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
        SteeringData sd = containerSteeringData.getData<SteeringData> ();

        angle = sd.getExampleData();
        */
        
        /*
            Lane detector sends a value between -26 and 26 to proxy. This converts it to a degree between max left for the servo
            (30 degrees), forward (70 degrees) and max right (110 degrees).

        */

        /*
            Turn left when angle is lower than 0, turn right when it is greater than 0 and drive straight when it equals 0
        */

    /*    if (angle < 0)
        {
            adjustedAngle = angle / multiplier;                 //  Get the angle to turn in degrees based on the steeringdata divided with a multiplier
            adjustedAngle = -adjustedAngle;                     //  Turn adjusted angle into a positive value
            adjustedAngle = forwardAngle - adjustedAngle;       //  Displace the angle based on the servo calibration, i.e. 70 - 20 = 50 degrees to turn
        }

        else if (angle == 0)
        {
            adjustedAngle = 70;
        }

        else if(angle > 0)
        {
            adjustedAngle = angle / multiplier;
            adjustedAngle = forwardAngle + adjustedAngle;       //  Displace angle for the servo, i.e. 70 + 20 = 90 degrees to turn
        }
        
        char buffer [10];
        sprintf(buffer,"%d",adjustedAngle);     //  Convert int to string

        char str1[16];
        char str2[16];
        strcpy(str1, buffer);
        strcpy(str2, " ");
        strcat(str1, str2);
        
        if (adjustedAngle > 99)
        {
            writeByte(buffer, fd, 4);  
   
        }

        else 
        {
            writeByte(buffer, fd, 3);
        }
            
        */

            cout << "Proxy: Captured " << captureCounter << " frames." << endl;

            
        }

        return ModuleState::OKAY;

    } 


}



