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
    int speed = 2;
    double headingAngle;
    double desiredSteeringWheelAngle = 0;
    double travelDistance1;
    double travelDistance2;

    double decelearatingPoint;
    double oldRearRight = -1.0;
    double oldFrontRight = -1.0;

    double usFrontRight;
    double usRearRight;

    bool boo_measuring = false;
    bool boo_findGap = false;
    bool boo_parking = false;
    bool boo_parkingReady = false;

    double currentSteeringWheel;



    
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

    bool Driver::find_gapStart(bool* is_measure,double* oldValue,double* newValue){

        if (!(*is_measure)&& (*oldValue > 0) && (*newValue < 0)){
            return true;
        }
        return false;
    }

    bool Driver::find_gapEnd(bool* is_measure,double* oldValue,double* newValue){

        if ((*is_measure)&& (*oldValue < 0) && (*newValue > 0)){
            return true;
        }
        return false;
    }

    void Driver::recording(double us_FrontRight,double us_RearRight){

        oldFrontRight = us_FrontRight;
        oldRearRight = us_RearRight;

    }

    void Driver::startMeasure(Point3 position){

        boo_measuring = true;
        desiredSteeringWheelAngle = 0;
        cout << "-------------GAP------START------------" << endl;
        recording(usFrontRight,usRearRight);

        travelDistance1 = position.getY();
        cout << "start Location:"<< travelDistance1 << endl;

    }

    void Driver::finishMeasure(Point3 position){
        travelDistance2 = position.getY();
        boo_measuring = false;
        if (travelDistance2 - travelDistance1 <= 8 ){
            speed = 0.5;
        }else{
            // equals 14.29
            cout << "wanted length: " << (travelDistance2 - decelearatingPoint) << endl;
            boo_findGap = true;
            cout << "--------------GAP------END---------------"<< "length: " << (travelDistance2 - travelDistance1) << endl << endl;
        }
        recording(usFrontRight,usRearRight);
    }

    void Driver::stopforParking(){
        speed = 0;
        desiredSteeringWheelAngle = 0;
        vc.setBrakeLights(true); 
        boo_parking = true;
        recording(usFrontRight,usRearRight);
    }

    void Driver::defaultDriving(){

        speed = 2;
        recording(usFrontRight,usRearRight);
    }



    void Driver::pre_parking(){

        speed = -1;

        if((oldRearRight > 0) && (usRearRight  < 0)){
            speed = 0;
        }

        
    }

    bool Driver::is_readyParking(){

        if((oldRearRight > 0) && (usRearRight  < 0)){
            return true;
        }

        return false;
    }

    void Driver::tureWheelToRight(){

        recording(usFrontRight,usRearRight);
        desiredSteeringWheelAngle = 26;
        speed = -0.2;

    }

    void Driver::tureWheelToLeft(){

        recording(usFrontRight,usRearRight);
        desiredSteeringWheelAngle = -26;
        speed = -0.2;
        boo_parkingReady = true;

    }


    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Driver::body() {


        //double back = Constants::PI + (Constants::PI/2);
        while (getModuleState() == ModuleState::RUNNING) {
            // In the following, you find example for the various data sources that are available:

            // 1. Get most recent vehicle data:
            Container containerVehicleData = getKeyValueDataStore().get(Container::VEHICLEDATA);
            VehicleData vd = containerVehicleData.getData<VehicleData> ();
            headingAngle = vd.getHeading() * Constants :: RAD2DEG;
            //cerr << "Most recent vehicle data: '" << vd.toString() << "'" << endl;
            cerr << "Heading Angle: " << headingAngle << endl;

            // 2. Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            sbd.setNumberOfSensors(6);
            

            //double usFrontCentre;
            usFrontRight = sbd.getValueForKey_MapOfDistances(US_FrontRight);
            usRearRight = sbd.getValueForKey_MapOfDistances(US_RearRight);

            //usFrontCentre = sbd.getValueForKey_MapOfDistances(US_FrontCenter);

            cerr << "usFrontRight value: " << usFrontRight << endl;
            cerr << "usReartRight value: " << usRearRight << endl << endl;

            cerr << "----------------------------" << endl <<endl;
            //cerr << "usFrontcentre value: " << usFrontCentre << endl;
            //cerr << "Most recent sensor board data: " << sbd.toString() << endl;
            

            // 3. Get most recent user button data:
            //Container containerUserButtonData = getKeyValueDataStore().get(Container::USER_BUTTON);
            //UserButtonData ubd = containerUserButtonData.getData<UserButtonData> ();
            //cerr << "Most recent user button data: '" << ubd.toString() << "'" << endl;

            // 4. Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();
            cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;

            // Design your control algorithm here depending on the input data from above.
            Point3 position = vd.getPosition();
            if (!boo_findGap && !boo_parking){

                if (find_gapStart(&boo_measuring,&oldFrontRight, &usFrontRight)){
                    //gap start
                    startMeasure(position);
                     

    /*            }else if((measuring && (usFrontRight > 0)) && ((oldFrontRight < 0) && !findGap)){
                    speed = 2;
                    decelearatingPoint = position.getY();
                    cout << "start counting_: " << decelearatingPoint << endl;
                    oldFrontRight = usFrontRight;
                    oldRearRight = usRearRight;
                    //cout << "acceleration" << vc.getAcceleration() << endl;
                    //vc.setAcceleration(-2);
                    //cout << "Changed acceleration: " << vc.getAcceleration() << endl;*/

                }else if(find_gapEnd(&boo_measuring,&oldRearRight, &usRearRight)){

                    //find the end of the gap
                    finishMeasure(position);

                }else{
                    defaultDriving();
                }

            }else if (boo_findGap && !boo_parking){

                stopforParking();

            }else if(boo_findGap && boo_parking){

                pre_parking();

                if (is_readyParking() && (headingAngle < 140)){

                    tureWheelToRight();

                }
                // suppose to stop when headingAngle > 135
                if (headingAngle > 140.0){

                    stopforParking();
                    tureWheelToLeft();
                
                }

                if (headingAngle < 91 && boo_parkingReady){
                    stopforParking();
                }
            }


            // Create vehicle control data.
            //VehicleControl vc;

            // With setSpeed you can set a desired speed for the vehicle in the range of -2.0 (backwards) .. 0 (stop) .. +2.0 (forwards)
            vc.setSpeed(10);
            //vc.setSpeed(speed);

            // With setSteeringWheelAngle, you can steer in the range of -26 (left) .. 0 (straight) .. +25 (right)
            vc.setSteeringWheelAngle(sd.getWheelAngle() * Constants::DEG2RAD);
            //vc.setSteeringWheelAngle(desiredSteeringWheelAngle * Constants::DEG2RAD);

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
