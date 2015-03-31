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

    // You should start your work in this method.
    void LaneDetector::processImage() {

		//Gray scale picture
    	IplImage *im_gray = cvCreateImage(cvGetSize(m_image),IPL_DEPTH_8U,1);
		cvCvtColor(m_image,im_gray,CV_RGB2GRAY);

		IplImage *out_image = cvCreateImage(cvGetSize(m_image),8,3);
		cvCvtColor(im_gray, out_image, CV_GRAY2RGB);

		//initiate values
		int w = m_image->width;
		int h = m_image->height;
		int gap = 65;
		int move = m_image->widthStep;
		int right = 0;
		int left = 0;
		int right_gap = 238;
		CvPoint middle_bottom = cvPoint (w/2, h);
		CvPoint middle_top = cvPoint(w/2,0);
		CvScalar red = CV_RGB(255,0,0);
		CvScalar blue = CV_RGB(0,0,255);
		CvScalar green = CV_RGB(0,255,0);
		CvPoint left_end;
		CvPoint right_end;
		CvPoint start_horz = cvPoint(w/2, h-gap);

		//Get left and right distance
		while(((m_image->imageData) + move * (h- gap+2)) [(w/2+right)*3]==0 && right < w/2 ){ right ++;}
		right_end = cvPoint(w/2+right, h-gap);


		//cout << "right:"<< right<<endl;

		
		//while(((m_image->imageData) + move * (h- gap+2)) [(w/2+left)*3]==0 && left < w/2 ){ left --;}
		left =right - 476;
		left_end = cvPoint(w/2+left, h-gap);

		//Draw lines
        if (m_debug) {
            if (m_image != NULL && im_gray!= NULL){

				cvLine(out_image, start_horz, right_end, green, 3, 8);
				cvLine(out_image, start_horz, left_end, blue, 3, 8);
				cvLine(out_image, middle_bottom, middle_top, red, 3, 8);

                cvShowImage("WindowShowImage", out_image);
                cvWaitKey(10);


            }
        }


		SteeringData sd;
		SpeedData spd;
		int i = 0;

		if (right == w/2){
			//stop
			spd.setSpeedData(0);

			//intersection handling
			//TODO

			//Continue straight for now
			while (i<100) i++;
			sd.setExampleData(0);
			spd.setSpeedData(1);

//		} else if (right == w/2) {
//			//Go straight in the middle of intersection
//			sd.setExampleData(0);
//			spd.setSpeedData(1);
		} else {

			//Steer the car
			int diff = right-right_gap;

			if (diff < -10) {
				sd.setExampleData(-10);
				spd.setSpeedData(0.4);
			} else if (diff >10){
				sd.setExampleData(10);
				spd.setSpeedData(0.4);
			} else {
				sd.setExampleData(0);
				spd.setSpeedData(1);
			}
		}


		//Send message
		Container c(Container::USER_DATA_1, sd);
		Container c_1(Container::USER_DATA_2, spd);
		getConference().send(c);
		getConference().send(c_1);

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

