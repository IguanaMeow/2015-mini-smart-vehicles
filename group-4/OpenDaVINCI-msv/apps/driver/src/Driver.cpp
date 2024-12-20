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

    int parkingMode = 0;
    int overtakingActive = 0;
    const int samplesize = 25;


    Driver::Driver(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "Driver"),parking(),overtaking()   {
        for(int i = 0; i < argc; i++)
            if(!strcmp(argv[i], "--parking"))
                parkingMode = 1;
            else if(!strcmp(argv[i], "--overtaking"))
                overtakingActive = 1;
        cout << "PARKING: " << parkingMode << endl;
        cout << "OVERTAKING: " << overtakingActive << endl;
    }

    Driver::~Driver() {}

    void Driver::setUp() {
    }

    void Driver::tearDown() {
        VehicleControl vc;
        vc.setSpeed(0);
        vc.setSteeringWheelAngle(0);
        Container c(Container::VEHICLECONTROL, vc);
        getConference().send(c);
    }

    float Driver::filter(float newData,float arr[]){

        float temp[samplesize];
        for (int i = 0; i < samplesize; ++i)
        {
            temp[i] = arr[i];

        }
        sort(temp, temp + samplesize);
        for (int i = 0; i < samplesize - 1; ++i)
        {   
            arr[i] = arr[i+1];

        }
        arr[samplesize - 1] = newData;

        return temp[samplesize / 2];

    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Driver::body()
    {
        Time *startTime = NULL;
        bool isReversing = false;
        bool atStopline = false;


        float usFrontRightArr[samplesize] = {-1};
        float usRearRightArr[samplesize] =  {-1};
        float usFrontCentreArr[samplesize] =  {-1};
        float irFrontRightArr[samplesize] = {-1};
        float irRearArr[samplesize] = {-1};
        float irRearRightArr[samplesize] = {-1};

        while (getModuleState() == ModuleState::RUNNING)
        {
            // 1. Get most recent vehicle data:
            VehicleData vd;
            Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
            vd = containerVehicleData.getData<VehicleData> ();
            float pathTraveled = vd.getAbsTraveledPath();
            //float headingAngle = vd.getHeading() * Constants :: RAD2DEG;
            //float currentSpeed = vd.getSpeed();

            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            sbd.setNumberOfSensors(6);
            
            float usFrontCentreData = sbd.getValueForKey_MapOfDistances(US_FrontCenter);
            float usFrontRightData = sbd.getValueForKey_MapOfDistances(US_FrontRight);
            float usRearRightData = sbd.getValueForKey_MapOfDistances(US_RearRight);
            float irFrontRightData = sbd.getValueForKey_MapOfDistances(IR_FrontRight);
            float irRearRightData = sbd.getValueForKey_MapOfDistances(IR_RearRight);
            float irRearData = sbd.getValueForKey_MapOfDistances(IR_Rear);

            float usFrontCentre = filter(usFrontCentreData,usFrontCentreArr);
            float usFrontRight = filter(usFrontRightData,usFrontRightArr);
            float usRearRight = filter(usRearRightData,usRearRightArr);
            float irFrontRight = filter(irFrontRightData,irFrontRightArr);
            float irRearRight = filter(irRearRightData,irRearRightArr);
            float irRear = filter(irRearData,irRearArr);

            if(irFrontRight > 20)
                irFrontRight = -1;
            if(irRearRight > 20)
                irRearRight = -1;
            if(irRear > 20)
                irRear = -1;

            float sensorData[6] = {
                usFrontCentre,
                usFrontRight,
                usRearRight,
                irFrontRight,
                irRearRight,
                irRear
            };

            // 3. Get most recent user button data:
            //Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
            //UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
            //cout << "Most recent user button data: '" << ubd.toString() << "'" << endl;

            // 4. Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();

            double speed = 3;
            double steeringWheelAngle = 0.0;

            if(parkingMode)
            {
                parking.doParking(speed, steeringWheelAngle, pathTraveled, sensorData);
            }
            else
            {
                steeringWheelAngle = sd.getWheelAngle();

                if(overtakingActive)
                {
                    overtaking.doOvertaking(pathTraveled, usFrontRight, irFrontRight, usFrontCentre, steeringWheelAngle);
                }
                // Emergency break
                else if(usFrontCentre < 10 && usFrontCentre > 0)
                {
                    speed = 0;
                }

                // Stopping at stoplines
                if(!atStopline && sd.getFrontDistance() < 40)
                {
                    atStopline = true;
                    Time *t = TimeFactory::getInstance().now();
                    startTime = t;
                }

                if(atStopline)
                {
                    Time *t = TimeFactory::getInstance().now();
                    int sec = t->getSeconds() - startTime->getSeconds();
                    if(sec > 6)
                    {
                        delete startTime;
                        startTime = NULL;
                        atStopline = false;
                    }
                    else if (sec < 3)
                    {
                        speed = 0;
                    }

                    if (t != startTime)
                    {
                        delete t;
                    }
                    t = NULL;
                }
            }

            VehicleControl vc;

            // Reversing car fix.
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
            vc.setSteeringWheelAngle(steeringWheelAngle * Constants::DEG2RAD);

            // You can also turn on or off various lights:
            //vc.setBrakeLights(false);
            //vc.setLeftFlashingLights(false);
            //vc.setRightFlashingLights(false);

            cout << left << setfill('-') << setw(34) << "+" << "+" << endl << setfill(' ');
            cout << setw(34) << "| VEHICLE & SENSOR DATA" << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(12) << "+" << "+" << endl << setfill(' ');
            cout << setw(22) << "| Speed"               << "| " << setw(10) << speed         << "|" << endl;
            cout << setw(22) << "| Heading Angle"       << "| " << setw(10) << steeringWheelAngle << "|" << endl;
            cout << setw(22) << "| Front Distance"      << "| " << setw(10) << sd.getFrontDistance() << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(12) << "+" << "+" << endl << setfill(' ');
            cout << setw(22) << "| usFrontCentre value" << "| " << setw(10) << usFrontCentre << "|" << endl;
            cout << setw(22) << "| usFrontRight value"  << "| " << setw(10) << usFrontRight  << "|" << endl;
            cout << setw(22) << "| usRearRight value"   << "| " << setw(10) << usRearRight   << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(12) << "+" << "+" << endl << setfill(' ');
            cout << setw(22) << "| irFrontRight value"  << "| " << setw(10) << irFrontRight  << "|" << endl;
            cout << setw(22) << "| irRearRight value"   << "| " << setw(10) << irRearRight   << "|" << endl;
            cout << setw(22) << "| irRear value"        << "| " << setw(10) << irRear        << "|" << endl;
            cout << setfill('-') << setw(22) << "+" << setw(12) << "+" << "+" << endl << setfill(' ');

            // Create container for finally sending the data.
            Container c(Container::VEHICLECONTROL, vc);
            // Send container.
            getConference().send(c);
        }

        return ModuleState::OKAY;
    }
}
