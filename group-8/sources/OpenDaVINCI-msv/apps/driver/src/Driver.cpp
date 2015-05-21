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
	        ConferenceClientModule(argc, argv, "Driver"),
                state(1),
                counter(0),
                SPEED(2) {
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

                // Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

                // Get most recent steering data as fill from lanedetector for example:
                Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                SteeringData sd = containerSteeringData.getData<SteeringData> ();
                cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

                // Design your control algorithm here depending on the input data from above.
                switch(state) {
                case 1:
                        cout << "state 1" << endl;
                        sd.setSpeedData(SPEED);

                        if (sd.getIntersectionLine() > 0) {
                                state = 2;
                        }
                        break;
                case 2:
                        cout << "state 2" << endl;
                        
                        sd.setSpeedData(0);
                        counter++;
                        
                        if (counter > 90 /*&& !isObject()*/) {
                                counter = 0;
                                state = 3;
                        }
                        break;
                case 3:
                        cout << "state 3" << endl;
                        sd.setSpeedData(SPEED);
                        if (sd.getIntersectionLine() < 1) {
                                sd.setIntersectionLine(0);
                                state = 1;
                        }
                        break;
                }


                // Create vehicle control data.
                VehicleControl vc;

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                vc.setSpeed(sd.getSpeedData());

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                //double desiredSteeringWheelAngle = 4; // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(sd.getHeadingData());

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

        // Checks for objects with the front-center and front-right Ultrasonics.
        bool Driver::isObject() 
        {     
                SensorBoardData sbd;
                KeyValueConfiguration kv = getKeyValueConfiguration();
                double carLength = kv.getValue<double> ("global.carLength");

                if (sbd.getValueForKey_MapOfDistances(3) > 0 && sbd.getValueForKey_MapOfDistances(3) < carLength * 3) {
                        return true;
                } else if (sbd.getValueForKey_MapOfDistances(4) > 0 && sbd.getValueForKey_MapOfDistances(4) < carLength * 3){
                        return true;
                } else {
                        return false;
                }
        }
} // msv

