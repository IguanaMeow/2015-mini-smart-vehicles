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
#include <stdlib.h>
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <sys/shm.h>
#include <ctype.h>
#include "Memory.h"

#include "core/io/ContainerConference.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"

#include "GeneratedHeaders_Data.h"

#include "Driver.h"


//CODE WRITTEN BY JOHAN HERMANSSON, JONATHAN KLEMETZ, LUDWIG BJÖRK

int count2 = 0;
int parkmode = 1;
int reverse2 = 0;

int intersect = 0;
double intersectStart = 0;
int count1 = 0;
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */
key_t key;
int shmid;
char * data;
int mode;
int loop = 10;

//LUDWIG
//making the data that will be sent to the odroid
char * sendingData(int steering, int speed){
    
    if(speed > 0){
        speed = 58;
	reverse2 = 0;
    }
    else if(speed < 0){
	if (reverse2 < 30){
		if (reverse2 > 15){
			speed = 49;
		}
		else {
			speed = 11;
		}
		reverse2++;
	}
	else{
		speed = 11;
	}
        
    }
    else{
        speed = 49;
    }
    char* retVal= (char*)malloc(4);
    sprintf(retVal,"%i%i",steering,speed);
    return retVal;
}

//LUDWIG
////Sends data to shared memory on the odroid
void writeToMem(int steering, int speed){
    
    char * sendData = (char*)malloc(8);
    
    sprintf(sendData,"%s%i",sendingData(steering,speed),loop);
    /* attach to the segment to get a pointer to it: */
    data = (char*)shmat(shmid, (void *)0, 0);
    if (data == (char *)(-1)) {
        perror("shmat");
    }
    char* inData;
    int next[3];
    inData = data + sizeof (next);
    next[0] = sprintf(inData,"%c%c",sendData[0],sendData[1])+1;
    inData += next[0];
    next[1] = sprintf(inData,"%c%c",sendData[2],sendData[3])+1;
    inData += next[1];
    next[2] = sprintf(inData,"%c%c",sendData[4],sendData[5])+1;
    
    /* read or modify the segment, based on the command line: */
    memcpy(data, &next, sizeof (next));
    
    
    /* detach from the segment: */
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }
}

namespace msv {
    
    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;
    
    
    Driver::Driver(const int32_t &argc, char **argv) :
    ConferenceClientModule(argc, argv, "Driver") {
    }
    
    Driver::~Driver() {}
    
    void Driver::setUp() {
        // This method will be call automatically _before_ running body().
        /* make the key: */
        if ((key = 456456) == -1) {
            perror("ftok");
        }
        
        /* connect to (and possibly create) the segment: */
        if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
            perror("shmget");
        }
    }
    
    void Driver::tearDown() {
        // This method will be call automatically _after_ return from body().
        if ((key = 456456) == -1) {
            perror("ftok");
            exit(1);
        }
        /* connect to (and possibly create) the segment: */
        if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
            perror("shmget");
            exit(1);
        }
        /* Deallocate the shared memory segment.*/
        shmctl (shmid, IPC_RMID, 0);
    }
    
    // This method will do the main data processing job.
    ModuleState::MODULE_EXITCODE Driver::body() {
        
        while (getModuleState() == ModuleState::RUNNING) {
            
            // Get most recent sensor board data:
            Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
            SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
            cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;
            
            
            // Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();

            //EMIL SUNDKLEV, JOHAN HERMANSSON
            
            //Get the data from the sensors that we are using
            double sen3 = sbd.getValueForKey_MapOfDistances(2);
            double sen5 = sbd.getValueForKey_MapOfDistances(4);

            loop++;
            if(loop > 98){
                loop = 10;
            }

	    switch(parkmode){
                case 1://tries to find a empty parking-spot
		    if (sen3 < 35 && count2 > 34) { //when a new object is found after the empty spot
			parkmode = 2;//goes to the next case.
			count2 = 0;
		    }
                    else if (sen3 < 35) {//if anything is detected on the right reset the counter
                        count2 = 0;
                    }
                    else if (sen3 > 35) {//if nothing is detected increment the counter
                        count2++;
                    }
		    writeToMem(50,1);
		    cerr << "Sen3  " << sen3  << endl;
		    cerr << "Count  " << count2  << endl;
		    cerr << "Parking  " << parkmode  << endl;
                    break;
                case 2://moves forward 5 frames.
                    if (count2 < 5) {
                        count2++;
                    }
                    else {
                        count2 = 0;
                        parkmode = 3;//goes to the next case.
                    }
		    writeToMem(50,1);
		    cerr << "Parking  " << parkmode  << endl;
                    break;
		case 3://start the reverse takes 1 second(30 frames) for extra safty
		    if (count2 < 30) {
                        count2++;
                    }
                    else {
                        count2 = 0;
                        parkmode = 4;//goes to the next case.
                    }
		    writeToMem(50,-1);
                    cerr << "Parking  " << parkmode  << endl;
                    break;
		case 4://reverse full right 50 frames
                    if (count2 < 50) {
                        count2++;
                    }
                    else {
                        count2 = 0;
                        parkmode = 5;//goes to the next case.
                    }
                    writeToMem(80,-1);
                    cerr << "Parking  " << parkmode  << endl;
                    break;
		case 5://reverse straight 18 frames
                    if (count2 < 18) {
                        count2++;
                    }
                    else {
                        count2 = 0;
                        parkmode = 6;//goes to the next case.
                    }
                    writeToMem(50,-1);
                    cerr << "Parking  " << parkmode  << endl;
                    break;
		case 6://reverse full left 50 frames or until the back-sensor detects any object less than 20 centimeter from the car.
		    if (count2 < 50) {
                        count2++;
                    }
                    else if (sen5 < 20) {
                        count2 = 0;
                        parkmode = 7;//goes to the next case.
                    }
		    else {
			count2 = 0;
                        parkmode = 7;//goes to the next case.
                    }
                    writeToMem(20,-1);
                    cerr << "Parking  " << parkmode  << endl;
                    break;
		case 7://stand still parking done.
		    writeToMem(50,0);
		    cerr << "Parking  " << parkmode  << endl;
		    break;
            }

        }
        
        return ModuleState::OKAY;
    }
} // msv



