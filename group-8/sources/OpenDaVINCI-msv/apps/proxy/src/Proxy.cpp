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
#include "core/data/control/VehicleControl.h"
#include "core/data/Constants.h"
#include "serial/serial.h"

#include "OpenCVCamera.h"

#include "GeneratedHeaders_Data.h"

#include "Proxy.h"

int countcounter = 0;

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;
    using namespace tools::recorder;
    using namespace serial;
    //VehicleData
    //using namespace core::data::environment;



    Proxy::Proxy(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "proxy"),
        m_recorder(NULL),
        m_camera(NULL),
        this_serial(NULL),
        endByte(0xFF),
        startByte(0xAA),
        outSer(),
        incomingSer(),
        oldIncomingSer(),
        speedOut(1521),
        steeringOut(91)     
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
        const bool headless = getKeyValueConfiguration().getValue<uint32_t>("global.headless") == 1;
        if (TYPE.compare("opencv") == 0) {
            m_camera = new OpenCVCamera(NAME, ID, WIDTH, HEIGHT, BPP, headless);
        }

        if (m_camera == NULL) {
            cerr << "No valid camera type defined." << endl;
        }

        //Establish serial port connection

    }

    void Proxy::tearDown() {
        // This method will be call automatically _after_ return from body().
        speedOut = 1520;
        steeringOut = 90;
        outSer[6] = 0;
        outSer[0] = startByte;
        outSer[1] = speedOut & 0xFF;
        outSer[2] = (speedOut >> 8) & 0xFF;
        outSer[3] = steeringOut & 0xFF;
        outSer[4] = (steeringOut >> 8) & 0xFF;
        outSer[5] = endByte;

        for(int i = 0; i < OUTSERIAL-1; i++){
            outSer[OUTSERIAL - 1] ^= outSer[i];
        } 
        for (int i = 0; i < 100; ++i){
            this_serial->write(outSer, 7);
        }
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
        cerr << "Gettingserial" << endl;
        uint8_t current = 0;
        uint8_t tempincoming[INSERIAL];
        uint8_t check = 0;
        //uint8_t success = 0;

        this_serial->read(&current,1);
        if(current != startByte){
            while (this_serial->read(&current,1) && current != endByte);
            this_serial->read(&current,1);
            this_serial->read(&current,1);
            //this_serial->read(&current,1);
        }
        tempincoming[0] = current;
        this_serial->read(tempincoming + 1, 16);
        // for(int i = 1; i < INSERIAL; i++){
        //     this_serial->read(&current,1);
        //     tempincoming[i] = current;
        // }
        for(int i = 0; i < INSERIAL; i++){
            check ^= tempincoming[i];
        }

        cout << (uint16_t)tempincoming[0] << endl;
        cout << (uint16_t)tempincoming[2] << endl;
        cout << (uint16_t)tempincoming[INSERIAL - 2] << endl;
        cout << (uint16_t)check << endl;
        if(tempincoming[0] == startByte && tempincoming[INSERIAL - 2] == endByte && check == 0){
            // memcpy(incomingSer, tempincoming, 17 * sizeof(uint8_t) );
            for(int i = 0; i < INSERIAL; i++){
                incomingSer[i] = tempincoming[i];
            }
            return 1;
        }else{
            return 1;
        }
    }

    void Proxy::sendSerial() {
        cerr << "Sending Serial: " << endl;
        Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
        VehicleControl vdata = containerVehicleControl.getData<VehicleControl> ();
        double speedSetting = vdata.getSpeed();
        double steeringSetting = vdata.getSteeringWheelAngle();
        //cerr << "speedSetting: " << speedSetting << endl;
        //cerr << "steeringSetting: " << steeringSetting << endl;
        //cerr << "RAD2DEG: " << Constants::RAD2DEG << endl;
        
        uint16_t speedOutTemp;
        uint16_t steeringOutTemp;

        // if(countcounter == 50){
        //     speedOut = 1;
        //     steeringOut = 34;
        // }

        // countcounter++;


        cerr << "start: " << (uint16_t)outSer[0] << endl;
        if(speedSetting < 0){
            speedOutTemp = 1200;
        }else if ((int)speedSetting == 0){
            speedOutTemp = 1520; 
        }else{
            speedOutTemp = 1560 + speedSetting;
        }
        steeringOutTemp = 90 - (uint16_t)(steeringSetting * Constants::RAD2DEG);
        if(steeringOutTemp < 65) steeringOutTemp = 65;
        if(steeringOutTemp > 115) steeringOutTemp = 115;
        // if(countcounter < 1000 || countcounter > 500){
        //     speedOut = 1600;
        // }

        // countcounter++;
        // cerr << "countcounter" << countcounter << endl;

        if(steeringOut != steeringOutTemp || speedOut != speedOutTemp){
            steeringOut = steeringOutTemp;
            speedOut = speedOutTemp;



            outSer[6] = 0;
            outSer[0] = startByte;
            outSer[1] = speedOut & 0xFF;
            outSer[2] = (speedOut >> 8) & 0xFF;
            outSer[3] = steeringOut & 0xFF;
            outSer[4] = (steeringOut >> 8) & 0xFF;
            outSer[5] = endByte;

            for(int i = 0; i < OUTSERIAL-1; i++){
                outSer[OUTSERIAL - 1] ^= outSer[i];
            } 

            //cerr << "checksum: " << (uint16_t)outSer[6] << endl;
            int sentnum = (int)this_serial->write(outSer, 7);
        uint16_t speedOutTemp;
        uint16_t steeringOutTemp;

        // if(countcounter == 50){
        //     speedOut = 1;
        //     steeringOut = 34;
        // }

        // countcounter++;


        cerr << "start: " << (uint16_t)outSer[0] << endl;
        if(speedSetting < 0){
            speedOutTemp = 1200;
        }else if ((int)speedSetting == 0){
            speedOutTemp = 1520; 
        }else{
            speedOutTemp = 1560 + speedSetting;
        }
        steeringOutTemp = 90 - (uint16_t)(steeringSetting * Constants::RAD2DEG);
        if(steeringOutTemp < 65) steeringOutTemp = 65;
        if(steeringOutTemp > 115) steeringOutTemp = 115;
        // if(countcounter < 1000 || countcounter > 500){
        //     speedOut = 1600;
        // }

        // countcounter++;
        // cerr << "countcounter" << countcounter << endl;

        //if(steeringOut != steeringOutTemp || speedOut != speedOutTemp){
            steeringOut = steeringOutTemp;
            speedOut = speedOutTemp;



            outSer[6] = 0;
            outSer[0] = startByte;
            outSer[1] = speedOut & 0xFF;
            outSer[2] = (speedOut >> 8) & 0xFF;
            outSer[3] = steeringOut & 0xFF;
            outSer[4] = (steeringOut >> 8) & 0xFF;
            outSer[5] = endByte;

            for(int i = 0; i < OUTSERIAL-1; i++){
                outSer[OUTSERIAL - 1] ^= outSer[i];
            } 

            //cerr << "checksum: " << (uint16_t)outSer[6] << endl;
            int sentnum = (int)this_serial->write(outSer, 7);
            uint16_t speedToArduino = outSer[1] + ((uint16_t)outSer[2] << 8);
            uint16_t steeringToArduino = outSer[3] + ((uint16_t)outSer[4] << 8);
            cerr << "sent bytes: " << sentnum << endl;
            cerr << "Speed sent to Arduino: " << speedToArduino << endl;
            cerr << "Steering sent to Arduino: " << steeringToArduino << endl;
       // }
    }

    void Proxy::distSerial() {
        VehicleData vd;

        SensorBoardData sbd;
        double irFrontRightDist;
        double irMiddleRightDist;
        double irBackDist;

        //Hardcodetest
        //uint16_t speed = 100;
        //uint16_t steering = 100;
        // uint16_t usFront = 100;
        // uint16_t usFrontRight = 100;
        // uint16_t irFrontRight = 100;
        // uint16_t irMiddleRight = 100;
        // uint16_t irBack = 100;

        uint16_t speed = ((uint16_t)incomingSer[2] << 8) | incomingSer[1];
        uint16_t steering = ((uint16_t)incomingSer[4] << 8) | incomingSer[3];
        uint16_t irFrontRight = ((uint16_t)incomingSer[6] << 8) | incomingSer[5];
        uint16_t irMiddleRight = ((uint16_t)incomingSer[8] << 8) | incomingSer[7];   
        uint16_t irBack = ((uint16_t)incomingSer[10] << 8) | incomingSer[9];
        uint16_t usFront = ((uint16_t)incomingSer[12] << 8) | incomingSer[11];
        uint16_t usFrontRight = ((uint16_t)incomingSer[14] << 8) | incomingSer[13];

        if(speed > 1520){
            vd.setSpeed((double)speed - 1560);
        }else if(speed < 1500){
            vd.setSpeed(-1);
        }else {
            speed = 0;
        }
        //vd.setHeading((double)steering);
        irFrontRightDist = (2914 / (irFrontRight +5))-1;
        irMiddleRightDist = (2914 / (irMiddleRight +5))-1;
        irBackDist = (2914 / (irBack +5))-1;
        sbd.putTo_MapOfDistances(0, (double)irFrontRightDist);
        sbd.putTo_MapOfDistances(1, (double)irBackDist);
        sbd.putTo_MapOfDistances(2, (double)irMiddleRightDist);
        sbd.putTo_MapOfDistances(3, (double)usFront);
        sbd.putTo_MapOfDistances(4, (double)usFrontRight);



        cerr << "SPEED RECEIVED from Arduino: " << speed << endl;
        cerr << "STEERING RECEIVED from Arduino: " << steering << endl;

        cerr << "Sensor 0 IR Front-Right: " << sbd.getValueForKey_MapOfDistances(0) << endl;
        cerr << " Sensor 1 IR Back: " << sbd.getValueForKey_MapOfDistances(1) << endl;
        cerr << " Sensor 2 IR Middle-Right: " << sbd.getValueForKey_MapOfDistances(2) << endl;
        cerr << " Sensor 3 US Front-Center: " << sbd.getValueForKey_MapOfDistances(3) << endl;
        cerr << " Sensor 4 US Front-Right: " << sbd.getValueForKey_MapOfDistances(4) << endl;

        Container contVD(Container::VEHICLEDATA, vd);
        distribute(contVD);
        Container contSBD(Container::USER_DATA_0, sbd);
        distribute(contSBD);
    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Proxy::body() {


        //Serial connection check
        uint32_t baud = 57600;
        string port = "/dev/ttyACM0"; 
        cerr << "Testing port: " << port << endl;
        // posible commands for getting serial port
        // CommandLineParser cmdParser;
        // cmdParser.addCommandLineArgument("id");

        int connection = 1;
        int correctserial = true;
        int serialcounter = 0;
        

        uint32_t captureCounter = 0;
        //Framerate
        //clock_t start;
        double cumduration;
        //double duration;
        //double oldduration = 0.0;
        //double smoothduration;
        //start = clock();
        
        time_t startTime = time(0);
        

        try{
            this_serial = new Serial(port, baud, Timeout::simpleTimeout(2000));
            //this_serial->flushInput();
        }catch (IOException e){
            cerr << "IO Exception - SerialPort" << port << "is not configured correctly" << endl;
            correctserial = false;
        }    
        
        


        while (getModuleState() == ModuleState::RUNNING) {


            // Capture frame.
            if (m_camera != NULL) {

                //cerr << "Capturing frame" << endl;
                
                core::data::image::SharedImage si = m_camera->capture();
                

                Container c(Container::SHARED_IMAGE, si);
                //cerr << "Distributing Frame" << endl;
                distribute(c);
                captureCounter++;
                // cumduration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
                // duration = cumduration - oldduration;
                // oldduration = cumduration;

                // smoothduration = (smoothduration * 0.9) + ((1/duration) * 0.1);

                // cout<<"process time = : "<< duration <<" secs." << endl;
                // cout<<"frame rate = : "<< 1/duration <<" /sec." << endl;
                // cout<<"smoothed frame rate = : "<< smoothduration <<" /sec." << endl;


            }

            // TODO: Here, you need to implement the data links to the embedded system
            // to read data from IR/US.
            //Serial
            
            if(correctserial && this_serial->isOpen()){
                cerr << "About to send serial" << endl;
                if(connection){
                    sendSerial();
                }
                //cerr << "Serial Baud =  " << this_serial->getBaudrate() << endl;
                if(this_serial->available() > 15){
                    connection = getSerial();
                }
                if(connection){
                    distSerial();
                    

                }else{
                    cout << "Proxy timed out on serial connection " << endl;
                    break;
                }
                serialcounter++;
                if(this_serial->available() > 32){
                    this_serial->flushInput();
                    serialcounter = 0;
                }
            }
            
        }
        //cumduration = ( clock() - start ) / (double) CLOCKS_PER_SEC;

        cout << "Proxy: Captured " << captureCounter << " frames." << endl;
        time_t endTime = time(0);
        cumduration = difftime(endTime, startTime);
        cout << "Proxy: Captured " << captureCounter/cumduration << " frames per sec." << endl;
        

        return ModuleState::OKAY;
    }

} // msv
