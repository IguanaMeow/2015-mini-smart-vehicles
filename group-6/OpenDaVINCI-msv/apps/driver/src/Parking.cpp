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
#include <time.h>
#include <ctime>
#include <math.h>

#include "core/io/ContainerConference.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"

#include "GeneratedHeaders_Data.h"

#include "Driver.h"

#define HOLA 0
#define LETS_PARK 1
#define PARKING 2
#define ALMOST 3
#define DONE 4 

namespace msv {

        using namespace std;
        using namespace core::base;
        using namespace core::data;
        using namespace core::data::control;
        using namespace core::data::environment;

/*Calculate distance between obstacles*/
        int counter = -1;
        int parkingParallel; 
        int init;

        bool spot_found = false;
        bool lets_park = true;
        bool parking = true;
        bool almost = true;
        bool done = true;
       
        double distanceBetweenObjects;
        double distance;
        timeval curTime;
        timeval timer;

/*Sensors data*/
        double Infrared_FrontRight;
        double Infrared_RearRight;
        double Infrared_Rear;
        double UltraSonic_FrontCenter;       
        

        Driver::Driver(const int32_t &argc, char **argv) :
            ConferenceClientModule(argc, argv, "Driver") {
        }

        Driver::~Driver() {}

        void Driver::setUp() {
            parkingParallel = HOLA;
            init = 0;
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
                cerr << "Most recent vehicle data: '" << vd.getPosition() << "'" << endl;

                
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

                
                //Sensors aoutput
                // Infrared_FrontRight = sbd.getValueForKey_MapOfDistances(0);
                // cout << "Infrared_FrontRight distance:" << Infrared_FrontRight <<endl;
                Infrared_Rear = sbd.getValueForKey_MapOfDistances(1);
                cout << "Infrared_Rear distance: " << Infrared_Rear << endl;

                Infrared_RearRight = sbd.getValueForKey_MapOfDistances(2);
                cout << "Infrared_RearRight distance:" << Infrared_RearRight <<endl;

                UltraSonic_FrontCenter = sbd.getValueForKey_MapOfDistances(3);
                cout << "UltraSonic_FrontCenter distance: " << UltraSonic_FrontCenter << endl;

                //Setting up Timer //http://linux.die.net/man/2/gettimeofday
                gettimeofday(&curTime,NULL);
                distanceBetweenObjects = (curTime.tv_sec - timer.tv_sec) * 1000.0;
                distanceBetweenObjects += (curTime.tv_usec - timer.tv_usec)/1000.0;
                distanceBetweenObjects /=1000.0;

                // Design your control algorithm here depending on the input data from above.
                // Create vehicle control data.
                VehicleControl vc;
            
                double desiredSteeringWheelAngle = 0;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(0);
                // PARKING = get distance between the spaces 
                //Measure the distance between each space.
                switch(parkingParallel){

                    case HOLA:
                         vc.setSpeed(2);
                        if(init == 0){
                            gettimeofday(&timer, NULL);
                            init = 1;
                        }

                        if((sbd.getValueForKey_MapOfDistances(2) <= -1) && (distanceBetweenObjects * vc.getSpeed() < 8) && (spot_found == true)){
                            cerr << "Gap: ";
                            cerr << "Current distance: '" << distanceBetweenObjects * vc.getSpeed() << " cm' " << endl;
                        }else if(distanceBetweenObjects * vc.getSpeed() >= 8){
                            spot_found = true;
                            cerr << "Spot found..." << endl;
                            vc.setSpeed(0);
                            distance = distanceBetweenObjects * vc.getSpeed();
                            cerr << "Distance: '" << distanceBetweenObjects << " cm'" << endl;
                            lets_park = false;

                        }else {
                            gettimeofday(&timer, NULL);
                        }

                    case LETS_PARK: 
                        if((distanceBetweenObjects >= 8) && (distanceBetweenObjects < 15) && (lets_park == false)){
                            cerr << "Let's park...!";
                            vc.setSpeed(-1);
                            vc.setSteeringWheelAngle(19 * Constants::DEG2RAD);    
                           parking = false;
                        }

                    case PARKING:
                        if((distanceBetweenObjects >= 15) && (distanceBetweenObjects < 24.5) && (parking == false) && 
                        (sbd.getValueForKey_MapOfDistances(1) >= -1 || sbd.getValueForKey_MapOfDistances(1) > 1.9)){//&&   (distanceBetweenObjects >= 16) && (distanceBetweenObjects < 23)
                            cerr << "Parking...!";
                            cout << "Infrared_Rear distance: " << Infrared_Rear << endl;
                            vc.setSpeed(-0.3);
                            vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD);  
                            almost = false;  
                        }
                    
                    case ALMOST:
                        if((distanceBetweenObjects >= 24.5) && (distanceBetweenObjects < 33) && (almost == false) &&
                           (sbd.getValueForKey_MapOfDistances(3) >= -1 || sbd.getValueForKey_MapOfDistances(3) >= 3)){
                            cerr << "Almost...!";
                            cout << "UltraSonic_FrontCenter distance: " << UltraSonic_FrontCenter << endl;
                            vc.setSpeed(0.3);
                            vc.setSteeringWheelAngle(10 * Constants::DEG2RAD); 
                            almost = false;  
                        }

                    case DONE:
                        if((distanceBetweenObjects >= 33) && (almost == false)){
                            cerr << "Done!!!"; 
                        }

                        break;
                             
                }



                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
               // vc.setSpeed(0.5);

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                // double desiredSteeringWheelAngle = 5.5; // 4 degree but SteeringWheelAngle expects the angle in radians!
                // vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

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

                /* ID Sensors
                0 = Infrared_FrontRight
                1 = Infrared_Rear
                2 = Infrared_RearRight

                3 = UltraSonic_FrontCenter
                4 = UltraSonic_FrontRight
                5 = UltraSonic_RearRight
                */


