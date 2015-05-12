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
            
           
            int yourangle; // in degrees 
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
            double getGapsize;
            double gapsize;
            int stage;
            // double speed;
            double getTraveledPath;
            int sec = 0;
            int selection;
            string mode = " ";
          // Menu here, fo
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
                Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
                UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
                cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;
                // 4. Get most recent steering data as fill from lanedetector for example:
                Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                SteeringData sd = containerSteeringData.getData<SteeringData> ();
                cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;
                
                cerr << "Current state '" << mode << "'" << endl;
                // Design your control algorithm here depending on the input data from above.
                // Create vehicle control data.
                VehicleControl vc;
                TimeStamp ts;
        
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                vc.setSpeed(0.8);
               
                yourangle = sd.getExampleData(); // in degrees 
                frontRightSonic = sbd.getValueForKey_MapOfDistances(4);
                infraRear = sbd.getValueForKey_MapOfDistances(1);
                frontCenter = sbd.getValueForKey_MapOfDistances(3);
                infraRightRear = sbd.getValueForKey_MapOfDistances(2);
                infraRightFront = sbd.getValueForKey_MapOfDistances(0);
                
                if(normalDriving){
                    
                    if(yourangle <100 ){
                        cerr <<" lane following" <<endl;
                        steerAngle = yourangle;
                        sec = 0;
                    }

                    if(yourangle >=102 ){
                        vc.setSpeed(0.5);
                        steerAngle = 0;
                    }

                    if(yourangle >= 204){
                       
                        intersectionFound = true;
                        normalDriving = false;
                        vc.setSpeed(0.0);
                        steerAngle = 0;
                    }
                }
                if(intersectionFound){
                    
                
                        cerr <<"intersection found, sec = " << sec <<endl;
                        if( sec < 100){
                            vc.setSpeed(0.0);
                            waiting = true;
                            sec ++;
                        }
                        else {
                            waiting = false;
                        }
                        
                    
                    if(!waiting && intersectionFound && (frontRightSonic < 0 || frontRightSonic > 10)
                     && (frontCenter < 0 || frontCenter > 10) ){
                        vc.setSpeed(1);
                        intersectionFound = false;
                        normalDriving = true;
                        
                }
                    
                   
               } 
                if(selection != 1){
                if((yourangle < -2 || yourangle > 2) && !obstacle){
                    rightTurn = 15; 
                    distToObstacle = 6;
                    cerr <<"poopcurve" <<endl;
                }
                if((yourangle >-1 && yourangle <1) && !obstacle){
                    rightTurn = 23;
                    distToObstacle = 7;
                    cerr <<"straight road" <<endl;
                }
                //overtaking. Using states
                // Detects an obstacle and takes a left. Overtaking starts and becomes true. 
                // No longer using lane detection. 
                if(frontCenter < distToObstacle && frontCenter > 0 && !obstacle){
                    cerr <<"obstacle found" <<endl;
                    vc.setSpeed(0.5);
                    steerAngle = -25; 
                    obstacle = true;
                    overtake = true;
                    normalDriving = false;
                    mode = "obstacle found";
                 
                } 
                // In the state where the car is in overtaking behavior, 
                // when infraRightFront detects the corner of the obstacle and turns right.
                //In leftlane state, leaving overtaking state
                if(overtake && infraRightFront < 3 && infraRightFront > 0){
                    steerAngle = rightTurn; 
                    inLHLane = true;
                    overtake = false;
                    mode = "going to LHLane";
                }
                // In leftlane state and infraRightRear detects obstacle,
                // and set wheelangle to zero. 
                // Back to rightlane 
                if(inLHLane && infraRightRear >= 1 ){
                    //normalDriving =true;
                    steerAngle = 0;
                    vc.setSpeed(1);
                    backToRHLane = true;
                    inLHLane = false;
                    mode = "inLHLane";
                   
                }
                // infraRightFront is free from obstacles and makes a right turn.            
                if(backToRHLane && infraRightFront < 0){
                   // normalDriving = false;
                    steerAngle = rightTurn;
                    mode ="going to RHLane";
                    backToRHLane = false;
                    inRHLane = true;
                }
                // Both infraRightFront and infraRightRear is clear and then the car turns back,
                /*----should follow lane detection, it slows down so might seem to try----*/
                //----but it does not...
                if(inRHLane && infraRightFront < 0 && infraRightRear < 0){                   
                    steerAngle = -25; 
                    normalDriving = true;
                    inRHLane = false;
                    obstacle = false;
                    mode = "follow lanedetection";
                }
                }
                if(selection == 1){ 
                    
                switch(stage){
                    //stage 1 is for big gap
                    case 1:
                        cerr<<"case1"<<endl;
                        normalDriving =false;
                        cerr << vd.getAbsTraveledPath() << "travel path" << endl;
                        cerr << gapsize << "gapsize" << endl;
                        vc.setSpeed(1);
                        getGapsize = vd.getAbsTraveledPath();
                        if(frontRightSonic > 0){
                           
                            if(getGapsize-gapsize > 7.5){
                                stage = 2;
                            }
                            cerr << "stop" << endl;
                            cerr << gapsize << "gapsize" << endl;
                            cerr << getGapsize << "the gap end" << endl;
                        }
                    break;
                    case 2:
                        normalDriving = false;
                        
                        cerr << "case2" <<endl;
                        vc.setSpeed(1);
                        if(frontRightSonic < 7){
                            stage = 5;
                        }
                    break;
                    //stage 3 is for small gap
                    case 3:
                        normalDriving = false;
                        vc.setSpeed(1);
                        cerr << "case3" << endl;
                        if(frontRightSonic < 0){
                            stage = 1;
                        }
                        if(infraRightRear < 0 &&
                            infraRightFront < 0 &&
                            frontRightSonic > 0){
                            cerr << "this is a gap" << endl;
                            stage = 4;
                        }
                    break;
                    case 4:
                        normalDriving = false;
                        vc.setSpeed(1);
                        cerr << "case4" << endl;
                        if(infraRightRear > 0 &&
                            infraRightFront > 0){
                            stage = 5;
                            cerr << "stop parking the car" << endl;
                        }
                    break;
                    // stage 5 is start parking
                   
                    case 5:
                        normalDriving = false; 
                        cerr << "go back" << endl;
                        vc.setSpeed(-0.5);
                        steerAngle = 60;
                        cerr << "case5" << endl;
                        
                        if(infraRear > 0){
                   
                            steerAngle = 50;
                            vc.setSpeed(0.5);
                            cerr <<"heeeeeeeeeeej" <<endl;
                        }
                        if(vd.getHeading() * Constants::RAD2DEG > 156){
                            stage = 6;
                        }
                    break;
                    case 6:
                        normalDriving =false;
                     
                        steerAngle = -60;
                        vc.setSpeed(-0.5);
                        cerr<<"case6"<<endl;
                        if (vd.getHeading() * Constants::RAD2DEG < 136)
                        {
                            stage=7;
                        }
                    break;
                    case 7:
                        normalDriving =false;
                     //   yourangle = sd.getExampleData(); 
                        steerAngle =80;
                        vc.setSpeed(0.5);
                        cerr<<"case7"<<endl;
                        if(vd.getHeading()*Constants::RAD2DEG < 110){
                            stage = 8;
                        }
                    break;
                    case 8:
                        normalDriving =false;
                        
                     //   yourangle = sd.getExampleData(); 
                        steerAngle = -80;
                        vc.setSpeed(-0.5);
                        cerr<<"case8"<<endl;
                        
                        if(vd.getHeading()*Constants::RAD2DEG <= 95){
                            vc.setSpeed(0);
                        }
                    break;
                    case 9:
                        normalDriving =false;
                        
                    //    yourangle = sd.getExampleData(); 
                        steerAngle = 10;
                        vc.setSpeed(0.5);
                        cerr<<"case9"<<endl;
                        if(vd.getHeading()*Constants::RAD2DEG <= 90){
                            vc.setSpeed(0);
                        }
                    break;
                  
                      //  yourangle = sd.getExampleData(); 
                   
                  
                    default:
                      //  yourangle = sd.getExampleData(); 
                        // steerAngle = 0;
                        // vc.setSpeed()=2;
                        cerr <<"default"<<endl;
                        getTraveledPath = vd.getAbsTraveledPath();
                       // yourangle = sd.getExampleData(); 
                        if(normalDriving){
                        yourangle = sd.getExampleData(); 
                            steerAngle = 0;
                            vc.setSpeed(1.5);
                            if(yourangle <100){
                                cerr <<" lane following" <<endl;
                                steerAngle = yourangle;
                                sec=0;
                            }
                            if(yourangle >=102){
                                vc.setSpeed(0.5);
                            }
                            if(yourangle >= 204){
                                if( sec <= 100){
                                    vc.setSpeed(0.0);
                                    sec= sec + 1;
                                }
                            }   
                        }
                        if(frontRightSonic<0){
                            normalDriving = false;
                            gapsize = getTraveledPath;
                            stage = 1;
                        }
                        if(infraRightRear>0&&
                            infraRightFront>0&&
                            frontRightSonic <3){
                            normalDriving=false;
                            gapsize = getTraveledPath;
                            stage = 3;
                        }
                    break; 
                }
                 }
                // You can also turn on or off various lights:
                vc.setBrakeLights(false);
                vc.setLeftFlashingLights(false);
                vc.setRightFlashingLights(false);
               

                vc.setSteeringWheelAngle(steerAngle* Constants::DEG2RAD);
                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
            }
            return ModuleState::OKAY;
        }
} // msv