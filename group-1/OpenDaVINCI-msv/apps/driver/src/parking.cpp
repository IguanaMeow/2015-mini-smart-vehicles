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

        double pi = 3.14159265;
        float start;
        float end;
        float eee;
        float mmm;
        int trig = 0;
        //float start2;
        //float es;
        float xd;  // distance the car needs to drive from point a before turning stearing wheel
        //float xdc;
      
        // sensors for y with travelpath
        float esy;
        float esz;
        float ystart;
        float yend;
        double Rs1; //first steer angle radius of curvature
        double Rs2; //second steer angle radius of curvature


        float qstart;
        float qend;
        float qesy;

        int state=0;
        int parking1=0;
        int parking2 =0;

        

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
                VehicleControl vc;	//vc is new car

                // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
              //  vc.setSpeed(1.0);	//car with speed of 1.0
                
                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                double desiredSteeringWheelAngle = sd.getExampleData(); // 4 degree but SteeringWheelAngle expects the angle in radians!
                vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);



                double IR_FR = sbd.getValueForKey_MapOfDistances(0);	//ir front right
                double IR_R = sbd.getValueForKey_MapOfDistances(1);   //ir rear rear
                double IR_RR = sbd.getValueForKey_MapOfDistances(2);	//ir rear right
                double US_FC = sbd.getValueForKey_MapOfDistances(3);  //ultra sonic front cener
                double US_FR = sbd.getValueForKey_MapOfDistances(4);	//ultra sonic front right
                double US_RR = sbd.getValueForKey_MapOfDistances(5);	//ultra sonic rear right
                //std::cout << "Sensor infrared front right"<< x << std::endl;


                //double cotan(double i) { return(1 / tan(i)); }
                
                double cl;  //car length
                cl = 4;
            //    double cw;  //car width
            //    cw = 2;
                double wl;  //length between front and back car wheel axels
                wl = 2.65;
                double ww;  //width between front or back car wheels
                ww = 1;
                double wfbf;    //front wheel to front bumper
                wfbf = 1;
                double fcr;  //full circle radious when car wheels at max angle
                fcr = 5.25;
                
            //    double ol;  //object length
            //    ol = 7;
                double ow;  //object width
                ow = 3.5;
            //    double owl; //object wheel length
            //    owl = 4.5;
            //    double oww; //object wheel width
            //    oww = 2.5;
                
                double parkspace;   //calculates the space needed to park
                parkspace = cl + sqrt((pow (fcr, 2) - pow(wl, 2))+(2+ pow(ww, 2))-pow((sqrt(pow(fcr, 2)- pow(wl, 2)-ow)), 2)-wl-wfbf);
                
                Rs1 = (wl*(tan(25*pi/180.0)))+ww/2;     //first steer angle radius of curvature
                Rs2 = (wl*(tan(25*pi/180.0)))+ww/2;     //second steer angle radius of curvature
                
                // cot(1/tan(45*pi/180.0)) - cot(1/(tan(45*pi/180.0)) = ww/wl
              //  double xc;
            //  xc = IR_FR; // distance between cars from ir

                eee = end - start;

                double aq1;    // distance aq
                double aq2;
                float aaqq;
              //  aq = cw +xc;       
                aq1 = Rs1 - ww/2;
                aq2 = Rs2 + ww/2;
                aaqq = (aq2-aq1)+2.3;
                xd = (pow((Rs1*Rs2),2)-pow((Rs1),2))+6;

                //double aa;  // angle of stearing wheel at trajectory b
                //aa = atan(xd/Rs1)* 180/ pi;
               //double bb;
               // bb = aa;
               //double lpo;
               //double loq;
               // lpo = (aa / 360) * 2*pi * (Rs1 - ww/2);
               // loq = (bb / 360) * 2*pi * (Rs2 - ww/2);
                // double FF;   // missing allignment mismagment of car
                 // FF = atan((xd/Rs1)/d)*pi/180;

                /*if(US_FR<8){
                    parking1=1;
                    parking2=0;
                }
                else 
                    parking2=1;
                    parking1=0;
                }*/

  
                if(parking1 ==0 && IR_FR>0){	
                    vc.setSpeed(1.0);
                        start = vd.getAbsTraveledPath();	//start calculating the traveled path
                }
                if(IR_FR<0 && trig == 0){
                    vc.setSpeed(1.0);
                        end = vd.getAbsTraveledPath();		//stop calculating the traveled path
                }
                 if(IR_RR>0 && state==0 && IR_FR<0){      //state is 0 when ir_rr  and ir_fr is -1
                  state=1; 
                    vc.setSpeed(1.0);
                }
                if(IR_RR<0 && state==1){                //state is 1 when is_rr is bigger than 0
                    std::cout << "STATE 1"<< state << std::endl;
                        ystart = vd.getAbsTraveledPath();    //start calculating the traveled path
                        state=2;
                        vc.setSpeed(1.0);
                }
                if(IR_RR>0 && state==2 && trig ==0){
                    std::cout << "STATE 2"<< state << std::endl;
                        yend = vd.getAbsTraveledPath();      //stop calculating the traveled path
                        vc.setSpeed(1.0);
                }
                esy = yend - ystart;
                 /******** change back xd(delete 30)  */ 
               if(esy > (xd+30) && xd > parkspace && trig ==0 ){       // if travel path esy is bigger than xd path and if xd path
                    vc.setSpeed(0.0);
                    
                    state =3;
                }
                else if(xd>esy && esy > parkspace && trig ==0 && parking2==0){
                    vc.setSpeed(0.0);
                    trig=55; // not needed ???
                    parking2=1;
                    parking1=2;
                    state=8;
                }
                if(state ==3  && trig ==0){                                     //in state 3,
                    std::cout << "STATE 3"<< state << std::endl;
                    mmm = 25.7;  // fix> how to controll distance //distance mmm is 25.2,
                    vc.setSteeringWheelAngle(26 * 10);              //the car wheels turn max to the right,
                    vc.setSpeed(-0.5);
                    qend =vd.getAbsTraveledPath()-mmm;
                    state =4;
                }
                if(state ==4  &&  qend  >= aaqq && trig ==0){
                    std::cout << "STATE 4"<< state << std::endl;
                    vc.setSpeed(0.0); 
                    vc.setSteeringWheelAngle(-26 * 10);
                    vc.setSpeed(-0.5);
                    state =5;
                }
                if(state == 5 && (IR_R > 0.6 && IR_R <=1.9) && trig == 0){
                    trig =1; // increase trigger to not go back to state less then 5
                    std::cout << "STATE 5"<< state << std::endl; 
                    vc. setSpeed(0.0);
                    state =6;
                }
                if(state == 6 && US_FC > 2 && trig == 1){
                    std::cout << "STATE 6"<< state << std::endl;
                    vc.setSteeringWheelAngle(27 * 10);
                    vc.setSpeed(0.5);
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
                    std::cout << "PARKING DONE "<< state << std::endl;
                    vc.setSpeed(0.0);
                }
                

