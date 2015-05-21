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
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/image/SharedImage.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include "tools/player/Player.h"

#include "GeneratedHeaders_Data.h"

#include "LaneDetector.h"

using namespace cv;
using namespace std;

 
void drawLine(Mat img, Point start, Point end, int thickness, int lineType, int B, int G, int R){
    line (img, start,end,Scalar(B,G,R),thickness, lineType);

}

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
			    //cvFlip(m_image, 0, -1);

			    retVal = true;
		    }
	    }
	    return retVal;
    }

  Point findWhitePixel(Point point, Mat& img, char side){    //this function searches for white pixel
  Vec3b pixel = img.at<Vec3b>(point); 			     //defines a a vector that contains the rgb values of each pixel
	if (side == 'r'){				     //r = right, l = left
        while(point.x != 640){	
        	point.x = point.x +1; 			     
        	pixel = img.at<Vec3b>(point); 
		if(pixel.val[0] == 255 && pixel.val[1] == 255 && pixel.val[2] == 255){	//break loop when white pixel is found
        		break;
		}
	}
	}
	else if (side == 'l'){
	while(point.x != 0){
        	point.x = point.x - 1; 
        	pixel = img.at<Vec3b>(point); 
        	if(pixel.val[0] == 255 && pixel.val[1] == 255 && pixel.val[2] == 255){
			break;
		}
	}
	}
	return point;
}

    // You should start your work in this method.
    void LaneDetector::processImage() {
        if (m_debug) {

        Mat mat_img(m_image);  //converts the IPL image to a mat image
        Mat gray, edge;  //initialise variables necessary to use for canny image
	int d1, d2;

        cvtColor(mat_img, gray, CV_BGR2GRAY);	//conversion of image to grayscale
        Canny( gray, edge, 50, 150, 3);		//detect edges and apply them to (edge) image

        cvtColor(edge, mat_img, CV_GRAY2BGR);	//conversion of image back to BGR scale
        gray.release(); edge.release();		//frees memory allocation for previous images

        int rows = mat_img.rows; int cols = mat_img.cols;

        Point centerStart;
        Point centerEnd;
        Point botArrowsStart;
        Point rightArrow;        
	Point leftTopArrow;
	Point leftBotArrow;
	Point leftTopArrowStart;
	Point backupLineStart;	//This line is used in case the car is critically close to the left lane and cannot find a line
	Point backupLine;
	
        centerStart.x=cols/2; 			centerStart.y=0;
        centerEnd.x=cols/2;  			centerEnd.y=rows;
        botArrowsStart.x = cols/2; 		botArrowsStart.y = 420;
	rightArrow.x = botArrowsStart.x;	rightArrow.y = botArrowsStart.y;
        leftBotArrow.x = botArrowsStart.x;  	leftBotArrow.y = botArrowsStart.y;
	leftTopArrowStart.x = cols/2; 		leftTopArrowStart.y = 350;
	leftTopArrow.x =leftTopArrowStart.x;    leftTopArrow.y = 350;
	backupLineStart.x = cols;		backupLineStart.y = 320;
	backupLine.x = cols;			backupLine.y = 320;
	

        rightArrow = findWhitePixel(rightArrow, mat_img, 'r');
    	leftBotArrow = findWhitePixel(leftBotArrow, mat_img, 'l');
	leftTopArrow = findWhitePixel(leftTopArrow, mat_img, 'l');
	backupLine = findWhitePixel(backupLine, mat_img, 'l');

	/*cout <<"leftBotArrow = " <<leftBotArrow.x<<endl;
	cout <<"right arrow = "<<rightArrow.x<<endl;
	cout <<"leftTopArrow = " <<leftTopArrow.x<<endl;
	cout <<"backupLine = " <<backupLine.x<<endl;*/

	drawLine(mat_img, centerStart ,centerEnd, 1, 8, 0, 0, 255);
	drawLine(mat_img, botArrowsStart,leftBotArrow, 1, 8, 240, 206, 81);
	drawLine(mat_img, botArrowsStart,rightArrow, 1, 8, 240, 206, 81);
        drawLine(mat_img, leftTopArrowStart,leftTopArrow, 1, 8, 240, 206, 81);
	drawLine(mat_img, backupLineStart,backupLine, 1, 8, 170, 150, 81);

        imshow("image", mat_img);      
	cvWaitKey(10);
        SteeringData sd;
        double angle = 0.0;
	d1 = rightArrow.x - 320;	//calculating first distance
	if(rightArrow.x > 620){
	d1 = 280;			//neutralising d1 in case right lane is not found
	}
	
	while(true){				//calculating d2 for different conditions
		if (leftBotArrow.x > 120) {
		d2 = 320 - leftBotArrow.x;
		break; 
			}
		else if (leftTopArrow.x > 110 && leftTopArrow.x < 300) {
		d2 = 320 - leftTopArrow.x;
		break;
			}
		else{
		d2 = 280;			//neutralise d2 in case no left lane is found
		break;
			} 
	}
	//cout <<"d1 = " <<d1<<endl;	
	//cout <<"d2 = " <<d2<<endl;
	
	angle = 0;
	sd.setExampleData(angle);

	if (rightArrow.x > 625 && leftBotArrow.x < 15){		//using emergency line here in case lines are not found
		if (backupLine.x > 0){
		angle = 18;					//hardcoded strong turn
		}
		else{
		angle = 0;			//if no lines at all are found, including the use of the emergency line, go 							//straight
		}
	sd.setExampleData(angle);
	//cout << "TURN right emergency: " << angle <<endl;
	}

	else if(d1 >= d2 + 50){			//balancing formulas for steering
	angle = d2 / 26 + 10;			//experimented non mathematical formulas used for dynamic steering
	sd.setExampleData(angle);
	//cout << "TURN Right: " << angle <<endl;
	}
	else if(d2 >= d1 + 50){
	angle = d1 / 9 - 23;
	sd.setExampleData(angle);
	//cout << "TURN Left: " << angle <<endl;
	}
	

        //TODO: Start here.
        // 1. Do something with the image m_image here, for example: find lane marking features, optimize quality, ...
        // 2. Calculate desired steering commands from your image features to be processed by driver.

        // Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.


        // Create container for finally sending the data.
        Container c(Container::USER_DATA_1, sd);
        // Send container.
        getConference().send(c);
    }
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


