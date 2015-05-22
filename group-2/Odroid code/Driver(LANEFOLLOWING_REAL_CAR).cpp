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

int count2 = 0;
int parkmode = 1;
int reverse2 = 0;
//int reverse1 = 0;
int intersect = 0;
double intersectStart = 0;
int count1 = 0;
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */
key_t key;
int shmid;
char * data;
int mode;
int loop = 10;

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

            
            //  Get most recent steering data as fill from lanedetector for example:
            Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
            SteeringData sd = containerSteeringData.getData<SteeringData> ();
           cerr << "Most recent steering data: '" << sd.toString() << "'" << endl;
            
        // This is where we check what state lane detector has deemed appropriate for the 
        // car to act upon. And then send how we want the car to act, into the steering board.
        // Jonathan Klemetz, Johan Hermannson.
            loop++;
            if(loop > 98){
                loop = 10;
            }
            if (sd.getDriveStraight()==true && intersect == 0){
                writeToMem(50, 1);//38
                cerr << "Straight '" << endl;
            }if (sd.getTurnLeft()==true && intersect == 0){
                writeToMem(40, 1);//29
                cerr << "left '" << endl;
            }if (sd.getTurnHarderLeft()==true && intersect == 0){
                writeToMem(30, 1);//20
                cerr << "Hleft '" << endl;
            }if (sd.getTurnRight()==true && intersect == 0){
                writeToMem(60, 1);//47
                cerr << "right" << endl;
            }if (sd.getTurnHarderRight()==true && intersect == 0){
                writeToMem(70, 1);//56
                cerr << "Hright" << endl;
            }

        }
        
        return ModuleState::OKAY;
    }
} // msv



