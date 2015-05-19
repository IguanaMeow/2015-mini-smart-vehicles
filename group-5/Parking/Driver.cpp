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

                enum State {FORWARD, SCANNING, MEASURING, STOP, REVERSE_RIGHT, STRAIGHTEN_UP_REVERSE, STRAIGHTEN_UP_FORWARD, FINAL_TOUCH, DONE};
                State state = FORWARD;
                double posA;
                double posB;
                double desiredSteeringWheelAngle;
                double heading;
                double headingCorr;

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

                switch(state){

                //Initial state for driving forward
                case FORWARD:
                cout << "STATE: FORWARD" << endl;
                vc.setSpeed(2.0);
                desiredSteeringWheelAngle = 0;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                posA = 0;
                posB = 0;
                if(sbd.getValueForKey_MapOfDistances(2) < 0) {
                    posA = vd.getAbsTraveledPath();
                    state = SCANNING;                    
                }
                break;

                //State for starting to scan a gap area, scanning starts when the ir rr is not sensing any obstacle
                case SCANNING:
                cout << "STATE: SCANNING" << endl;
                vc.setSpeed(1.4);
                desiredSteeringWheelAngle = 0;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                if(sbd.getValueForKey_MapOfDistances(2) > 0) {
                    state = MEASURING;                    
                }
                break;

                //State for measuring if the size of the gap is big enough, scanning ends and measuring starts when the ir rr starts sensing an obstacle in its range again
                case MEASURING:
                cout << "STATE: MEASURING" << endl;
                posB = vd.getAbsTraveledPath();
                vc.setSpeed(0.5);
                desiredSteeringWheelAngle = 0;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                if(posB - posA > 4) {
                    heading = vd.getHeading();
                    state = STOP;
                }
                else {
                    state = FORWARD;
                }
                break;

                //State for stopping the vehicle before going into the parking trajectory, necessary for slowing down the brake-path and getting a better starting position
                case STOP:
                cout << "STATE: STOP" << endl;
                vc.setSpeed(0.0);
                if (vd.getSpeed() < 0.5) {
                        state = REVERSE_RIGHT;
                }
                break;

                //Initial state of the parking trajectory, the vehicle starts reversing to the right
                case REVERSE_RIGHT:
                cout << "STATE: REVERSE_RIGHT" << endl;
                desiredSteeringWheelAngle = 26;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(-0.5);
                headingCorr = (heading * Constants::RAD2DEG) + 60;
                if (headingCorr > 359) {
                        headingCorr -= 359;
                }
                if(vd.getHeading() * Constants::RAD2DEG > headingCorr) {
                    state = STRAIGHTEN_UP_REVERSE;
                }
                break;

                //State for straightening up the heading while reversing
                case STRAIGHTEN_UP_REVERSE:
                cout<< "STATE: STRAIGHTEN_UP_REVERSE" << endl;                
                desiredSteeringWheelAngle = -30;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(-0.3);
                headingCorr = (heading * Constants::RAD2DEG) + 1;
                if (headingCorr > 359) {
                        headingCorr -= 359;
                }
                if(vd.getHeading() * Constants::RAD2DEG > (heading * Constants::RAD2DEG) && vd.getHeading() * Constants::RAD2DEG < headingCorr) {
                    state = FINAL_TOUCH;                    
                }
                if(sbd.getValueForKey_MapOfDistances(1) < 2.5 && sbd.getValueForKey_MapOfDistances(1) > 0) {
                    state = STRAIGHTEN_UP_FORWARD;
                }
                break;

                //State for straightening up the heading while driving forward
                case STRAIGHTEN_UP_FORWARD:
                cout<< "STATE: STRAIGHTEN_UP_FORWARD" << endl;
                desiredSteeringWheelAngle = 30;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(0.3);
                headingCorr = (heading * Constants::RAD2DEG) + 1;
                if (headingCorr > 359) {
                        headingCorr -= 359;
                } 
                if(vd.getHeading() * Constants::RAD2DEG > (heading * Constants::RAD2DEG) && vd.getHeading() * Constants::RAD2DEG < headingCorr) {
                    state = FINAL_TOUCH;
                } 
                if(sbd.getValueForKey_MapOfDistances(3) < 2) {
                    state = STRAIGHTEN_UP_REVERSE;
                }                
                break;

                //Final touch for efficient parking, making sure the vehicle is parked at the closest position to the obstacle in front of it in an attempt to use as little space as possible
                case FINAL_TOUCH:
                cout << "STATE: FINAL_TOUCH" << endl;
                if (sbd.getValueForKey_MapOfDistances(3) < 3 && sbd.getValueForKey_MapOfDistances(3) > 2.5){
                    state = DONE;
                }
                else if(sbd.getValueForKey_MapOfDistances(3) > 3 || sbd.getValueForKey_MapOfDistances(3) < 0){
                desiredSteeringWheelAngle = 0;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(0.3);
                }
                else {
                desiredSteeringWheelAngle = 0;
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                vc.setSpeed(-0.3); 
                }
                break;

                //End state of the parking trajectory for when the vehicle is correctly parked
                case DONE:
                cout << "STATE: DONE" << endl;
                vc.setSpeed(0.0);
                posA = 0;
                posB = 0;
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
