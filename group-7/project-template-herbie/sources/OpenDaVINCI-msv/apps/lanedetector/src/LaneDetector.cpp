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
		//Get the white pixel, ctrl to get left or right side
		int move = m_image->widthStep;
		int w = m_image->width, h = m_image->height;

		while(((m_image->imageData) + move * (h- gap+2)) [(w/2+x)*3]==0 && x < w/2){
			if (ctrl == -1) x --;
			else x ++;
		}
		if (x > w/2) x = w/2;
		else if (x < -w/2)  x = -w/2;
		return x;

	}

	bool straightLine (int *x_pos, int *y_pos){
		//Check if line is straight
		int i;
		double angle[3];

		for (i = 1; i < 4; i++){
			angle[i-1] = atan2((y_pos[0] - y_pos[i]), (x_pos[0]-x_pos[i]));
		}

		//If calculated angles are almost the same, line is straight
		for (i = 1; i < 3; i++){
			if (fabs(angle[0] - angle[i]) > 0.016) return false;

		}

		return true;
	}

	bool proportional (int *x_pos){
		//If differences between one line and its previous and after are not almost the same
		//line may be not a continuous line
		int i;
		int diff[3];
		int diff_1[2];
		for (i = 0; i<3; i++){
			int prev = *x_pos;
			x_pos++;
			diff[i] = prev - *x_pos ;
		}

		for (i= 0; i<2; i++){
			diff_1[i] = abs(diff[i] - diff[i+1]);
			if (abs(diff_1[i])>7) return false;
		}

		return !(abs(diff_1[0] - diff_1[1])> 5 || diff_1[0]*diff_1[1] < 0);

	}

	double getAngle (int *x_pos, int *y_pos, int w){
		//Get angle (use when the lane is not fully detected))
		int new_x_pos[4], new_y_pos[4];
		int i, j=0;

		for(i=0; i<4;i++){
			if(abs(*x_pos) < w/2){
				new_x_pos[j] = *x_pos;
				new_y_pos[j] = *y_pos;
				j++;
			}
			x_pos++;
			y_pos++;
		}

		//Check if those chosen lines are on the same line or not (e.g. 2 on the dash lines
		//other one is on the left lane)
		if ( j> 2 && (((x_pos[0] < x_pos[1]) && (x_pos[1] < x_pos[2]))
					 ||((x_pos[0] > x_pos[1]) && (x_pos[1] > x_pos[2])))){
			return 0;
		}

		//If only 1 line satisfied, we cannot calculate the tangent either
		if (j < 2){
			return 0;
		} else {
			double angle = atan2((new_y_pos[0] - new_y_pos[1]), (new_x_pos[0]-new_x_pos[1]));
			return angle;
		}
	}



	/***Pimmie***/
	//compare array left or right to w/2
	int checkLane(int *arr, int w){

		int i;
		int check = 0, count = 0;

		for(i=0; i<4; i++){
			if(w/2 == abs(arr[i]))count = count+1;
			if(w/2 > abs(arr[i]))count = count -1;
		}


		if(count == 4) check = 1; //Lane is not detected
		if(count == (-4)) check = 2; //Lane is detected
		if(count < 4 && count != (-4)) check =3;

		return check;

	}

	/***Phuong***/
	//initiate global values
	int gap[4] = {60, 95, 125, 150};
	//The gap distance gets smaller, 3d drawing kind of thing :v
	CvScalar red = CV_RGB(255,0,0);
	CvScalar blue = CV_RGB(0,0,255);
	CvScalar green = CV_RGB(0,255,0);
	CvPoint left_pos[4], right_pos[4], start_pos[4];

	double steeringAngle;
	int dist[2];


	SteeringData sd;
	LongDistanceData ldd;
	ShortDistanceData sdd;
	Intersection id;
	LaneDetected ld;


	void LaneDetector::processImage() {


		IplImage *img_gray = cvCreateImage(cvGetSize(m_image),IPL_DEPTH_8U,1);
		IplImage *canny_image = cvCreateImage(cvGetSize(m_image),IPL_DEPTH_8U,1);

		//Canny image
		cvCvtColor(m_image,img_gray, CV_RGB2GRAY);
		cvCanny(img_gray, canny_image, 100, 150, 3 );
		cvCvtColor(canny_image, m_image, CV_GRAY2RGB);

		//Release unused image
		cvReleaseImage(&img_gray);
		cvReleaseImage(&canny_image);

		//initiate values
		int w = m_image->width, h = m_image->height;
		int right[4], left[4], i;

		int y_pos[4] = {h -gap[0], h-gap[1], h-gap[2], h-gap[3]};

		CvPoint middle_bottom = cvPoint (w/2, h);
		CvPoint middle_top = cvPoint(w/2,0);


		//Get left and right distance
		for (i = 0; i< 4; i ++) {
			start_pos[i] = cvPoint(w/2, y_pos[i]);

			left[i] = count(0, gap[i], m_image, -1);
			left_pos[i] = cvPoint(w/2 + left[i], y_pos[i]);

			right[i] = count(0, gap[i], m_image, 1);
			right_pos[i] = cvPoint(w/2 + right[i], y_pos[i]);
		}

		//Draw lines
        if (m_debug) {
            if (m_image != NULL){

				for (i = 0; i<4; i++){
					cvLine(m_image, start_pos[i], right_pos[i], green, 2, 8);
					cvLine(m_image, start_pos[i], left_pos[i], blue, 2, 8);
				}

				cvLine(m_image, middle_bottom, middle_top, red, 2, 8);

                cvShowImage("WindowShowImage", m_image);
                cvWaitKey(10);

            }
        }

		//Get old data
		Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
		SteeringData sd_old = containerSteeringData.getData<SteeringData> ();
		double steer = sd_old.getExampleData();

		Container containerLongDistanceData = getKeyValueDataStore().get(Container::USER_DATA_3);
		LongDistanceData ldd_old = containerLongDistanceData.getData<LongDistanceData>();
		dist[0] = (int)ldd_old.getLongDistanceData();


		Container containerShortDistanceData = getKeyValueDataStore().get(Container::USER_DATA_4);
		ShortDistanceData sdd_old = containerShortDistanceData.getData<ShortDistanceData>();
		dist[1] = (int)sdd_old.getShortDistanceData();

		bool laneDetected = false;
		bool intersect = false;

		cout<<"-----------------------------------------------------------"<<endl;
		cout<<"Old steering data: "<<steer * Constants::RAD2DEG<<endl;


		if (checkLane(left, w) == 1 && checkLane(right,w) == 1){ //No lane detected

			//If car is going quite straight, possibly car is at intersection, go straight
			if (abs(steer)< 2.5*Constants::DEG2RAD) steeringAngle = 0;
			//else do what it did before (for seeing no right lane)
			else steeringAngle = steer + 0.01*steer;

			cout<<"State 0: Detect no lane"<<endl;

		} else if (right[3] == w/2 && right[2] == w/2 && right[1] != w/2 && right[0] != w/2
				   && abs(steer) < 2.1*Constants::DEG2RAD){

			//Try to detect intersection
			//Try to detect intersection line
			int check;

			//check for intersection line
			//If they appears in this state, it gonna have to be between gap[1] and[2]
			for (i = gap[1]; i<gap[2]; i++){

				check = count(0,i,m_image,w);

				if (check < 5){
					//because the red line is 2 pixel, check is 2 when it detects a white spot
					//then car go to intersection mode
					intersect = true;
					break;
				}
			}
			
			steeringAngle = 0;
			cout<<"State 1: At Intersection"<<endl;

		} else if(checkLane(left,w) == 3 && checkLane(right,w) != 2) {
			//When left is not detected and we can only get some data from right

			if (abs(steer) < 2.1*Constants::DEG2RAD ) {
				//If car is going quite straight, it probably in the middle of intersection
				steeringAngle = 0;
				cout << "State 2: Middle of intersection" << endl;

			} else {
				//Else we lost the sight of right side, following left lane
				double angle = getAngle(left, y_pos, w);
				if (abs(angle) < 0.001) steeringAngle = steer;
				else steeringAngle = abs(angle) > 1.5 ? steer : angle / 3.5;

				cout << "State 3: Follow left side" << endl;
			}


		} else if (checkLane(left, w) == 1 && checkLane(right, w) !=2){

			//Not enough data to work with
			//Do what it did before

			steeringAngle = steer;
			cout<<"State 4: Not enough data"<<endl;

		} else if (checkLane(right, w) == 2 || checkLane(left,w) == 2){

			//This case, either left or right lane is fully detected
			laneDetected = true;

			if (checkLane(right, w) == 2 && straightLine(right, y_pos)) {
				//right lane detected and is a straight line

				if ((right[3] == dist[1] || dist[0] == 0)
						&& abs(steer) < 0.001){

					//car is parallel to the road, go straight and save critical distance
					steeringAngle = 0;
					dist[0] = right[0];
					dist[1] = right[3];
					cout <<  "State 5: Straight, right lane" << endl;

				} else {

					//else balance itself
					int diff = right[3] - dist[1];
					steeringAngle = diff /(abs(diff) < 70? 30: 15) * Constants::DEG2RAD;
					cout << "State 6: Balancing, right lane" << endl;

				}

			} else if (checkLane(left, w) == 2 && straightLine(left, y_pos)) {
				//left lane detected and is a straight line

				if ((left[3] == dist[1] || dist[0] == 0)
					&& abs(steer) < 0.001){

					//Car is parallel to the road, go straight and save critical distance
					steeringAngle = 0;
					dist[0] = abs(left[0]);
					dist[1] = abs(left[3]);
					cout << "State 7: Straight, left lane" << endl;

				} else {

					//Balance the car
					int diff = dist[1] - abs(left[3]);
					steeringAngle = diff /(abs(diff) < 70? 30: 15) * Constants::DEG2RAD;
					cout << "State 8: Balancing, left lane" << endl;

				}

			} else if (checkLane(right,w) == 2){
				//Detect full lane on the right, in curve
				//Check to prevent car from steering to early
				if(right[0] == dist[0] && abs(steer) < 0.001){
					steeringAngle = 0;
					cout << "State 9: Almost at curve, right lane" << endl;
				} else if (proportional(right)){
					//Proportional check to see if the line is possibly continuous or not
					//If yes, steer use steering angle
					int diff = right[3] - dist[1];

					//If diff and steer does not have same sign, it means the car changed direction
					//Therefore sharp turn
					//Else an less sharp turn
					if ((diff * steer) < 0.000001){
						steeringAngle = (diff/6.3 * Constants::DEG2RAD);
					} else {
						steeringAngle = (diff/11 * Constants::DEG2RAD);
					}
					cout << "State 10: Turn, right lane" << endl;
				} else {
					//else do what it did before
					steeringAngle = steer;
					laneDetected = false;
					cout << "State 11: Use old data, right lane" << endl;
				}


			} else {

				//Same of right curve but this time, left lane is fully detected

				if(left[0] == dist[0] && abs(steer) < 0.001){
					steeringAngle = 0;
					cout << "State 12: Almost at curve, left lane" << endl;
				} else if (proportional(left)){

					int diff = dist[1] - abs(left[3]);
					if ((diff * steer) < 0.000001){
						steeringAngle = (diff/6.3 * Constants::DEG2RAD);
					} else {
						steeringAngle = (diff/11 * Constants::DEG2RAD);
					}
					cout << "State 13: Turn, left lane" << endl;

				} else {
					steeringAngle = steer;
					laneDetected = false;
					cout << "State 15: Use old data, left lane" << endl;
				}


			}

		}
		//Send data to container
		id.setIntersection(intersect);
		sd.setExampleData(steeringAngle);
		ldd.setLongDistanceData(dist[0]);
		sdd.setShortDistanceData(dist[1]);
		ld.setLaneDetected(laneDetected);



		//Send message
		Container c(Container::USER_DATA_1, sd);
		Container c_2(Container::USER_DATA_3, ldd);
		Container c_3(Container::USER_DATA_4, sdd);
		Container c_4(Container::USER_DATA_5,ld);
		Container c_5(Container::USER_DATA_6, id);

		getConference().send(c);
		getConference().send(c_2);
		getConference().send(c_3);
		getConference().send(c_4);
		getConference().send(c_5);

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

