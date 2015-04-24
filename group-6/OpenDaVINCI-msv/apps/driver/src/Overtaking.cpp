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

        /*Overtaking variables*/
        int counter;
        bool obstacleFound;
        bool overtakingLeft;
        bool passObstacle;

        /*Sensors*/
        int UltraSonic_FrontCenter;
        double Infrared_RearRight;

        Driver::Driver(const int32_t &argc, char **argv) :
          ConferenceClientModule(argc, argv, "Driver") {
        }

        Driver::~Driver() {}

        void Driver::setUp() {
            counter = 0;
            obstacleFound = false;
            overtakingLeft = false;
            passObstacle = false;

          // This method will be call automatically _before_ running body().
        }

        void Driver::tearDown() {
          // This method will be call automatically _after_ return from body().
        }

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {

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

                // Design your control algorithm here depending on the input data from above.
                // Sensors output
                UltraSonic_FrontCenter = sbd.getValueForKey_MapOfDistances(3);
                cerr << "UltraSonic_FrontCenter distance: '" << UltraSonic_FrontCenter << "m'" << endl;

                Infrared_RearRight = sbd.getValueForKey_MapOfDistances(2);
                // Create vehicle control data.
                VehicleControl vc;

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                vc.setSpeed(2);

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                double desiredSteeringWheelAngle = 0; // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

                // First try: Obstacle dected by UltraSonic_FrontCenter around 10mts, car stops
                if(UltraSonic_FrontCenter < 9){
                    obstacleFound = true;
                    vc.setSpeed(0); // Test 1: car stops at 10 mts far away from the obstacle
                    if(UltraSonic_FrontCenter <= 10){
                        overtakingLeft = true;
                        // vc.setSteeringWheelAngle(-15 * Constants::DEG2RAD);
                        vc.setSpeed(0.5);
                    }else{
                        overtakingLeft = false;
                        vc.setSpeed(0);
                    }
                }
                
                if(obstacleFound){
                    // cerr << "counter: " << counter << endl;
                    cerr << "Infrared_RearRight: '" << Infrared_RearRight << " '" <<  endl;

                    if((counter <= 90) && (overtakingLeft == true)){ //Turn left
                        vc.setSteeringWheelAngle(-15 * Constants::DEG2RAD);
                    }else if((counter <= 120) && (counter <= 180) && (overtakingLeft == true)){ //turn right
                        vc.setSteeringWheelAngle(25 * Constants::DEG2RAD);
                        passObstacle = true;
                    }else if((counter <= 180) && (counter <= 250) && (passObstacle == true)){ //gets the correct angle to be parallel to the object
                        vc.setSteeringWheelAngle(20 * Constants::DEG2RAD);
                    }else if((counter <= 250) && (counter <= 320) && (passObstacle == true)){ // keeps straight to overtake the obstacle
                        vc.setSteeringWheelAngle(0 * Constants::DEG2RAD);
                    }else if(passObstacle && (sbd.getValueForKey_MapOfDistances(2) <= -1)){
                        vc.setSpeed(0);
                    }

                    counter++;
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

//                 /* ID Sensors
//                 0 = Infrared_FrontRight
//                 1 = Infrared_Rear
//                 2 = Infrared_RearRight

//                 3 = UltraSonic_FrontCenter
//                 4 = UltraSonic_FrontRight
//                 5 = UltraSonic_RearRight
//                 */
