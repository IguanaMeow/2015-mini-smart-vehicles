/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe 
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

#include <iostream>
#include <vector>

#include "core/macros.h"
#include "core/base/Lock.h"
#include "core/wrapper/SerialPortFactory.h"
#include "core/wrapper/SerialPort.h"
#include "core/wrapper/NetstringsProtocol.h"
#include "core/wrapper/StringProtocol.h"

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/image/SharedImage.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include "SerialKiller.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;


    SerialKiller::SerialKiller(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "serial_killer"),
        sensorValues(),
        sbd(),
        m_bufferMutex(),
        m_buffer("")
	    {}

    SerialKiller::~SerialKiller() {}

    void SerialKiller::setUp() {}

    void SerialKiller::tearDown() {}

    void SerialKiller::handleConnectionError() {
        cout << "SerialKiller: connection error.'" << endl;
    }

    void SerialKiller::nextString(const string &s) {
        //cout << "SerialKiller: '" << s << "'" << endl;
        sensorValues = std::map<uint32_t, double>();
        {   
            string value = ""; 
            string label = "";
            for ( std::string::const_iterator it=s.begin(); it!=s.end(); ++it) {
                //std::cout << *it;
                if (*it == '[') {
                    label = "";
                    ++it;
                    if (it == s.end()) {
                        break;
                    }
                    label += *it;
                    ++it;
                    if (it == s.end()) {
                        break;
                    }
                    label += *it;
                } else if (*it == ']') {
                    takeAction(label, value);
                    //cout << value << endl;
                    value = "";
                } else {
                    value += *it;
                }
            }


            //Lock l(m_bufferMutex);
            //m_buffer = s;
        }
        sbd.setNumberOfSensors(6);

        sbd.setMapOfDistances(sensorValues);

        Container c(Container::USER_DATA_0, sbd);

        getConference().send(c);


    }

    void SerialKiller::takeAction(const string& title, const string& data) {



        if (title == "OA" || title == "WC") {
            int value = 0;
            bool neg = false;
            vector<int> values = vector<int>();
            for ( std::string::const_iterator it=data.begin(); it!=data.end(); ++it) {
                if (*it == ')' || *it == ';') {
                    if (neg) {
                        value = -value;
                    }
                    values.push_back(value);
                    value = 0;
                    neg = false;
                } else if (*it >= '0' && *it <= '9') {
                    value = (value * 10) + ((*it) - '0');
                } else if (*it == '-') {
                    neg = true;
                } else if (*it == '(') {
                    value = 0;
                    neg = false;
                }
            }
            if (title == "WC") {
                Container vehicleDataThing = getKeyValueDataStore().get(Container::VEHICLEDATA);
                VehicleData vdd = vehicleDataThing.getData<VehicleData>();
                vdd.setSpeed(values.at(3));
                vdd.setAbsTraveledPath(values.at(1));
                Container c(Container::VEHICLEDATA, vdd);
                getConference().send(c);
                /*cout << title;
                for (vector<int>::const_iterator it = values.begin(); it != values.end(); ++it) {
                    cout << " " << *it;
                }
                cout << endl;*/
            }
        } else if (title == "IR" || title == "US") {
            for ( std::string::const_iterator it=data.begin(); it!=data.end(); ++it) {
                int id = 0;
                int value = 0;
                bool neg = false;
                for (;*it != ')';++it) {
                     if (*it == ';') {
                        if (neg) {
                            value = -value;
                        }
                        //cout << value << endl;
                        id = value;
                        value = 0;
                        neg = false;
                    } else if (*it >= '0' && *it <= '9') {
                        value = (value * 10) + ((*it) - '0');
                    } else if (*it == '-') {
                        neg = true;
                    } else if (*it == '(') {
                        value = 0;
                        neg = false;
                    }
                }
                if (neg) {
                    value = -value;
                }
                if (title == "IR") {
                    sensorValues[id] = (double)value;
                }
                else if (title == "US") {
                    if (id == 226) {
                        sensorValues[US_FrontCenter] = (double)value;
                    } else if (id == 228) {
                        sensorValues[US_FrontRight] = (double)value;
                    } else if (id == 230) {
                        sensorValues[US_RearRight] = (double)value;
                    }
                    
                }
                //cout << id << " " << value << endl;
                value = 0;
                neg = false;
                ++it;
            }
        }
        //cout << title << " " << data << endl;
    }

    string to_string(const int& value) {
        int temp_value = value;
        string temp_string = ""; 
        if (temp_value == 0) {
            return "0";
        }   
        while (temp_value != 0) {
            temp_string = ((char)((temp_value % 10) + '0')) + temp_string;
            temp_value = temp_value / 10; 
        }   
        return temp_string;
    }   


    ModuleState::MODULE_EXITCODE SerialKiller::body() {
        // Create SerialPort.
        core::wrapper::SerialPort *serialPort = core::wrapper::SerialPortFactory::createSerialPort("/dev/ttyACM0", 115200);

        core::wrapper::StringProtocol sp;
        core::wrapper::NetstringsProtocol np;
        np.setStringListener(this);
        np.setStringSender(serialPort);
        serialPort->setPartialStringReceiver(&np);

        serialPort->start();

    	while (getModuleState() == ModuleState::RUNNING) {
            cout << "SerialKiller: Do something..." << endl;

            {
                Container vehicleControlData = getKeyValueDataStore().get(Container::VEHICLECONTROL);
                VehicleControl vc = vehicleControlData.getData<VehicleControl>();

                double speed = vc.getSpeed();
                double heading = vc.getSteeringWheelAngle() * Constants::RAD2DEG;

                if(speed > 0)
                    speed = 1600;
                else if(speed < 0)
                    speed = 1300;
                else
                    speed = 1500;
                heading = heading + 45;
                if (heading > 90) {
                    heading = 90;
                } else if (heading < 0) {
                    heading = 0;
                }
                m_buffer = "[VA(" + to_string((int)speed) + ";" + to_string((int)heading)  + ")]";
                //0
                //90
                cout << "Sending" << m_buffer << endl;
                Lock l(m_bufferMutex);
                np.send(m_buffer);
                m_buffer = "";
            }
        }

        // Destroy connections to UDP_Server.
        OPENDAVINCI_CORE_DELETE_POINTER(serialPort);

    	return ModuleState::OKAY;
    }

} // examples
