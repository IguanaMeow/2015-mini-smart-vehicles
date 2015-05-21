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

Driver::~Driver() {
}

void Driver::setUp() {
	// This method will be call automatically _before_ running body().
}

void Driver::tearDown() {
	// This method will be call automatically _after_ return from body().
}

// This method will do the main data processing job.
ModuleState::MODULE_EXITCODE Driver::body() {

	enum State {
		FORWARD,
		BELOW_THRESHOLD_LEFT,
		BOTH_IR_SEEN_OBSTACLE,
		FRONT_IR_LESS_THAN_BACK,
		FRONT_IR_DETECTS_NOTHING,
        ULTRASONIC_DETECT_NOTHING,
		CALL_LANEDETECTOR
	};
	State state = FORWARD;
	//double start_count;
	//double end_count;
	//double result_count;

	while (getModuleState() == ModuleState::RUNNING) {
		// In the following, you find example for the various data sources that are available:

		// 1. Get most recent vehicle data:
		Container containerVehicleData = getKeyValueDataStore().get(
				Container::VEHICLEDATA);
		VehicleData vd = containerVehicleData.getData<VehicleData>();
		cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

		// 2. Get most recent sensor board data:
		Container containerSensorBoardData = getKeyValueDataStore().get(
				Container::USER_DATA_0);
		SensorBoardData sbd =
				containerSensorBoardData.getData<SensorBoardData>();
		cerr << "Most recent sensor board data: '" << sbd.toString() << "'"
				<< endl;

		// 3. Get most recent steering data as fill from lanedetector for example:
		Container containerSteeringData = getKeyValueDataStore().get(
				Container::USER_DATA_1);
		SteeringData sd = containerSteeringData.getData<SteeringData>();
		cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

		// Design your control algorithm here depending on the input data from above.

		// Create vehicle control data.
		VehicleControl vc;

		double LaneAngle = sd.getExampleData(); // Gets example steering data

		switch (state) {

		case FORWARD:

			cout << "STATE: MOVING FORWARD" << endl;
			vc.setSteeringWheelAngle(LaneAngle * Constants::DEG2RAD);
			vc.setSpeed(1.0);
			if (sbd.getValueForKey_MapOfDistances(3) > 4.4) {
				state = BELOW_THRESHOLD_LEFT;
			}
			break;

			/*			cout << "STATE: MOVING FORWARD" << endl;
			 vc.setSteeringWheelAngle(LaneAngle * Constants::DEG2RAD);
			 if (LaneAngle <= -10) {
			 vc.setSpeed(1.0);
			 } else if (LaneAngle >= 10) {
			 vc.setSpeed(1.0);
			 } else if (sbd.getValueForKey_MapOfDistances(3) > 7) {
			 vc.setSpeed(1.0);
			 //state = BELOW_THRESHOLD_LEFT;
			 } else {
			 vc.setSpeed(1.0);
			 }

			 break;*/

		case BELOW_THRESHOLD_LEFT:
			// STEER MAXIMUM LEFT
			cout << "STATE: STEER MAXIMUM LEFT" << endl;
			//start_count = 0;
			//end_count = 0;
			//result_count = 0;
			//start_count = vd.getAbsTraveledPath();
			vc.setSteeringWheelAngle(-26);
			vc.setSpeed(1.0);
			if (sbd.getValueForKey_MapOfDistances(0) > 0
					&& sbd.getValueForKey_MapOfDistances(2) > 0) {
				state = BOTH_IR_SEEN_OBSTACLE;
			}
			break;

		case BOTH_IR_SEEN_OBSTACLE:
			// STEER MAXIMUM RIGHT
			cout << "STATE: BOTH_IR_SEEN_OBSTACLE" << endl;
			//end_count = 0;
			//end_count = vd.getAbsTraveledPath();
			vc.setSteeringWheelAngle(25);
			vc.setSpeed(0.5);
			if (sbd.getValueForKey_MapOfDistances(2) > 1.25
					&& sbd.getValueForKey_MapOfDistances(2) < 1.7
					&& sbd.getValueForKey_MapOfDistances(0) > 1.25
					&& sbd.getValueForKey_MapOfDistances(0) < 1.7) {
				state = FRONT_IR_DETECTS_NOTHING;
			}

			break;

		case FRONT_IR_DETECTS_NOTHING:
			cout << "STATE: FRONT IR DETECTS NOTHING" << endl;
			// GO STRAIGHT AHEAD UNTIL TOP RIGHT INFRARED RETURN -1
			vc.setSteeringWheelAngle(0);
			vc.setSpeed(1.0);
			if (sbd.getValueForKey_MapOfDistances(0) < 0) {
				state = ULTRASONIC_DETECT_NOTHING;
			}

			break;

		case ULTRASONIC_DETECT_NOTHING:
			cout << "STATE: ULTRASONIC DETECT NOTHING" << endl;
			// FRONT IR DETECTS NOTHING
			vc.setSteeringWheelAngle(25);
			vc.setSpeed(1.0);
			if (sbd.getValueForKey_MapOfDistances(5) < 0) {
				vc.setSteeringWheelAngle(25);
				vc.setSpeed(1.0);
				state = CALL_LANEDETECTOR;
			}
			break;

		case CALL_LANEDETECTOR:
			cout << "STATE: CALL LANEDETECTOR" << endl;
			// CALL LANEDETECTOR
			vc.setSpeed(1.0);
			vc.setSteeringWheelAngle(LaneAngle * Constants::DEG2RAD);
			if (LaneAngle <= -10) {
				vc.setSpeed(1.0);
			} else if (LaneAngle >= 10) {
				vc.setSpeed(1.0);
			}
			break;

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
