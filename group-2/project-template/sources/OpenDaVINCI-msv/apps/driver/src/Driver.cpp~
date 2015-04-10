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

	bool LaneSwitch = false;
	double FirstPathLength = 0;

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



                // Design your control algorithm here depending on the input data from above
	

                // Create vehicle control data.
                VehicleControl vc;
  		// With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                double desiredSteeringWheelAngle = 0; // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                vc.setSpeed(20);

		double traveled = vd.getAbsTraveledPath();
		cerr << "Distance traveled: '" << traveled << "'" << endl;
		double sen6 = sbd.getValueForKey_MapOfDistances(0);
                double sen = sbd.getValueForKey_MapOfDistances(1);
                double sen2 = sbd.getValueForKey_MapOfDistances(2);
                double sen3 = sbd.getValueForKey_MapOfDistances(3);
                double sen4 = sbd.getValueForKey_MapOfDistances(4);
                double sen5 = sbd.getValueForKey_MapOfDistances(5);
                cerr <<sen<< "Rear IR"<<endl;
                cerr <<sen2<< "Rear right IR"<<endl;
                cerr <<sen6<< "Front right IR"<<endl;
                cerr <<sen3<< "Front sonic"<<endl;
                cerr <<sen4<< "Front right sonic"<<endl;
                cerr <<sen5<< "Rear right sonic"<<endl; 
      		cerr <<FirstPathLength<< "FirstPathLength"<<endl; 
        	
		if(LaneSwitch){
			//if (traveled)
			double currently = vd.getAbsTraveledPath();
 			if((currently - FirstPathLength) >= 4.5 ){

			 	vc.setSteeringWheelAngle(45);	
				if((currently - FirstPathLength) >= 9.3)
				{
					vc.setSteeringWheelAngle(0);
					
				}
					if(sen2 < 0 && sen6 < 0){
						vc.setSteeringWheelAngle(45);

						if((currently - FirstPathLength) >= 20.6){
							vc.setSteeringWheelAngle(-45);
						
							if((currently - FirstPathLength) >= 25){
							vc.setSteeringWheelAngle(0);
							FirstPathLength = 0;
							LaneSwitch = false;
								}
						}

						
					}	


			}
					 
			else {
				vc.setSteeringWheelAngle(-45);	
			}

		}
		else if(sen3 < 7){
			FirstPathLength = vd.getAbsTraveledPath();
			LaneSwitch = true;

		}

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

