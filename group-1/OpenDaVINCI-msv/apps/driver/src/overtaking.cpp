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
#define _USE_MATH_DEFINES
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

                bool obstacleFront=false;
                bool startTurningToLeft=false;
                int state=0;
                int count1=0;
                int count2=0;
                double time=3.0;
                double distance;
                std::clock_t start;
                double duration;
                int angle1=30;
                double l = 2.5;
                double distance1, result, val, degreeTeta;
                double result2, degree, finaldegree;
                int x = 35;
                double T = 3;
                double safedistance = 4, y=2.5;
                bool straight = false; 
            

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

                // 1. Get most recent vehicle data:9                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            m
                Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
                VehicleData vd = containerVehicleData.getData<VehicleData> ();
                //cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;

                // 2. Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                //cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;

                // 3. Get most recent user button data:
                Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
                UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
                //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

                // 4. Get most recent steering data as fill from lanedetector for example:
                Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
                SteeringData sd = containerSteeringData.getData<SteeringData> ();
                //cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

                // Design your control algorithm here depending on the input data from above.

                // Create vehicle control data.
                VehicleControl vc;
                double desiredSteeringWheelAngle;
                start = std::clock();
                val = M_PI/180;
                result = sin(angle1*val);
                distance1 = 1.5*l * sqrt((1/pow(result,2))-1);
                degree = safedistance/(sqrt(pow(safedistance,2)+pow(y,2)));
                result2 = asin(degree) * 180.0 / M_PI;
                finaldegree = 90 - result2;
                straight = true;
                          
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();
                         /* 
                        cout<<"x is : "<< vd.getPosition().getX()<<endl;
                        cout<<"y is : "<< vd.getPosition().getY()<<endl;
                        cout<< "following angle : "<<sd.getExampleData()<<endl;
                        */
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                if(sbd.getValueForKey_MapOfDistances(3)>distance1 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "Moving  " << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();

                    }else if(sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<distance1 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "Changing boolean"<<endl;
                        obstacleFront=true;
                        state=1;

                    }else if(state==1 && obstacleFront==true) { // check if obstacle is found in front
                        cout<< "obstacle found" << endl;
                       
                        startTurningToLeft=true;
                        obstacleFront=false;

                    }else if(startTurningToLeft==true && sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<distance1) {//&& sbd.getValueForKey_MapOfDistances(0)<0
                        cout<< "Start turning" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=-30;
                        count1++;
                        state=2;
                        
                    }else if(state==2 && sbd.getValueForKey_MapOfDistances(3)<0){
                        vc.setSpeed(2);
                        cout << "we are in lane following"<< endl;
                        desiredSteeringWheelAngle=sd.getExampleData();
                        state=3;

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(4)>0){
                     vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(2)>0){
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=sd.getExampleData();

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(2)<1 && sbd.getValueForKey_MapOfDistances(5)<2.3){// && (b2-a2<=a-b)){
                            cout<< "Start turning back" << sbd.getValueForKey_MapOfDistances(5)<< endl;
                            startTurningToLeft = false;
                            desiredSteeringWheelAngle=finaldegree;
                            cout << "the degree is : " << finaldegree << endl;
                            vc.setSpeed(2);
                            state=4;

                    }else if (state == 4 && sbd.getValueForKey_MapOfDistances(5)<safedistance){
                            cout << "in state 4 : " << sbd.getValueForKey_MapOfDistances(5) << endl;
                            desiredSteeringWheelAngle=finaldegree;
                            vc.setSpeed(2);
                            count2++;

                    }else if (state == 4 && sbd.getValueForKey_MapOfDistances(5)>safedistance){
                            cout << "in state 4 : " << sbd.getValueForKey_MapOfDistances(5) << endl;
                            vc.setSpeed(2);
                            state=5;

                    }else if(state==5 && count2>0){
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=-10;
                            count2--;
    
                    }else if(state==5 && count2==0){
                            cout<< " last count2: "<< count2<<endl;
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=sd.getExampleData();
                            state=6;
                        }
                          







                          ///////////////////////////////////////////////////////mahsa
                if(sd.getExampleData()<=-7){
                                straight=false;
                                cout << "we are in curve now" << endl;
                                vc.setSpeed(2);
                                angle1=25;
                                safedistance=4;
                                desiredSteeringWheelAngle=sd.getExampleData();
                         /* 
                        cout<<"x is : "<< vd.getPosition().getX()<<endl;
                        cout<<"y is : "<< vd.getPosition().getY()<<endl;
                        cout<< "following angle : "<<sd.getExampleData()<<endl;
                        */
                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                if(sbd.getValueForKey_MapOfDistances(3)>distance1 && obstacleFront==false && startTurningToLeft==false) {
                        cout<< "driving in curve" << endl;
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();

                    }else if(sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<distance1 && obstacleFront==false && startTurningToLeft==false) {
                            cout<< "detecting object "<<endl;
                            obstacleFront=true;
                            state=1;

                    }else if(state==1 && obstacleFront==true) { // check if obstacle is found in front
                            cout<< "obstacle found cur" << endl;
                            startTurningToLeft=true;
                            obstacleFront=false;

                    }else if(startTurningToLeft==true && sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<distance1) {//&& sbd.getValueForKey_MapOfDistances(0)<0
                            cout<< "Start turning cur" << endl;
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=-30;
                            count1++;
                            state=2;

                        
                    }else if(state==2 && sbd.getValueForKey_MapOfDistances(3)<0){
                            vc.setSpeed(2);
                            cout << "we are in lane following"<< endl;
                            desiredSteeringWheelAngle=sd.getExampleData();
                            state=3;

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(4)>0){
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=sd.getExampleData();

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(2)>0){
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=sd.getExampleData();

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(2)<1 && sbd.getValueForKey_MapOfDistances(5)<2.3){// && (b2-a2<=a-b)){
                            cout<< "Start turning back" << sbd.getValueForKey_MapOfDistances(5)<< endl;
                            startTurningToLeft = false;
                            desiredSteeringWheelAngle=finaldegree;
                            cout << "the degree is : " << finaldegree << endl;
                            vc.setSpeed(2);
                            state=4;

                    }else if (state == 4 && sbd.getValueForKey_MapOfDistances(5)<3.3){
                            cout << "10 degree " << endl;
                            desiredSteeringWheelAngle=5;
                            vc.setSpeed(2);
                            count2++;

                    }else if (state == 4 && sbd.getValueForKey_MapOfDistances(5)>3.3){
                            vc.setSpeed(2);
                            state=5;

                    }else if(state==5 && count2>0){
                            desiredSteeringWheelAngle=-35;
                            vc.setSpeed(2);
                            count2--;
                            cout << "20 degree"<< endl;
    
                    }else if(state==5 && count2==0){
                            cout<< " last count2: "<< count2<<endl;
                            desiredSteeringWheelAngle=sd.getExampleData();
                            vc.setSpeed(2);
                            state=6;
                    }
                }

                                ///////////////////////////////////////////////
                 if (sd.getExampleData()>15){
                    cout<< "we are in intersection :D:DD "<<endl;
                    straight=false;
                    //dynamic distance calculation
                    val =3.14159265/180;
                    result= sin(x*val);
                    distance1= 1.5*l * sqrt((1/pow(result,2))-1);

                    degreeTeta=T/(sqrt(pow(T,2)+pow(y,2)));
                    result2 = asin (degreeTeta) * 180.0 / M_PI;
                    finaldegree=90-result2;
                    // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)

                    if(obstacleFront==false && startTurningToLeft==false) {
                        vc.setSpeed(2);
                        desiredSteeringWheelAngle=sd.getExampleData();
                        cout<< "lanedetector     1 " << count1<<endl;
                    }

                    if(sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<distance1 && obstacleFront==false && startTurningToLeft==false) {
                        obstacleFront=true;
                        desiredSteeringWheelAngle=sd.getExampleData();
                        cout<< "lanedetector  2 " << count1<<endl;
                       
                    }else if(obstacleFront) { // check if obstacle is found in front
                            startTurningToLeft=true;
                            obstacleFront=false;

                    }else if(startTurningToLeft==true && sbd.getValueForKey_MapOfDistances(3)>0 && sbd.getValueForKey_MapOfDistances(3)<distance1 && sbd.getValueForKey_MapOfDistances(0)<0) {
                            vc.setSpeed(1);
                            desiredSteeringWheelAngle=-30;
                            count1++;
          
                    }else if(startTurningToLeft==true &&  sbd.getValueForKey_MapOfDistances(3)<0 && sbd.getValueForKey_MapOfDistances(0)<0){
                            vc.setSpeed(1);
                        
                    }else if(startTurningToLeft==true &&  sbd.getValueForKey_MapOfDistances(3)<0 && sbd.getValueForKey_MapOfDistances(0)>0){
                            startTurningToLeft=false;
                            vc.setSpeed(1);
                            state=1;

                    }else if(state==1 && sbd.getValueForKey_MapOfDistances(2)<0){
                            vc.setSpeed(1);
                            desiredSteeringWheelAngle=24;
                         

                    }else if(state==1 && sbd.getValueForKey_MapOfDistances(2)>0){
                            desiredSteeringWheelAngle=0;
                            vc.setSpeed(1);
                            state=2;
                         
                     }else if(state==2 && sbd.getValueForKey_MapOfDistances(0)>0){
                            vc.setSpeed(1);
                            desiredSteeringWheelAngle=0;
                            vc.setSpeed(1);
                         
                    }else if(state==2 && sbd.getValueForKey_MapOfDistances(0)<0){
                            vc.setSpeed(1);
                            state=3;

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(2)>0){
                            vc.setSpeed(1);

                    }else if(state==3 && sbd.getValueForKey_MapOfDistances(2)<0){
                            desiredSteeringWheelAngle=finaldegree;
                            vc.setSpeed(1);
                            state=4;

                    }else if(state==4 && sbd.getValueForKey_MapOfDistances(5)<T){
                            vc.setSpeed(1);
                            count2++;

                    } else if(state==4 && sbd.getValueForKey_MapOfDistances(5)>T && count1>0){
                            vc.setSpeed(1);
                            state=5;
                
                    }else if(state==5 && count2>0){
                            vc.setSpeed(1);
                            desiredSteeringWheelAngle=-24;
                            count2--;

                    }else if(state==5 && count2==0){
                            vc.setSpeed(2);
                            desiredSteeringWheelAngle=sd.getExampleData();
                    }
                }


                cout << "                  state :" << state << endl;
                duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
                // std::cout<<"printf: "<< duration <<'\n'; 

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                 // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

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

