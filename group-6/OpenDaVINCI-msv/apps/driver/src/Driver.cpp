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

        double overtaking = 0;  // time when we started overtaking
        bool prevFound;  // was there an object found last loop?
        double speed = 2,
            angle = 0;

        Driver::Driver(const int32_t &argc, char **argv) :
          ConferenceClientModule(argc, argv, "Driver") {
        }

        Driver::~Driver() {}

        void Driver::setUp() {
          
        }

        void Driver::tearDown() {}

        // This method will do the main data processing job.
        ModuleState::MODULE_EXITCODE Driver::body() {

          while (getModuleState() == ModuleState::RUNNING) {
                // In the following, you find example for the various data sources that are available:

            Container containerSBD = getKeyValueDataStore().get(Container::USER_DATA_0);
            SteeringData sd = getKeyValueDataStore().get(Container::USER_DATA_1).getData<SteeringData>();

            SensorBoardData sbd = containerSBD.getData<SensorBoardData>();
            double distanceFront = sbd.getValueForKey_MapOfDistances(3);
            VehicleControl vc;

            // not overtaking? just follow the lane
            if((int)overtaking == 0 && (distanceFront < 0 || distanceFront > 10)) {
                
                angle = sd.getExampleData();
                cout << "angle" << angle<< endl;
                speed = sd.getSpeedData();

                vc.setSpeed(speed);
                vc.setSteeringWheelAngle(angle * Constants::DEG2RAD);

                Container c(Container::VEHICLECONTROL, vc);
                getConference().send(c);
                continue;
            }

            bool objectFound = (sbd.getValueForKey_MapOfDistances(4) > 0 &&
                                sbd.getValueForKey_MapOfDistances(4) < 30) ||
                               (sbd.getValueForKey_MapOfDistances(0) > 0 &&
                                sbd.getValueForKey_MapOfDistances(0) < 30);

            cout << time(0) - overtaking << endl;

            // BEGINNING
            if((int)overtaking == 0) {
                cout <<"Enter overtaking: " << endl;
                angle = -15;
                speed = 0.5;
                overtaking = time(0);
            }
            // RESTART LANE DETECTION 
            else if(time(0) - overtaking > 10) {
                cout << "turn back HEEERE! " << overtaking << " " << time(0) << endl;
                angle = 0;
                speed = 2;
                if((prevFound && !objectFound) && time(0) - overtaking > 13) overtaking = 0; // this is when!!
                angle = 12;
                angle = sd.getExampleData();
            }
            // AFTER BEGINNING  
            else if(time(0) - overtaking > 3) {
                angle = 15;
            }  

            


            prevFound = objectFound;

            vc.setSteeringWheelAngle(angle);
            vc.setSpeed(speed);

            Container d(Container::VEHICLECONTROL, vc);
            getConference().send(d);
          }

          return ModuleState::OKAY;
        }
} // msv
//                      ID Sensors
// //                 0 = Infrared_FrontRight
// //                 1 = Infrared_Rear
// //                 2 = Infrared_RearRight

// //                 3 = UltraSonic_FrontCenter
// //                 4 = UltraSonic_FrontRight
// //                 5 = UltraSonic_RearRight
// //                  
