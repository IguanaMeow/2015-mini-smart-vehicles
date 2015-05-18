/**
 * parker - Sample application for calculating steering and acceleration commands.
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
#include "core/base/KeyValueConfiguration.h"

#include "GeneratedHeaders_Data.h"

#include "Parker.h"

namespace msv {

        using namespace std;
        using namespace core::base;
        using namespace core::data;
        using namespace core::data::control;
        using namespace core::data::environment;


        // States for parking
        const int SCANNING = 0;
        const int MEASURING = 1;
        const int ALIGNING = 2;
        const int BACK_RIGHT = 3;
        const int BACK_STRAIGHT = 8;
        const int BACK_LEFT = 4;
        const int STRAIGHTEN = 5;
        const int STOPPING = 6;
        const int STOP = 7;
        const int ABORT = 9;


        Parker::Parker(const int32_t &argc, char **argv) :
	        ConferenceClientModule(argc, argv, "Parker") {
        }

        Parker::~Parker() {}

        void Parker::setUp() {
	        // This method will be call automatically _before_ running body().
        }

        void Parker::tearDown() {
	        // This method will be call automatically _after_ return from body().
        }

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Parker::body() {

                int mode = SCANNING;
                int counter = 0;
                double currentTraveledPath;
                double initialHeading;

                KeyValueConfiguration kv = getKeyValueConfiguration();
                double carLength = kv.getValue<double> ("global.carLength");
                

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
                
                switch (mode) {

                        case SCANNING:
                        cout << "Forward - Mode SCANNING" << endl;
                        vc.setSpeed(2);
                        vc.setSteeringWheelAngle(sd.getHeadingData());
                        if(sbd.getValueForKey_MapOfDistances(0) < 0 && (sbd.getValueForKey_MapOfDistances(4) < 0 || sbd.getValueForKey_MapOfDistances(4) > carLength * 2)){
                                mode = MEASURING;
                                currentTraveledPath = vd.getAbsTraveledPath();
                        }
                        break;

                        case MEASURING:
                        
                        cout << "Forward - Mode MEASURING" << endl;
                        cout << "Measured distance: " << vd.getAbsTraveledPath() - currentTraveledPath << endl;
                        vc.setSpeed(1);
                        vc.setSteeringWheelAngle(sd.getHeadingData());
                        if((sbd.getValueForKey_MapOfDistances(0) > -1 && vd.getAbsTraveledPath() - currentTraveledPath < carLength * 1.50) || (sd.getHeadingData() > 10 || sd.getHeadingData() < -10)){
                                mode = SCANNING;
                        }
                        else if (vd.getAbsTraveledPath() - currentTraveledPath >= carLength * 1.50){
                                mode = ALIGNING;
                                currentTraveledPath = vd.getAbsTraveledPath();
                        }
                        break;

                        case ALIGNING:
                        cout << "Reverse - Mode ALIGNING" << endl;
                        cout << "Measured distance: " << vd.getAbsTraveledPath() - currentTraveledPath << endl;
                        vc.setSpeed(1);
                        if((vd.getAbsTraveledPath() - currentTraveledPath) > carLength * 0.9){
                                counter = 0;
                                mode = STOPPING;
                                initialHeading = vd.getHeading();
                        }
                        
                        break;


                        
                        case STOPPING:
                        cout << "Reverse - Mode STOPPING" << endl;
                        ++counter;
                        vc.setSpeed(0);
                        if(counter >= 30){
                                mode = BACK_RIGHT;
                                currentTraveledPath = vd.getAbsTraveledPath();
                        }

                        break;
                        

                        case BACK_RIGHT:
                        cout << "Reverse - Mode BACK_RIGHT" << endl;
                        cout << "Measured distance: " << vd.getAbsTraveledPath() - currentTraveledPath << endl;
                        cout << "Initial heading(): " << initialHeading * Constants::RAD2DEG << " Heading: " << vd.getHeading() * Constants::RAD2DEG << endl;
                        cout << "Angle diff: " << angleDifference(initialHeading, vd.getHeading()) << endl;
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(25 * Constants::DEG2RAD);
                        if (angleDifference(initialHeading, vd.getHeading()) > 40){
                                mode = BACK_STRAIGHT;
                                currentTraveledPath = vd.getAbsTraveledPath();
                                vc.setSteeringWheelAngle(0);
                        }
                        if(sbd.getValueForKey_MapOfDistances(1) < carLength * 0.10 && sbd.getValueForKey_MapOfDistances(1) > 0){
                                mode = ABORT;
                                currentTraveledPath = vd.getAbsTraveledPath();
                                vc.setSpeed(0);
                        }

                        break;

                        case BACK_STRAIGHT:
                        cout << "Reverse - Mode: BACK_STRAIGHT" << endl;
                        cout << "Measured distance: " << vd.getAbsTraveledPath() - currentTraveledPath << endl;
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(0);
                        if((vd.getAbsTraveledPath() - currentTraveledPath) > carLength * 0.6){
                                mode = BACK_LEFT;
                                currentTraveledPath = vd.getAbsTraveledPath();
                                vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD);
                        }

                        break;

                        case BACK_LEFT:
                        cout << "Reverse - Mode: BACK_LEFT" << endl;
                        cout << "Initial heading(): " << initialHeading * Constants::RAD2DEG << " Heading: " << vd.getHeading() * Constants::RAD2DEG << endl;
                        cout << "Angle diff: " << angleDifference(initialHeading, vd.getHeading()) << endl;
                        cout << "Rear: " << (sbd.getValueForKey_MapOfDistances(1) < (carLength * 0.5)) << endl;
                        cout << "Rear: " << (sbd.getValueForKey_MapOfDistances(1) > 0) << endl;
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD);
                        if(angleDifference(initialHeading, vd.getHeading()) < 5 || (sbd.getValueForKey_MapOfDistances(1) < (carLength * 0.5) && sbd.getValueForKey_MapOfDistances(1) > 0)){
                                vc.setSpeed(0);
                                mode = STRAIGHTEN;
                        }

                        break;

                        case STRAIGHTEN:
                        cout << "Forward - Mode: STRAIGHTEN" << endl;
                        cout << "Initial heading: " << initialHeading * Constants::RAD2DEG << " Heading: " << vd.getHeading() * Constants::RAD2DEG << endl;
                        cout << "Angle diff: " << angleDifference(initialHeading, vd.getHeading()) << endl;
                        vc.setSteeringWheelAngle(25);
                        vc.setSpeed(0.5);
                        //|| sbd.getValueForKey_MapOfDistances(3) < 0.1 * carLength
                        if(angleDifference(initialHeading, vd.getHeading()) < 0.5){
                                vc.setSpeed(-1);
                                mode = STOP;
                        }
                        else if(sbd.getValueForKey_MapOfDistances(3) < (0.5 * carLength) && sbd.getValueForKey_MapOfDistances(3) > 0){
                                mode = BACK_LEFT;
                        }

                        break;

                        case STOP:
                        cout << "Mode: STOP" << endl;
                        vc.setSpeed(0);
                        break;

                }

                cerr << "Rear right: " << sbd.getValueForKey_MapOfDistances(2) << endl;
                cerr << "Front right: " << sbd.getValueForKey_MapOfDistances(0) << endl;
                cerr << "US front right: " << sbd.getValueForKey_MapOfDistances(4) << endl;
                cerr << "Rear: " << sbd.getValueForKey_MapOfDistances(1) << endl;

                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }

        double Parker::angleDifference(double initialHeading, double heading){

                
                return (180 - abs(abs(heading - initialHeading) - 180)) * Constants::RAD2DEG; 
                //initialHeading = abs(initialHeading) * Constants::RAD2DEG;
                //heading = abs(heading) * Constants::RAD2DEG;

                //if (heading < initialHeading)
                //        return heading + 360 - initialHeading;
                //return heading - initialHeading;
        }
} // msv

