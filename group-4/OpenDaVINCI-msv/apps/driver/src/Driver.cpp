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
#include "core/wrapper/Time.h"

#include "GeneratedHeaders_Data.h"

#include "Driver.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;
    using core::wrapper::Time;
    using core::wrapper::TimeFactory;
//  using namespace libdata::generated::msv;

    int parkingMode = 0;
    float usFrontRightArr[5] = {-1};
    float usRearRightArr[5] =  {-1};
    float usFrontCentreArr[5] =  {-1};
    float irFrontRightArr[5] = {-1};
    float irRearArr[5] = {-1};
    float irRearRightArr[5] = {-1};

    Driver::Driver(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "Driver"),parking()   {
        for(int i = 0; i < argc; i++)
            if(!strcmp(argv[i], "--parking"))
                parkingMode = 1;
        cout << "PARKING: " << parkingMode << endl;
    }

    Driver::~Driver() {}

    void Driver::setUp() {
        // This method will be called automatically _before_ running body().
    }

    void Driver::tearDown() {
        // This method will be called automatically _after_ returning from body().
        VehicleControl vc;
        vc.setSpeed(0);
        vc.setSteeringWheelAngle(0);
        Container c(Container::VEHICLECONTROL, vc);
        getConference().send(c);
    }

    float Driver::filter(float newData,float arr[]){

        for (int i = 0; i < 4; ++i)
        {   
            arr[i] = arr[i+1];
        }
        arr[4] = newData;
        sort(arr, arr + 5);
        return arr[2];
    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Driver::body()
    {
        Time *startTime = NULL;
        bool isReversing = false;

        while (getModuleState() == ModuleState::RUNNING)
        {
            // 1. Get most recent vehicle data:
            VehicleData vd;
            Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
            vd = containerVehicleData.getData<VehicleData> ();
            float headingAngle = vd.getHeading() * Constants :: RAD2DEG;
            float currentSpeed = vd.getSpeed();

            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            sbd.setNumberOfSensors(6);
            
            
        
            float usFrontRightData = sbd.getValueForKey_MapOfDistances(US_FrontRight);
            float usRearRightData = sbd.getValueForKey_MapOfDistances(US_RearRight);
            float usFrontCentreData = sbd.getValueForKey_MapOfDistances(US_FrontCenter);
            float irFrontRightData = sbd.getValueForKey_MapOfDistances(IR_FrontRight);
            float irRearData = sbd.getValueForKey_MapOfDistances(IR_Rear);
            float irRearRightData = sbd.getValueForKey_MapOfDistances(IR_RearRight);

            float usFrontRight = filter(usFrontRightData,usFrontRightArr);
            float usRearRight = filter(usRearRightData,usRearRightArr);
            float usFrontCentre = filter(usFrontCentreData,usFrontCentreArr);
            float irFrontRight = filter(irFrontRightData,irFrontRightArr);
            float irRear = filter(irRearData,irRearArr);
            float irRearRight = filter(irRearRightData,irRearRightArr);


            // 3. Get most recent user button data:
            //Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
            //UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
            //cout << "Most recent user button data: '" << ubd.toString() << "'" << endl;

            // 4. Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();

            cout << left << setfill('-') << setw(30) << "+" << "+" << endl << setfill(' ');
            cout << setw(30) << "| VEHICLE & SENSOR DATA" << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(8) << "+" << "+" << endl << setfill(' ');
            cout << setw(22) << "| Heading Angle"       << "| " << setw(6) << headingAngle  << "|" << endl;
            cout << setw(22) << "| current Speed"       << "| " << setw(6) << currentSpeed  << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(8) << "+" << "+" << endl << setfill(' ');
            cout << setw(22) << "| usFrontCentre value" << "| " << setw(6) << usFrontCentre << "|" << endl;
            cout << setw(22) << "| usFrontRight value"  << "| " << setw(6) << usFrontRight  << "|" << endl;
            cout << setw(22) << "| usRearRight value"   << "| " << setw(6) << usRearRight   << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(8) << "+" << "+" << endl << setfill(' ');
            cout << setw(22) << "| irFrontRight value"  << "| " << setw(6) << irFrontRight  << "|" << endl;
            cout << setw(22) << "| irRearRight value"   << "| " << setw(6) << irRearRight   << "|" << endl;
            cout << setw(22) << "| irRear value"        << "| " << setw(6) << irRear        << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(8) << "+" << "+" << endl << setfill(' ');

            float speed = 1;
            float desiredSteeringWheelAngle = 0.0;


            if(parkingMode)
            {
                if(parking.hasGap(usRearRight, irFrontRight,irRearRight))
                {
                    speed = 0;
                    cout << "stop stop stop stop " << endl;
                }

                if (parking.canTurnRight(usFrontRight,irFrontRight,irRearRight))
                {
                    speed = -3;
                    desiredSteeringWheelAngle = 26.0;
                    cout << "turn right turn right turn right" << endl;
                }

                if (parking.canTurnLeft(irRear,usRearRight))
                {
                    speed = -3;
                    desiredSteeringWheelAngle = -26;
                    cout << "turn left turn left turn left " <<endl;
                }

                if (parking.stop(usRearRight, irRear))
                {
                    speed = 0;
                    desiredSteeringWheelAngle = 0;
                    cout << "stop stop stop stop " <<endl;
                }
            }
            else
            {
                desiredSteeringWheelAngle = sd.getWheelAngle();
				// emergency break
				if(usFrontCentre < 10 && usFrontCentre > 0)
				{
					speed = 0;
				}
            }

            VehicleControl vc;

            //Reversing car fix.
            if (speed < 0 && !isReversing) {
                Time *t = TimeFactory::getInstance().now();

                if (startTime == NULL) {
                    startTime = t;
                    speed = -3;
                } else {
                    if ((t->getSeconds() - startTime->getSeconds()) <= 1) {
                        speed = -3;
                    } else if ((t->getSeconds() - startTime->getSeconds()) > 3) {
                        delete startTime;
                        startTime = NULL;
                        speed = -3;
                        isReversing = true;
                    } else {
                        speed = 0;
                    }
                    if (t != startTime) {
                        delete t;
                    }
                    t = NULL;
                }

            } else if (speed > 0 && isReversing) {
                isReversing = false;
                delete startTime;
                startTime = NULL;
            }

            vc.setSpeed(speed);
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
}
