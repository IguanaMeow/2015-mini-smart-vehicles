#include "Parking.h"

#include <iostream>

namespace msv {

    using namespace std;


    Parking::Parking():step(0)
    {

    }



    bool Parking::hasGap(float usRearRight, float irFrontRight, float irRearRight){

        cerr << "step" << step << endl;
        if (step == 0 && usRearRight > 11  && (irFrontRight - irRearRight < 0.001) && irFrontRight > 0 && irRearRight > 0){
     
                step = 1;
                cerr << "step" << step << endl;
                return true;

        }

        return false;

    }

    bool Parking::canTurnRight(float usFrontRight, float irFrontRight, float irRearRight){

        cerr << "step" << step << endl;

        //if (step == 1 && (usRearRight > 8 || usRearRight < 0) && (irFrontRight > 0 || irRearRight > 0)){
        if (step == 1 && usFrontRight > 0 && (irFrontRight > 0 || irRearRight > 0)){         

            return true;
                    
        } 

        //if (step == 1 && (usRearRight < 8 || usFrontRight < 0)){
        if (step == 1 &&  usFrontRight < 0){

            step = 2;

        }
        return false;
    }

    bool Parking::canTurnLeft( float irRear, float usRearRight){

        cerr << "step" << step << endl;

        //if (step == 2 && (usFrontCentre > 5 || usFrontCentre < 20) && usRearRight < 4.5 && irRear < 3){
        if (step == 2 && (usRearRight < 0 || usRearRight > 5.5) && (irRear > 2 || irRear < 0)){
        //if (step == 2 && (irRear < 0 || irRear > 2)){


            cerr << "look at here" << endl;
            cerr << "look at here" << endl;
            cerr << "look at here" << endl;
            cerr << "look at here" << endl;
            cerr << "look at here" << endl;

            return true;


        }


        //if (step == 2 && usRearRight < 5 && irRearRight < 0 && irFrontRight < 0 && irRear < 3 ){
        if (step == 2 && (usRearRight < 6 || (irRear > 0 && irRear < 2)) ){
        //if (step == 2 && ((irRear > 0 && irRear < 2 )|| (usFrontCentre > 5.5 && usFrontCentre < 5.7))){


            step = 3;

        }

        return false;
    }

    bool Parking::stop(float usRearRight, float irRear){

        cerr << "step" << step <<endl;

        //if (step == 3 && usFrontRight > 5 && usFrontCentre > 5 && usRearRight < 5 && (irRear < 0 || irRear > 2)){
        if (step == 3 && (usRearRight < 6 || irRear < 2)){
        //if (step == 3 &&( irRear < 2 || usFrontCentre > 5.5)){

            return true;

        }

        return false;
    }

    




}

/*when turning right:
    us_frontright + -> -
    us_rearRight -

when turning left:
    ir_back - -> +
*/