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

        const int MIN_PARKING_SPACE = 5;
        double start;
        double end;
        int parkingState = 0;
        double gaplength;
        double TotalParkingSpace;
        int state;
        int count=0;
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
                TimeStamp ts;
                
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
               
                
                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                double desiredSteeringWheelAngle = sd.getExampleData(); // 4 degree but SteeringWheelAngle expects the angle in radians!
               
                if(sd.getIntersectionFound()<1.0 && state==0){
                        
                vc.setSpeed(5.0);
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);


                }else{
                        state=1;
        }
        if(state==1){
            
                cout<<"state 1"<<endl;
                vc.setSpeed(0.0);
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                count++;
                if(count > 100){
                        state=0;
                        sd.setIntersectionFound(0.0);
                        sd.setPassedIntersection(1.0);
                        count=0;
                }
        }
         
                // Get distance of sensor 0 IFFr
                /*double infraredFrontRight = sbd.getValueForKey_MapOfDistances(0);

                double infraredRearRight = sbd.getValueForKey_MapOfDistances(2);
                std::cout << "Sensor infrared front right"<< infraredRearRight  << std::endl;

                if(infraredFrontRight>0){
                        start = vd.getAbsTraveledPath();
                }
                std::cout << "Parking start"<< start << std::endl;

                if(infraredFrontRight<0){
                        end = vd.getAbsTraveledPath();
                }
                std::cout << "Parking end"<< end << std::endl;

                gaplength = end - start;
                std::cout << "Parking distance"<< gaplength << std::endl;

                //Check if have a min space to park and save Total Parking Space
                if(gaplength > MIN_PARKING_SPACE){
                        parkingState = 1;
        
                        if(gaplength >0){
                                TotalParkingSpace = gaplength;
                        }


                }
                std::cout << "Total Parking length"<< TotalParkingSpace << std::endl;

                //Stop the car when rear right infrared sensor is in line with object.
                if(parkingState==1 && infraredRearRight > 0){
                        vc.setSpeed(0.0);
                }


                
        


                
                
                */


                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                //double desiredSteeringWheelAngle = 4; // 4 degree but SteeringWheelAngle expects the angle in radians!
                //vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

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
