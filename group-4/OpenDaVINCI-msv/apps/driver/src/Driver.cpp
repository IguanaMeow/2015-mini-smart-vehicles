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
//        using namespace libdata::generated::msv;






    
    Driver::Driver(const int32_t &argc, char **argv) : 
            ConferenceClientModule(argc, argv, "Driver"),parking()   {

    }

    Driver::~Driver() {}

    void Driver::setUp() {
           //VehicleControl vc;
            //vc.setSpeed(0.0);
            // This method will be call automatically _before_ running body().
    }

    void Driver::tearDown() {
            
            // This method will be call automatically _after_ return from body().
            vc.setSpeed(0);
            vc.setSteeringWheelAngle(0);
            Container c(Container::VEHICLECONTROL, vc);
            getConference().send(c);
    }



   


    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Driver::body() {
	int countOfWhileLoop = 0;
        //float back = Constants::PI + (Constants::PI/2);
        while (getModuleState() == ModuleState::RUNNING) {
            // In the following, you find example for the various data sources that are available:

            // 1. Get most recent vehicle data:
            VehicleData vd;
	    VehicleControl vc;
            Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
            vd = containerVehicleData.getData<VehicleData> ();
            float headingAngle = vd.getHeading() * Constants :: RAD2DEG;

            float currentSpeed = vd.getSpeed();


            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            sbd.setNumberOfSensors(6);
            

            //float usFrontCentre;
            float usFrontRight = sbd.getValueForKey_MapOfDistances(US_FrontRight);
            float usRearRight = sbd.getValueForKey_MapOfDistances(US_RearRight);
            float usFrontCentre = sbd.getValueForKey_MapOfDistances(US_FrontCenter);
            float irFrontRight = sbd.getValueForKey_MapOfDistances(IR_FrontRight);
            float irRear = sbd.getValueForKey_MapOfDistances(IR_Rear);
            float irRearRight = sbd.getValueForKey_MapOfDistances(IR_RearRight);


           

            cerr << "Heading Angle:       |" << headingAngle << "|"<< endl; 
            cerr << "current Speed:       |" << currentSpeed << "|"<< endl;
            cerr << "----------------------" << endl;
            cerr << "usFrontCentre value: |" << usFrontCentre<< "|"<< endl;
            cerr << "----------------------" << endl;
            cerr << "usFrontRight value:  |" << usFrontRight << "|"<< endl;
            cerr << "----------------------" << endl;
            cerr << "usRearRight value:  |" << usRearRight  << "|"<< endl;
            cerr << "----------------------" << endl;
            cerr << "irFrontRight value:  |" << irFrontRight << "|"<< endl;
            cerr << "----------------------" << endl;
            cerr << "irRearRight value:   |" << irRearRight  << "|"<< endl;
            cerr << "----------------------" << endl;
            cerr << "irRear value:        |" << irRear       << "|"<< endl;
            cerr << "----------------------" << endl;
            
            
            //cerr << "Most recent sensor board data: " << sbd.toString() << endl;
            

            // 3. Get most recent user button data:
            //Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
            //UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
            //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

            // 4. Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();
            cout << "Wheel Angle          |" << sd.getWheelAngle() << endl;
            cerr << "----------------------------------------" << endl << endl;

            // Design your control algorithm here depending on the input data from above.


	    if (countOfWhileLoop > 60) {
			countOfWhileLoop = 0;		
		} else if (countOfWhileLoop > 30) { //frequency = 30, therefore every 30th time it executes this code
			vc.setSpeed(0);
		}

            /* ---------------------   side way parking area --------------------- */
            float speed;
            float desiredSteeringWheelAngle = 0.0;
            


            speed = 1;
            
            if(parking.hasGap(usRearRight, irFrontRight,irRearRight)){

                speed = 0;
                cerr << "stop stop stop stop " << endl;

            } 

            if (parking.canTurnRight(usFrontRight,irFrontRight,irRearRight)){
                
                speed = -0.5;
                desiredSteeringWheelAngle = 26.0;
                cerr << "turn right turn right turn right" << endl;

            }

            if (parking.canTurnLeft(irRear,usRearRight)){

                speed = -0.5;
                desiredSteeringWheelAngle = -26;
                cerr << "turn left turn left turn left " <<endl;
                
            }

            if (parking.stop(usRearRight, irRear)){

                speed = 0;
                desiredSteeringWheelAngle = 0;
                cerr << "stop stop stop stop " <<endl;
            }







            /* ---------------------   side way parking area END --------------------- */


            // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
            //vc.setSpeed(10);
            vc.setSpeed(speed);

            // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
            //vc.setSteeringWheelAngle(sd.getWheelAngle() * Constants::DEG2RAD);
            
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
