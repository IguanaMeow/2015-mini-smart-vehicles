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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 

#define SHM_SIZE 1024

#include <ctype.h>
#include <cstring>
#include <cmath>

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"

#include "OpenCVCamera.h"

#include "GeneratedHeaders_Data.h"

#include "Proxy.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
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

            // TODO: Here, you need to implement the data links to the embedded system
            // to read data from IR/US.

		key_t key;
                int shmid;
                char *data;
                //char *ptr;
                //int mode;
                //char* shared_memory[6];
                //int *p;
                /* make the key: */
                if ((key = 123123) == -1) {
                    perror("ftok");
                    exit(1);
                }
                /* connect to (and possibly create) the segment: */
                if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
                    perror("shmget");
                    exit(1);
                }
                /* attach to the segment to get a pointer to it: */
                data = (char*) shmat(shmid, (void *)0, 0);
                if (data == (char *)(-1)) {
                    perror("shmat");
                    exit(1);
                }
                /*
                printf("Data is = %p\n", data);
                printf("d : %c\n", data[0]);
                printf("d : %c\n", data[1]);
                printf("d : %c\n", data[2]);
                printf("d : %c\n", data[3]);
                printf("d : %c\n", data[4]);
                printf("d : %c\n", data[5]);
                printf("d : %c\n", data[6]);
                printf("d : %c\n", data[7]);
                */

                char sensor1[3], sensor2[3], sensor3[3], sensor4[3], sensor5[3];
                sensor1[0] = data[0];
                sensor1[1] = data[1];
                sensor1[2] = data[2];
                sensor2[0] = data[3];
                sensor2[1] = data[4];
                sensor2[2] = data[5];
                sensor3[0] = data[6];
                sensor3[1] = data[7];
                sensor3[2] = data[8];
                sensor4[0] = data[9];
                sensor4[1] = data[10];
                sensor4[2] = data[11];
                sensor5[0] = data[12];
                sensor5[1] = data[13];
                sensor5[2] = data[14];



                double sense0 = atoi(sensor1);
                double sense1 = atoi(sensor2);
                double sense2 = atoi(sensor3);
                double sense3 = atoi(sensor4);
                double sense4 = atoi(sensor5);

                //printf("s0 %f, s1 %f, s2 %f, s3 %f, s4 %f\n", sense0, sense1, sense2, sense3, sense4);
                

                SensorBoardData sbd;

                sbd.putTo_MapOfDistances(0, sense0);
                sbd.putTo_MapOfDistances(1, sense1);
                sbd.putTo_MapOfDistances(2, sense2);
                sbd.putTo_MapOfDistances(3, sense3);
                sbd.putTo_MapOfDistances(4, sense4);
                // double temp = sbd.getValueForKey_MapOfDistances(0);
                // printf("TEMP %f\n", temp);
                // Create container for finally sending the data.
                Container c(Container::USER_DATA_0, sbd);
                // Send container.
                getConference().send(c);
                

                if (shmdt(data) == -1) {
                    perror("shmdt");
                    exit(1);
                }

        }

        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }

} // msv

