/**
 * lanedetector - Sample application for detecting lane markings.
 * Copyright (C) 2012 - 2015 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either ver_centr_startion 2
 * of the License, or (at your option) any later ver_centr_startion.
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
    using namespace cv;

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
//Sara Johansson
    void LaneDetector::processImage() {

        //changing the image
        const int low_threshold  = 50;
        const int high_threshold = 150;
        IplImage* grayImg;
        IplImage* cannyImg;

        grayImg = cvCreateImage( cvSize(m_image->width, m_image->height), IPL_DEPTH_8U, 1 );
        cvCvtColor( m_image, grayImg, CV_RGB2GRAY );//makes the image grey
        cannyImg = cvCreateImage(cvGetSize(m_image), IPL_DEPTH_8U, 1);

        cvCanny(grayImg, cannyImg, low_threshold, high_threshold, 3);//does canny edge detection on image
        cvCvtColor( cannyImg, m_image, CV_GRAY2RGB );

        cvReleaseImage(&grayImg);
        cvReleaseImage(&cannyImg);

   //variables

    int width = m_image -> width;
    int height = m_image -> height;
	  cout << "width"<< width<<endl;
    int move  = m_image ->widthStep;

    unsigned char* image = (unsigned char*)m_image->imageData;
    int sample_right = 60; // define near vision
		int right_2 = 200;
    int left_2 = 200;
    int sample_left1 = 60; // define further vision
		int sample_left2 = 100;
		int sample_left3 = 140;
		int sample_left4 = 180;
        //desired_right determines where we want the car to be positioned when going straight.
    int desired_right = 220;//244 220 (112 for the real track)
		int desired_left = 174;
		int desired_left2 = 206;
		int desired_left3 = 168;
		int desired_left4 = 131;

    // start points x and y variables for the lines
		cv::Point pt1; //right_sample_start
		pt1.x=width/2;
		pt1.y= height -60;

		cv::Point pt3; //left_sample_start1
		pt3.x=width/2;
		pt3.y=height -60;

		cv::Point pt5; //left_sample_start2
		pt5.x=width/2;
		pt5.y=height -100;

		cv::Point pt7; //left_sample_start3
		pt7.x=width/2;
		pt7.y=height -140;

		cv::Point pt9; //left_sample_start3
		pt9.x=width/2;
		pt9.y=height -180;

		cv::Point pt11; //left_sample_start3
		pt11.x=width/2;
		pt11.y=height -200;

        cv::Point pt13;
        pt13.x=width/2;
        pt13.y=height - 200;



		CvScalar blue = CV_RGB(0,0,250);
        int thickness = 1;
        int connectivity = 8;

        CvPoint ver_centr_start = cvPoint(width/2,height);
        CvPoint ver_centr_end = cvPoint(width/2,0);



        //TODO: Start here.

        int right = 0;

        int left = 0;
        int left2 = 0;
        int left3 = 0;
        int left4 = 0;

        // the while loops count the number of widthsteps has to be taken before reaching a lane marking.
        while((image + move * (height- sample_right+2)) [(width/2+right)*3]==0 && right < width/2 ){ right ++;
        }
        while((image + move * (height- sample_left1+2)) [(width/2+left)*3]==0 && left < width/2 ) {left--;
        }
        while((image + move * (height- sample_left2+2)) [(width/2+left2)*3]==0 && left2 < width/2 ) {left2--;
        }
        while((image + move * (height- sample_left3+2)) [(width/2+left3)*3]==0 && left3 < width/2 ) {left3--;
        }
        while((image + move * (height- sample_left4+2)) [(width/2+left4)*3]==0 && left4 < width/2 ) {left4--;
        }

//Simon
//These lines are created to detect and acknowledge an intersection
        int right_i = 0;
        int left_i = 0;
        while((image + move * (height- right_2 + 2)) [(width/2+right_i)*3]==0 && right_i < width/2 ){ right_i ++;}
        while((image + move * (height- left_2 + 2)) [(width/2+left_i)*3]==0 && left_i < width/2 ){ left_i --;}
        cv::Point pt12;//rightinter end
        pt12.x=width/2 +right_i;
        pt12.y=height-200;

        cv::Point pt14;
        pt14.x=width/2 + left_i;
        pt14.y=height-200;




//Sara Johansson
	  	cout << "right:"<< right<<endl;
	  	cout << "left:"<< left<<endl;
	  	// all the endpoints x and y variables of the lines
		cv::Point pt2;//right end
		pt2.x=width/2+right;
		pt2.y=height -60;

		cv::Point pt4;//left1 end
		pt4.x=width/2 +left;
		pt4.y=height-60;

		cv::Point pt6;//left2 end
		pt6.x=width/2 +left2;
		pt6.y=height-100;

		cv::Point pt8;//left3 end
		pt8.x=width/2 +left3;
		pt8.y=height-140;

		cv::Point pt10;//left3 end
		pt10.x=width/2 +left4;
		pt10.y=height-180;



       if (m_debug) {
                  if (m_image != NULL) {

                    //Drawing the lines on the m_image
                    cvLine(m_image,ver_centr_start,ver_centr_end,blue,thickness,connectivity);//center line
                    cvLine(m_image,pt1,pt2,blue,thickness,connectivity);//line one
                    cvLine(m_image,pt3,pt4,blue,thickness,connectivity);//line two (1)
                    cvLine(m_image,pt5,pt6,blue,thickness,connectivity);//line two (2)
                    cvLine(m_image,pt7,pt8,blue,thickness,connectivity);//line two (3)
                    cvLine(m_image,pt9,pt10,blue,thickness,connectivity);//line two (4)
                    cvLine(m_image,pt11,pt12,blue,thickness,connectivity);// second right line
                    cvLine(m_image,pt13,pt14,blue,thickness,connectivity);

                    cvShowImage("WindowShowImage", m_image);
                    cvWaitKey(10);
            }
        }

        SteeringData sd;

        double angle = ((right + 1) - desired_right); //difference between the distance we want and the actual distance
        //All the backupangles were a backup plan incase the lanefollowing needed more assistans on the real track.
        double backupangle1 = ((left +1) +desired_left);
        double backupangle2 = ((left2 +1) +desired_left2);
        double backupangle3 = ((left3 +1) +desired_left3);
        double backupangle4 = ((left4 +1) +desired_left4);
        double backupangle = 0;


        cout << "angle1 ="<< backupangle<<endl;//-489  on line 70

        if( right<= 3 || right >= 320 || right_i <=5 || right_i >=320) {// if the right line does not work for some reason then it uses the 4 left lines instead.
            if (backupangle1 <= 10 || backupangle1 >= -10) { backupangle = backupangle1;
                if(backupangle2 <=10 || backupangle2 >= -10) { backupangle = backupangle2;
                    if(backupangle3 <=10 || backupangle3 >= -10) { backupangle = backupangle3;
                        if(backupangle4 <=10 || backupangle4 >= -10) { backupangle = backupangle4;
                        }
                    }
                }
            }
        backupangle = backupangle * 0.20491803278;
        sd.setExampleData(backupangle);
        }
//Simon
        if(right >= 320) { angle = 0;}

        if(((left_i <=-320 && left4 <=-320) || (right_i >=320) )&&((right > 200 && right < 300) || (left2 < -200 && left2 > -300) )){angle = 500;}

        if((left<=-320 && left2<=-320 && left3<=-320 && left4 <=-320)&&(right_i >=320 || left_i <= -320)){angle = 1000;}

        angle = angle * 0.20491803278;
        cout << "angle"<< angle<<endl;
        sd.setExampleData(angle); // sending the steering wheel degree to driver.



        // Create container for finally sending the data.
        Container c(Container::USER_DATA_1, sd);
        // Send container.
        getConference().send(c);
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