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
            
           
            int wheel_angle; // in degrees 
            double frontRightSonic;
            double frontCenter;
            double infraRear;
            double infraRightRear;
            double infraRightFront;
            bool normalDriving = true;
            bool intersectionFound = false;
            bool obstacle = false;
            bool overtake = false;
            double steerAngle = 0;
            bool inLHLane = false;
            bool backToRHLane = false; 
            int distToObstacle = 0;
            int rightTurn = 0; 
            bool inRHLane = false;
            bool waiting = false;
            int stage;
            int counter = 0;
            int delay = 0;
            string laneType = "";
            bool counting = false;
            int timer = 0;
            int selection;
            string mode = " ";
          
          cout << "Choose your driving mode:" << endl;
          cout << "=========================" << endl;
          cout << "Key 1 for Parking" << endl;
          cout << "Any key for normal Driving" << endl;
          cout << "=========================" << endl;
          cin >> selection;
          cout << endl;
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
               /* Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
                UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
                cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;*/
                // 4. Get most recent steering data as fill from lanedetector for example:
                Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                SteeringData sd = containerSteeringData.getData<SteeringData> ();
                cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;
                
                cerr << "Current state '" << mode << "'" << endl;
                // Design your control algorithm here depending on the input data from above.
                // Create vehicle control data.
                VehicleControl vc;
              
        
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
               vc.setSpeed(0.7);
               
                wheel_angle = sd.getExampleData(); // in degrees 
                cerr << "wheel_angle " << wheel_angle << endl;
                cerr << "counter " << counter << endl;
                cerr << "delay " << delay << endl;
                cerr << "lane type " << laneType << endl;
                frontRightSonic = sbd.getValueForKey_MapOfDistances(4);
                infraRear = sbd.getValueForKey_MapOfDistances(1);
                frontCenter = sbd.getValueForKey_MapOfDistances(3);
                infraRightRear = sbd.getValueForKey_MapOfDistances(2);
                infraRightFront = sbd.getValueForKey_MapOfDistances(0);
                cerr << "speed " << vd.getSpeed() << endl;
                if(normalDriving){
                    // follows lane unless angle is greater than 102, sees an intersection at 204
                    if(wheel_angle <100 ){
                        cerr <<" lane following" << endl;
                        vc.setSpeed(0.8);
                        steerAngle = wheel_angle;
                        timer = 0;
                    }

                   if(wheel_angle >=102){
                        vc.setSpeed(0.5);
                        steerAngle = 0;
                    }

                    if(wheel_angle >= 204 && !obstacle){
                        intersectionFound = true;
                        normalDriving = false;
                        steerAngle = 0;
                    }      
                }  
        /***** intersection code ********/     
                if(intersectionFound && !obstacle){ 
                    
                    if(timer < 100){ // delays car on stopping at intersection
                        cerr <<"intersection found, timer = " << timer <<endl;
                        vc.setSpeed(0.0);
                        waiting = true;
                        timer ++;
                    }
                    else{
                        waiting = false;
                    }
                     // if intersection clear and delay completed cross   
                    if(!waiting && intersectionFound && (frontRightSonic < 0 || frontRightSonic > 10)
                     && (frontCenter < 0 || frontCenter > 10) ){
                        vc.setSpeed(1);
                        intersectionFound = false;
                        normalDriving = true;    
                    }  
                }
                // set options for straight or curve
                if(selection != 1 && !intersectionFound){
                    if((wheel_angle < -8 || wheel_angle > 8) && !obstacle){
                        vc.setSpeed(0.6);
                        rightTurn = 21; 
                        distToObstacle = 6;
                        laneType = "curve";
                    }
                    if((wheel_angle > -8 && wheel_angle < 8) && !obstacle){
                        vc.setSpeed(0.6);
                        rightTurn = 25;
                        distToObstacle = 6;
                        laneType = "straight";
                    }
               
                /****** overtaking mode *********/
                // Detects an obstacle and takes a left. Overtaking starts and becomes true. 
                // stops lane detection. 
                    if(frontCenter < distToObstacle && frontCenter > 0 && !obstacle){
                        vc.setSpeed(0.7);
                        steerAngle = -25; 
                        obstacle = true;
                        overtake = true;
                        normalDriving = false;
                        mode = "obstacle found";
                    } 
                 
             
                // when infraRightRear detects the obstacle and turns right.
                    if(overtake && infraRightRear <= 3 && infraRightRear > 0){
                        counting = true;
                        steerAngle = rightTurn; 
                        inLHLane = true;
                        mode = "going to LHLane";      
                    }
                    // starts a counter to measure length of turn, delay to give car time to get in
                    // left lane
                    if(counting && overtake){
                        counter ++;
                        delay ++;   
                    }
                // In leftlane state and infraRightRear detects obstacle,
                
                    if(delay >= 90 && inLHLane && infraRightRear > 0){
                        counting = false;
                 // if infrared sensors have almost equal values enable lane following
                        if( infraRightRear >= (infraRightFront - 0.1) && infraRightRear <= (infraRightFront + 0.1) ){
                            steerAngle = 0;
                            normalDriving = true;
                            overtake = false;
                            backToRHLane = true;
                            inLHLane = false;  
                            mode = "inLHLane"; 
                            delay = 0;
                            }   
                    }
                // front right US is free from obstacles and makes a right turn.            
                    if(backToRHLane && (frontRightSonic < 0 || frontRightSonic > 10)){
                        normalDriving = false;
                        steerAngle = rightTurn;
                        mode ="going to RHLane";
                        backToRHLane = false;
                        inRHLane = true;
                        counting = true;
                    } 
               
                // decreases counter to 0
                    if(counting && inRHLane){
                        counter --; 
                    } 
                // if counter is 0 can turn left and follow RH lane                
                    if(counter <= 0 && inRHLane){ 
                        counter = 0;                  
                        steerAngle = -25; 
                        if(wheel_angle < 50){ // lane detection has enough data to work correctly
                            normalDriving = true;
                            inRHLane = false;
                            overtake = false;
                            obstacle = false;
                            counter = false;
                            mode = "follow lanedetection";
                        }
                    }
                }

    /************* parking mode *************************/            
                if(selection == 1){ 
                    
                switch(stage){ 
                    case 1:{
                        normalDriving = false;
                        vc.setSpeed(1);
                        steerAngle = 0;
                        cerr<<"stage 1"<<endl;
                        if(infraRightRear<0){
                            stage =2;
                        }
                    }break;
                    case 2:{
                        normalDriving = false;
                        vc.setSpeed(1);
                        steerAngle = 0;
                        cerr<<"stage 2"<<endl;
                        if(infraRightRear>0){
                            stage = 4;

                        }
                        //for Parking-hard situation
                        if (infraRightRear>0&&infraRightFront<0&&frontRightSonic<0)
                        {
                            stage = 3;
                        }
                    }break;
                    //drive a little bit more and start parking for parking-hard
                    case 3:{
                        normalDriving = false;
                        vc.setSpeed(1);
                        steerAngle = 0;
                        cerr<<"case 3"<<endl;
                        if (infraRightRear<0)
                        {
                            stage = 4;
                        }
                    }break;
                    //start parking
                    case 4:{
                        normalDriving = false;
                        vc.setSpeed(-1);
                        steerAngle =90;
                        cerr<<"case 4"<<endl;
                        if(frontRightSonic<0 && infraRightFront>1.4){
                            stage = 5;
                        }
                    }break;
                    case 5:{
                        normalDriving = false;
                        vc.setSpeed(-0.5);
                        steerAngle = -120;
                        cerr<<"case 5"<<endl;                 
                        if(frontRightSonic>2.4&&frontCenter<0){
                            stage = 6;
                        }
                    }break;
                    case 6:{
                        normalDriving = false;
                        vc.setSpeed(-0.5);
                        steerAngle = -130;
                        cerr<<"case 6"<<endl;                      
                        if(frontCenter>5||(infraRear<2&&infraRear>0)){
                            stage = 7;
                            // vc.setSpeed(0);
                        }
                    }break;
                    case 7:{
                        normalDriving = false;
                        vc.setSpeed(0.5);
                        steerAngle = 150;
                        cerr<<"stage 7"<<endl;
                        if((frontCenter<2&&frontCenter>0)||
                            (frontCenter<4&&frontCenter>0&&infraRear<0)||
                            (frontCenter<3.5&&frontCenter>0&&infraRear>0)){
                            vc.setSpeed(0);
                        }
                    }break;

                    default:{
                           
                        cerr <<"default"<<endl;
                        if(normalDriving){
                        wheel_angle = sd.getExampleData(); 
                            steerAngle = 0;
                            vc.setSpeed(2);
                            if(wheel_angle < 100){
                                cerr <<" lane following" <<endl;
                                steerAngle = wheel_angle;
                                timer=0;
                            }
                            if(wheel_angle >= 102){
                                vc.setSpeed(0.5);
                            }
                            if(wheel_angle >= 204){
                                if( timer <= 50){
                                    timer= timer + 1;
                                }
                            }   
                        }
                        if(frontRightSonic < 0 && infraRightFront < 0){
                            stage = 1;
                            cerr<<"found the big gap"<<endl;
                        }
                    }break; 
                }
                 }
            
                vc.setSteeringWheelAngle(steerAngle* Constants::DEG2RAD);
                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
            }
            return ModuleState::OKAY;
        }
} // msv