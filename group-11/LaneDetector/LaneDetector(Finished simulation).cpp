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
			    cvFlip(m_image, 0, -1);

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
		if(pixel.val[0] == 255 && pixel.val[1] == 255 && pixel.val[2] == 255){
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

        cvtColor(mat_img, gray, CV_BGR2GRAY);
        Canny( gray, edge, 50, 150, 3);

        cvtColor(edge, mat_img, CV_GRAY2BGR);
        gray.release(); edge.release();

        int rows = mat_img.rows; int cols = mat_img.cols;

        Point centerStart;	
        Point centerEnd;
        Point botArrowsStart;
        Point rightArrow;        
	Point leftTopArrow;
	Point leftBotArrowStart;
	Point leftBotArrow;
	Point leftTopArrowStart;
	
	
        centerStart.x=cols/2; 			centerStart.y=0;
        centerEnd.x=cols/2;  			centerEnd.y=rows;
        botArrowsStart.x = cols/2; 		botArrowsStart.y = 420;
	rightArrow.x = botArrowsStart.x;	rightArrow.y = botArrowsStart.y;
	leftBotArrowStart.x = botArrowsStart.x; leftBotArrowStart.y = 400;
        leftBotArrow.x = botArrowsStart.x;  	leftBotArrow.y = 400;
	leftTopArrowStart.x = cols/2; 		leftTopArrowStart.y = 350;
	leftTopArrow.x =leftTopArrowStart.x;    leftTopArrow.y = 350;
	

        rightArrow = findWhitePixel(rightArrow, mat_img, 'r');
    	leftBotArrow = findWhitePixel(leftBotArrow, mat_img, 'l');
	leftTopArrow = findWhitePixel(leftTopArrow, mat_img, 'l');
	
	cout <<"leftTopArrow = " <<leftTopArrow.x<<endl;
	cout <<"leftBotArrow = " <<leftBotArrow.x<<endl;
	cout <<"rightArrow = " <<rightArrow.x<<endl;

	drawLine(mat_img, centerStart ,centerEnd, 1, 8, 0, 0, 255);
	drawLine(mat_img, leftBotArrowStart,leftBotArrow, 1, 8, 240, 206, 81);
	drawLine(mat_img, botArrowsStart,rightArrow, 1, 8, 240, 206, 81);
        drawLine(mat_img, leftTopArrowStart,leftTopArrow, 1, 8, 240, 206, 81);

        imshow("image", mat_img);      
	cvWaitKey(10);
        SteeringData sd;
        double angle = 0.0;

	d1 = rightArrow.x - 320;
	if(rightArrow.x > 620){
	d1 = 240;
	}
	
	while(true){
		if (leftBotArrow.x > 110) {
		d2 = 320 - leftBotArrow.x;
		break; 
			}
		else if (leftTopArrow.x > 100 && leftTopArrow.x < 300) {
		d2 = 320 - leftTopArrow.x;
		break;
			}
		else{
		d2 = 240;
		break;
			} 
	}
	cout <<"d1 = " <<d1<<endl;	cout <<"d2 = " <<d2<<endl;
	
	angle = 0;
	sd.setExampleData(angle);

	if(d1 >= d2 + 30){
	angle = d2 / 26 + 10;
	sd.setExampleData(angle);
	cout << "TURN Right: " << angle <<endl;
}
	else if(d2 >= d1 + 30){
	angle =  d1 / 9 - 23;
	sd.setExampleData(angle);
	cout << "TURN Left: " << angle <<endl;
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


