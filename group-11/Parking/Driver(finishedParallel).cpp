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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
        int count = 0;
        bool park = false;
        bool smallspace = false;
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
// Create vehicle control data.
                        VehicleControl vc;
                        
// With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)


                         vc.setSpeed(1);
                         vc.setAcceleration(1);
                         double angle = sd.getExampleData();

                        if (vd.getAbsTraveledPath() >= count + 5 && sbd.getValueForKey_MapOfDistances(0) < 0){

                                park = true;

                         }  
                        

                        if (sbd.getValueForKey_MapOfDistances(2) > 0 && park == false){
                                count = vd.getAbsTraveledPath(); 

        
                        }


                       
                      

                         if (park == true){
                            vc.setSpeed(0);
                            angle = 0;
                            
                            if (vd.getAbsTraveledPath() >= count && vd.getAbsTraveledPath() <=count + 10){
                                vc.setSpeed(1);
                            }
                            if (vd.getAbsTraveledPath() >= count + 10 && vd.getAbsTraveledPath() <= count + 21.5){
                                vc.setSpeed(-1);
                            }
                            if (vd.getAbsTraveledPath() >= count + 11 && vd.getAbsTraveledPath() <= count + 15){
                                angle = 100000;
                            }
                            if (vd.getAbsTraveledPath() >= count + 15 && vd.getAbsTraveledPath() <= count + 18){
                                angle = 0;
                            }
                            if (vd.getAbsTraveledPath() >= count + 18 && vd.getAbsTraveledPath() <= count + 21.5){
                            
                                angle = -100000;
                            }
                            /*

                if (vd.getAbsTraveledPath() >= count + 20 && (sbd.getValueForKey_MapOfDistances(1) > 2.3 || sbd.getValueForKey_MapOfDistances(1) < 0) && (sbd.getValueForKey_MapOfDistances(3) > 2.4 || sbd.getValueForKey_MapOfDistances(3) <= 0) && (sbd.getValueForKey_MapOfDistances(4) <= 0 || sbd.getValueForKey_MapOfDistances(4) >= 2.6)){
                                angle = 0;
                                vc.setSpeed(0);
                            }*/


                            if (vd.getAbsTraveledPath() >= count + 18 && (sbd.getValueForKey_MapOfDistances(1) > 0 && sbd.getValueForKey_MapOfDistances(1) <= 2.2) && park == true){
                                smallspace = true; 
                                
                            }

                            if (smallspace == true && vd.getAbsTraveledPath() <= count + 25){
                                vc.setSpeed(1);
                                if (vd.getAbsTraveledPath() >= count + 20){
                                    angle = 100000;
                                }
                                if (vd.getAbsTraveledPath() >= count + 21){
                                    angle = 0;
                                    vc.setSpeed(0);
                                }
                                if (vd.getAbsTraveledPath() >= count + 22.5){
                                    vc.setSpeed(-1);
                                    angle = -10000;
                                }
                                if (vd.getAbsTraveledPath() >= count + 23.5){
                                    vc.setSpeed(0);
                                    angle = 0;
                                }
                                if (vd.getAbsTraveledPath() >= count + 24){
                                    vc.setSpeed(1);
                                    angle = 100000;
                                }
                                if (vd.getAbsTraveledPath() >= count + 24.5){
                                    vc.setSpeed(0);
                                    angle = 0;
                                }
                                if (vd.getAbsTraveledPath() >= count + 24.8){
                                    vc.setSpeed(-1);
                                }

                            }
                            
 /*
                            if (vd.getAbsTraveledPath() >= count + 20 && sbd.getValueForKey_MapOfDistances(4) >= 0 && (sbd.getValueForKey_MapOfDistances(3) > 10 || sbd.getValueForKey_MapOfDistances(3) <= 0)){
                                vc.setSpeed(-1);
                                angle = -100000;
                            }

                            if (vd.getAbsTraveledPath() >= count + 18 && (sbd.getValueForKey_MapOfDistances(3) > 0 && sbd.getValueForKey_MapOfDistances(3) <= 2.2)){
                                vc.setSpeed(-1);
                            }
                            if (vd.getAbsTraveledPath() >= count + 20 && (sbd.getValueForKey_MapOfDistances(3) > 0 && sbd.getValueForKey_MapOfDistances(3) <= 2.2)){
                                vc.setSpeed(-1);
                                angle = -100000;
                            }

*/
                            

                            if (vd.getAbsTraveledPath() >= count + 25 && (sbd.getValueForKey_MapOfDistances(1) > 2.3 || sbd.getValueForKey_MapOfDistances(1) <= 0)){
                                angle = 0;
                                vc.setSpeed(0);
                                
                            }

                            if (vd.getAbsTraveledPath() >= count + 25 && (sbd.getValueForKey_MapOfDistances(3) > 2 || sbd.getValueForKey_MapOfDistances(3) < 0)){
                                vc.setSpeed(0);
                                angle = 0;
                            }

                         }        
                                

                        
                        
                        
// With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                        
                        vc.setSteeringWheelAngle(angle * Constants::DEG2RAD);



                        //sbd.getValueForKey_MapOfDistances(2);
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