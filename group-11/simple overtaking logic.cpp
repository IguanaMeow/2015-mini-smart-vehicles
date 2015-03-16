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

 /* Hard-coded trajectory for overtaking logic - Milestone 2*/

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

    
				if (vd.getAbsTraveledPath() >= 0 && vd.getAbsTraveledPath() < 63.0) {

		                vc.setSpeed(2.0);
						vc.setAcceleration(20.0);

				}

				// Maneuvering forwards
                double desiredSteeringWheelAngle = 0; 

                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                 
                
				// Start overtaking, turn left
                if (vd.getAbsTraveledPath() > 30 && vd.getAbsTraveledPath() < 36.2) {

		                desiredSteeringWheelAngle = -15;
						vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD); 
                

                }

				// Stabilize on left lane, turn right
                if(vd.getAbsTraveledPath() > 36.2 && vd.getAbsTraveledPath() < 42.4) {

                      	desiredSteeringWheelAngle = 15; 
		      			vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
		      
                }

				// proceed forwards when back on course
                if(vd.getAbsTraveledPath() > 42.4 && vd.getAbsTraveledPath() < 48.6) {

                       desiredSteeringWheelAngle = 0;
		     		   vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
		         
                }

				// Turn right
                if(vd.getAbsTraveledPath() > 48.6 && vd.getAbsTraveledPath() < 55.2) {

                      desiredSteeringWheelAngle = 15; 
		              vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
	               
                }

				// Stabilize, turn left
                if(vd.getAbsTraveledPath() > 55.2 && vd.getAbsTraveledPath() < 61.2) {

                      desiredSteeringWheelAngle = -15;
		              vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD); 
		      
                }

				// proceed on course, forwards
                if(vd.getAbsTraveledPath() > 62 && vd.getAbsTraveledPath() < 63) {
 
				      desiredSteeringWheelAngle = 0; 
				      vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

			      	  // Halt the vehicle
		              vc.setSpeed(0.0);
			          vc.setAcceleration(0.0);

		      }


                }
		

                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }
} // msv



