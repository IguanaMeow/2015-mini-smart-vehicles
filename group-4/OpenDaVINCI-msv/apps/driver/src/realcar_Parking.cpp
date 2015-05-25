#include "Parking.h"

#include <iostream>

namespace msv {

    using namespace std;


    Parking::Parking():step(0)
    {

    }

    void Parking::doParking(double &speed, double &steeringWheelAngle, float sensorData[])
    {
        float usFrontCentre = sensorData[0];
        //float usFrontRight = sensorData[1];
        float usRearRight = sensorData[2];
        float irFrontRight = sensorData[3];
        float irRearRight = sensorData[4];
        float irRear = sensorData[5];
        const double normalSpeed = 2;
        
        if(hasGap(usRearRight, irRearRight))
        {
            speed = 0;
            cout << "stop stop stop stop " << endl;
        }


        if (canBackRight(irFrontRight,irRearRight))
        {
            speed = -(normalSpeed/2);
            steeringWheelAngle = 23.5;
            cout << "turn right turn right turn right" << endl;
        }

        if (canGoBack(irRear))
        {
            speed = -(normalSpeed/2);
            steeringWheelAngle = 0;
            cout << "go back go back go back " <<endl;
        }

        if (canForwardRight(usFrontCentre))
        {
            speed = (normalSpeed/2);
            steeringWheelAngle = 26;
            cout << "forward right forware right " <<endl;
        }

        if (stop(usFrontCentre))
        {
            speed = 0;
        }

    }

    bool Parking::hasGap(float usRearRight,  float irRearRight){

        cout << "step" << step << endl;
        if (step == 0 && (usRearRight > 11 || usRearRight < 0) && irRearRight > 0)
        {
                step = 1;
                cout << "step" << step << endl;
                return true;
        }

        return false;

    }

    bool Parking::canBackRight(float irFrontRight, float irRearRight)
    {
        cout << "step" << step << endl;

        if (step == 1  && (irFrontRight > 0 || irRearRight > 0)  )
        {  
            return true;         
        }

        if(step == 1 && (irFrontRight < 0 && irRearRight < 0) )
        {
            step = 2;
        }

        return false;
    }

    bool Parking::canGoBack( float irRear)
    {
        cout << "step" << step << endl;

        if (step == 2 && (irRear < 0 || irRear > 2) )
        {
            return true;
        }


        if (step == 2 &&  (irRear > 0 && irRear < 2) )
        {
            step = 3;
        }

        return false;
    }

    bool Parking::canForwardRight(float usFrontCentre)
    {
        cout << "step" << step <<endl;

        if (step == 3 && (usFrontCentre > 5 || usFrontCentre < 0) )
        {
            return true;
        }

        if (step == 3 && (usFrontCentre < 5 || usFrontCentre > 0) )
        {
            step = 4;
        }

        return false;
    }

    bool Parking::stop(float usFrontCentre)
    {
        cout << "step" << step << endl;
        if (step == 4 && usFrontCentre < 5)
        {
            return true;
        }

        return false;
    }    
}

//Vehicle.posX = -35      Vehicle.posY = -8
//irus.sensor1.distanceFOV = 4  irus.sensor1.clampDistance = 3.9   
