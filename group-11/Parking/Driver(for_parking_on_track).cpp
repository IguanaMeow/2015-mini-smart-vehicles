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
        double count = 0;
        double subtractPark = 0;
        double countParkDistance = 0;
        bool park = false;
        bool smallspace = false;
        bool rearObjectDetected = false;
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
         //               cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;
// 2. Get most recent sensor board data:
                        Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                        SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
              //          cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;
// 3. Get most recent user button data:
                        Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
                        UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
           //             cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;
// 4. Get most recent steering data as fill from lanedetector for example:
                        Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                        SteeringData sd = containerSteeringData.getData<SteeringData> ();
             //           cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;
// Design your control algorithm here depending on the input data from above.
// Create vehicle control data.

                       // Container containerVehicleControl = getKeyValueDataStore().get(Container::VEHICLECONTROL);

                       // VehicleControl vc = containerVehicleControl.getData<VehicleControl> (); 


                        VehicleControl vc;
                        
// With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)



                         sd.setSpeedData(1.0);
                        // vc.setAcceleration(1);
                         double angle = sd.getExampleData();
                         cout  << "Driver: ExampleData = " << sd.getExampleData() << endl;
                         cout  << "Driver: SpeedData = " << sd.getSpeedData() << endl;


                        cout << "Speed is = " << vc.getSpeed() << endl;
                        

                        if (vd.getAbsTraveledPath() >= count + 50 && sbd.getValueForKey_MapOfDistances(0) < 0 && park == false && rearObjectDetected == false){
                        cout << "Park is now true! AbsPath is = " << vd.getAbsTraveledPath() << endl;
                                park = true;
                         }  
                      

                        if (sbd.getValueForKey_MapOfDistances(2) > 0 && park == false && rearObjectDetected == false){
                            cout << "Trying to find parking space .. AbsPath = " << vd.getAbsTraveledPath() << endl;
                                count = vd.getAbsTraveledPath();        
                        }


                        //Escape to lane if object is detected
                        if (rearObjectDetected == true){
                            cout << "RearObject now true!" << endl;

                            cout << "RearObject: AbsPath = " << vd.getAbsTraveledPath() << endl;
                            sd.setSpeedData(1.0);
                            cout << "RearObject: Count = " << count << endl;
                            cout << "RearObject: CountParkDistance = " << countParkDistance << endl;
                           
                            if (vd.getAbsTraveledPath() >= count + 16){
                                sd.setExampleData(23.0);
                            }

                            if (vd.getAbsTraveledPath() >= count + countParkDistance){
                                sd.setExampleData(0.0);
                                cout << "RearObject: Setting angle to 0 now and ending." << endl;
                                count = 10000;
                                park = false,
                                rearObjectDetected = false;

                            }
                        }
                                             

                         if (park == true && rearObjectDetected == false){
                            cout << "Park now true!" << endl;
                            cout << "AbsPath in park-mode = " << vd.getAbsTraveledPath() << endl;
                            sd.setSpeedData(1.0);
                            sd.setExampleData(0.0);

                            if (vd.getAbsTraveledPath() <= count + 160 && sbd.getValueForKey_MapOfDistances(1) > 0 && sbd.getValueForKey_MapOfDistances(1) < 2.5){
                                cout << "RearObject has been detected, reversing to road.." << endl;
                                cout << "AbsPath when rearobject was detected: " << vd.getAbsTraveledPath() << endl;
                                countParkDistance = countParkDistance - subtractPark;
                                count = vd.getAbsTraveledPath();                        
                                rearObjectDetected = true;
                                park = false;                               
                            }
                            
                            if (vd.getAbsTraveledPath() >= count && vd.getAbsTraveledPath() <=count + 100){
                                cout << "Moving forward in parking" << endl;
                                subtractPark = count + 109;
                                sd.setSpeedData(1.0);
                            }
                            if (vd.getAbsTraveledPath() >= count + 100 && vd.getAbsTraveledPath() <= count + 215){
                                 cout << "Setting speed -1 in parking" << endl;
                                countParkDistance = vd.getAbsTraveledPath();
                                sd.setSpeedData(-1.0);
                            }
                            if (vd.getAbsTraveledPath() >= count + 110 && vd.getAbsTraveledPath() <= count + 150){
                                 cout << "Setting angle to right in parking" << endl;
                                sd.setExampleData(23.0);
                            }
                            if (vd.getAbsTraveledPath() >= count + 150 && vd.getAbsTraveledPath() <= count + 170){
                                cout << "Setting angle to 0 in parking" << endl;
                                sd.setExampleData(0.0);
                            }
                            if (vd.getAbsTraveledPath() >= count + 170 && vd.getAbsTraveledPath() <= count + 215){
                                cout << "Setting angle to left in parking" << endl;
                                sd.setExampleData(23.0);
                            }




                            if (vd.getAbsTraveledPath() >= count + 180 && (sbd.getValueForKey_MapOfDistances(1) > 0 && sbd.getValueForKey_MapOfDistances(1) <= 2.2) && park == true){
                                cout << "Rear IR has detected object, this is a small space" << endl;
                                smallspace = true; 
                                
                            }

                            if (smallspace == true && vd.getAbsTraveledPath() <= count + 250){
                                cout << "Small space is now true!" << endl;
                                sd.setSpeedData(1.0);
                                
                                if (vd.getAbsTraveledPath() >= count + 200){
                                    sd.setExampleData(23.0);
                                }
                                if (vd.getAbsTraveledPath() >= count + 210){
                                    sd.setExampleData(7.0);
                                    sd.setSpeedData(0);

                                }
                          

                            }
                            

/*
                            //Emergancy break
                            if (vd.getAbsTraveledPath() >= count + 250 && (sbd.getValueForKey_MapOfDistances(1) < 2.8 || sbd.getValueForKey_MapOfDistances(1) <= 0)){
                            cout << "Rear IR has detected object, stopping" << endl;
                                sd.setExampleData(0.0);
                                sd.setSpeedData(0.0);                            
                            }


                            //Emergancy break
                            if (vd.getAbsTraveledPath() >= count + 250 && (sbd.getValueForKey_MapOfDistances(3) < 2 || sbd.getValueForKey_MapOfDistances(3) < 0)){
                                 cout << "Front Ultra has detected object, stopping" << endl;
                                sd.setSpeedData(0.0);
                                sd.setExampleData(0.0);
                            }
                         }        
                                
*/
                         }  
                     
                        
                        
// With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                        angle = sd.getExampleData();
                        vc.setSteeringWheelAngle(angle * Constants::DEG2RAD);



                        //sbd.getValueForKey_MapOfDistances(2);
// You can also turn on or off various lights:
                        vc.setBrakeLights(false);
                        vc.setLeftFlashingLights(false);
                        vc.setRightFlashingLights(true);
// Create container for finally sending the data.
                        //Container c (Container::VEHICLECONTROL, vc);
                        Container c(Container::USER_DATA_1, sd);

// Send container.
                        getConference().send(c);
                }
                return ModuleState::OKAY;
        }
} // msv
