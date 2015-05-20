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

        double pi = 3.14159265; // pi
        float start;    // start, starts calculating the traveled path 
        float end;      // end, stops calculating the traveled path 
        float eee;      // result of end - start
        float mmm;      // yend+2.4; //controll distance of first curvature
        float xd;       // distance the car needs to drive from point the beginning of state 3 before turning stearing wheel
        float esy;      // result of yend - ystart
        float ystart;   // ystart, starts calculating the traveled path a second time
        float yend;     // yend, stops calculating the traveled path a second time
        double Rs1;     // first steer angle radius of curvature
        double Rs2;     // second steer angle radius of curvature
        float qend;     // AbsTraveledPath - mmm;
        int state=0;    // states to transitions through if loops
        int parking1=0; // controles parking state on or off

        

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
              //  vc.setSpeed(1.0); //car with speed of 1.0
                
                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                double desiredSteeringWheelAngle = sd.getExampleData(); // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);



                double IR_FR = sbd.getValueForKey_MapOfDistances(0);    //ir front right
                double IR_R = sbd.getValueForKey_MapOfDistances(1);   //ir rear rear
                double IR_RR = sbd.getValueForKey_MapOfDistances(2);    //ir rear right
                double US_FC = sbd.getValueForKey_MapOfDistances(3);  //ultra sonic front cener
                double US_FR = sbd.getValueForKey_MapOfDistances(4);    //ultra sonic front right
                double US_RR = sbd.getValueForKey_MapOfDistances(5);    //ultra sonic rear right
                
                double cl;  //car length
                cl = 4;
                double wl;  //length between front and back car wheel axels
                wl = 2.65;
                double ww;  //width between front or back car wheels
                ww = 1;
                double wfbf;    //front wheel to front bumper
                wfbf = 1;
                double fcr;  //full circle radious when car wheels at max angle
                fcr = 5.25;
                double ow;  //object width
                ow = 3.5;
                
                double parkspace;   //calculates the space needed to park
                parkspace = cl + sqrt((pow (fcr, 2) - pow(wl, 2))+(2+ pow(ww, 2))-pow((sqrt(pow(fcr, 2)- pow(wl, 2)-ow)), 2)-wl-wfbf);
                
                Rs1 = (wl*(tan(25*pi/180.0)))+ww/2;     //first steer angle radius of curvature
                Rs2 = (wl*(tan(25*pi/180.0)))+ww/2;     //second steer angle radius of curvature

                eee = end - start;

                double aq1;    // radius of the curvature corresponding to the first steering angle
                double aq2;    // radius of the curvature corresponding to the second steering angle
                float aaqq;    // 
                aq1 = Rs1 - ww/2;
                aq2 = Rs2 + ww/2;
                aaqq = (aq2-aq1)+2.3;
                xd = (pow((Rs1*Rs2),2)-pow((Rs1),2))+5.6;   // 


  
                if(parking1 ==0 && IR_FR>0){    
                    vc.setSpeed(1.0);
                        start = vd.getAbsTraveledPath();    //start calculating the traveled path
                }
                if(IR_FR<0){
                    vc.setSpeed(1.0);
                        end = vd.getAbsTraveledPath();      //stop calculating the traveled path
                }
                 if(state==0 && IR_RR>0 && IR_FR<0){      //state is 0 when ir_rr  and ir_fr is -1
                  state=1; 
                    vc.setSpeed(1.0);
                }
                if(state==1 && IR_RR<0){                //state is 1 when is_rr is bigger than 0
                    std::cout << "STATE 1"<< state << std::endl;
                        ystart = vd.getAbsTraveledPath();    //start calculating the traveled path
                        state=2;
                        vc.setSpeed(1.0);
                }
                if(state==2 && IR_RR>0){
                    std::cout << "STATE 2"<< state << std::endl;
                        yend = vd.getAbsTraveledPath();      //stop calculating the traveled path
                        mmm = yend+2.4; //controll distance
                        vc.setSpeed(1.0);
                }
                esy = yend - ystart;
               if(esy > (xd+3) && xd > parkspace){       // if travel path esy is bigger than xd path and if xd path
                    vc.setSpeed(0.0);
                    state =3;
                }
                if(state ==3 && US_RR>-1){                                     //in state 3,
                    vc.setSpeed(1.0);
                }
                if(state ==3 && US_RR<0){                                     //in state 3,
                    std::cout << "STATE 3"<< state << std::endl;
                    vc.setSteeringWheelAngle(26 * 10);              //the car wheels turn max to the right,
                    vc.setSpeed(-0.5);
                    qend =vd.getAbsTraveledPath()-mmm;
                    state =4;
                }
                if(state ==4  &&  qend  >= aaqq){
                    std::cout << "STATE 4"<< state << std::endl;
                    vc.setSpeed(0.0); 
                    vc.setSteeringWheelAngle(-26 * 10);
                    vc.setSpeed(-0.5);
                    state =5;
                }
                if(state == 5 && (IR_R > 0.6 && IR_R <=1.9)){
                    std::cout << "STATE 5"<< state << std::endl; 
                    vc. setSpeed(0.0);
                    state =6;
                }
                else if (state==6 &&  US_FC < 2.1) {
                    std::cout << "STATE 6"<< state << std::endl;
                    vc.setSpeed(0.0);
                    state=7;
                }
                else if(state == 7 && IR_R <0){
                    std::cout << "STATE 7"<< state << std::endl;
                    vc.setSpeed(-0.5);                       
                }
                else if(state == 7 && IR_R >2.3){
                    std::cout << "PARKING DONE - STATE "<< state << std::endl;
                    vc.setSpeed(0.0);
                }
        

                std::cout << "START "<< start << std::endl;
                std::cout << "END "<< end << std::endl;
                std::cout << "Y-START "<< ystart << std::endl;
                std::cout << "Y-END "<< yend << std::endl;
                std::cout << "Parking Distance ESY "<< esy << std::endl;
                //std::cout << "----------"<< xd<< std::endl;
                std::cout << "Q-end "<< qend << std::endl;
                std::cout << "MMM "<< mmm << std::endl;
                std::cout << "AAQQ "<< aaqq << std::endl;
                //std::cout << "----------"<< xd<< std::endl;
                
                std::cout << "XD XD XD XD "<< xd << std::endl;
                std::cout << "Parking Space "<< parkspace << std::endl;
                
                std::cout << "PARKING111111 "<< parking1 << std::endl;
                std::cout << "STATE "<< state << std::endl;
                std::cout << "IR_FR "<< IR_FR << std::endl;
                std::cout << "IR_RR "<< IR_RR << std::endl;
                std::cout << "IR_R "<< IR_R << std::endl;
                std::cout << "US_FC "<< US_FC << std::endl;
                std::cout << "US_FR "<< US_FR << std::endl;
                std::cout << "US_RR "<< US_RR << std::endl;

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
