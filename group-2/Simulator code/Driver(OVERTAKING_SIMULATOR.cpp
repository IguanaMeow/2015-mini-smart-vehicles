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

 int intersect = 0;
 double intersectStart = 0;
 int count1 = 0;
bool hugLeft = false;

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


		//CODE WRITTEN BY WILLIAM BERGENDAHL, CARL-OSCAR PERSSON, JOHAN HERMANSSON, JONATHAN KLEMETZ


		//WILLIAM & CALLE WROTE THE 2 SUPERHARDTURN FUNCTIONS REST WAS MADE BY JONATHAN & JOHAN

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
		
		

                double desiredSteeringWheelAngle = 0;
                // Create vehicle control data.
                VehicleControl vc;
                
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                if (sd.getDriveStraight()==true && intersect == 0){
                        vc.setSpeed(1.2);
                        desiredSteeringWheelAngle = 0; 
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                        cerr << "Straight '" << endl;
                }
		if (sd.getTurnLeft()==true && intersect == 0){
                        vc.setSpeed(0.8);
                        desiredSteeringWheelAngle = -3; 
                        cerr << "left '" << endl;
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                }if (sd.getTurnHarderLeft()==true && intersect == 0){
                        vc.setSpeed(0.8);
                        desiredSteeringWheelAngle = -13;
                        cerr << "Hleft '" << endl; 
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                }if (sd.getTurnSuperHardLeft()==true && intersect == 0){
                        vc.setSpeed(1);
                        desiredSteeringWheelAngle = -28;
                        cerr << "SHleft" << endl;
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                }if (sd.getTurnRight()==true && intersect == 0){
                        vc.setSpeed(0.8);
                        desiredSteeringWheelAngle = 3; 
                        cerr << "Right" << endl;
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                }if (sd.getTurnHarderRight()==true && intersect == 0){
                        vc.setSpeed(0.8);
                        desiredSteeringWheelAngle = 13;
                        cerr << "Hrigt" << endl;
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                }if (sd.getTurnSuperHardRight()==true && intersect == 0){
                        vc.setSpeed(0.8);
                        desiredSteeringWheelAngle = 15;
                        cerr << "SHright" << endl;
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                }if (sd.getStopAtIntersect()==true && intersect == 0){
                        vc.setSpeed(0.0);
                        desiredSteeringWheelAngle = 0;
                        vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                        intersect = 1;
                        intersectStart = vd.getAbsTraveledPath();
                        count1 = 0;
                        cerr << "Stopp" << endl;

                }if (intersect == 1){
                        if (count1 <=40){
                                count1++;
                                vc.setSpeed(0.0);
                                desiredSteeringWheelAngle = 0;
                                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                                cerr << "Waiting" << endl;

                        }
                        if (count1 >= 41){
                                vc.setSpeed(1.2);
                                desiredSteeringWheelAngle = 0;
                                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                                cerr << "Continuing" << endl;
                                if ((vd.getAbsTraveledPath() - intersectStart) >= 13){
                                        count1 = 0;
                                        intersect = 0;
                                        intersectStart = 0;
                                        cerr << "Resuming" << endl;
                                }
                        }

                }

		
		
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


