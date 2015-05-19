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
                double front_us, fr_ir, rr_ir, fr_us, rear_ir;
                
                bool intersect = false;
                int intersectionState, overtakingState, parkingState, demoState = 0;
                bool obstacleFound = false;
                bool inSimulator = false;
                double speed = 1;
                bool doLaneFollowing = true;
                bool canFollowLane = false;
                double desiredSteeringWheelAngle = 0;
                double laneFollowAngle = 0;
                int distToObject = 7;
                VehicleControl vc;
                string state = "start";
                int rightTurnSteerAngle = 0;
                int menuChoice = 0;
                const double car_length = 5;
                double distance_1, distance_2, distance_3;
                int intersectionCounter = 0;
                int leftCounter = 0; 
                int delay = 0;
                double turnSpeed = 0;
             
                // menu 
                cout << "Enter 1 if using simulator, any key for real car." << endl;
                cin >> menuChoice;
                if(menuChoice == 1) inSimulator = true;

                cout << "Enter 1 for parking mode, 2 for hardware demo or any key for normal mode." << endl;
                cin >> menuChoice;
                
            

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
                cerr << (canFollowLane ? "canFollowLane = true" : "canFollowLane = false") << endl;
                

                laneFollowAngle = (sd.getExampleData() * Constants::RAD2DEG);
                cerr << "laneFollowAngle = " << laneFollowAngle << endl;
                cerr << "rightTurnSteerAngle = " << rightTurnSteerAngle << endl;
                cerr << "leftCounter = " << leftCounter << endl;
               
                cerr << "IR front = " << fr_ir << endl;
                cerr << "IR rear = " << rr_ir << endl;
                cerr << "Delay = " << delay << endl;
            
                fr_ir = sbd.getValueForKey_MapOfDistances(0);
                rear_ir = sbd.getValueForKey_MapOfDistances(1);
                rr_ir = sbd.getValueForKey_MapOfDistances(2);
                front_us = sbd.getValueForKey_MapOfDistances(3);
                fr_us = sbd.getValueForKey_MapOfDistances(4);
                
                
                // Design your control algorithm here depending on the input data from above.

                /* Normal driving mode, car handles overtaking and intersections */
                if(menuChoice != 1  && menuChoice != 2){

                /* changes steering angle used when overtaking based on the current angle */
              
                if((laneFollowAngle < -9 || laneFollowAngle > 9) && !obstacleFound){
                    distToObject = 8;
                    rightTurnSteerAngle = 21; 
                    turnSpeed = 0.5;
                }
                if((laneFollowAngle > -9 && laneFollowAngle < 9) && !obstacleFound){
                    distToObject = 6;
                    rightTurnSteerAngle = 23; 
                    turnSpeed = 0.7;
                }


                /******************** intersection control ************************/

                switch(intersectionState){ 

                
                    case 0: // if intersection detected disable lane following and stop car      
                        state = "normal";
                        if(intersect && !obstacleFound){   
                            doLaneFollowing = false;  
                            speed = 0;
                            intersectionState = 1;                
                        }
                        break;

                    case 1: //at intersection, check if clear
                        if(intersectionCounter < 50){
                            intersectionCounter ++;
                            state = "at intersection"; 
                        }
                        else if((front_us < 0 || front_us > 10) && (fr_us < 0 || fr_us > 20)) intersectionState = 2;                
                        break;
                    case 2: //drive through intersection
                        state = "drive through intersection";
                        speed = 1;          
                        desiredSteeringWheelAngle = 0;
                        if(canFollowLane){ 
                            intersectionCounter = 0; 
                            intersectionState = 0;    
                            doLaneFollowing = true; 
                        }
                        break;
                }
                /********************** Overtaking control ************************/
                switch(overtakingState){
                    case 0: // hard left
                    if(!obstacleFound && front_us > -1 && front_us < distToObject && doLaneFollowing){  
                       
                        desiredSteeringWheelAngle = -25;   
                        obstacleFound = true; 
                        doLaneFollowing = false; 
                        speed = 0.6;  
                        state = "to LHLane"; 
                        overtakingState = 1;                                             
                    }
                    break;
                    case 1: // right turn when corner seen
                    if(fr_ir > -1 && fr_ir <= 3){
                        state = "seen by front IR"; 
                        overtakingState = 2;                     
                    }
                    break;
                    case 2:
                    state = "waiting for rear";
                    if(rr_ir > -1 && rr_ir <= 3){
                        state = "seen by rear IR"; 
                        overtakingState = 3; 
                        desiredSteeringWheelAngle = 25;
                    }
                    break;
                    case 3: // follow left lane
                    leftCounter ++;
                    delay ++;
                    if(delay >= 60){
                        if(rr_ir >= (fr_ir - 0.1) && rr_ir <= (fr_ir + 0.1) ){
                            desiredSteeringWheelAngle = 0;
                            state = "follow LHLane";
                            overtakingState = 4; 
                        }
                    }
                    break;
                    case 4:
                    if(fr_us < 0) overtakingState = 5;
                    break;
                    case 5: // turn right when front right US is clear
                        delay = 0;
                        leftCounter --;
                        speed = turnSpeed;
                        desiredSteeringWheelAngle = rightTurnSteerAngle;
                        state = "to RHLane"; 
                        if(leftCounter == 0){ 
                        overtakingState = 6; 
                    } 
                    break;
                    case 6: // turn left and follow right lane 
                        desiredSteeringWheelAngle = -25;
                        if(laneFollowAngle >= - 2 && laneFollowAngle <= 2 && canFollowLane){ 
                            doLaneFollowing = true;
                            obstacleFound = false;
                            state = "in RHLane";  
                            overtakingState = 0;  
                        }      
                    
                }      
            }                
        /****************************** parking control *****************************/
                if(menuChoice == 1){
                    switch(parkingState){ // using switch-case to change state
                            case 0: // find space for parking and right place to park (found the obstacle)
                               //get speed data and wheelangle from the lane detector through container
                                speed = spd.getSpeedData();
                                //make sure that it is not curve, before going to the next state.
                                state = "parking: state 0 - drive normal";
                                if((fr_us < 0 || fr_us > (car_length * 1.6)) && laneFollowAngle <= 0.02 && laneFollowAngle >= 0) parkingState = 1;
                                break;
                            case 1:
                                state = "parking: state 1 - finding enough gap";
                                laneFollowAngle = false;
                                if((fr_us <0 || fr_us > (car_length * 0.8)) && fr_ir < 0 && rr_ir < 0){

                                    speed = 0;
                                    desiredSteeringWheelAngle = 0;
                                    parkingState = 2; //state gap enough
                                    distance_1 = vd.getAbsTraveledPath();
                                }
                                if(fr_us > 0 && fr_us < (car_length * 0.8))
                                {
                                    parkingState = 0;
                                } 
                                break;

                            case 2: //gap enough sate, then drive more around 2 times of the car length to find appropriate distance to park to start parking state

                                if(vd.getAbsTraveledPath() < distance_1 + (car_length * 1.9)){
                                    speed = 1;
                                    desiredSteeringWheelAngle = 0;
                                }
                                
                                state = "parking: state 2";
                                if(vd.getAbsTraveledPath() >= distance_1 + (car_length * 1.9)){
                                    parkingState = 3;
                                    distance_2 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 3: // start parking state, drive backward.
                                speed = -0.6;
                                desiredSteeringWheelAngle = 15.5;
                                state = "parking: state 3";
                                if(vd.getAbsTraveledPath() >= distance_2 + (car_length * 1.6)){
                                    parkingState = 4;
                                    distance_3 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 4: // parking state 3 (The car detect behind object or drive more 1.5 times of the car length)
                                speed = -0.5;
                                desiredSteeringWheelAngle = -26;
                                //if it doesn't detect any car behind after drive 10 meters more.
                                if(vd.getAbsTraveledPath() >= distance_3 + (car_length * 0.82) && rear_ir < 0)
                                {
                                    state = "parking: state 4 to 7 - don't find obstacle behind";
                                    parkingState = 7;
                                }
                                if(rear_ir <= (car_length * 0.39) && rear_ir > 0){
                                    state = "parking: state 4 - find an obstacle behind";
                                    speed = 0;
                                    desiredSteeringWheelAngle = 0;
                                    parkingState = 5;
                                }
                                break;

                            case 5:// move 1 to make the car park right
                                speed = 0.5;
                                desiredSteeringWheelAngle = 25;
                                state = "parking state 5";
                                //if IF_Rear doesn't detect any object Or fr_us not detect any object
                             if(rear_ir < 0 ||(front_us < (car_length * 0.3) && front_us > 0)){
                                    
                                    state = "parking state 5 to 6";
                                    parkingState = 6;
                                }
                                break;

                            case 6: // finding the appropriated position of the car
                                speed = -0.4;
                                desiredSteeringWheelAngle = -26;
                                state = "parking state 6";
                                if(rear_ir <= (car_length * 0.39) && rear_ir > 0) parkingState = 7;
                                break;

                            case 7:
                                 state = "parking state 7";
                                speed = 0;
                                desiredSteeringWheelAngle = 0;
                                break;
                        }
                } 
         /****************************** proxy / hardware deḿo code ******************************/
                if(menuChoice == 2){
                    doLaneFollowing = false;
                    switch(demoState){
                        case 0:
                        speed = 1;
                        desiredSteeringWheelAngle = 0;
                        delay ++;
                        if(delay >= 100){
                            demoState = 1;
                            delay = 0;
                        }
                        break;
                        case 1:
                        speed = 0;
                        delay ++;
                        if(delay >= 100){
                            demoState = 2;
                            delay = 0;
                        }
                        break;
                        case 2:
                        speed = -1;
                        delay ++;
                        if(delay >= 100){
                            demoState = 3;
                            delay = 0;
                        }
                        break;
                        case 3:
                        speed = 1;
                        desiredSteeringWheelAngle = -25;
                        delay ++;
                        if(delay >= 100){
                            demoState = 4;
                            delay = 0;
                        }
                        break;
                        case 4:
                        desiredSteeringWheelAngle = 25;
                        delay ++;
                        if(delay >= 100){
                            speed = 0;
                            desiredSteeringWheelAngle = 0;
                            delay = 0;
                            demoState = 0;
                        }
                        break;
                    }
                }
        /************ end of demo ************/
                
               if(doLaneFollowing && canFollowLane){
                    desiredSteeringWheelAngle = laneFollowAngle;
                    speed = 0.6;
                    state = "normal driving";
               } 
               if(!inSimulator){
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                   if (laneFollowAngle < 0 ) {      
                    desiredSteeringWheelAngle = laneFollowAngle -3;     
                  }     
                  else if (laneFollowAngle > 0 ) {      
                    desiredSteeringWheelAngle = laneFollowAngle +3;     
                  }
                }else{
                    
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle);
                }
                
                vc.setSpeed(speed);
               
                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
            }
            return ModuleState::OKAY;
        }
} // msv


