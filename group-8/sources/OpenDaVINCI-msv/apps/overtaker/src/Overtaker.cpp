/**
 * Overtaker - Sample application for calculating steering and acceleration commands.
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
#include <math.h>

#include "core/io/ContainerConference.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"
#include "core/base/KeyValueConfiguration.h"

#include "GeneratedHeaders_Data.h"

#include "Overtaker.h"

namespace msv {

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::data::control;
using namespace core::data::environment;

Overtaker::Overtaker(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "Overtaker"),
        m_followLane(1),
        m_turnOut(2),
        m_straighten(3),
        m_turnBack(4),
        m_counter(0),
        m_steering(19),
        m_sensor(0),
        m_speed(1.5)
        {}

Overtaker::~Overtaker() {}

void Overtaker::setUp() {
        // This method will be call automatically _before_ running body().
}

void Overtaker::tearDown() {
        // This method will be call automatically _after_ return from body().
}

// This method will do the main data processing job.
ModuleState::MODULE_EXITCODE Overtaker::body()   

{

KeyValueConfiguration kv = getKeyValueConfiguration();
m_carLength = kv.getValue<double> ("global.carLength");
int m_state = m_followLane;

while (getModuleState() == ModuleState::RUNNING) {

// Setting up different containers and getting data from them.
Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
SteeringData sd = containerSteeringData.getData<SteeringData> ();
VehicleControl vc;

vc.setSpeed(m_speed);

// if ultrasonic front exist.
if(sbd.containsKey_MapOfDistances(3))
{

    switch(m_state)
    {
    case 1 : 
    cerr << "follow lane" << endl;
        if(sbd.getValueForKey_MapOfDistances(3) < (2 * m_carLength) && sbd.getValueForKey_MapOfDistances(3) > 0)
        {
            
            m_state = m_turnOut;
        }
        vc.setSteeringWheelAngle(sd.getHeadingData());
        cout << "Value from Ultrasonic Front" << sbd.getValueForKey_MapOfDistances(3) << endl;
        break;
    case 2 :
    // Turn out to left lane until InfraRed sensor detects the object, Keep incrementing counter to know how much to turn back later.
    cerr << "Turn out to left lane" << endl;
        if(sbd.getValueForKey_MapOfDistances(m_sensor) > 0)
        {
            m_state = m_straighten;
            break;
        }
        ++m_counter;
        std::cout << "counter " << m_counter << std::endl;
        cout << "Value from Front InfraRed" << sbd.getValueForKey_MapOfDistances(m_sensor) << endl;
        vc.setSteeringWheelAngle(-m_steering * Constants::DEG2RAD);
        break;

    case 3 : 
    // Turn car fully until Infrared front detects the object is close enough, This is done to align car to the object.
        m_steering = 25;
        if(sbd.getValueForKey_MapOfDistances(0) < 0 || sbd.getValueForKey_MapOfDistances(0) > 1.5)
        {
            std::cout << "turning straight " << std::endl;
            cout << "Value from Front InfraRed" << sbd.getValueForKey_MapOfDistances(0) << endl;
            vc.setSteeringWheelAngle(m_steering * Constants::DEG2RAD);
            break;
        }
        else 
        {
            m_state = m_turnBack;
            break;
        }
    break;
    case 4 :
    // If Front InfraRed and Front Right Ultrasonic is not detecting an object turn back half of counter. Go back to lanefollowing.
    cerr << "turn back to Lane" << endl;
        if((sbd.getValueForKey_MapOfDistances(4) > (1.25 * m_carLength) || sbd.getValueForKey_MapOfDistances(4) < 0) && sbd.getValueForKey_MapOfDistances(0) < 0)
        {
            if(m_counter >= 0)
            {
               std::cout << "counter out " << m_counter << std::endl;
                vc.setSteeringWheelAngle(m_steering * Constants::DEG2RAD);
                m_counter -= 2;
                break;
            }
            else 
            {
                m_counter = 0;
                m_steering = 20;
                m_state = m_followLane;
                break;   
            }
            break;
        }
        vc.setSteeringWheelAngle(sd.getHeadingData());
        cout << "Value from Ultrasonic Front Right" << sbd.getValueForKey_MapOfDistances(3) << endl;
        cout << "Value from Front InfraRed" << sbd.getValueForKey_MapOfDistances(0) << endl;
        break;
    }
}

// Create container for finally sending the data.
Container c(Container::VEHICLECONTROL, vc);
// Send container.
getConference().send(c);
}

return ModuleState::OKAY;
}
} // msv

