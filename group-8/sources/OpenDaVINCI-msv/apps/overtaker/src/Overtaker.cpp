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
        int turnOut = 2;
        int turnBack = 3;
        int state = followLane;
        int count = 0;

        while (getModuleState() == ModuleState::RUNNING) {
        // In the following, you find example for the various data sources that are available:

        // 1. Get most recent vehicle data:
        Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
        VehicleData vd = containerVehicleData.getData<VehicleData> ();
        cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

        // 2. Get most recent sensor board data:
        Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
        cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

        // 3. Get most recent user button data:
        Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
        UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
        cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

        // 4. Get most recent steering data as fill from lanedetector for example:
        Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
        SteeringData sd = containerSteeringData.getData<SteeringData> ();
        cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

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
        cerr << "follow lane" << endl;
                if(sbd.getValueForKey_MapOfDistances(3) < 5 && sbd.getValueForKey_MapOfDistances(3) > 0){
                        state = turnOut;
                }
                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                //double desiredSteeringWheelAngle = 4; // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(sd.getHeadingData());
                break;
        case 2 :
        cerr << "Turn out" << endl;
                if(sbd.getValueForKey_MapOfDistances(0) > 0 && sbd.getValueForKey_MapOfDistances(3) < 0)
                {
                        state = turnBack;
                        break;
                }
                vc.setSteeringWheelAngle(-26);
                break;
        case 3 :
        cerr << "turnBack" << endl;
                if(sbd.getValueForKey_MapOfDistances(0) < 0 && sbd.getValueForKey_MapOfDistances(2) < 0 && sbd.getValueForKey_MapOfDistances(4) < 0 && sbd.getValueForKey_MapOfDistances(3) < 0)
                {
                        vc.setSteeringWheelAngle(25);

                        if(count > 20)
                        {
                                state = followLane;
                                count = 0;
                                break;   
                        }

                        count++;
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

