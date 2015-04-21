/**
 * lanedetector - Sample application for detecting lane markings.
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

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <unistd.h>
#include <math.h>
#include "core/data/Constants.h"

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/image/SharedImage.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include "tools/player/Player.h"

#include "GeneratedHeaders_Data.h"

#include "LaneDetector.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::image;
    using namespace tools::player;

    LaneDetector::LaneDetector(const int32_t &argc, char **argv) : ConferenceClientModule(argc, argv, "lanedetector"),
        m_hasAttachedToSharedImageMemory(false),
        m_sharedImageMemory(),
        m_image(NULL),
        m_debug(false) {}

    LaneDetector::~LaneDetector() {}

    void LaneDetector::setUp() {
	    // This method will be call automatically _before_ running body().
	    if (m_debug) {
		    // Create an OpenCV-window.
		    cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
		    cvMoveWindow("WindowShowImage", 300, 100);
	    }
    }

    void LaneDetector::tearDown() {
	    // This method will be call automatically _after_ return from body().
	    if (m_image != NULL) {
		    cvReleaseImage(&m_image);
	    }

	    if (m_debug) {
		    cvDestroyWindow("WindowShowImage");
	    }
    }

    bool LaneDetector::readSharedImage(Container &c) {
	    bool retVal = false;

	    if (c.getDataType() == Container::SHARED_IMAGE) {
		    SharedImage si = c.getData<SharedImage> ();

		    // Check if we have already attached to the shared memory.
		    if (!m_hasAttachedToSharedImageMemory) {
			    m_sharedImageMemory
					    = core::wrapper::SharedMemoryFactory::attachToSharedMemory(
							    si.getName());
		    }

		    // Check if we could successfully attach to the shared memory.
		    if (m_sharedImageMemory->isValid()) {
			    // Lock the memory region to gain exclusive access. REMEMBER!!! DO NOT FAIL WITHIN lock() / unlock(), otherwise, the image producing process would fail.
			    m_sharedImageMemory->lock();
			    {
				    const uint32_t numberOfChannels = 3;
				    // For example, simply show the image.
				    if (m_image == NULL) {
					    m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
				    }

				    // Copying the image data is very expensive...
				    if (m_image != NULL) {
					    memcpy(m_image->imageData,
							   m_sharedImageMemory->getSharedMemory(),
							   si.getWidth() * si.getHeight() * numberOfChannels);
				    }
			    }

			    // Release the memory region so that the image produce (i.e. the camera for example) can provide the next raw image data.
			    m_sharedImageMemory->unlock();

			    // Mirror the image.
			    cvFlip(m_image, 0, -1);

			    retVal = true;
		    }
	    }
	    return retVal;
    }

	/***Phuong***/
	int count(int x, int gap, IplImage *m_image, int ctrl){
		int move = m_image->widthStep;
		int w = m_image->width, h = m_image->height;

		while(((m_image->imageData) + move * (h- gap+2)) [(w/2+x)*3]==0 && x < w/2){
			if (ctrl == -1) x --;
			else x ++;
		}
		if (x > w/2){ x = w/2; }
		else if (x < -w/2) { x = -w/2;}

		//cout << "x:"<< x<<endl;
		return x;

	}

	bool straightLine (int *x_pos, int *y_pos){
		int i;
		double angle[3];

		for (i = 1; i < 4; i++){
			angle[i-1] = atan2((y_pos[0] - y_pos[i]), (x_pos[0]-x_pos[i]));
		}

		for (i = 1; i < 3; i++){
			if (fabs(angle[0] - angle[i]) > 0.021){
				cout << "Diff_angle :"<< fabs(angle[0] - angle[i])<<endl;
				return false;
			}
		}

		return true;
	}


	/***Pimmie***/
	//compare array left and right to w/2
	int checkLane(int *arr1, int w){

		int i;
		int check = 0, count = 0;

		for(i=0; i<4; i++){
			if(w/2 == abs(arr1[i]))count = count+1;

			if(w/2 > abs(arr1[i]))count = count -1;
		}


		if(count==4)check = 1; //Lane is not detected
		if(count==(-4))check = 2; //Lane is detected
		if(count<4&&count!=(-4))check =3;

		return check;

	}

		/***Phuong***/
    void LaneDetector::processImage() {


		IplImage *out_image = cvCreateImage(cvGetSize(m_image),8,3);
		cvThreshold(m_image, out_image, 127, 255, CV_THRESH_BINARY );


		//initiate values
		int w = m_image->width, h = m_image->height;
		int gap[4] = {60, 95, 125, 150};
		//The gap distance gets smaller, 3d drawing kind of thing :v

		int right[4], left[4], i;
		int y_pos[4] = {h -gap[0], h-gap[1], h-gap[2], h-gap[3]};

		CvPoint middle_bottom = cvPoint (w/2, h);
		CvPoint middle_top = cvPoint(w/2,0);

		CvScalar red = CV_RGB(255,0,0);
		CvScalar blue = CV_RGB(0,0,255);
		CvScalar green = CV_RGB(0,255,0);

		CvPoint left_pos[4], right_pos[4], start_pos[4];


		//Get left and right distance
		for (i = 0; i< 4; i ++) {

			start_pos[i] = cvPoint(w/2, y_pos[i]);

			left[i] = count(0, gap[i], out_image, -1);
			left_pos[i] = cvPoint(w/2 + left[i], y_pos[i]);

			right[i] = count(0, gap[i], out_image, 1);
			right_pos[i] = cvPoint(w/2 + right[i], y_pos[i]);
		}

		//Draw lines
        if (m_debug) {
            if (m_image != NULL){

				for (i = 0; i<4; i++){
					cvLine(out_image, start_pos[i], right_pos[i], green, 2, 8);
					cvLine(out_image, start_pos[i], left_pos[i], blue, 2, 8);
				}

				cvLine(out_image, middle_bottom, middle_top, red, 2, 8);

                cvShowImage("WindowShowImage", out_image);
                cvWaitKey(10);

            }
        }

		//cout<<"Right: "<<right[0]<<" "<<right[1]<<" "<<right[2]<<" "<<right[3]<<endl;
		//cout<<"gap: "<<y_pos[0]<<" "<<y_pos[1]<<" "<<y_pos[2]<<" "<<y_pos[3]<<endl;


		SteeringData sd;
		SpeedData spd;
		DistanceData dd;
		//double steeringLight = 8.6 * Constants::DEG2RAD ;
		double steeringSteep = 9.05 * Constants::DEG2RAD ;

		Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
		SteeringData sd_old = containerSteeringData.getData<SteeringData> ();
		double steer = sd_old.getExampleData();

		Container containerDistanceData = getKeyValueDataStore().get(Container::USER_DATA_3);
		DistanceData dd_old = containerDistanceData.getData<DistanceData>();
		int dist = dd_old.getDistanceData();

		if (checkLane(left, w) == 1 && checkLane(right,w) == 1 ){
			//if no line detected, go straight

			sd.setExampleData(0);
			spd.setSpeedData(1);
			dd.setDistanceData(dist);

			cout<<"Mode: 1"<<endl;

		} else if ((checkLane(left, w) != 1 && checkLane(right, w) == 1) ||
				(checkLane(left, w) == 1 && checkLane(right, w) !=2)){
			//if no line just do what it did before

			Container containerSpeedData = getKeyValueDataStore().get(Container::USER_DATA_2);
			SpeedData spd_old = containerSpeedData.getData<SpeedData> ();
			double speed = spd_old.getSpeedData();

			sd.setExampleData(steer);
			spd.setSpeedData(speed);
			dd.setDistanceData(dist);
			cout<<"Mode: 2"<<endl;


		} else if((checkLane(left,w) == 3 && checkLane(right,w) == 3)){

			cout << "Old Steering Data " << steer<< endl;
			cout << "Old distant  " << dist<< endl;
			dd.setDistanceData(dist);
			sd.setExampleData(abs(steer));
			spd.setSpeedData(2);
			cout<<"Mode: 8"<<endl;


		} else {
			cout << "Old Steering Data " << steer<< endl;
			cout << "Old distant  " << dist<< endl;

			//int dist = 215;

			if (checkLane(right, w) != 1 && straightLine(right, y_pos) == true) {

				if (((right[0] < dist+2 && right[0] > dist -2) || dist == 0)
						&& abs(steer) < 0.001){

					//cout << "Distance: " << right[0] << endl;
					sd.setExampleData(0);
					spd.setSpeedData(2);
					dd.setDistanceData(right[0]);
					cout << "Mode: 3" << endl;

				} else {

//					if (right[0] > dist +2){
//						sd.setExampleData((right[0] - dist) / 30 * Constants::DEG2RAD);
//						//sd.setExampleData(-abs(steer/1.01));
//					} else if (right[0] < dist -2){
						//sd.setExampleData(abs(steer/1.01));
						sd.setExampleData((right[0] - dist) / 30 * Constants::DEG2RAD);
//					} else if (right[0] > dist +10 && right[0] < dist +30){
//						sd.setExampleData(abs(steer/2));
//					} else if (right[0] < dist -10 && right[0] > dist -30) {
//						sd.setExampleData(-abs(steer/2));
//					} else {
//						sd.setExampleData(-(steer/2));
//					}

					spd.setSpeedData(2);
					dd.setDistanceData(dist);
					cout << "Mode: 9" << endl;

				}

			} else if (checkLane(left, w) != 1 && straightLine(left, y_pos) == true) {

				if (((left[0] < dist+2 && left[0] > dist -2) || dist == 0)
					&& abs(steer) < 0.001){

					//cout << "Distance: " << right[0] << endl;
					sd.setExampleData(0);
					spd.setSpeedData(2);
					dd.setDistanceData(left[0]);
					cout << "Mode: 4" << endl;

				} else {

					sd.setExampleData((dist - left[0])/27 * Constants::DEG2RAD);
					spd.setSpeedData(2);
					dd.setDistanceData(dist);
					cout << "Mode: 5" << endl;

				}

			} else {

				double tangent = atan2(y_pos[0] - y_pos[1], right[0] - right[1]);
				cout << tangent << endl;
				double steeringAngle;

//				if (fabs(tangent) > 0.66) {
//					steeringAngle = steeringLight;
//				} else {
					steeringAngle = steeringSteep;
//				}

				if(right[0] == dist){
					sd.setExampleData(0);
					spd.setSpeedData(2);
					cout << "Mode: 8" << endl;
				}
				else if (abs(tangent) < 0) {
					sd.setExampleData(steeringAngle);
					//cout << "Diff: "<<right[0] - dist<<endl;
					//cout << "Distance: " << right[0] << endl;
					spd.setSpeedData(2);
					cout << "Mode: 7" << endl;
//				} else if (tangent  0) {
//					sd.setExampleData((right[0] - dist)/15 * Constants::DEG2RAD);
//					//cout << "Distance: " << right[0] << endl;
//					spd.setSpeedData(2);
//					cout << "Mode: 6" << endl;
				} else {
					//cout << "Diff: "<<right[0] - dist<<endl;
					sd.setExampleData(- steeringAngle);
					spd.setSpeedData(2);
					cout << "Mode: 6" << endl;
				}
//				else {
//					sd.setExampleData((right[0] - dist)/15 * Constants::DEG2RAD);
//					spd.setSpeedData(2);
//					cout << "Mode: 7" << endl;
//				}

				dd.setDistanceData(dist);

			}
		}

		//Send message
		Container c(Container::USER_DATA_1, sd);
		Container c_1(Container::USER_DATA_2, spd);
		Container c_2(Container::USER_DATA_3, dd);
		getConference().send(c);
		getConference().send(c_1);
		getConference().send(c_2);

    }


    // This method will do the main data processing job.
    // Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
    ModuleState::MODULE_EXITCODE LaneDetector::body() {
	    // Get configuration data.
	    KeyValueConfiguration kv = getKeyValueConfiguration();
	    m_debug = kv.getValue<int32_t> ("lanedetector.debug") == 1;

        Player *player = NULL;
/*
        // Lane-detector can also directly read the data from file. This might be interesting to inspect the algorithm step-wisely.
        core::io::URL url("file://recorder.rec");

        // Size of the memory buffer.
        const uint32_t MEMORY_SEGMENT_SIZE = kv.getValue<uint32_t>("global.buffer.memorySegmentSize");

        // Number of memory segments.
        const uint32_t NUMBER_OF_SEGMENTS = kv.getValue<uint32_t>("global.buffer.numberOfMemorySegments");

        // If AUTO_REWIND is true, the file will be played endlessly.
        const bool AUTO_REWIND = true;

        player = new Player(url, AUTO_REWIND, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS);
*/

        // "Working horse."
	    while (getModuleState() == ModuleState::RUNNING) {
		    bool has_next_frame = false;

		    // Use the shared memory image.
            Container c;
            if (player != NULL) {
		        // Read the next container from file.
                c = player->getNextContainerToBeSent();
            }
            else {
		        // Get the most recent available container for a SHARED_IMAGE.
		        c = getKeyValueDataStore().get(Container::SHARED_IMAGE);
            }                

		    if (c.getDataType() == Container::SHARED_IMAGE) {
			    // Example for processing the received container.
			    has_next_frame = readSharedImage(c);
		    }

		    // Process the read image.
		    if (true == has_next_frame) {
			    processImage();
		    }
	    }

        OPENDAVINCI_CORE_DELETE_POINTER(player);

	    return ModuleState::OKAY;
    }

} // msv

