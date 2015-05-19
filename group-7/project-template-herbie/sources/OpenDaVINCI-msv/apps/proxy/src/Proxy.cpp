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
#include <string>
#include <cmath>
#include <unistd.h>
#include <sstream>

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"
#include "core/data/control/VehicleControl.h"
#include "OpenCVCamera.h"
#include "core/data/environment/VehicleData.h"
#include "GeneratedHeaders_Data.h"

#include "Proxy.h"
#include "Netstring.h"
#include "../serial/include/serial/serial.h"

 double extractData(string, string);

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
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

    double front_us, fr_ir, rr_ir, fr_us, rear_ir; // values to pass to HLB
    SensorBoardData sbd;
    VehicleControl vc;
    VehicleData vd;
    string vcDataString, sensorData;
    serial::Serial my_serial("/dev/ttyACM0", 9600, serial::Timeout::simpleTimeout(1000));
    stringstream ss;
    bool dataReceived = true;
   
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
           
            Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);
            vc = containerVehicleControl.getData<VehicleControl> ();     
            Container c1(Container::USER_DATA_0, sbd);
            Container c2(Container::VEHICLEDATA, vd);
            
        
        if(dataReceived){
        ss << (vc.getSteeringWheelAngle() * -1);
        vcDataString = "WA=" + ss.str();
        ss.str("");
        ss << vc.getSpeed();
        vcDataString += "SP=";
        vcDataString += ss.str();
        ss.str("");
        my_serial.write(encodeNetstring(vcDataString));
        cerr << vcDataString << endl;
        dataReceived = false;
    }

        if(my_serial.available()) {
            string result =  my_serial.readline(1024, ",");
            sensorData = decodeNetstring(result);
            cerr << result << endl;
            sbd.putTo_MapOfDistances(0, extractData("IR1", sensorData)); // IR front right
            sbd.putTo_MapOfDistances(1, extractData("IR2", sensorData)); // IR rear
            sbd.putTo_MapOfDistances(2, extractData("IR3", sensorData)); // IR rear right
            sbd.putTo_MapOfDistances(3, extractData("US1", sensorData)); // US front
            sbd.putTo_MapOfDistances(4, extractData("US2", sensorData)); // US front right
            vd.setAbsTraveledPath(75); // hardcoded to test
            distribute(c1);
            distribute(c2);
            dataReceived = true;
            }
        }

        cout << "Proxy: Captured " << captureCounter << " frames." << endl;

        return ModuleState::OKAY;
    }

} // msv

double extractData(string label, string sbData){

    double sensorValue;
    int stringPos = sbData.find(label);
    stringstream convert(sbData.substr(stringPos + 4));
    convert >> sensorValue;
    return sensorValue;
}
