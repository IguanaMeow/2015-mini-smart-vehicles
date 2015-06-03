/**
 * driver - Sample application for calculating steering and acceleration commands.
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

#include "Driver.h"

namespace msv {

        using namespace std;
        using namespace core::base;
        using namespace core::data;
        using namespace core::data::control;
        using namespace core::data::environment;

        bool prevNoLane = true; // was a lane found last loop?

        int state = 0; // 0 = follow lane, 1-7 = overtaking phases

        double timer = 0, // set to time(0) to calculate a duration
               prevFrontRight = 0, // value for front right IR last loop
               speed = 2, // -2 to 2
               angle = 0; // -25 to 25

        Driver::Driver(const int32_t &argc, char **argv) :
          ConferenceClientModule(argc, argv, "Driver") {
        }

        Driver::~Driver() {}

        void Driver::setUp() {
          
        }

        void Driver::tearDown() {}

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {

          while (getModuleState() == ModuleState::RUNNING) {
                // In the following, you find example for the various data sources that are available:

            Container containerSBD = getKeyValueDataStore().get(Container::USER_DATA_0);
            SteeringData sd = getKeyValueDataStore().get(Container::USER_DATA_1).getData<SteeringData>();
            SensorBoardData sbd = containerSBD.getData<SensorBoardData>();
            VehicleControl vc;

            double sonarFront = sbd.getValueForKey_MapOfDistances(3);

            // nothing in front sonar? just follow the lane
            if(state == 0 && (sonarFront < 0.9 || sonarFront > 9)) {
                angle = sd.getExampleData();
                speed = sd.getSpeedData();

                vc.setSpeed(speed);
                vc.setSteeringWheelAngle(angle * Constants::DEG2RAD);

                Container c(Container::VEHICLECONTROL, vc);
                getConference().send(c);

                continue;
            }

            double sonarRight = sbd.getValueForKey_MapOfDistances(4),
                   irFrontRight = sbd.getValueForKey_MapOfDistances(0),
                   irBackRight = sbd.getValueForKey_MapOfDistances(2);

            double duration = time(0) - timer; // seconds till last timer update
            
            switch(state){

                case 0: // object found in front of car, initiate overtaking
                    state = 1;
                    angle = -20 + sd.getExampleData() * 0.5;
                    speed = 1;
                    timer = time(0);
                    break;

                case 1: // drive onto the overtaking lane
                    if(sonarRight < 0 || sonarRight > 20) break;
                    angle = 0;
                    speed = 2;
                    state = 2;
                    break;

                case 2: // counter-steer to position straight on lane
                    if(irBackRight < 0 || irBackRight > 15) break;
                    angle = 10;
                    timer = time(0);
                    state = 3;
                    break;

                case 3: // drive straight up the lane
                    if(duration < 2 && irFrontRight > 5) break;
                    angle = 0;
                    state = 4;
                    break;

                case 4: // straighten out the car using the IRs
                    speed = 2;
                    angle = 0;
                    timer = time(0);
                    if(prevFrontRight < irFrontRight) angle = 25;
                    else if(irFrontRight > irBackRight) angle = 12;
                    else state = 5;

                    if(irFrontRight < 0 || irFrontRight > 3) state = 5;

                    break;

                case 5: // make a right turn to overtake the object
                    angle = -1;
                    if(duration < 1 || (irFrontRight > -1 && irBackRight < 0) || (irFrontRight > -1 && irFrontRight < 25) || (sonarRight > 1 && sonarRight < 25)) break;

                    state = 6;
                    angle = 25;
                    timer = time(0);
                    break;

                case 6: // counter-steer to position straight onto right lane
                    if(duration < 4 && (duration < 1 || sd.getExampleData() < 13 || prevNoLane)) break;
                    angle = -25;
                    state = 7;
                    timer = time(0);
                    break;
    
                case 7: // switch back to the "lane following" state
                    if(duration < 3 && (duration < 1.2 || sd.getExampleData() < 1 || prevNoLane)) break;
                    state = 0;
                    break;
            }

            prevFrontRight = irFrontRight;
            prevNoLane = sd.getExampleData() < 1 && sd.getExampleData() > -1;

            vc.setSteeringWheelAngle(angle * Constants::DEG2RAD);
            vc.setSpeed(speed);

            Container d(Container::VEHICLECONTROL, vc);
            getConference().send(d);
          }

          return ModuleState::OKAY;
        }
} // msv
//                      ID Sensors
// //                 0 = Infrared_FrontRight
// //                 1 = Infrared_Rear
// //                 2 = Infrared_RearRight

// //                 3 = UltraSonic_FrontCenter
// //                 4 = UltraSonic_FrontRight
// //                 5 = UltraSonic_RearRight
// //                  
