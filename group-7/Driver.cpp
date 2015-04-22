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
#include "generated/msv/SensorBoardData.h" 

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
                int state;
                double US_Front;
                double US_FR;
               // double US_RR;
                double IF_FR;
                double IF_RR;
                double IF_Rear;
                 // Create vehicle control data.
                VehicleControl vc;
                 double desiredSteeringWheelAngle = 0; 
                 double speed = 0;

                 double distance_1, distance_2, distance_3;

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

                //5. Get most recent 
                Container containerSpeedData = getKeyValueDataStore().get(Container::USER_DATA_2);
                SpeedData spd_old = containerSpeedData.getData<SpeedData> ();
                cerr << "Most recent speed data: '" << spd_old.toString() << "'" << endl;


                // Design your control algorithm here depending on the input data from above.


                US_Front = sbd.getValueForKey_MapOfDistances(3);
                US_FR = sbd.getValueForKey_MapOfDistances(4);
               // US_RR = sbd.getValueForKey_MapOfDistances(5);
                IF_FR = sbd.getValueForKey_MapOfDistances(0);
                IF_RR = sbd.getValueForKey_MapOfDistances(2);
                IF_Rear = sbd.getValueForKey_MapOfDistances(1);
                
                //  //chack if the using sensors is found
                // if((sbd.containsKey_MapOfDistances(IF_FR)==true)&&(sbd.containsKey_MapOfDistances(IF_Rear)==true)
                //         &&(sbd.containsKey_MapOfDistances(IF_RR)==true)&&(sbd.containsKey_MapOfDistances(US_RR)==true)
                //         &&(sbd.containsKey_MapOfDistances(US_FR)==true)&&(sbd.containsKey_MapOfDistances(US_Front)==true)){
                            
                        switch(state){ // using switch-case to change state

                            case 0:
                                speed = 0;
                                desiredSteeringWheelAngle = 0;
                                break;
                            
                            case 1: //gap enough sate, then drive 10 units more to find appropriate distance to park to start parking state
                                speed = 1;
                                desiredSteeringWheelAngle = 0;
                                if(vd.getAbsTraveledPath() >= distance_1+12){
                               // if(sbd.getValueForKey_MapOfDistances(US_RR)<0){
                                    state = 2;
                                    distance_2 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 2: // start parking state, drive backward.
                                speed=-1.5;
                                desiredSteeringWheelAngle = 15;
                                if(vd.getAbsTraveledPath()>= distance_2+7.8){
                                    state = 3;
                                    distance_3 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 3: // parking state 3 (The car detect behind object or drive more 5 meter)
                                speed = -1;
                                desiredSteeringWheelAngle = -26;

                                if(vd.getAbsTraveledPath()>= distance_3+7.5 //if it doesn't detect any car behind after drive 10 meters more.
                                    && IF_Rear <0){

                                    state = 0;
                                }

                                if(IF_Rear <= 3 && IF_Rear >0) state = 4;
                                break;

                            case 4:// move 1 to make the car park right
                                speed = 1;
                                desiredSteeringWheelAngle = 20;
                                //if IF_Rear doesn't detect any object Or US_Front detect any object
                                if(IF_Rear <0 ||(US_Front <3.5 && US_Front >0)){
                                   
                                    state = 5;
                                   // distance_4 = vd.getAbsTraveledPath();
                                }
                                break;

                            case 5: // finding the appropriated position of the car
                                speed = -0.4;
                                desiredSteeringWheelAngle = -20;
                                if(IF_Rear<2.75 && IF_Rear>0) state = 0;
                                break;

                            default: // find space for parking and right place to park (found the obstacle)
                               
                               //get speed data and wheelangle from the lane detector through container
                                speed = spd_old.getSpeedData();
                                desiredSteeringWheelAngle = sd.getExampleData();

                                if(US_FR <0 // 6 meter enough to park
                                    && IF_FR <0 && IF_RR <0){
                                   // &&sbd.getValueForKey_MapOfDistances(US_RR)>0){
                                    state = 1; //state gap enough
                                    distance_1 = vd.getAbsTraveledPath();
                                }
                                break;
                        }
                            
                //}
                /*********************End parking**********************/

                /****************************/
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
               vc.setSpeed(speed);

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
             //   double desiredSteeringWheelAngle = -20; // 4 degree but SteeringWheelAngle expects the angle in radians!
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

