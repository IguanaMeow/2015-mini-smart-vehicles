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
                int currentTraveledPath;
                double distanceBackRight;
                double distanceRear;
                double absTraveledPath;
                double initialHeading;
                double heading;
                double steeringWheelAngle;
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
                distanceBackRight = sbd.getValueForKey_MapOfDistances(2);
                distanceRear = sbd.getValueForKey_MapOfDistances(3);
                absTraveledPath = vd.getAbsTraveledPath();
                steeringWheelAngle = sd.getHeadingData();
                heading = vd.getHeading();

                // Create vehicle control data.
                VehicleControl vc;
                
                switch (mode) {

                        case SCANNING:
                        vc.setSpeed(1);
                        vc.setSteeringWheelAngle(sd.getHeadingData());
                        if(distanceBackRight < 0){
                                mode = MEASURING;
                                currentTraveledPath = absTraveledPath;
                        }
                        break;

                        case MEASURING:
                        vc.setSpeed(1);
                        vc.setSteeringWheelAngle(sd.getHeadingData());
                        if((distanceBackRight > -1 && (absTraveledPath - currentTraveledPath) < carLength * 1.75) || (steeringWheelAngle > 10 || steeringWheelAngle < -10)){
                                vc.setSpeed(0);
                                mode = SCANNING;
                        }
                        else if ((absTraveledPath - currentTraveledPath) >= carLength * 1.75){
                                mode = ALIGNING;
                                currentTraveledPath = absTraveledPath;
                        }
                        break;

                        case BACK_RIGHT:
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(25 * Constants::DEG2RAD);
                        if (angleDifference(initialHeading, heading) > 45){
                                mode = BACK_STRAIGHT;
                                currentTraveledPath = absTraveledPath;
                                vc.setSteeringWheelAngle(0);
                        }
                        if(distanceRear < carLength * 0.25 && distanceRear > 0){
                                mode = ABORT;
                                currentTraveledPath = absTraveledPath;
                                vc.setSpeed(0);
                        }

                        break;

                        case BACK_STRAIGHT:
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(0);
                        if((absTraveledPath - currentTraveledPath) > carLength * 0.4){
                                mode = BACK_LEFT;
                                currentTraveledPath = absTraveledPath;
                                vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD);
                        }
                        if(distanceRear < carLength * 0.25 && distanceRear > 0){
                                mode = ABORT;
                                currentTraveledPath = absTraveledPath;
                                vc.setSpeed(0);
                        }
                        break;

                        case BACK_LEFT:
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD);
                        if(angleDifference(initialHeading, heading) < 10){
                                vc.setSpeed(0);
                                currentTraveledPath = absTraveledPath;
                                mode = STRAIGHTEN;
                        }
                        if(distanceRear < carLength * 0.25 && distanceRear > 0){
                                mode = ABORT;
                                currentTraveledPath = absTraveledPath;
                                vc.setSpeed(0);
                        }

                        break;

                        case STRAIGHTEN:
                        vc.setSteeringWheelAngle(25);
                        vc.setSpeed(0.5);
                        if(angleDifference(initialHeading, heading) < 0.5)
                                mode = STOP;

                        break;

                        case ALIGNING:
                        vc.setSpeed(0.5);
                        if((absTraveledPath - currentTraveledPath) > carLength * 0.625){
                                mode = STOPPING;
                                initialHeading = heading;
                        }
                        
                        break;

                        case STOPPING:
                        vc.setSpeed(0);
                        if(vd.getSpeed() < 0.01){
                                mode = BACK_RIGHT;
                                currentTraveledPath = absTraveledPath;
                        }

                        break;

                        case STOP:
                        vc.setSpeed(0);
                        break;

                        case ABORT:
                        vc.setSpeed(0.5);
                        if((absTraveledPath - currentTraveledPath) > carLength * 0.67)
                                mode = SCANNING;
                        break;

                }
                cerr << "Heading: " << heading * Constants::RAD2DEG << endl;
                cerr << "Mode: " << mode << endl;
                cerr << "absTraveledPath" << absTraveledPath << endl;

                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }

        double Parker::angleDifference(double initialHeading, double heading){

                initialHeading = abs(initialHeading) * Constants::RAD2DEG;
                heading = abs(heading) * Constants::RAD2DEG;

                if (heading < initialHeading)
                        return heading + 360 - initialHeading;
                return heading - initialHeading;
        }
} // msv

