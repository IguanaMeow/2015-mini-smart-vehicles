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
#include "RunningMedian.h"

#include "Proxy.h"

int countcounter = 0;

namespace msv {

    using namespace std;
    using namespace serial;
    using namespace core::base;
    using namespace core::data;
    using namespace tools::recorder;
    using namespace core::data::control;
    using namespace core::data::environment;


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
        speedOut(1500),
        steeringOut(90),
        irFrontRightMedian(),
        irMiddleRightMedian(),
        irBackMedian(),
        usFrontMedian(),
        usFrontRightMedian()     
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

    /*---This method will be call automatically _after_ return from body().---*/

    void Proxy::tearDown() {
        
        /*---send neutral signals to arduino---*/
        speedOut = 1500;
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
        for (int i = 0; i < 300; ++i){
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

    /*---Send the serial packet containing vehicle control data---*/

    void Proxy::sendSerial() {

        uint16_t speedOutTemp;
        uint16_t steeringOutTemp;

        /*---Collect vehicle control data---*/

        Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
        VehicleControl vdata = containerVehicleControl.getData<VehicleControl> ();
        
        double speedSetting = vdata.getSpeed();
        double steeringSetting = vdata.getSteeringWheelAngle();


        /*---Calculate speed and steering settings---*/
        
        if(speedSetting < 0){
            speedOutTemp = 1180;
        }else if ((int)speedSetting == 0){
            speedOutTemp = 1500; 
        }else{
            if(steeringSetting > 21 || steeringSetting < -21){
                speedOutTemp = 1565;
            }else{
                speedOutTemp = 1570 + speedSetting;
            }
        }

        //for full steering set max physical angle
        if(steeringSetting >= 25) steeringSetting = 40;
        if(steeringSetting <= -26) steeringSetting = -40;

        steeringOutTemp = 90 + (int16_t)(steeringSetting * Constants::RAD2DEG);


        /*---Do not send packet unless values have changed---*/
        if(steeringOut != steeringOutTemp || speedOut != speedOutTemp){
            steeringOut = steeringOutTemp;
            speedOut = speedOutTemp;


            
            /*---deconstruct speed and steering settings---*/
            outSer[0] = startByte;
            outSer[1] = speedOut & 0xFF;
            outSer[2] = (speedOut >> 8) & 0xFF;
            outSer[3] = steeringOut & 0xFF;
            outSer[4] = (steeringOut >> 8) & 0xFF;
            outSer[5] = endByte;

            /*--- calculate the checksum---*/
            outSer[6] = 0;

            for(int i = 0; i < OUTSERIAL-1; i++){
                outSer[OUTSERIAL - 1] ^= outSer[i];
            } 

            /*---Write byte array to Serial device---*/

            int sentnum = (int)this_serial->write(outSer, 7);
            cerr << "Succesfully sent bytes: " << sentnum << endl;
        }
    }

    /*---Read sensor data from the car/arduino from serial device---*/

    int Proxy::getSerial() {

        uint8_t current = 0;
        uint8_t tempincoming[INSERIAL];
        uint8_t check = 0;

        /*---check that the first byte in line is the startByte else read to next endByte and reset start byte---*/
        this_serial->read(&current,1);
        if(current != startByte){
            while (this_serial->read(&current,1) && current != endByte);
            this_serial->read(&current,1);
            this_serial->read(&current,1);
        }


        
        tempincoming[0] = current;
        this_serial->read(tempincoming + 1, 16);

        //calculate checksum
        for(int i = 0; i < INSERIAL; i++){
            check ^= tempincoming[i];
        }

        /*---If all the expected bits are in place and checksum is satisfactory put 
            temporary array to global and return success value---*/
        
        if(tempincoming[0] == startByte && tempincoming[INSERIAL - 2] == endByte && check == 0){
            for(int i = 0; i < INSERIAL; i++){
                incomingSer[i] = tempincoming[i];
            }
            return 1;
        }else{
            return 0;
        }
    }

    /*---If an incoming serial packet is validated, it will be distributed to shared memory---*/

    void Proxy::distSerial() {
        
        VehicleData vd;
        SensorBoardData sbd;

        double irFrontRightDist;
        double irMiddleRightDist;
        double irBackDist; 

        /*---Reconstruct the byte array into expected variables---*/

        uint16_t absDistance = ((uint16_t)incomingSer[2] << 8) | incomingSer[1];
        uint16_t absDirection = ((uint16_t)incomingSer[4] << 8) | incomingSer[3];
        uint16_t irFrontRight = ((uint16_t)incomingSer[6] << 8) | incomingSer[5];
        uint16_t irMiddleRight = ((uint16_t)incomingSer[8] << 8) | incomingSer[7];   
        uint16_t irBack = ((uint16_t)incomingSer[10] << 8) | incomingSer[9];
        uint16_t usFront = ((uint16_t)incomingSer[12] << 8) | incomingSer[11];
        uint16_t usFrontRight = ((uint16_t)incomingSer[14] << 8) | incomingSer[13];

        /*---Convert IR values from Analogue to Digital raw values to CM---*/

        irFrontRightDist = (2914 / (irFrontRight +5))-1;
        irMiddleRightDist = (2914 / (irMiddleRight +5))-1;
        irBackDist = (2914 / (irBack +5))-1;

        /*---Smooth out values with RunningMedian - push current value to "sliding window"
            and sorted array---*/
            
        irFrontRightMedian.addValue(irFrontRightDist);
        irMiddleRightMedian.addValue(irMiddleRightDist);
        irBackMedian.addValue(irBackDist);
        usFrontMedian.addValue(usFront);    
        usFrontRightMedian.addValue(usFrontRight);

        /*---Smooth out values with RunningMedian - retreive median value---*/

        irFrontRightDist = irFrontRightMedian.getMedian();
        irMiddleRightDist = irMiddleRightMedian.getMedian();
        irBackDist = irBackMedian.getMedian();
        usFront = usFrontMedian.getMedian();
        usFrontRight = usFrontRightMedian.getMedian();

        /*---limits of the sensor range return -1 if over---*/
        int irLimit = 40;
        int usLimit = 120;

        /*---Put fully processed values into the Sensor Board Data container---*/

        if(irFrontRightDist > irLimit){
            sbd.putTo_MapOfDistances(0, -1);
        }else{
            sbd.putTo_MapOfDistances(0, (double)irFrontRightDist/100);
        }
        if(irBackDist > irLimit){
            sbd.putTo_MapOfDistances(1, -1);
        }else{
            sbd.putTo_MapOfDistances(1, (double)irBackDist/100);
        }
        if(irMiddleRightDist > irLimit){
            sbd.putTo_MapOfDistances(2, -1);
        }else{
            sbd.putTo_MapOfDistances(2, (double)irMiddleRightDist/100);
        }
        if(usFront > usLimit){
            sbd.putTo_MapOfDistances(3, -1);
        }else{
            sbd.putTo_MapOfDistances(3, (double)usFront/100);
        }
        if(usFrontRight > usLimit){
            sbd.putTo_MapOfDistances(4, -1);
        }else{
            sbd.putTo_MapOfDistances(4, (double)usFrontRight/100);
        }
        
        /*---Put fully processed values into the Sensor Board Data container---*/

        vd.setAbsTraveledPath((double)absDistance/100);
        vd.setHeading((double)absDirection  * Constants::DEG2RAD);
        
        cout << "AbsoluteDistance : " << absDistance << endl;
        cout << "AbsoluteDirection: " << absDirection << endl;

        cout << "Sensor 0 IR Front-Right: " << sbd.getValueForKey_MapOfDistances(0) << endl;
        cout << "Sensor 1 IR Back: " << sbd.getValueForKey_MapOfDistances(1) << endl;
        cout << "Sensor 2 IR Middle-Right: " << sbd.getValueForKey_MapOfDistances(2) << endl;
        cout << "Sensor 3 US Front-Center: " << sbd.getValueForKey_MapOfDistances(3) << endl;
        cout << "Sensor 4 US Front-Right: " << sbd.getValueForKey_MapOfDistances(4) << endl;

        /*---Distribute containers---*/

        Container contVD(Container::VEHICLEDATA, vd);
        distribute(contVD);
        Container contSBD(Container::USER_DATA_0, sbd);
        distribute(contSBD);
    }

    /*---This method does the main data processing job.---*/

    ModuleState::MODULE_EXITCODE Proxy::body() {
        
        /*---Real time frequency measurement---*/
        uint32_t captureCounter = 0;
        double cumulDuration;
        time_t startTime = time(0);
        
        /*---Set serial settings---*/
        uint32_t baud = 115200;
        string port = "/dev/ttyACM0";
        cerr << "Trying port: " << port << endl;

        int goodSerial = 1;
        int correctSerialDevice = true;

        try{
            this_serial = new Serial(port, baud, Timeout::simpleTimeout(2000));
            //this_serial->flushInput();
        }catch (IOException e){
            cerr << "IO Exception - SerialPort" << port << "is not configured correctly" << endl;
            correctSerialDevice = false;
        }    
        
        while (getModuleState() == ModuleState::RUNNING) {

            /*---Capture image frame.--*/

            if (m_camera != NULL) {                
                core::data::image::SharedImage si = m_camera->capture();
                Container c(Container::SHARED_IMAGE, si);
                distribute(c);
                captureCounter++;
                cout << "Captured Frame" << endl;
            }

            /*---Start serial sequence---*/            

            if(correctSerialDevice && this_serial->isOpen()){
                //if not receiving serial do not send
                if(goodSerial){
                    sendSerial();
                }

                //only attempt serial read if there is a packet to read
                if(this_serial->available() > 15){
                    goodSerial = getSerial();
                }
                
                //if a packet is captured, distribute it to shared memory
                if(goodSerial){
                    distSerial();
                }

                // if incoming serial buffer is falling behind, flush it
                if(this_serial->available() > 32){
                    this_serial->flushInput();
                }
            }
            
        }

        /*---Show realtime frequency---*/
        cout << "Proxy: Captured " << captureCounter << " frames." << endl;
        time_t endTime = time(0);
        cumulDuration = difftime(endTime, startTime);
        cout << "Proxy: Captured " << captureCounter/cumulDuration << " frames per sec." << endl;
        

        return ModuleState::OKAY;
    }

} // msv
