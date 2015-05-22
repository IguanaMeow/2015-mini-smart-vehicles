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
                //checking obstacle in front of the car
                bool obstacleFront=false;
                //checking turn to left 
                bool startTurningToLeft=false;
                // State 
                int state=0;
                // counter 1 
                int count1=0;
                // counter 2
                int count2=0;
                // the distance for calculating travel path
                double distance;
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

            while (getModuleState() == ModuleState::RUNNING) {
                // In the following, you find example for the various data sources that are available:

                // 1. Get most recent vehicle data:
                Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
                VehicleData vd = containerVehicleData.getData<VehicleData> ();
              //  cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

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


                // Create vehicle control data.
                VehicleControl vc;
                double desiredSteeringWheelAngle;
                distance= vd.getAbsTraveledPath();
                 
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                //  When car is moving in a distance less than 90 or bigger than 150 we are in straight
                if(distance<90 || distance>150){
                // lane following when US_FC is not detecting or has distance that is less then 10 and when IR_FR is not detecting
                if((sbd.getValueForKey_MapOfDistances(3)<0 || sbd.getValueForKey_MapOfDistances(3)>10 ) && sbd.getValueForKey_MapOfDistances(0)<0 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "tracking" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();
                       
                //Obstacle detected from front when 0<US_FC<10. 
                } else if(sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<10 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "Changing boolean"<<endl;
                        obstacleFront=true;
                        state=1;

                //changing boolean turning to the left to true in this statement
                }else if(state==1 && obstacleFront==true) { // check if obstacle is found in front
                        cout<< "obstacle found" << endl;
                        startTurningToLeft=true;
                        obstacleFront=false;
                
                // in this statement car start to turn to the left with -20 degree and count2 increase until it reaches 100
                }else if(state==1 && startTurningToLeft==true && count2<100){
                        cout<< "turning to left" << endl;
                        desiredSteeringWheelAngle=-20;
                        vc.setSpeed(1);
                        count2++;

                // end of state1 
                }else if(state==1){
                        state=2;

                //checking whether US_FR IR_FR or IR_RR is detecting obstacle, then switch to follow the left lane
                }else if(state ==2 && (sbd.getValueForKey_MapOfDistances(4)>0 || sbd.getValueForKey_MapOfDistances(2)>0 || sbd.getValueForKey_MapOfDistances(0)>0 )){
                        cout<< "lane following in left side" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();

                //checking IR_FR and US_FR are not detecting, the car has passed the obstacle and start to steer back with 15 degrees to follow 
                // the right lane again when the counter reaches 50.
                }else if(state ==2 && sbd.getValueForKey_MapOfDistances(0)<0 && sbd.getValueForKey_MapOfDistances(4)<0 && count1<50){
                        cout<< "turning back to right lane" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=15;
                        count1++;
                        
                // return back to state 0 when the car has completely passed the obstacle.
                }else{
                       count1=0;
                       obstacleFront=false;
                       startTurningToLeft=false;
                       state=0;
                       count2=0;
                }
//__________________________________________________________________________________________________________________________________________________________________________________________________________________
                // when the car is moving in a distance bigger than 90 and less than 150 we are in curve
                //(we had to make this if condition as in curve the lane following was not recognizing the vanishing point)
                // in line 189, we only added a small steering to the left with 15 degrees before adjusting it with lane following after the front detection
                }else if (distance>90 && distance<150){
                if((sbd.getValueForKey_MapOfDistances(3)<0 || sbd.getValueForKey_MapOfDistances(3)>10 ) && sbd.getValueForKey_MapOfDistances(0)<0 && obstacleFront==false && startTurningToLeft==false) {
                    cout<< "tracking" << endl;
                    vc.setSpeed(2);
                    desiredSteeringWheelAngle=sd.getExampleData();
                       
                }else if(sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<10 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "Changing boolean"<<endl;
                        obstacleFront=true;
                        state=1;

                }else if(state==1 && obstacleFront==true) {
                        cout<< "obstacle found" << endl;
                        startTurningToLeft=true;
                        obstacleFront=false;
                
                }else if(state==1 && startTurningToLeft==true &&count2<100){
                        cout<< "turning to left" << endl;
                        desiredSteeringWheelAngle=-20;
                        vc.setSpeed(1);
                        count2++;

                }else if (state==1){
                        state=2;
                
                }else if (state ==2 && (sbd.getValueForKey_MapOfDistances(4)>0 || sbd.getValueForKey_MapOfDistances(2)>0 || sbd.getValueForKey_MapOfDistances(0)>0 )){
                         cout<< "turn to adjust curve" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=15;

                }else if (state ==2 && (sbd.getValueForKey_MapOfDistances(4)>0 || sbd.getValueForKey_MapOfDistances(2)>0 || sbd.getValueForKey_MapOfDistances(0)>0 )){
                         cout<< "lane following in left side" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();

                }else if(state ==2 && sbd.getValueForKey_MapOfDistances(0)<0 && sbd.getValueForKey_MapOfDistances(4)<0 && count1<20){
                        cout<< "turning back to right lane" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=15;
                        count1++;
                }else{
                    count1=0;
                    obstacleFront=false;
                    startTurningToLeft=false;
                    state=0;
                    count2=0;
                }
            }
                //printing out the distance and the states.
                cout << "                  state :" << state << endl;
                cout << "                  distance is  :" << distance << endl;

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                 // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

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
