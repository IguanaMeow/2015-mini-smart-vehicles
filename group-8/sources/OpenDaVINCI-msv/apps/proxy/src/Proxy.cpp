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
#include <serial/serial.h>

#include "OpenCVCamera.h"

#include "GeneratedHeaders_Data.h"

#include "Proxy.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace tools::recorder;
    using namespace serial;
    //VehicleData
    //using namespace core::data::environment;

    const int INSERIAL = 16;

    Proxy::Proxy(const int32_t &argc, char **argv) :
	    ConferenceClientModule(argc, argv, "proxy"),
        m_recorder(NULL),
        this_serial(NULL),
        endByte(0xFF),
        startByte(0xAA),
        incomingSer(),
        oldIncomingSer(),
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

        //Establish serial port connection

        uint32_t baud = 9600;
        string port = "/dev/ttyS0" 
        // posible commands for getting serial port
        // CommandLineParser cmdParser;
        // cmdParser.addCommandLineArgument("id");
        Serial this_serial(port, baud, serial::Timeout::simpleTimeout(2000));
        if(!this_serial.isOpen()){
            cerr << "SerialPort is not open" << endl;
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

    int Proxy::getSerial() {

        uint8_t current = 0;
        uint8_t tempincoming[INSERIAL];
        uint8_t check = 0;
        uint8_t success = 0;

        while (this_serial.read(&current,1) && current != endByte);
        success = this_serial.read(&tempincoming,INSERIAL);
        for(int i = 0; i > (INSERIAL- 1); i++)
            check ^= tempincoming[i];
        }
        if(success){
            if(tempincoming[0] == startByte && tempincoming[INSERIAL - 2] == endByte && check == 0){
                incomingSer = tempincoming;
            }
            return 1;
        }else{
            return 0;
        }
    }

    // void Proxy::sendSerial() {
    // }

    void Proxy::distSerial() {
        VehicleData vd;
        SensorBoardData sbd;

        //Hardcodetest
        // uint16_t speed = 100;
        // uint16_t steering = 100;
        // uint16_t usFront = 100;
        // uint16_t usFrontRight = 100;
        // uint16_t irFrontRight = 100;
        // uint16_t irMiddleRight = 100;
        // uint16_t irBack = 100;

        uint16_t speed = ((uint16_t)incomingSer[2] << 8) | incomingSer[1];
        uint16_t steering = ((uint16_t)incomingSer[3] << 8) | incomingSer[4];
        uint16_t irFrontRight = ((uint16_t)incomingSer[5] << 8) | incomingSer[6];
        uint16_t irMiddleRight = ((uint16_t)incomingSer[7] << 8) | incomingSer[8];
        uint16_t irBack = ((uint16_t)incomingSer[9] << 8) | incomingSer[10];
        uint16_t usFront = ((uint16_t)incomingSer[11] << 8) | incomingSer[12];
        uint16_t usFrontRight = ((uint16_t)incomingSer[13] << 8) | incomingSer[14];


        // vd.setSpeed((double)speed);
        // vd.setSteering((double)steering);
        sbd.putTo_MapOfDistances(0, (double)irFrontRight);
        sbd.putTo_MapOfDistances(1, (double)irBack);
        sbd.putTo_MapOfDistances(2, (double)irMiddleRight);
        sbd.putTo_MapOfDistances(3, (double)usFront);
        sbd.putTo_MapOfDistances(4, (double)usFrontRight);



        // cerr << "Speed: " << vd.getSpeed(); << endl;
        // cerr << "Steering: " << vd.getHeading(); << endl;

        cerr << "Sensor 0 IR Front-Right: " << sbd.getValueForKey_MapOfDistances(0) << endl;
        cerr << " Sensor 1 IR Back: " << sbd.getValueForKey_MapOfDistances(1) << endl;
        cerr << " Sensor 2 IR Middle-Right: " << sbd.getValueForKey_MapOfDistances(2) << endl;
        cerr << " Sensor 3 US Front-Center: " << sbd.getValueForKey_MapOfDistances(3) << endl;
        cerr << " Sensor 4 US Front-Right: " << sbd.getValueForKey_MapOfDistances(4) << endl;

        // Container contVD(Container::VEHICLEDATA, vd);
        // distribute(contVD);
        Container contSBD(Container::USER_DATA_0, sbd);
        distribute(contSBD);
    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() {

        //Serial connection check
        int connection = 1;
        this_serial.flushInput();

        uint32_t captureCounter = 0;
        //Framerate
        clock_t start;
        double cumduration;
        double duration;
        double oldduration = 0.0;
        double smoothduration;
        start = clock();


        while (getModuleState() == ModuleState::RUNNING) {
            // Capture frame.
            if (m_camera != NULL) {
                
                core::data::image::SharedImage si = m_camera->capture();

                Container c(Container::SHARED_IMAGE, si);
                distribute(c);
                captureCounter++;
                cumduration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
                duration = cumduration - oldduration;
                oldduration = cumduration;

                smoothduration = (smoothduration * 0.9) + ((1/duration) * 0.1);

                cout<<"process time = : "<< duration <<" secs." << endl;
                cout<<"frame rate = : "<< 1/duration <<" /sec." << endl;
                cout<<"smoothed frame rate = : "<< smoothduration <<" /sec." << endl;


            }

            // TODO: Here, you need to implement the data links to the embedded system
            // to read data from IR/US.
            //Serial
            connection = getSerial();
            distSerial();


            if(connection){
                this_serial.flushInput();
            }else{
                cout << "Proxy timed out on serial connection " << endl;
                break;
            }
        }
        cumduration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
        cout << "Proxy: Captured " << captureCounter << " frames." << endl;
        cout << "Proxy: Captured " << captureCounter/cumduration << " frames per sec." << endl;
        

        return ModuleState::OKAY;
    }

} // msv