//Y-START 10.1442
//Y-END 22.2443
//Parking Distance ESY 12.1001
//XD XD XD XD 12.0637
//Parking Space 5.68819

//Y-START 16.3954
//Y-END 21.5377
//Parking Distance ESY 5.14239
//XD XD XD XD 12.0637
//Parking Space 5.68819


                // parking2, if parallel parking is not enough space then park backwards...
            
               else if(parking2 == 1 && state ==8){
                    std::cout << "PARKING state 2222 "<< state << std::endl;
                    vc.setSpeed(0.0);
                    parking2 =3;
                    state =10;
                
                     

    }   

    // check if other sensor can be used instead ...
    // STATE 10: Go forwared left in the if statement below
    if (state ==10 && parking2==3 && US_RR <10){
        vc.setSpeed(0.5);
        vc.setSteeringWheelAngle(-0.2);
        
    // STATE 10: Go backwards Right in the if statement below
    } if (state ==10 && parking2==3 && IR_RR >2.35) {
        vc.setSpeed(-0.5);
        vc.setSteeringWheelAngle(0.23);
        state=11;
    //
    }if (state ==11 && parking2==3 && (IR_RR > 1.5 && IR_RR < 2.6)) {
        vc.setSpeed(-0.5);
        vc.setSteeringWheelAngle(0.3);
       } else if( state==11 && parking2==3 && IR_RR <1.5){
        vc.setSpeed(-0.5);
        vc.setSteeringWheelAngle(0.5);

       }if(state==11 && parking2==3 && IR_FR <1 && IR_RR <1.23 && US_FR <1){
         vc.setSpeed(0.0);
        state=12;

       }
        
            
            
          

                    

                //std::cout << "START "<< start << std::endl;
                //std::cout << "END "<< end << std::endl;
                std::cout << "Y-START "<< ystart << std::endl;
                std::cout << "Y-END "<< yend << std::endl;
                std::cout << "Parking Distance ESY "<< esy << std::endl;
                //std::cout << "----------"<< xd<< std::endl;
                //std::cout << "Q-START "<< qstart << std::endl;
                //std::cout << "QESY "<< qesy << std::endl;
                //std::cout << "MMM "<< mmm << std::endl;
                //std::cout << "AAQQ "<< aaqq << std::endl;
                //std::cout << "----------"<< xd<< std::endl;
                
                std::cout << "XD XD XD XD "<< xd << std::endl;

                 
                //std::cout << "Rs1 <<<<<<<<<<<<< "<< Rs1 << std::endl;
                //std::cout << "aq1 <<<<<<<<<<<<< "<< aq1 << std::endl;              
                //std::cout << "aq2 <<<<<<<<<<<<< "<< aq2 << std::endl;
                std::cout << "Parking Space "<< parkspace << std::endl;
                
                std::cout << "PARKING111111 "<< parking1 << std::endl;
               // std::cout << "PARKING222222 "<< parking2 << std::endl;
                std::cout << "STATE "<< state << std::endl;
                std::cout << "IR_FR "<< IR_FR << std::endl;
                std::cout << "IR_RR "<< IR_RR << std::endl;
                std::cout << "IR_R "<< IR_R << std::endl;
                std::cout << "US_FC "<< US_FC << std::endl;
                std::cout << "US_FR "<< US_FR << std::endl;
                std::cout << "US_RR "<< US_RR << std::endl;

                // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
                //double desiredSteeringWheelAngle = 4; // 4 degree but SteeringWheelAngle expects the angle in radians!
                //vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

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

