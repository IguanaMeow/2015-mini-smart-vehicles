#include "Parking.h"
#include <iostream>
#include "core/io/ContainerConference.h"

namespace msv {

    using namespace std;


    Parking::Parking():step(0),path_gapStart(0),path_gapEnd(0),path_middlePark(0),carSize(8.0),parkingSpeed(1)
    {

    }

    void Parking::doParking(double &speed, double &steeringWheelAngle, float pathTraveled, float sensorData[])
    {
        //float usFrontCentre = sensorData[0];
        float usFrontRight = sensorData[1];
        //float usRearRight = sensorData[2];
        float irFrontRight = sensorData[3];
        //float irRearRight = sensorData[4];
        float irRear = sensorData[5];
        //const double normalSpeed = 2;

        //float travelGapPath;



        cout << setw(22) << "| Step" << "| " << setw(10) << step << "|" << endl;
        cout << setw(22) << "| TravelPath " << "| " << setw(10) << pathTraveled << "|" << endl;
        cout << setw(22) << "| middlePath " << "| " << setw(10) << path_middlePark << "|" << endl;
        cout << left << setfill('-') << setw(34) << "+" << "+" << endl << setfill(' ');
        
        if(hasGap(usFrontRight, irFrontRight, pathTraveled))
        {
            speed =  parkingSpeed;
            steeringWheelAngle = 0;
            cout << " Gap Start Found ; Gap Start Found ; Gap Start Found " << endl;
        }

        if (hasReady(pathTraveled))
        {
            speed = parkingSpeed;
            cout << " Finding Gap End ; Finding Gap End ; Finding Gap End " << endl;
        }
        if(canBackRight(pathTraveled))
        {
            speed = -(parkingSpeed/2);
            steeringWheelAngle = 26;
            cout << "Back Right ; Back Right ; Back Right" << endl;
        }

        if(canBackLeft(pathTraveled,irRear))
        {
            speed = -(parkingSpeed/2);
            steeringWheelAngle = -26;
            cout << "Back Left ; Back Left ; Back Left" << endl;
        }

        if(canGoRight(pathTraveled))
        {
            speed = parkingSpeed/2;
            steeringWheelAngle = 26;
            cout << "GO RIGHT ; GO RIGHT ; GO RIGHT" << endl;
        }

        if(temp(step))
        {
            speed = 0;
            cout << "Temp Stop ; Temp Stop ; Temp Stop" << endl;
        }

        cout << left << setfill('-') << setw(34) << "+" << "+" << endl << setfill(' ');
        cout << endl << endl;

    }

    bool Parking::isInRange(float sensorValue, int min, int max)
    {
        if (sensorValue > min && sensorValue < max)
        {
            return true;
        }
            return false;
        
    }

    bool Parking::outsideRange(float sensorValue, int min, int max)
    {
        if (sensorValue < min && sensorValue > max)
        {
            return true;
        }
            return false;
        
    }

    bool Parking::hasGap(float usFrontRight,  float irFrontRight, float pathTraveled)
    {
//     if (step == 0 && usFrontRight > 55  && irFrontRight > 0)
        if (step == 0 && (usFrontRight > 11 || usFrontRight < 0) && irFrontRight > 0)
        {
                step = 1;
                path_gapStart = pathTraveled;
                return true;
        }
        return false;
    }

    bool Parking::hasReady(float pathTraveled)
    {
        if ( step == 1 && (pathTraveled - path_gapStart) < (carSize * 1.2 )  )
        {
            return true;
        }
        if (step == 1 && (pathTraveled - path_gapStart) > (carSize * 1.2))
        {
            path_gapEnd = pathTraveled;
            step = 2;
        }
        return false;
    }

    bool Parking::canBackRight(float pathTraveled)
    {
        if (step == 2 && (pathTraveled - path_gapEnd) < (carSize * 0.65))
        {
            return true;
        }
        if (step == 2 && (pathTraveled - path_gapEnd) > (carSize * 0.65))
        {
            path_middlePark = pathTraveled;
            step = 3;
        }

        return false;
    }

    bool Parking::canBackLeft(float pathTraveled,float irRear)
    {
        if (step == 3 && !isInRange(irRear,0,2.6) && (pathTraveled - path_middlePark) < (carSize * 0.32))
        {
            return true;
        }
        if (step == 3 &&( isInRange(irRear,0,2.6) || ((pathTraveled - path_middlePark) > (carSize * 0.32)) ))
        {
            path_middlePark = pathTraveled;
            step = 4;
        }
        return false;
    }

    bool Parking::canGoRight(float pathTraveled)
    {
        if (step == 4 && (pathTraveled - path_middlePark) < (carSize * 0.18))
        {
            return true;
        }
        if (step == 4 && (pathTraveled - path_middlePark) > (carSize * 0.18))
        {
            step = 5;
        }
        return false;

    }

    bool Parking::temp(int stage)
    {
        if(stage == 5 )
        {
            return true;
        }
        return false;
    }
}



