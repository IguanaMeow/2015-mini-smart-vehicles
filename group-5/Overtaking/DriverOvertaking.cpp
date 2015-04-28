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


/*	
This part of the lane changing trajectory is terminated at time point (b) once both infrared-based 
distance sensors mounted at the right hand side have “seen” the obstacle for the first time.
The current state of the vehicle according to Fig. 13(a) and 13(b) is now interpreted as such that the vehicle’s center is on the left lane.

Now, the vehicle needs to steer with a maximum to the right to orientate it in parallel to the obstacle. 
This part of the trajectory is terminated at time point (c) once both infrared-based
distance sensors return the same distance to the obstacle. 

Next, the vehicle continues on the neighboring lane to actually 
pass the obstacle until the ultrasonic distance sensor mounted at the
top/right corner does not “see” the obstacle anymore at time point (d).
Finally, the vehicle returns to its original lane by driving the inverted trajectory from the beginning of the overtaking process. 
Therefore, the algorithm has tracked the duration during the left and right arc in the initial overtaking phase.
*/


                // Create vehicle control data.
                VehicleControl vc;



               
                /* Set the steering angle straight. */
                if(vd.getAbsTraveledPath() > 57 && vd.getHeading() > 1.56 && vd.getHeading() < 1.58){
                	vc.setSteeringWheelAngle(0);
                	vc.setSpeed(0.0);
                }

                /* Start steering to the left once the rear right infrared sensor detect nothing.*/
             	else if(vd.getAbsTraveledPath() > 43 && sbd.getValueForKey_MapOfDistances(2) < 0){
                	vc.setSteeringWheelAngle(-26);
                	vc.setSpeed(1.0);
                }

                /* Start steering back to the right lane once the front right infrared sensor detect nothing. */
             	else if(vd.getAbsTraveledPath() > 42 && sbd.getValueForKey_MapOfDistances(0) < 0){
                	vc.setSteeringWheelAngle(25);
                	vc.setSpeed(1.0);
                }

                /* Straighten up the steering angle once infrared rear right becomes larger than infrared front right. */
             	else if(vd.getAbsTraveledPath() > 41 && sbd.getValueForKey_MapOfDistances(2) > sbd.getValueForKey_MapOfDistances(0)){
             		vc.setSteeringWheelAngle(0);
             		vc.setSpeed(2.0);
             	}

             	/* Start steering maximum to the right once both infrared front right _and_ rear right sees the obstacle. */
                else if(vd.getAbsTraveledPath() > 41 && sbd.getValueForKey_MapOfDistances(0) > 0 && sbd.getValueForKey_MapOfDistances(2) > 0){
                	vc.setSteeringWheelAngle(25);
                	vc.setSpeed(1.0);
                }

                /* Start steering maximum to the left once Ultrasonic front_center is under 7 meters from obstacle. */
                else if(vd.getAbsTraveledPath() > 37 && sbd.getValueForKey_MapOfDistances(3) < 7){
                	vc.setSteeringWheelAngle(-26);
                	vc.setSpeed(1.0);
                }

                /* Base case. */
                else{
                	vc.setSpeed(1.0);
                }




                // // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                // vc.setSpeed(1.0);

                // // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                // double desiredSteeringWheelAngle = 4; // 4 degree but SteeringWheelAngle expects the angle in radians!
                // vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

                // // You can also turn on or off various lights:
                // vc.setBrakeLights(false);
                // vc.setLeftFlashingLights(false);
                // vc.setRightFlashingLights(true);

                // // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // // Send container.
                getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }
} // msv
