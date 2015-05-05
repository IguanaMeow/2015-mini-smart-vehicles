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

        Driver::Driver(const int32_t &argc, char **argv) :
            ConferenceClientModule(argc, argv, "Driver") {
        }

        Driver::~Driver() {}

        void Driver::setUp() {
            // This method will be call automatically _before_ running body().
        }

        void Driver::tearDown() {
            // This method will be call automatically _after_ return from body().
        }

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {
                int front_us, fr_ir, rr_ir, fr_us, rear_ir;
                bool intersect = false;
                int intersectionState = 0;
                bool obstacleFound = false;
                double speed = 0.8;
                bool laneFollow = true;
                bool canFollowLane = false;
                double desiredSteeringWheelAngle = 0;
                double laneFollowAngle = 0;
                int distToObject = 7;
                VehicleControl vc;
                string state = "start";
                int rightTurnSteerAngle = 0;
                int menuChoice = 0;
                int pMode = 0;
                int otMode = 0;
                const double car_length = 5;
                double distance_1, distance_2, distance_3;
                
                // menu here, needs improving!
                
                cout << "Enter 1 for parking mode, any key for normal mode." << endl;
                cin >> menuChoice;
                cout << "Choice: " << menuChoice << endl;
            

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
                cerr << "Current state '" << state << "'" << endl;
                 Container containerSpeedData = getKeyValueDataStore().get(Container::USER_DATA_2);
                SpeedData spd = containerSpeedData.getData<SpeedData>();

                Container containerLaneFollowing = getKeyValueDataStore().get(Container::USER_DATA_5);
                LaneDetected ld = containerLaneFollowing.getData<LaneDetected>();

                 Container containerIntersection = getKeyValueDataStore().get(Container::USER_DATA_6);
                Intersection id = containerIntersection.getData<Intersection> ();
                intersect = id.getIntersection();

                canFollowLane = ld.getLaneDetected();
                canFollowLane ? cerr << "canFollowLane = true" << endl : cerr << "canFollowLane = false" << endl;
                

                laneFollowAngle = (sd.getExampleData() * Constants::RAD2DEG);
                cerr << "laneFollowAngle = " << laneFollowAngle << endl;
                cerr << "rightTurnSteerAngle = " << rightTurnSteerAngle << endl;
                rear_ir = sbd.getValueForKey_MapOfDistances(1);
                front_us = sbd.getValueForKey_MapOfDistances(3);
                fr_us = sbd.getValueForKey_MapOfDistances(4);
                fr_ir = sbd.getValueForKey_MapOfDistances(0);
                rr_ir = sbd.getValueForKey_MapOfDistances(2);
                // Design your control algorithm here depending on the input data from above.
                if(menuChoice != 1){
                if((laneFollowAngle < -2 || laneFollowAngle > 2) && !obstacleFound){
                    distToObject = 8;
                    rightTurnSteerAngle = 15; 
                }
                if((laneFollowAngle > -1 && laneFollowAngle < 1) && !obstacleFound){
                    distToObject = 7;
                    rightTurnSteerAngle = 23; 
                }

                // intersection control

                switch(intersectionState){ 
                    case 0:         //normal lane following
                        cout << "state 0: normal" << endl;
                        if(intersect == true){   //check for intersection
                            laneFollow = false;
                            speed = 0;
                            intersectionState = 1; //at intersection state
                        }
                        break;

                    case 1: //at intersection, check if clear
                        
                        sleep(5);
                        cout << "state 1: at intersection" << endl;
                        
                        if(front_us <0 && fr_us <0){   //nothing detected in intersection
                            intersectionState = 2;                //move through intersection state
                        } else { 
                            sleep(5);               //wait, then go through. 
                            intersectionState = 2;              //else will stop forever if non-moving obstacle
                        }
                        break;

                    case 2:
                        cout << "state 2: drive through intersection" << endl;
                        speed = 1;          //drive through intersection
                        desiredSteeringWheelAngle = 0;

                        if(canFollowLane == true){    //lane is detected
                            intersectionState = 0;    // startlane following state
                            laneFollow = true;
                        }
                        break;
                }
                // Create vehicle control data.
                switch(otMode){
                    case 0:
                    if(!obstacleFound && front_us > -1 && front_us < distToObject && laneFollow){  
                    desiredSteeringWheelAngle = -25;   
                    obstacleFound = true; 
                    laneFollow = false; 
                    speed = 0.4;  
                    state = "toLHLane"; 
                    otMode = 1;                                               
                }
                break;
                    case 1:
                     if(fr_ir > -1 && fr_ir < 3){
                    desiredSteeringWheelAngle = rightTurnSteerAngle;
                    speed = 1;
                    state = "inLHLane"; 
                    otMode = 2;                  
                }
                break;
                     case 2:
                    if(rr_ir < 3 && rr_ir > 0){
                        desiredSteeringWheelAngle = 0;
                        state = "follow LHLane";
                        otMode = 3; 
               }
               break;
                    case 3:
                    if(fr_ir < 0){
                    desiredSteeringWheelAngle = rightTurnSteerAngle;
                    state = "toRHLane";  
                    otMode = 4;               
                }
                break;
                    case 4:
                      if(rr_ir < 0){
                    desiredSteeringWheelAngle = -25;  
                    laneFollow = true;
                    obstacleFound = false;
                    state = "inRHLane";  
                    otMode = 0;                
                }

                }
                }                

                if(menuChoice == 1){
                    switch(pMode){ // using switch-case to change state
                            case 0: // find space for parking and right place to park (found the obstacle)
                               //get speed data and wheelangle from the lane detector through container
                                speed = spd.getSpeedData();
                                //make sure that it is not curve, before going to the next state.
                                if((fr_us < 0 || fr_us > (car_length * 1.6)) && laneFollowAngle <= 0.02 && laneFollowAngle >= 0) pMode = 1; 
                                break;
                            case 1:
                                laneFollow = false;
                                if((fr_us <0 || fr_us > (car_length * 1.1)) && fr_ir < 0 && rr_ir < 0){
                                    pMode = 2; //state gap enough
                                    distance_1 = vd.getAbsTraveledPath();
                                }
                                if(fr_us > 0 && fr_us < (car_length * 1.1)) pMode = 0;
                                break;
                            
                            case 2: //gap enough sate, then drive more around 2 times of the car length to find appropriate distance to park to start parking state
                                speed = 1;
                                desiredSteeringWheelAngle = 0;
                                
                                if(vd.getAbsTraveledPath() >= distance_1 + (car_length * 2.096)){
                                    pMode = 3;
                                    distance_2 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 3: // start parking state, drive backward.
                                speed = -1.5;
                                desiredSteeringWheelAngle = 16;
                                if(vd.getAbsTraveledPath() >= distance_2 + (car_length * 1.5)){
                                    pMode = 4;
                                    distance_3 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 4: // parking state 3 (The car detect behind object or drive more 1.5 times of the car length)
                                speed = -1;
                                desiredSteeringWheelAngle = -26;
                                //if it doesn't detect any car behind after drive 10 meters more.
                                if(vd.getAbsTraveledPath() >= distance_3 + (car_length * 1.5) && rear_ir < 0) pMode = 7;
                                if(rear_ir <= 2.6 && rear_ir > 0){
                                    speed = 0;
                                    desiredSteeringWheelAngle = 0;
                                    pMode = 5;
                                } 
                                break;

                            case 5:// move 1 to make the car park right
                                speed = 1;
                                desiredSteeringWheelAngle = 25;
                                //if IF_Rear doesn't detect any object Or fr_usont detect any object
                             if(rear_ir < 0 ||(fr_us < (car_length * 0.44) && fr_us > 0)){
                                    speed = 0;
                                    desiredSteeringWheelAngle = 0;
                                    pMode = 6;
                                }
                                break;

                            case 6: // finding the appropriated position of the car
                                speed = -0.4;
                                desiredSteeringWheelAngle = -26;                   
                                if(rear_ir <= (car_length * 0.48) && rear_ir > 0) pMode = 7;                         
                                break;

                            case 7:
                                speed = 0;
                                desiredSteeringWheelAngle = 0;
                                break;                           
                        }
                } 
                
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                

             
               if(laneFollow && canFollowLane){
                    desiredSteeringWheelAngle = laneFollowAngle;
                   // speed = spd.getSpeedData();
                    state = "normal driving";
               } 
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(speed);

                // You can also turn on or off various lights:
                vc.setBrakeLights(false);
                vc.setLeftFlashingLights(false);
                vc.setRightFlashingLights(false);

                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
            }
            return ModuleState::OKAY;
        }
} // msv

