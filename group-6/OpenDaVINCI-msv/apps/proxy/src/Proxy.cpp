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

#include <stdio.h> 
#include <iostream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <cstring>
#include <cmath>

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/TimeStamp.h"
#include "core/data/control/VehicleControl.h"
#include "OpenCVCamera.h"
#include "GeneratedHeaders_Data.h"
#include "Proxy.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace tools::recorder;

    Proxy::Proxy(const int32_t &argc, char **argv) :
	    ConferenceClientModule(argc, argv, "proxy"),
        m_recorder(NULL),
        m_camera(NULL)
    {}

    Proxy::~Proxy() {}

    void discover();

    int getFileDescriptor(char*);

    int receiver = -1,
        sender = -1;

    void Proxy::setUp() {
	    // This method will be call automatically _before_ running body().
        if (getFrequency() < 20) {
            cerr << endl << endl << "Proxy: WARNING! Running proxy with a LOW frequency (consequence: data updates are too seldom and will influence your algorithms in a negative manner!) --> suggestions: --freq=20 or higher! Current frequency: " << getFrequency() << " Hz." << endl << endl << endl;
        }

        // Discover arduinos
        discover();

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

    SensorBoardData sbd;

    VehicleControl vc;

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() {
        uint32_t captureCounter = 0;
        while (getModuleState() == ModuleState::RUNNING) {

            // Capture Frame
            if (m_camera != NULL) {
                core::data::image::SharedImage si = m_camera->capture();

                Container c(Container::SHARED_IMAGE, si);
                distribute(c);
                captureCounter++;
            }

            // Send Data
            Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
            vc = containerVehicleControl.getData<VehicleControl>();

            char buffer[7];

            int direction = 25 + vc.getSteeringWheelAngle() / Constants::DEG2RAD,
                speed = 20 + 10 * vc.getSpeed();
                
            if(direction > 50) direction = 50;
            else if(direction < 0) direction = 0;
            
            if(speed > 40) speed = 40;
            else if(speed < 0) speed = 0;
                
            int checksum = (direction % 10 + direction / 10 + speed % 10 + speed / 10) % 10,
                n = sprintf(buffer, "s%02d%02d%d", direction, speed, checksum);

            if(receiver > -1) write(receiver, buffer, n);

            
            // Receive Data
            if(sender < 0) continue;

            char incoming[255],
                 message[18] = "00000000000000000";

            read(sender, &incoming, 255);

            for(int i = 0, index = 0; i < 255; index++, i++) {

                if(!isdigit(incoming[i])) { index = -1; continue; }

                message[index] = incoming[i];

                if(index < 17) continue;

                memset(incoming, 0, 255);

                tcflush(sender, TCIFLUSH);

                string m = (string)message;

                int sonarFront = atoi(m.substr(0,4).c_str()),
                    sonarSide = atoi(m.substr(4,4).c_str()),
                    irFront = atoi(m.substr(8,3).c_str()),
                    irMiddle = atoi(m.substr(11,3).c_str()),
                    irBack = atoi(m.substr(14,3).c_str());

                
                cout << endl << endl
                     << "Frame:     " << captureCounter << endl
                     << "Speed:     " << speed << endl
                     << "Direction: " << direction << endl
                     << "US Front:  " << sonarFront << " cm" << endl
                     << "US Side:   " << sonarSide << " cm" << endl
                     << "IR Front:  " << irFront << " cm" << endl
                     << "IR Middle: " << irMiddle << " cm" << endl
                     << "IR Back:   " << irBack << " cm" << endl;

                sbd.putTo_MapOfDistances(0, irFront);
                sbd.putTo_MapOfDistances(1, irBack);
                sbd.putTo_MapOfDistances(2, irMiddle);
                sbd.putTo_MapOfDistances(3, sonarFront);
                sbd.putTo_MapOfDistances(4, sonarSide);

                Container containerSensorBoardData = Container(Container::USER_DATA_0, sbd);

                distribute(containerSensorBoardData);
                
                break;

            }

        }

        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }


    void discover() {
        DIR *d = opendir("/dev/serial/by-id/");

        struct dirent *de = NULL;

        while((de = readdir(d))){
            if(strstr(de->d_name,"Leonardo") != 0) receiver = getFileDescriptor(de->d_name);
            else if(strstr(de->d_name,"arduino") != 0) sender = getFileDescriptor(de->d_name);
        }

        cout << "receiver: " << receiver << endl << "sender: " << sender << endl;

        closedir(d);
    }

    int getFileDescriptor(char* name) {

        char s[PATH_MAX+20],
               buf[1024];

        sprintf(s, "%s%s", "/dev/serial/by-id/", name);
        
        int len;

        if((len = readlink(s, buf, sizeof(buf)-1)) != -1) buf[len] = '\0';

        char portname[PATH_MAX+1],
               settings[PATH_MAX + 160];

        realpath(s, portname);

        int fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
        
        /* Set up the control structure */
        sprintf(settings, "stty -F %s cs8 115200 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts", portname);
        
        system(settings);
        
        cout << settings << endl;

        /* Wait for the Arduino to reset */
         usleep(1000*1000);

         /* Flush anything already in the serial buffer */
         tcflush(fd, TCIOFLUSH);

        return fd;

    }

} // msv

