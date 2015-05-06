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
			    m_sharedImageMemory->lock();{
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

    void drawLine(Mat img, Point start, Point end, int thickness, int lineType, int B, int G, int R){
    line (img, start,end,Scalar(B,G,R),thickness, lineType);
    }

    // You should start your work in this method.
    void LaneDetector::processImage() {
        if (m_debug) {

        Mat mat_img(m_image);  //converts the IPL image to a mat image
        Mat grayImg, cannyImg; //variables needed for detecting edges

        cvtColor(mat_img, grayImg, CV_BGR2GRAY); //converts the image to grayscale
        Canny( grayImg, cannyImg, 50, 150, 3); //finds edges with specified threshold

        cvtColor(cannyImg, mat_img, CV_GRAY2BGR); //converts back to RGB color scale
        grayImg.release();
        cannyImg.release();

        int rows = mat_img.rows;
        int cols = mat_img.cols;


        //Vertices for drawing the arrows
        Point pt1;  /*Central Arrow Start*/             Point pt2;  /*Central Arrow End*/
        Point pt3;  /*Right, leftBottom Arrow Start*/   Point pt4;  /*Right Arrow End*/
        Point pt5;  /*LeftBottom, Arrow Start*/         Point pt6;  /*LeftTop Arrow Start*/
        Point pt7;  /*LeftMid Arrow Start*/             Point pt9;  /*LeftMid Arrow End*/

        pt1.x=cols/2;    pt1.y=0; /*starting point for central arrow*/
        pt2.x=cols/2;    pt2.y=rows; /*ending point for central arrow*/

        drawLine(mat_img, pt1,pt2, 1, 8, 0, 0, 255); //central Arrow

        pt3.x = cols/2; pt3.y = 350;
        pt4.x = pt3.x;  pt4.y = pt3.y;

        pt5.x = pt3.x;  pt5.y = pt3.y;
        pt6.x = pt3.x;  pt6.y = 290;

        pt7.x = pt3.x;  pt7.y = 320;
        pt9.x = pt3.x;  pt9.y = pt7.y;


        Vec3b rightLaneColor = mat_img.at<Vec3b>(pt4); //defines the color at current positions
        while(pt4.x != cols){
            pt4.x = pt4.x +1; //extend the arrow
            rightLaneColor = mat_img.at<cv::Vec3b>(pt4); //checks for color at current position
            if(rightLaneColor.val[0] == 255 && rightLaneColor.val[1] == 255 && rightLaneColor.val[2] == 255){
                break; //color is white at current position
            }
        }

      drawLine(mat_img, pt3,pt4, 1, 8, 240, 206, 81); //Right Side Arrow

      Vec3b botleftLaneColor = mat_img.at<Vec3b>(pt5);
        while(pt5.x != 0){
            pt5.x = pt5.x -1;
            botleftLaneColor = mat_img.at<Vec3b>(pt5);
            if(botleftLaneColor.val[0] == 255 && botleftLaneColor.val[1] == 255 && botleftLaneColor.val[2] == 255){
                break;
            }
        }
         drawLine(mat_img, pt3,pt5, 1, 8, 240, 206, 81); //Left Bottom Arrow

        Vec3b midLeftColor = mat_img.at<Vec3b>(pt9);
        while(pt9.x != 0){
            pt9.x = pt9.x - 1;
            midLeftColor = mat_img.at<Vec3b>(pt9);
            if(midLeftColor.val[0] == 255 && midLeftColor.val[1] == 255 && midLeftColor.val[2] == 255){
                break;
            }
        }
         drawLine(mat_img, pt7,pt9, 1, 8, 240, 206, 81); //Left Mid Arrow


        imshow("image", mat_img);
        cvWaitKey(10);

        //Steering instructions from here
        SteeringData sd;
        double angle = 0.0;

        if(pt4.x < 470 && pt4.x > 370){
        angle = -1 * (pt4.x % 26);              //turns right
        sd.setExampleData(angle);  //sends the steering data to the driver.cpp code
        }
        else if(pt5.x > 190 || pt9.x > 200){
        angle =  pt9.x / 26 + 10;           //turns left
        sd.setExampleData(angle);
        }
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

