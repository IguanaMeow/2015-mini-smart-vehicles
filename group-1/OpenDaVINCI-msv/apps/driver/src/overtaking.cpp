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

                bool obstacleFront=false;
                bool startTurningToLeft=false;
                bool turn_Left_Comleate=false;
                int state=0;
                int turnState=0;
                int count1=0;
                int count2=0;
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



                // Design your control algorithm here depending on the input data from above.



                // Create vehicle control data.
                VehicleControl vc;
                double desiredSteeringWheelAngle;

                //start with laneflowing
                //if(sbd.getValueForKey_MapOfDistances(3)<0){
                //vc.setSpeed(2);
                //desiredSteeringWheelAngle=sd.getExampleData();

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                 if((sbd.getValueForKey_MapOfDistances(3)<0 || sbd.getValueForKey_MapOfDistances(3)>6.8 ) && sbd.getValueForKey_MapOfDistances(5)<0 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "Moving" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();
                       
                        
                }
                else if(sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<6.8 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "Changing boolean"<<endl;
                        obstacleFront=true;
                        state=1;

                    }else if(state==1 && obstacleFront==true) { // check if obstacle is found in front
                        cout<< "obstacle found" << endl;
                        startTurningToLeft=true;
                        obstacleFront=false;
                      
                }else if(startTurningToLeft==true && sbd.getValueForKey_MapOfDistances(3)<6.8 && sbd.getValueForKey_MapOfDistances(0)<0){
                                    cout<< "Start turning 1" << endl;
                                    desiredSteeringWheelAngle--;
                                    vc.setSpeed(1);
                                    state=2;
                   
                      
                }else if(state==2 && sbd.getValueForKey_MapOfDistances(0)>0 && sbd.getValueForKey_MapOfDistances(2)<0){//&& sbd.getValueForKey_MapOfDistances(5)<0){
                                    vc.setSpeed(1);
                                   cout<< "Start turning 2" << endl;
                                   startTurningToLeft=false;
                                   desiredSteeringWheelAngle--; 
                                  

                }   else if( state==3 && sbd.getValueForKey_MapOfDistances(2)>0 && sbd.getValueForKey_MapOfDistances(5)<0){
                                    vc.setSpeed(1);
                                   cout<< "Start turning 3" << endl;
                                   startTurningToLeft=false;
                                   desiredSteeringWheelAngle++; 
                                   //count1++;
                                   state=4;

                    
                }else if(state==4 && sbd.getValueForKey_MapOfDistances(5)>0){
                                   cout<< "we are in lane following" << endl;
                                   desiredSteeringWheelAngle=sd.getExampleData(); 
                                   //count1++;
                                   vc.setSpeed(2);
                                   //state=4;
                        

                }else if (sbd.getValueForKey_MapOfDistances(0)>0 && sbd.getValueForKey_MapOfDistances(2)>0 && sbd.getValueForKey_MapOfDistances(5)>0){
                    cout<< "turning back after overtaking" << endl;
                    desiredSteeringWheelAngle++;
                    cout<< desiredSteeringWheelAngle << endl;
                    vc.setSpeed(1); 
                }

                else if (sbd.getValueForKey_MapOfDistances(0)<0 && sbd.getValueForKey_MapOfDistances(2)<0 && sbd.getValueForKey_MapOfDistances(5)>0){
                    cout<< "turning again to lane " << endl;
                    desiredSteeringWheelAngle=sd.getExampleData()+3;
                    cout<< desiredSteeringWheelAngle << endl;
                    vc.setSpeed(2);
                }
                 else if (sbd.getValueForKey_MapOfDistances(0)<0 && sbd.getValueForKey_MapOfDistances(2)<0 && sbd.getValueForKey_MapOfDistances(5)<0){
                    cout<< "lane followig again " << endl;
                    desiredSteeringWheelAngle= sd.getExampleData();
                    vc.setSpeed(2);
                }
            
                cout << "                  state :" << state << endl;

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
