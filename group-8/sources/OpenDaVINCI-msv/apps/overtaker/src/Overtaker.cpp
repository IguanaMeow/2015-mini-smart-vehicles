/**
 * Overtaker - Sample application for calculating steering and acceleration commands.
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

#include "Overtaker.h"

 namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;

    Overtaker::Overtaker(const int32_t &argc, char **argv) :
    ConferenceClientModule(argc, argv, "Overtaker"),
    m_counter(0),
    m_steering(25),
    m_sensor(0),
    m_speed(1.5)
    {}

    Overtaker::~Overtaker() {}

    void Overtaker::setUp() {
        // This method will be call automatically _before_ running body().
    }

    void Overtaker::tearDown() {
        // This method will be call automatically _after_ return from body().
        
        VehicleControl vc;

        for (int i = 0; i < 10; ++i)
        {
            vc.setSpeed(0);
            vc.setSteeringWheelAngle(0);
        }
        Container c(Container::VEHICLECONTROL, vc);
    // Send container.
        getConference().send(c);


    }

    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Overtaker::body()   

    {

        KeyValueConfiguration kv = getKeyValueConfiguration();
        m_carLength = kv.getValue<double> ("global.carLength");
        int m_state = m_followLane;
        double m_initialHeading;
        double m_currentHeading;
        double m_initialAbsPath;
        double m_currentAbsPath;

        //int m_currentSteering;

        while (getModuleState() == ModuleState::RUNNING) {

        // Setting up different containers and getting data from them.
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();
            Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
            VehicleData vd = containerVehicleData.getData<VehicleData> ();

            VehicleControl vc;

            m_currentHeading = vd.getHeading();
            m_currentAbsPath = vd.getAbsTraveledPath();

            

            vc.setSpeed(m_speed);

        // if ultrasonic front exist.
            if(sbd.containsKey_MapOfDistances(3))
            {

                switch(m_state)
                {
                    case m_followLane : 
                        // && (m_currentSteering > -3 || m_currentSteering < 3)
                        if(sbd.getValueForKey_MapOfDistances(3) < (m_carLength * 1.75) && sbd.getValueForKey_MapOfDistances(3) > 0)
                        {   
                            cerr << "------Turnout because :" << endl;
                            cout << "Value from Ultrasonic Front ("<<(m_carLength * 1.75)<< ") and (greater than 0): " << sbd.getValueForKey_MapOfDistances(3) << endl;
                            m_initialHeading = m_currentHeading;
                            cout << "initial heading = " << m_initialHeading * Constants::RAD2DEG << endl;
                            cout << "m_currentHeading = " << m_currentHeading * Constants::RAD2DEG << endl;
                            vc.setSpeed(0);
                            m_state = m_turnOut;
                        }
                        vc.setSteeringWheelAngle(sd.getHeadingData());
                        //m_currentSteering = (sd.getHeadingData() * Constants::RAD2DEG);
                        break;
                    case m_turnOut :
        // Turn out to left lane until InfraRed sensor detects the object, Keep incrementing counter to know how much to turn back later.
                        
                        if(sbd.getValueForKey_MapOfDistances(m_sensor) > 0 || angleDifference(m_initialHeading, m_currentHeading) > 8)
                        {   
                            cerr << "-----Straighten in left lane because :" << endl;
                            cout << "Value from Front InfraRed (greater than 0)" << sbd.getValueForKey_MapOfDistances(0) << endl;
                            cerr << "ANGLE DIFFERENCE (greater than 8):   " << angleDifference(m_initialHeading, m_currentHeading) << endl;
                            m_initialHeading = m_currentHeading;
                            m_initialAbsPath = m_currentAbsPath;
                            m_state = m_straighten;
                            break;
                        }
                        //++m_counter;
                        vc.setSteeringWheelAngle(-(m_steering + 1) * Constants::DEG2RAD);
                        break;

                    case m_straighten : 
        // Turn car fully until Infrared front detects the object is close enough, This is done to align car to the object.
                        m_steering = 25;
                        if((sbd.getValueForKey_MapOfDistances(0) < (m_carLength * 0.5) && sbd.getValueForKey_MapOfDistances(0) > 0) || angleDifference(m_initialHeading, m_currentHeading) < -15)
                        {
                            cerr << "-------left lane, lane following Because: " << endl;
                            cout << "Value from Front InfraRed" << sbd.getValueForKey_MapOfDistances(0) << endl;
                            cerr << "ANGLE DIFFERENCE less than -15: " << angleDifference(m_initialHeading, m_currentHeading) << endl;
                            m_initialHeading = m_currentHeading;
                            m_state = m_turnBack;
                        }
                        else 
                        {                              
                            vc.setSteeringWheelAngle(m_steering * Constants::DEG2RAD);
                        }
                        break;
                    case m_turnBack :
                        // If Front InfraRed and Front Right Ultrasonic is not detecting an object turn back half of counter. Go back to lanefollowing.
                        
                        if((m_currentAbsPath - m_initialAbsPath) > 0.3 && sbd.getValueForKey_MapOfDistances(0) < 0 &&
                            (sbd.getValueForKey_MapOfDistances(4) > (1.25 * m_carLength) || sbd.getValueForKey_MapOfDistances(4) < 0))
                        {
                            cerr << "-----return to right lane (swing right) because:" << endl;
                            cout << "Distance travelled: " << (m_currentAbsPath - m_initialAbsPath) << endl;
                            cout << "Value from Ultrasonic Front Right" << sbd.getValueForKey_MapOfDistances(3) << endl;
                            cout << "Value from Front InfraRed" << sbd.getValueForKey_MapOfDistances(0) << endl;
                            m_initialHeading = m_currentHeading;
                            cout << "initial heading = " << m_initialHeading * Constants::RAD2DEG << endl;
                            cout << "m_currentHeading = " << m_currentHeading * Constants::RAD2DEG << endl;
                            m_state = m_returnNormal;

                        }else{
                            vc.setSteeringWheelAngle(sd.getHeadingData());
                            
                        }
                        break;
                    case m_returnNormal :

                        m_steering = 25;
                        if(angleDifference(m_initialHeading, m_currentHeading) > -35)
                        {
                            vc.setSteeringWheelAngle(m_steering * Constants::DEG2RAD);
                        }
                        else 
                        {   
                            cerr << "-----Follow lane because:" << endl;
                            //cerr << "Value from Ultrasonic Front Right: " <<sbd.getValueForKey_MapOfDistances(4) << endl;
                            //cerr << "IR front right less thsan 0" << sbd.getValueForKey_MapOfDistances(0) << endl;
                            cerr << "ANGLE DIFFERENCE less than -35: " << angleDifference(m_initialHeading, m_currentHeading) << endl;
                            m_state = m_followLane;   
                        }
                        break;
                }
            }
            // Create container for finally sending the data.
            Container c(Container::VEHICLECONTROL, vc);
            // Send container.
            getConference().send(c);
        }



        return ModuleState::OKAY;
    }

    double Overtaker::angleDifference(double initialHeading, double heading){

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

