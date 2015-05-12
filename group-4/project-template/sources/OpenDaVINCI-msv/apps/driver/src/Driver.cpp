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
                //VehicleControl vc;
		//vc.setSpeed(0.0);
	        // This method will be call automatically _before_ running body().
        }

        void Driver::tearDown() {
		
	        // This method will be call automatically _after_ return from body().
        }

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {
		VehicleControl vc;
		int speed = 2;
		double back = Constants::PI + (Constants::PI/2);
	        while (getModuleState() == ModuleState::RUNNING) {
                // In the following, you find example for the various data sources that are available:

                // 1. Get most recent vehicle data:
                Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
                VehicleData vd = containerVehicleData.getData<VehicleData> ();
                //cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;
		cerr << vd.getHeading() << endl;
                // 2. Get most recent sensor board data:
                //Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                //SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                //cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

                // 3. Get most recent user button data:
                //Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
                //UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
                //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

                // 4. Get most recent steering data as fill from lanedetector for example:
                //Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                //SteeringData sd = containerSteeringData.getData<SteeringData> ();
                //cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;



                // Design your control algorithm here depending on the input data from above.
		Point3 position = vd.getPosition();
                double desiredSteeringWheelAngle = 0;
		if(position.getY() > 15 && vd.getHeading() >= Constants::PI && vd.getHeading() <= 1.2 * Constants::PI)
		{
			// backing
			cerr << "WALLA" << endl;
			speed = -1;
			desiredSteeringWheelAngle = 25;
		}
		else if(position.getY() > 15 && vd.getHeading() <= back)
		{
			speed = 1;
			desiredSteeringWheelAngle = -26;
		}
		else if(position.getY() < 0)
		{
			vc.setBrakeLights(true);
			speed = 0;
		}
		else
		{
			speed = 2;
			desiredSteeringWheelAngle = 0;
		}

                // Create vehicle control data.
                //VehicleControl vc;

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                vc.setSpeed(speed);

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

                // You can also turn on or off various lights:
                //vc.setBrakeLights(false);
                //vc.setLeftFlashingLights(false);
                //vc.setRightFlashingLights(false);

                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }
} // msv
