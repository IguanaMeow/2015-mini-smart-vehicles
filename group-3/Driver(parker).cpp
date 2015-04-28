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

				int mode=0;
				double speed = 0;
                double IF_RR;
                double IF_Rear;
                double US_Front;
                int CurrentPath;
                int absPath;

                while (getModuleState() == ModuleState::RUNNING) {
                // In the following, you find example for the various data sources that are available:

                // 1. Get most recent vehicle data:
                Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
                VehicleData vd = containerVehicleData.getData<VehicleData> ();
           //     cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

                // 2. Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
           //     cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

                // 3. Get most recent user button data:
                Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
                UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
           //     cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

                // 4. Get most recent steering data as fill from lanedetector for example:
                Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                SteeringData sd = containerSteeringData.getData<SteeringData> ();
             //   cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

                // Create vehicle control data.
                VehicleControl vc;
                // Sensors
                IF_Rear = sbd.getValueForKey_MapOfDistances(1); 
                IF_RR = sbd.getValueForKey_MapOfDistances(2);
                US_Front = sbd.getValueForKey_MapOfDistances(3);

                switch(mode){

                //Starts moving the car until it finds a parking spot
                case 0:
                speed =2.0; // let's go fast
                vc.setSteeringWheelAngle(0 * Constants::DEG2RAD);
                if(IF_RR < 0) 
                {
                    CurrentPath = vd.getAbsTraveledPath();
                    mode = 1;                    
                }
                break;
                //looks for a parking spot , if a parking spot is found we check it size
                case 1:
                speed =2.0;// let's look go fast for a parking spot
                vc.setSteeringWheelAngle(0 * Constants::DEG2RAD);
                if(IF_RR > 0) 
                {
                    mode = 2;                    
                }
                break;

                //Checks the size of the parking spot , if it fits it parks , if it doesnt it starts scanning again
                case 2:
                absPath = vd.getAbsTraveledPath();
                speed =0.5;
                vc.setSteeringWheelAngle(0 * Constants::DEG2RAD);
                if(absPath - CurrentPath > 3) 
                {
                	speed = 0.0; //Stops the car so we can back up too park
                    if(vd.getSpeed() < 0.5)  // if Vehicle speed is under 0.5 start backing up
                    {
                    	mode = 3;
                    }
                }
                else
                 {
                    mode = 0; // if parking spot doesnt fit the starts scanning again
                }
                break;

                //the Vehicle starts backing into the parking spot while turning right
                case 3:
                vc.setSteeringWheelAngle(26 * Constants::DEG2RAD);
                speed =-0.5;
                if(vd.getHeading() * Constants::RAD2DEG > 155) // 150 doesnt go enough in and 160 goes too far out
                {  
                    mode = 4;
                }
                break;

                //Trying to straighten up the car while backing
                case 4:
                vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD); //turns left 
                speed =-0.5; // backuo slowly
                if(IF_Rear < 2 && IF_Rear > 0) 
                {
                    mode = 5;
                }
                else if(vd.getHeading() * Constants::RAD2DEG < 91) 
                {
                    mode = 6;
                }
                break;

                // Trying too correct the car while driving foward
                case 5:
                vc.setSteeringWheelAngle(26 * Constants::DEG2RAD); // steering too the right
                speed =0.7;
                if(vd.getHeading() * Constants::RAD2DEG < 91) 
                {
                    mode = 6; // trys too get the car perfect in the midle
                } 
                if(US_Front < 2) 
                {
                   mode = 4;
                }                
                break;

                //What gets the parking perfectly in the middle
                case 6:
                if(US_Front > 3 || US_Front < 0)//Drives the car abit more front so it get perfectly in the middle
                {
                vc.setSteeringWheelAngle(0 * Constants::DEG2RAD);
                speed =0.4;
                }
                else if (US_Front < 3 && US_Front > 2) //What gets the parking perfectly in the middle
                {
                speed =0.0;
                }
                break;
                }
               vc.setSpeed(speed);

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
