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

//#include "generated/msv/SensorBoardData.h"
namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;
//        using namespace libdata::generated::msv;

    VehicleControl vc;
    VehicleData vd;

    float speed = 1.0;

    float headingAngle;
    float currentSpeed;
    float desiredSteeringWheelAngle = 0;


    float gapStartPosition;
    float gapEndPosition;
    float gapLength;
    float currentSteeringWheel;


    float decelearatingPoint;
    float oldRearRight = -1.0;
    float oldFrontRight = -1.0;
    float oldIrRear = -1.0;

    float usFrontRight;
    float usRearRight;
    float usFrontCentre;
    float irRear;



    const float defaultHeading = 180;
    const int gapMin = 8;
    const float slowSpeed = 0.5;
    const float middleSpeed = 1.0;
    const float fastSpeed = 1.5;

    const float wheelAngleRight = 17.0;
    const float wheelANgleLeft = -17.0;

    bool boo_measuring = false;
    bool boo_findGap = false;
    bool boo_parking = false;
    bool boo_preReady = false;
    bool boo_parkingLoop = false;
    bool boo_parkingReady = false;






    
    Driver::Driver(const int32_t &argc, char **argv) :
            ConferenceClientModule(argc, argv, "Driver") {
    }

    Driver::~Driver() {}

    void Driver::setUp() {
            //VehicleControl vc;
            //vc.setSpeed(0.0);
            // This method will be call automatically _before_ running body().
    }

    void Driver::tearDown() {
            
            // This method will be call automatically _after_ return from body().
    }

    bool Driver::find_gapStart(bool is_measure,float* oldValue,float* newValue){
    
        // for big gap

        if ((is_measure == false)&& (*oldValue > 0) && (*newValue < 0)){
            return true;
        }



        //for small gap
        // when the gap is small, there will be oldFrontRight, increas more then 5


        if ((is_measure == false) && (*newValue - *oldValue > 7)){
            return true;
        }

        return false;
    }

    bool Driver::find_gapEnd(bool is_measure,float* oldValue,float* newValue){
        // for big gap


        if ((is_measure == true)&& (*oldValue < 0) && (*newValue > 0)){
            return true;
        }

        // for small gap
        // at the end of the gap, the rearRight sensor data will decrease more then 5
        if ((is_measure == true) && (*oldValue - *newValue > 7)){
            return true;

        }

        return false;
    }

    void Driver::recording(float us_FrontRight,float us_RearRight,float ir_Rear){

        oldFrontRight = us_FrontRight;
        oldRearRight = us_RearRight;
        oldIrRear = ir_Rear;

    }

    void Driver::startMeasure(Point3 position){


        speed = middleSpeed;
        boo_measuring = true;
        desiredSteeringWheelAngle = 0;
        gapStartPosition = position.length();

        cout << "-------------GAP------START------------" << endl;
        recording(usFrontRight,usRearRight,irRear);



        cout << "start Location:"<< gapStartPosition << endl;




    }

    void Driver::finishMeasure(Point3 position){


        gapEndPosition = position.length();
        gapLength = gapEndPosition - gapStartPosition;
        
        cout << "Start Location:"<< gapStartPosition << endl;
        cout << "End Location:"<< gapEndPosition << endl;
        cout << "Gap Length:"<< gapLength << endl;


        if (gapLength <= gapMin ){

            speed = slowSpeed;

        }else if ((gapLength > gapMin) && boo_measuring){

            // equals 14.29
            speed = slowSpeed;
            //cout << "wanted length: " << (travelDistance2 - decelearatingPoint) << endl;
            boo_findGap = true;
            cout << "--------------GAP------END---------------"<< "length: " << gapLength << endl << endl;
        }
        boo_measuring = false;
        recording(usFrontRight,usRearRight,irRear);

    }


    bool Driver::pre_parking(){

        if((oldRearRight > 0) && (usRearRight  < 0)){
            return true;
        }

        return false;

        
    }

    void Driver::stopforParking(){

        //recording(usFrontRight,usRearRight,irRear);
        vc.setBrakeLights(true);
        speed = 0;
        desiredSteeringWheelAngle = 0;
        vc.setBrakeLights(true); 
        boo_parking = true;
        recording(usFrontRight,usRearRight,irRear);

    }

    void Driver::defaultDriving(){

        speed = fastSpeed;
        recording(usFrontRight,usRearRight,irRear);

        vc.setBrakeLights(false);
        vc.setLeftFlashingLights(false);
        vc.setRightFlashingLights(false);
        
    }

    void Driver::defaultParking(){

        speed = -slowSpeed;
        recording(usFrontRight,usRearRight,irRear);

        vc.setBrakeLights(false);
        vc.setLeftFlashingLights(false);
        vc.setRightFlashingLights(false);
        
    }


    void Driver::tureWheelToRightBack(){

        vc.setRightFlashingLights(true);
        recording(usFrontRight,usRearRight,irRear);
        desiredSteeringWheelAngle = wheelAngleRight;
        speed = -slowSpeed;

    }

    void Driver::tureWheelToLeftBack(){

        vc.setLeftFlashingLights(true);
        recording(usFrontRight,usRearRight,irRear);
        desiredSteeringWheelAngle = wheelANgleLeft;
        speed = -slowSpeed;
        boo_parkingLoop = true;


    }

    void Driver::tureWheelToRightForward(){

        vc.setRightFlashingLights(true);
        recording(usFrontRight,usRearRight,irRear);
        desiredSteeringWheelAngle = wheelAngleRight;
        speed = slowSpeed;


    }

    void Driver::tureWheelToLeftForward(){

        vc.setLeftFlashingLights(true);
        recording(usFrontRight,usRearRight,irRear);
        desiredSteeringWheelAngle = wheelANgleLeft;
        speed = slowSpeed;

    }


    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Driver::body() {
        //float back = Constants::PI + (Constants::PI/2);
        while (getModuleState() == ModuleState::RUNNING) {
            // In the following, you find example for the various data sources that are available:

            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
            vd = containerVehicleData.getData<VehicleData> ();
            headingAngle = vd.getHeading() * Constants :: RAD2DEG;

            currentSpeed = vd.getSpeed();


            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            sbd.setNumberOfSensors(6);
            

            //float usFrontCentre;
            usFrontRight = sbd.getValueForKey_MapOfDistances(US_FrontRight);
            usRearRight = sbd.getValueForKey_MapOfDistances(US_RearRight);
            usFrontCentre = sbd.getValueForKey_MapOfDistances(US_FrontCenter);
            irRear = sbd.getValueForKey_MapOfDistances(IR_Rear);

           

            cerr << "Heading Angle:       |" << headingAngle << "|"<< endl; 
            cerr << "current Speed:       |" << currentSpeed << "|"<< endl;
            cerr << "-----------------------" << endl;
            cerr << "usFrontCentre value: |" << usFrontCentre<< "|"<< endl;
            cerr << "usFrontRight value:  |" << usFrontRight << "|"<< endl;
            cerr << "OLd usFrontRight value:  |" << oldFrontRight << "|"<< endl;
            cerr << "-----------------------" << endl;
            cerr << "usReartRight value:  |" << usRearRight  << "|"<< endl;
            cerr << "OLd usRearRight value:  |" << oldRearRight << "|"<< endl;
            cerr << "-----------------------" << endl;
            cerr << "irRear value:        |" << irRear       << "|"<< endl;
            cerr << "Old irRear value:    |" << oldIrRear    << "|"<< endl;

            cerr << "----------------------------------------" << endl << endl;
            
            //cerr << "Most recent sensor board data: " << sbd.toString() << endl;
            

            // 3. Get most recent user button data:
            //Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
            //UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
            //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

            // 4. Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();

            // Design your control algorithm here depending on the input data from above.




            /* ---------------------   side way parking area --------------------- */


            Point3 position = vd.getPosition();
            if (!boo_findGap && !boo_parking){

                // for big 
                if (find_gapStart(boo_measuring, &oldFrontRight, &usFrontRight)){

                    startMeasure(position);

                    
                }else if(find_gapEnd(boo_measuring,&oldRearRight, &usRearRight)){

                    //find the end of the gap
                    finishMeasure(position);

                }else{
                    defaultDriving();
                }

            }else if (boo_findGap && !boo_parking){

                stopforParking();

            }else if(boo_findGap && boo_parking){

                if (!pre_parking() && usRearRight > 0){

                    speed = -slowSpeed;

                }

                if (pre_parking() && (usRearRight < 0)){

                    stopforParking();
                    boo_preReady = true;

                }

                if (boo_preReady && !boo_parkingReady && (headingAngle < defaultHeading + 100)){
                    cerr << "here we go go go " << defaultHeading + 90 << endl;
                    cerr << "here we go go go " << defaultHeading + 90 << endl;
                    cerr << "here we go go go " << defaultHeading + 90 << endl;
                    cerr << "here we go go go " << defaultHeading + 90 << endl;
                    cerr << "here we go go go " << defaultHeading + 90 << endl;
                    cerr << "here we go go go " << defaultHeading + 90 << endl;

                    tureWheelToRightBack();
                    boo_parkingReady = true;

                }

                if (boo_parkingReady && (headingAngle > defaultHeading + 90)){

                    stopforParking();

                }

                if(boo_parkingLoop && (headingAngle < (defaultHeading + 1) && headingAngle > (defaultHeading - 1)) ){
                                            
                    stopforParking();  

                }


            }


            /* ---------------------   side way parking area END --------------------- */



            /* ---------------------   paralle parking area START--------------------- */


/*            Point3 position = vd.getPosition();
            if (!boo_findGap && !boo_parking){

                // for big 
                if (find_gapStart(boo_measuring, &oldFrontRight, &usFrontRight)){

                    startMeasure(position);

                    
                }else if(find_gapEnd(boo_measuring,&oldRearRight, &usRearRight)){

                    //find the end of the gap
                    finishMeasure(position);

                }else{
                    defaultDriving();
                }

            }else if (boo_findGap && !boo_parking){

                stopforParking();

            }else if(boo_findGap && boo_parking){

                if (!boo_parkingReady && headingAngle < (defaultHeading + 50)){

                    tureWheelToRightBack();
                    boo_parkingReady = true;

                }
                // suppose to stop when headingAngle > defaultHeading + 50
                if ( boo_parkingReady && headingAngle > (defaultHeading + 50)){

                    //stopforParking();
                    tureWheelToLeftBack();

                }

                if (boo_parkingLoop && irRear > 1.5 && irRear < 2.0 ){

                    
                    

                    if (headingAngle > (defaultHeading+1) && currentSpeed < 0){

                            tureWheelToRightForward();

                        }

                    if (headingAngle < (defaultHeading-1) && currentSpeed < 0){

                            tureWheelToLeftForward();

                        }

                        //if ( headingAngle < 91 && headingAngle > 89) {
                    if (usFrontCentre < 2.5 && usFrontCentre > 2.0) {

                        stopforParking();
                        }

                }

                

                if(boo_parkingLoop && (headingAngle < (defaultHeading+1) && headingAngle > (defaultHeading-1)) ){
                                            
                    stopforParking();  

                }


            }*/

            /* ---------------------   paralle parking area END--------------------- */
 
            // Create vehicle control data.
            //VehicleControl vc;

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

} // msv
