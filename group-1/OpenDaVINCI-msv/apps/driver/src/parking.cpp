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
#include <unistd.h>
#include <time.h>

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

        double desiredSteeringWheelAngle;
        double pi = 3.14159265;
        double Rs1; //first steer angle radius of curvature
        double Rs2; //second steer angle radius of curvature
        float xd;  // distance the car needs to drive from point a before turning stearing wheel
        
        //Measure of gap used with IR sensors
        float gapSum;
        float gapStart;
        float gapEnd;

        // counters to adjust movement length
        float counter=0; // used as counter to measure the distance traveled and
        int trajCounter=0; // used to count the trajectory in the first curve
        
        //controlling states
        int state=0; // states for parking
        int parking1=0; // adjust to go to parking if states
        int trigger = 0; //used to trigger states, with no go back
        

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
                VehicleControl vc;  //vc is new car

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
                // vc.setSpeed(1.0); //car with speed of 1.0
                
                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                // double desiredSteeringWheelAngle = sd.getExampleData(); // 4 degree but SteeringWheelAngle expects the angle in radians!
                //vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);


                //SENSORS
                double IR_FR = sbd.getValueForKey_MapOfDistances(0);    //ir front right
                double IR_R = sbd.getValueForKey_MapOfDistances(1);   //ir rear rear
                double IR_RR = sbd.getValueForKey_MapOfDistances(2);    //ir rear right
                //double US_FC = sbd.getValueForKey_MapOfDistances(3);  //ultra sonic front cener
                //double US_FR = sbd.getValueForKey_MapOfDistances(4);    //ultra sonic front right
                double US_RR = sbd.getValueForKey_MapOfDistances(5);    //ultra sonic rear right


                //Algorithm below used for parellel parking, - source of algorithm used stated in documentation
                double cl;  //car length
                cl = 4;
                double wl;  //length between front and back car wheel axels
                wl = 2.65;
                double ww;  //width between front or back car wheels
                ww = 1;
                double wfbf;   //front wheel to front bumper
                wfbf = 1;
                double fcr;  //full circle radious when car wheels at max angle
                fcr = 5.25;
                double ow;  //object width
                ow = 3.5;
                double parkspace;   //calculates the space needed to park
                parkspace = cl + sqrt((pow (fcr, 2) - pow(wl, 2))+(2+ pow(ww, 2))-pow((sqrt(pow(fcr, 2)- pow(wl, 2)-ow)), 2)-wl-wfbf);
                
                Rs1 = (wl*(tan(25*pi/180.0)))+ww/2;     //first steer angle radius of curvature
                Rs2 = (wl*(tan(25*pi/180.0)))+ww/2;     //second steer angle radius of curvature
                
                double aq1;    // radius of the curvature corresponding to the first steering angle
                double aq2;    // radius of the curvature corresponding to the second steering angle
                float aaqq;   // (aq2-aq1) 
                 //  aq = cw +xc;       
                aq1 = Rs1 - ww/2;
                aq2 = Rs2 + ww/2;
                aaqq = (aq2-aq1); // Used to get the frst distanceo of trajectory
                xd = (pow((Rs1*Rs2),2)-pow((Rs1),2))+5.6; //distance from start of gap and to whre the car is "paralell" with next object after end of gap
              
                /*
                State 0 = counting when when gap is detected/start of gap
                State 1 = counting when when gap is ending/end of gap
                State 2 = checks if gap doesnt have an object
                State 3 = if no object detected in parking gap, its going to state 4, else stay on state 3 and continue forward
                State 4 = implement trajectory path. Stop when IR rear sensor gets to close to object.
                State 5 = Adjust car in parking slot, by using heading and IR rear
                */

                // speed to 1 and lanefollowing when parking1(state) is equal 0
                if (parking1 ==0){    
                    vc.setSpeed(1.0);
                    desiredSteeringWheelAngle = sd.getExampleData();
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle); // lane following
                } if(IR_RR<0 && state==0){                //state is 1 when is_rr is bigger than 0
                    gapStart = vd.getAbsTraveledPath();    //start calculating the traveled path
                    state=1;
                } if(IR_RR>0 && state==1 && trigger ==0){
                    gapEnd = vd.getAbsTraveledPath();      //stop calculating the traveled path
                }
                    gapSum = gapEnd - gapStart;  
                  if(gapSum > xd && xd > parkspace && trigger ==0 ){       // if travel path gapSum is bigger than xd path and if xd path
                    vc.setSpeed(0.0);
                    state =2;
                } if(state ==2 && US_RR>0 && trigger ==0){                                     //in state 3,
                    vc.setSpeed(1.0);
                    counter++;
                } if(state ==2 && US_RR<0 && IR_FR <0 && trigger ==0){                                     //in state 3,
                    vc.setSpeed(0.0);
                    counter=143;
                    state=3;
                } if(state ==2 && US_RR<0 && IR_RR >0 && IR_FR >0  && trigger ==0){                                     //in state 3,
                    vc.setSpeed(0.0);
                    parking1=1; // the speed on state 0 will be of
                    counter=158;
                    state=4;
                } if(state ==3 && counter > 140 && trigger<2){      
                    trigger=1;
                    vc.setSpeed(1.0);
                    counter++;
                } if(state ==3 && counter > 163 && trigger==1){      
                    vc.setSpeed(0.0);
                    parking1=1;
                    trigger=2;
                    state=4;
                } if(state ==4 && counter >150 && trigger ==2){                                     //in state 3,
                    vc.setSteeringWheelAngle(26);              //the car wheels turn max to the right,
                    vc.setSpeed(-0.5);
                    trajCounter++;
                } if(state ==4  &&  (trajCounter/100)  >= aaqq && trigger ==2){
                    vc.setSteeringWheelAngle(-26);
                    vc.setSpeed(-0.4);
                } if(state == 4 && (IR_R > 0.9 && IR_R <=1.9) && trigger == 2){
                    trigger =3; // increase triggerger to not go back to state less then 5
                    vc. setSpeed(0.0);
                    state =5;
                } if(state == 5 && vd.getHeading() >= 0.09 && trigger==3){
                    vc.setSteeringWheelAngle(27 );
                    vc.setSpeed(0.2);   
                } else if (state==5 &&   vd.getHeading() <= -0.09 && trigger==3) {
                    vc.setSteeringWheelAngle(-27 );
                    vc.setSpeed(0.2);
                } else if(state == 5 && trigger==3  &&  IR_R <0){
                    vc.setSpeed(-0.2);                       
                } else if(state == 5 && trigger==3 && (IR_R > 1.5 && IR_R <=1.9)){
                    vc.setSpeed(0);
                }
             
                //Printouts
                std::cout << "Parking Space "<< parkspace << std::endl;
                std::cout << "Heading "<< vd.getHeading() << std::endl;
                std::cout << "STATE "<< state << std::endl;
              
           

                // You can also turn on or off various lights:
                vc.setBrakeLights(false);
                vc.setLeftFlashingLights(false);
                // vc.setRightFlashingLight(true);
                // Create container for finally sending the data.
                Container c(Container::VEHICLECONTROL, vc);
                // Send container.
                getConference().send(c);
            }

            return ModuleState::OKAY;
        }
} // msv
