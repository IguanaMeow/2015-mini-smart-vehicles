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
        m_straCounter(0),
        m_sensor(0),
        m_speed(3)

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
    // In the following, you find example for the various data sources that are available:

    Container containerLaneData = getKeyValueDataStore().get(Container::USER_DATA_3);
    LaneData ld = containerLaneData.getData<LaneData> ();
   // cerr << "Most recent Lane data: '" << ld.toString() << "'" << endl;

    // 2. Get most recent sensor board data:
    Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
    SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
    //cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

    // 4. Get most recent steering data as fill from lanedetector for example:
    Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
    SteeringData sd = containerSteeringData.getData<SteeringData> ();
    //cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

    //FrontCenter = 3
    //IR front right = 0
    //IR rear right = 2
    //FrontRight = 4

    // Create vehicle control data.
    VehicleControl vc;

    // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
    vc.setSpeed(m_speed);

    if(sbd.containsKey_MapOfDistances(3))
    {

    switch(m_state){
    case 1 : 
        cerr << "follow lane" << endl;
            if(sbd.getValueForKey_MapOfDistances(3) < (2 * m_carLength) && sbd.getValueForKey_MapOfDistances(3) > 0)
            {

                cerr << "heading = " << sd.getHeadingData() << endl;
                
                m_state = m_turnOut;
            }
            vc.setSteeringWheelAngle(sd.getHeadingData());
            break;
    case 2 :
        cerr << "Turn out to left lane" << endl;
            if(sbd.getValueForKey_MapOfDistances(m_sensor) > 0)
            {
                m_state = m_straighten;
                m_straCounter = m_counter / 2;
                break;
            }
            ++m_counter;
            std::cout << "counter " << m_counter << std::endl;
            vc.setSteeringWheelAngle(-m_steering * Constants::DEG2RAD);
            break;

    case 3 : 
            m_steering = 25;
            
            // cout << "straight counter " << m_straCounter << endl;

            // if(m_straCounter > 0)
            // {
            //     std::cout << "turning straight " << std::endl;
            //     vc.setSteeringWheelAngle(m_steering * Constants::DEG2RAD);
            //     --m_straCounter;
            //     break;
            // }
            if(sbd.getValueForKey_MapOfDistances(0) < 0 || sbd.getValueForKey_MapOfDistances(0) > 1.5)
            {
                std::cout << "turning straight " << std::endl;
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
        cerr << "turn back to Lane" << endl;
        m_steering = 17;
           // std::cout << "distance " << sbd.getValueForKey_MapOfDistances(4) << std::endl;
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

