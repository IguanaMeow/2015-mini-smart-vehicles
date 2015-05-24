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
        double Driver::angle(double heading){
                double difference = heading * Constants::RAD2DEG;
                if(difference> 360)
                {
                  difference=difference-360;
                }
              return difference;
        }
        // This method will do the main data processing job.
        // Written By Nicolas Kheirallah
        ModuleState::MODULE_EXITCODE Driver::body() {


                const int CARSIZE = 4;
                int mode=0;
                double speed;
                int steering;
                double absPath;
                double curentPath;
                double initheading;
                double correctHeading;
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

              double  IRRear = sbd.getValueForKey_MapOfDistances(1);
              double  IR_RR=sbd.getValueForKey_MapOfDistances(2);
               double   USFrontCenter = sbd.getValueForKey_MapOfDistances(3);


                // Design your control algorithm here depending on the input data from above.



                // Create vehicle control data.
                VehicleControl vc;

                switch(mode){

                case 0:
                cout << "Start Driving" << endl;
                speed =2.0;
                steering =0;
                absPath = 0;
               curentPath = 0;
                if(IR_RR < 0) { // we strt driving incase sensor doesnt find anything 
                    absPath = vd.getAbsTraveledPath();
                    mode = 1;                    
                }
                break;

                case 1:
                cout << "Finding parkingspace" << endl;
                speed =1.5;
                steering =0;
                if(IR_RR > 0) { // if sensor finds an object we change mode too mesure it size of the spot
                    mode = 2;                    
                }
                break;

                case 2:
                cout << "Parking space is big enough" << endl;
                curentPath = vd.getAbsTraveledPath();
                speed =1;
                steering =0;
                if(curentPath - absPath > CARSIZE) { // we start mesuring the size of the spot from the spot where we found an object until we find another object again
                    initheading = vd.getHeading(); // if free space is bigger than carsize we slow down and park.
                    mode = 3;
                }
                else {
                    mode = 0; // if parking isnt big enough , we keep looking
                }
                break;

                case 3:
                cout << "Prepairing for backing" << endl;
                speed =0.0;
                if (vd.getSpeed() < 0.5) { // slow down the car and start backing it up
                        mode = 4;
                }
                break;

                case 4: 
                cout << "Backing up right" << endl;
                steering =26; // backing up right
                speed =-1;
                correctHeading = angle(initheading)+ 50; // backing in too the parkingspace , +50 too get a better angle of parking
                if(vd.getHeading() * Constants::RAD2DEG > correctHeading) // checks current heading angle is bigger than initheading angle - 360
                {initheading angle
                    mode = 5;
                }
                break;

                case 5:
                cout<< "Backing left" << endl;                
                steering =-26; // backing up left
                speed =-1;
                correctHeading = angle(initheading); // Used too calculate the anglediffrence
                if(vd.getHeading() * Constants::RAD2DEG >angle(initheading) && vd.getHeading() * Constants::RAD2DEG < correctHeading) {
                    mode = 7;                    
                }
                if(IRRear < 2.5 && IRRear > 0) { 
                    mode = 6;
                }
                break;

                case 6:
                cout<< "Forward right" << endl;
                steering =26;
                speed =1;
                correctHeading = angle(initheading)+1;
                if(vd.getHeading() * Constants::RAD2DEG > angle(initheading) && vd.getHeading() * Constants::RAD2DEG < correctHeading) {
                    mode = 7;
                } 
                if(USFrontCenter < 2) {
                    mode = 5;
                }                
                break;

                case 7:
                cout << "Make that parking bautiful" << endl;
                if (USFrontCenter < 2.5 && USFrontCenter > 2){ // 2-2.4 jungles back in foward as a swing , this is because the USFrontCenter sensor goes between 2.8-3.1
                    mode = 8;
                }
                else if(USFrontCenter > 3 || USFrontCenter < 0){ // drive abit front
                steering =0;
                speed =0.5;
                }
                else { // ddrives abit back
                steering =0;
                speed =-0.5; 
                }
                break;

                case 8:
                cout << "turning of the ferrari" << endl;
                speed =0.0; // turn of the car
                steering =0; // steering as middle
                break;

                }
                vc.setSteeringWheelAngle(steering * Constants::DEG2RAD);
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
