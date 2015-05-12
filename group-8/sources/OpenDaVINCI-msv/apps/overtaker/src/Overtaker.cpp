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

#include "GeneratedHeaders_Data.h"

#include "Overtaker.h"

namespace msv {

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::data::control;
using namespace core::data::environment;

Overtaker::Overtaker(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "Overtaker") {
}

Overtaker::~Overtaker() {}

void Overtaker::setUp() {
        // This method will be call automatically _before_ running body().
}

void Overtaker::tearDown() {
        // This method will be call automatically _after_ return from body().
}

// This method will do the main data processing job.
ModuleState::MODULE_EXITCODE Overtaker::body() {
        int followLane = 1;
        int turnOutStra = 2;
        int turnBackStra = 3;
        int curve = 0;
        int state = followLane;
        int const carLength = 4;
        int counter = 0;
        double steering = 25;

        while (getModuleState() == ModuleState::RUNNING) {
        // In the following, you find example for the various data sources that are available:

        // 1. Get most recent vehicle data:
        Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
        VehicleData vd = containerVehicleData.getData<VehicleData> ();
        //cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

        Container containerLaneData = getKeyValueDataStore().get(Container::USER_DATA_3);
        LaneData ld = containerLaneData.getData<LaneData> ();
       // cerr << "Most recent Lane data: '" << ld.toString() << "'" << endl;

        // 2. Get most recent sensor board data:
        Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
        //cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

        // 3. Get most recent user button data:
        Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
        UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
        //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

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
        vc.setSpeed(sd.getSpeedData());

        if(sbd.containsKey_MapOfDistances(3))
        {

        switch(state){
        case 1 : 
        //cerr << "follow lane" << endl;
                if(sbd.getValueForKey_MapOfDistances(3) < (1.5 * carLength) && sbd.getValueForKey_MapOfDistances(3) > 0){

                    cerr << "heading = " << sd.getHeadingData() << endl;
                    
                    if(sd.getHeadingData() > 0.10)
                    {
                        curve = 2;
                    }
                    else if(sd.getHeadingData() < -0.10)
                    {
                        curve = 3;
                    }
                        state = turnOutStra;
                }
                vc.setSteeringWheelAngle(sd.getHeadingData());
                break;
        case 2 :
       // cerr << "Turn out straight" << endl;
                if(sbd.getValueForKey_MapOfDistances(0) > 0 )
                {
                    if(curve > 2)
                    {
                        counter = counter/2;
                    }
                    else if(curve > 1)
                    {
                        counter = counter + (counter/2);
                    }
                        state = turnBackStra;
                        curve = 0;
                        break;
                }
                ++counter;
                //std::cout << "counter " << counter << std::endl;
                vc.setSteeringWheelAngle(-steering * Constants::DEG2RAD);
                break;
        case 3 :
      // cerr << "turnBack straight" << endl;
               // std::cout << "distance " << sbd.getValueForKey_MapOfDistances(4) << std::endl;
                if(sbd.getValueForKey_MapOfDistances(4) > 5 || sbd.getValueForKey_MapOfDistances(4) < 0)
                {
                        if(counter > 0)
                        {
                               // std::cout << "counter out " << counter << std::endl;
                                vc.setSteeringWheelAngle(steering);
                                --counter;
                                break;
                        }
                        else 
                        {
                                counter = 0;
                                state = followLane;
                                break;   
                        }
                        break;
                }
                vc.setSteeringWheelAngle(sd.getHeadingData());
                break;
        }
        }

        // You can also turn on or off various lights:
        vc.setBrakeLights(false);
        vc.setLeftFlashingLights(false);
        vc.setRightFlashingLights(true);

        // Create container for finally sending the data.
        Container c(Container::VEHICLECONTROL, vc);
        // Send container.
        getConference().send(c);
                }

        return ModuleState::OKAY;
        }
} // msv

