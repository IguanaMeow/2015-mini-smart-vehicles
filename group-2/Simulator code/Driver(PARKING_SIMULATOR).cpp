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

double freeSpaceStart = 0;
double freepath;
double back0 = 0;
double back1 = 0;
int mode = 1;
double forward = 0;

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
            
            
            
            // Create vehicle control data.
            VehicleControl vc;
            
            
            //CODE WRITTEN BY JOHAN HERMANSSON, EMIL SUNDKLEV
            
            //get all data from the sensors
            double abt = vd.getAbsTraveledPath();
            double sen6 = sbd.getValueForKey_MapOfDistances(0);
            double sen = sbd.getValueForKey_MapOfDistances(1);
            double sen2 = sbd.getValueForKey_MapOfDistances(2);
            double sen3 = sbd.getValueForKey_MapOfDistances(3);
            double sen4 = sbd.getValueForKey_MapOfDistances(4);
            double sen5 = sbd.getValueForKey_MapOfDistances(5);
            double desiredSteeringWheelAngle = 0;
            cerr <<sen<< "Rear IR"<<endl;
            cerr <<sen2<< "Rear right IR"<<endl;
            cerr <<sen6<< "Front right IR"<<endl;
            cerr <<sen3<< "Front sonic"<<endl;
            cerr <<sen4<< "Front right sonic"<<endl;
            cerr <<sen5<< "Rear right sonic"<<endl;
            cerr <<abt<< "Traveled path"<<endl;
            cerr <<freeSpaceStart<< "FreeStart path"<<endl;
            
            
            switch(mode){
                case 1://start-case, searching for empty parking-spot
                    vc.setSpeed(1.5);
                    desiredSteeringWheelAngle = 0;
                    vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                    if (sen2 > 0){ // if the sensor detect anything, reset the freeSpaceStart,
                        freeSpaceStart = 0;
                    }
                    else if(sen2 < 0){ //if the sensor don't detect anything,
                        if(freeSpaceStart <= 0){ //if freeSpaceStart been reset, set it to the AbsTraveledPath
                            freeSpaceStart = abt;
                            cerr <<freeSpaceStart<< "FreeStart path set"<<endl;
                        }
                        freepath = abt - freeSpaceStart;
                        cerr <<freepath<< "Free = "<<endl;
                        if(freepath >= 6.5){ //checks if the spot is big enough for the car to park.
                            vc.setSpeed(0.0);
                            mode = 5; //goes to mode 5 if the car can park.
                        }
                    }
                    else{
                    }
                    break;
                case 2://(case 3) car reverse full right 5.8 into the gap.
                    if (back0 <= 0){
                        back0 = abt;
                    }
                    if ((abt - back0) >= 5.8){
                        back0 = 0;
                        mode = 7;//goes to mode 7 when its complete
                    }
                    desiredSteeringWheelAngle = 25;
                    vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                    vc.setSpeed(-0.3);
                    break;
                case 3://(case 5) car reverse full left until the back-sensor detect an object less than 1.9 from the car.
                    if (back1 <= 0){
                        back1 = abt;
                    }
                    if (sen > 0 && sen < 1.9){
                        forward = 0;
                        mode = 6;//goes to mode 6 when comlete
                    }
                    desiredSteeringWheelAngle = -30;
                    vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                    vc.setSpeed(-0.2);
                    break;
                case 4://(case 8) parking done(car stand straight in the spot) Final case
                    vc.setSpeed(0.0);//stand still.
                    break;
                case 5://(case 2) move the car forward 2.25
                    if(forward <= 0) forward = abt;
                    vc.setSpeed(0.3);
                    if ((abt - forward) >= 2.25) mode = 2;//if completed go to mode 2
                    break;
                case 6://(case 6) moves forward 1.5
                    if(forward <= 0) forward = abt;
                    if((abt - forward) > 1.5) mode = 8;//goes to mode 8 when comlete
                    back0 = 0;
                    desiredSteeringWheelAngle = 25;
                    vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                    vc.setSpeed(0.3);
                    break;
                case 7://(case 4) car reverse straight 1.4
                    if (back0 <= 0){
                        back0 = abt;
                    }
                    if ((abt - back0) >= 1.4){
                        back0 = 0;
                        mode = 3;//goes to mode 3 when comlete
                    }
                    desiredSteeringWheelAngle = 0;
                    vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                    vc.setSpeed(-0.3);
                    break;
                case 8://(case 7) reverse 1.0
                    if (back0 <= 0){
                        back0 = abt;
                    }
                    if ((abt - back0) >= 1.0){
                        back0 = 0;
                        mode = 4;//goes to mode 4 when comlete
                    }
                    desiredSteeringWheelAngle = -20;
                    vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);
                    vc.setSpeed(-0.2);
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
