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

        void Parker::setUp() {}

        void Parker::tearDown() {}

        ModuleState::MODULE_EXITCODE Parker::body() {

                int mode = SCANNING;
                int counter = 0;
                double currentTraveledPath; // Marker for a certain position
                double absTraveledPath; 
                double initialHeading; // Heading when parking was initiated
                double heading;
                double IRFrontRight;
                double USFrontRight;
                double USFrontCenter;
                double IRRear;
                double steeringAngle;


                KeyValueConfiguration kv = getKeyValueConfiguration();
                double carLength = kv.getValue<double> ("global.carLength");
                

	        while (getModuleState() == ModuleState::RUNNING) {

                // Get most recent vehicle data:
                Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
                VehicleData vd = containerVehicleData.getData<VehicleData> ();
                cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

                // Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

                // Get most recent steering data as fill from lanedetector for example:
                Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                SteeringData sd = containerSteeringData.getData<SteeringData> ();
                cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

                VehicleControl vc;
                IRFrontRight = sbd.getValueForKey_MapOfDistances(0);
                IRRear = sbd.getValueForKey_MapOfDistances(1);
                USFrontCenter = sbd.getValueForKey_MapOfDistances(3);
                USFrontRight = sbd.getValueForKey_MapOfDistances(4);
                absTraveledPath = vd.getAbsTraveledPath();
                heading = vd.getHeading();
                steeringAngle = sd.getHeadingData();
                
                switch (mode) {

                        case SCANNING:
                        vc.setSpeed(2);
                        vc.setSteeringWheelAngle(steeringAngle);
                        
                        // If IR sensor and US sensor is far enough change to measuring mode
                        if(IRFrontRight < 0 && (USFrontRight < 0 || USFrontRight > carLength * 2)){
                                mode = MEASURING;
                                currentTraveledPath = absTraveledPath;
                        }
                        break;

                        // Mode for measuring the distance of the detected gap
                        case MEASURING:
                        vc.setSpeed(1);
                        vc.setSteeringWheelAngle(steeringAngle);
                        
                        // If something is detected during measuring, go back to SCANNING mode
                        if (absTraveledPath - currentTraveledPath >= carLength * 1.50){
                                mode = ALIGNING;
                                currentTraveledPath = absTraveledPath;
                        }

                        else if(IRFrontRight > -1 || steeringAngle > 10 || steeringAngle < -10){
                                mode = SCANNING;
                        }
                        // If gap is wide enough change mode to ALIGNING
                        
                        break;

                        // Mode for aligning the car to park
                        case ALIGNING:
                        vc.setSpeed(1);
                        
                        // Car has traveled enough to stop and park
                        if((absTraveledPath - currentTraveledPath) > carLength * 0.9){
                                counter = 0;
                                mode = STOPPING;
                                initialHeading = heading;
                        }
                        break;


                        // Mode for stopping car to reverse
                        case STOPPING:
                        ++counter;
                        vc.setSpeed(0);

                        // Enough time has passed to reverse
                        if(counter >= 30){
                                mode = BACK_RIGHT;
                                currentTraveledPath = absTraveledPath;
                        }

                        break;
                        
                        // Mode to back and turn right
                        case BACK_RIGHT:
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(25 * Constants::DEG2RAD);
                        
                        // When heading has changed enough, change mode to BACK_STRAIGHT
                        if (angleDifference(initialHeading, heading) > 40){
                                mode = BACK_STRAIGHT;
                                currentTraveledPath = absTraveledPath;
                                vc.setSteeringWheelAngle(0);
                        }
                        // STOP if something is detected in the rear
                        if(IRRear < carLength * 0.10 && IRRear > 0){
                                mode = STOP;
                                currentTraveledPath = absTraveledPath;
                                vc.setSpeed(0);
                        }

                        break;

                        // Mode to back straight
                        case BACK_STRAIGHT:
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(0);

                        // Change mode to BACK_LEFT when car traveled enough distance
                        if((absTraveledPath - currentTraveledPath) > carLength * 0.6){
                                mode = BACK_LEFT;
                                currentTraveledPath = absTraveledPath;
                        }

                        break;

                        // Mode to back and turn left
                        case BACK_LEFT:
                        vc.setSpeed(-0.5);
                        vc.setSteeringWheelAngle(-26 * Constants::DEG2RAD);
                        
                        // When car is almost parallel or IR rear detects object close enough, change mode to STRAIGHTEN
                        if(angleDifference(initialHeading, heading) < 5 || (IRRear < (carLength * 0.5) && IRRear > 0)){
                                mode = STRAIGHTEN;
                        }

                        break;

                        // Mode to straighten the car
                        case STRAIGHTEN:
                        vc.setSteeringWheelAngle(25 * Constants::DEG2RAD);
                        vc.setSpeed(0.5);
                        
                        // Change mode to STOP is the car is parallel to road
                        if(angleDifference(initialHeading, heading) < 0.5){
                                mode = STOP;
                        }
                        // Change mode back to BACK_LEFT if US front detects object close enough
                        else if(USFrontCenter < (0.5 * carLength) && USFrontCenter > 0){
                                mode = BACK_LEFT;
                        }

                        break;

                        // Car is done
                        case STOP:
                        vc.setSpeed(0);
                        break;

                }

                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
	        }

	        return ModuleState::OKAY;
        }




        /*
        *       Function to calculate the difference between two angles
        */
        double Parker::angleDifference(double initialHeading, double heading){

                double difference = heading * Constants::RAD2DEG - initialHeading * Constants::RAD2DEG;
                double absDifference = abs(difference);

                if (absDifference <= 180)
                        return difference;

                else if (heading > initialHeading)
                        return absDifference - 360;

                else
                        return 360 - absDifference;
        }

} // msv

