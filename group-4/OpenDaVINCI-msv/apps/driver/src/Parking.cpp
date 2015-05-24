#include "Parking.h"
#include <iostream>
#include "core/io/ContainerConference.h"

namespace msv {

    // setting position
    // Vehicle.posX = -35                     
    // Vehicle.posY = -9 

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

        cout << setw(22) << "| Step" << "| " << setw(10) << step << "|" << endl;
        cout << setw(22) << "| TravelPath " << "| " << setw(10) << pathTraveled << "|" << endl;
        cout << setw(22) << "| middlePath " << "| " << setw(10) << path_middlePark << "|" << endl;
        cout << left << setfill('-') << setw(34) << "+" << "+" << endl << setfill(' ');
        
        // step 1: find the gap
        if(hasGap(usFrontRight, irFrontRight, pathTraveled))
        {
            speed =  parkingSpeed;
            steeringWheelAngle = 0;
            cout << " Gap Start Found ; Gap Start Found ; Gap Start Found " << endl;
        }

        // step 2: fint the right position for parking
        if (hasReady(pathTraveled))
        {
            speed = parkingSpeed;
            cout << " Finding Gap End ; Finding Gap End ; Finding Gap End " << endl;
        }

        // step 3: start driving back with turning right
        if(canBackRight(pathTraveled))
        {
            speed = -(parkingSpeed/2);
            steeringWheelAngle = 17;
            cout << "Back Right ; Back Right ; Back Right" << endl;
        }

        // step 4: keep dring back with turning left 
        if(canBackLeft(pathTraveled,irRear))
        {
            speed = -(parkingSpeed/4);
            steeringWheelAngle = -27;
            cout << "Back Left ; Back Left ; Back Left" << endl;
        }

        // step 5: drving frorward with turning right
        if(adjust_1(pathTraveled))
        {
            speed = parkingSpeed/4;
            steeringWheelAngle = 27;
            cout << "GO RIGHT ; GO RIGHT ; GO RIGHT" << endl;
        }

        // step 6: finish parking with turning left with driving backwards
        if(adjust_2(pathTraveled))
        {
            speed = -parkingSpeed/4;
            steeringWheelAngle = -15;
            cout << "Finish , Finish , Finish" << endl;
        }

        // step 7: stop when parking done
        if(stop(step))
        {
            speed = 0;
            cout << "Stop Stop ; Stop Stop ; Stop Stop" << endl;
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
        if (step == 0 && (usFrontRight > 6 || usFrontRight < 0) && irFrontRight > 0)
        {
                step = 1;
                path_gapStart = pathTraveled;
                return true;
        }
        return false;
    }

    bool Parking::hasReady(float pathTraveled)
    {
        if ( step == 1 && (pathTraveled - path_gapStart) < (carSize * 1.45 )  )
        {
            return true;
        }
        if (step == 1 && (pathTraveled - path_gapStart) > (carSize * 1.45))
        {
            path_gapEnd = pathTraveled;
            step = 2;
        }
        return false;
    }

    bool Parking::canBackRight(float pathTraveled)
    {
        if (step == 2 && (pathTraveled - path_gapEnd) < (carSize * 1.1))
        {
            return true;
        }
        if (step == 2 && (pathTraveled - path_gapEnd) > (carSize * 1.1))
        {
            path_middlePark = pathTraveled;
            step = 3;
        }

        return false;
    }

    bool Parking::canBackLeft(float pathTraveled,float irRear)
    {
        if (step == 3 && !isInRange(irRear,0,2) && (pathTraveled - path_middlePark) < (carSize * 0.35))
        {
            return true;
        }
        if (step == 3 &&( isInRange(irRear,0,2) || ((pathTraveled - path_middlePark) > (carSize * 0.35)) ))
        {
            path_middlePark = pathTraveled;
            step = 4;
        }
        return false;
    }

    bool Parking::adjust_1(float pathTraveled)
    {
        if (step == 4 && (pathTraveled - path_middlePark) < (carSize * 0.19))
        {
            return true;
        }
        if (step == 4 && (pathTraveled - path_middlePark) > (carSize * 0.19))
        {
            path_middlePark = pathTraveled;
            step = 5;
        }
        return false;
    }

    bool Parking::adjust_2(float pathTraveled)
    {
        if (step == 5 && (pathTraveled - path_middlePark) < (carSize * 0.1))
        {
            return true;
        }
        if (step == 5 && (pathTraveled - path_middlePark) > (carSize * 0.1))
        {
            path_middlePark = pathTraveled;
            step = 6;
        }
        return false;
     }


    bool Parking::stop(int stage)
    {
        if(stage == 6)
        {
            return true;
        }
        return false;
    }
}



