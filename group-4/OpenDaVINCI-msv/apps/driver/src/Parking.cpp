#include "Parking.h"

#include <iostream>
#include "core/io/ContainerConference.h"

namespace msv {

    using namespace std;


    Parking::Parking():step(0)
    {

    }

    void Parking::doParking(double &speed, double &steeringWheelAngle, float sensorData[])
    {
        float usFrontCentre = sensorData[0];
        float usFrontRight = sensorData[1];
        float usRearRight = sensorData[2];
        //float irFrontRight = sensorData[3];
        float irRearRight = sensorData[4];
        float irRear = sensorData[5];
        const double normalspeed = 1;

        cout << setw(22) << "| Step"               << "| " << setw(10) << step         << "|" << endl;


        if(hasGap(usRearRight, irRearRight))
        {
            speed = 0;
        }

        if (canTurnRight(usFrontRight))
        {
            speed = -(normalspeed/1);
            steeringWheelAngle = 27.0;
        }

        if (canTurnLeft(irRear,usRearRight))
        {
            speed = -(normalspeed/2);
            steeringWheelAngle = -27;
        }

        if (goForward(usFrontCentre))
        {
            speed = (normalspeed/2);
            steeringWheelAngle = 27;
        }

        if (stop(usRearRight, irRear, usFrontCentre))
        {
            speed = 0;
            steeringWheelAngle = 0;
        }
        cout << left << setfill('-') << setw(34) << "+" << "+" << endl << setfill(' ');
        cout << endl << endl;
    }

    bool Parking::hasGap(float usRearRight,  float irRearRight)
    {
        if (step == 0 && (usRearRight > 11 || usRearRight < 0 ) && irRearRight > 0)
        {
                step = 1;
                cerr << "step" << step << endl;
                return true;
        }
        return false;
    }

    bool Parking::canTurnRight(float usFrontRight)
    {
        if (step == 1 && usFrontRight > 0)
        {         
            return true;
        } 
        if (step == 1 &&  usFrontRight < 0)
        {
            step = 2;
        }
        return false;
    }

    bool Parking::canTurnLeft( float irRear, float usRearRight)
    {
        if (step == 2 && (usRearRight < 0 || usRearRight > 5.5) && (irRear > 2.3 || irRear < 0))
        {
            return true;
        }

        if (step == 2 && (usRearRight < 6 || (irRear > 0 && irRear < 2.3)) )
        {
            step = 3;
        }
        return false;
    }


    bool Parking::goForward(float usFrontCentre)
    {
        if (step == 3 && (usFrontCentre < 0 || usFrontCentre > 2.3) )
        {
            return true;
        }
        if (step == 3 && (usFrontCentre < 2.3 && usFrontCentre > 0))
        {
            step = 4;
        }
        return false;
    }


    bool Parking::stop(float usRearRight, float irRear,float usFrontCentre)
    {        
        if (step == 4 && (usRearRight < 6 || irRear < 0 || usFrontCentre < 2.3))
        {
            return true;
        }
        return false;
    }

}


