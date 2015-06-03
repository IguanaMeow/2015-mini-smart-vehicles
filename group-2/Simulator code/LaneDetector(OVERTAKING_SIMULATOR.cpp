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
#include "time.h"

bool r1 = false;
bool l1 = false;
bool s1 = false;
bool hr1 = false;
bool hl1 = false;
bool Overtake = false;
bool firstTimeLaneSwitch = true;
bool LaneSwitch = false;
bool LaneSwitchBack = false;

struct timespec LaneSwitchStart;
struct timespec LaneSwitchStart2;
time_t baseTimor;
time_t baseTimor2;
time_t baseTimorMillis;
time_t baseTimorMillis2;


// sec/msec target time
// sec2/msec2 current time
bool hasTimePassed(time_t sec, time_t msec, time_t sec2, time_t msec2){
	if(sec2 > sec)
	{
		return true;
	
	}
	else if(sec2 == sec)
	{
		if(msec2 >= msec)
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}
	else
	{
		return false;
	}
}

//code by Magnus Johansson; function just draws a line between two Points
void DrawLine( cv::Mat img, cv::Point start, cv::Point end, cv::Scalar color)
{
  int thickness = 2;
  int lineType = 8;
  line( img,
        start,
        end,
        //cv::Scalar( 0, 0, 255 ),
        color,
        thickness,
        lineType );
}

void DrawCircle( cv::Mat img, cv::Point center)
{
	int thickness = 2;
	int lineType = 8;
	circle( img,
			center,
			50,
			cv::Scalar(0,0,255),
			thickness,
			lineType );
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

    // You should start your work in this method.
    void LaneDetector::processImage() {
    	// Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.
        SteeringData sd;
        sd.setExampleData(1234.56);

   // 2. Get most recent sensor board data:
                Container containerSensorBoardData = getKeyValueDataStore().get(Container::USER_DATA_0);
                SensorBoardData sbd = containerSensorBoardData.getData<SensorBoardData> ();
                cerr << "Most recent sensor board data: '" << sbd.toString() << "'" << endl;
        
        
        //CODE WRITTEN BY WILLIAM BERGENDAHL, CARL-OSCAR PERSSON, JONATHAN KLEMETZ, JOHAN HERMANSSON, MAGNUS JOHANSSON


    	double distance = 0;
	double leftDistance = 0;
    	double distanceToStop = 0;
    	
        // Create a Mat object image and then draw lines between points
        if (m_debug) {
        	cv::Mat img(m_image);
	        int rows = img.rows;
	        int cols = img.cols;
	        cv::Size s = img.size();

	        rows = s.height;
	        cols = s.width;

	        cv::Point p1, p2, p3, p4, p5;

	        p1.x=cols/2;
	        p1.y=400;
	        p2.x=cols/2;
	        p2.y=rows;
	        p3.x=cols/2;
	        p3.y=400;
	        p4.x=cols/2;
	        p4.y=400;
	        p5.x=cols/2;
	        p5.y=400;

	        //we find the white lines by checking the intensity of a pixel
	     	cv::Vec3b intensity = img.at<cv::Vec3b>(p3);

	        //drawing the left lane checker
			
	        while (p3.x != 0){
	        	intensity = img.at<cv::Vec3b>(p3);
	        	//when intensity is white, we stop drawing the line
	        	if (intensity[0] == 255 && intensity[1] == 255 && intensity[2] == 255){
	        		break;
	        	}
	        	p3.x = p3.x -1;
	        }
	        	
	        DrawLine(img, p3, p4, cv::Scalar(255,0,0));
		leftDistance = p4.x - p3.x;

	        //drawing the right lane checker
	        cv::Vec3b intensity2 = img.at<cv::Vec3b>(p5);
	        while (p5.x != cols){
	        	intensity2 = img.at<cv::Vec3b>(p5);
	        	if (intensity2[0] == 255 && intensity2[1] == 255 && intensity2[2] == 255){
	        		break;
	        	}
	        	p5.x = p5.x +1;
	        }
	        DrawLine(img, p5, p4, cv::Scalar(0,255,0));

	        distance = p4.x - p5.x;

	        //we also need to find stop lines at intersections

	        cv::Vec3b intensity3 = img.at<cv::Vec3b>(p1);

	        while (p1.y != 0){
	        	intensity3 = img.at<cv::Vec3b>(p1);
	        	if (intensity3[0] != 0 && intensity3[1] != 0 && intensity3[2] != 0){
	        		break;
	        	}
	        	p1.y = p1.y -1;
	        }
	        DrawLine(img, p1, p2, cv::Scalar(0,0,255));
	        distanceToStop = p1.y - p4.y;

        	cvShowImage("WindowShowImage", m_image);
			cvWaitKey(10);
        }

        //TODO: Start here.
        
        /*
        cv::Mat roi(mat_img, cv::Rect(10,10,100,100));
        roi = cv::Scalar(0,255,0);

        MyLine(m_image,pt,pt2); */


        // 1. Do something with the image m_image here, for example: find lane marking features, optimize quality, ...

	
        sd.setDistanceToStop(distanceToStop);
	sd.setDistanceToRight(distance);
	sd.setDistanceToLeft(leftDistance); 
	  double sen3 = sbd.getValueForKey_MapOfDistances(3);	
  	  double sen6 = sbd.getValueForKey_MapOfDistances(0);	
  	  double sen2 = sbd.getValueForKey_MapOfDistances(2);	
 	  double sen4 = sbd.getValueForKey_MapOfDistances(4);

		//BY WILLIAM & CALLE
	if(sen3 < 10 && sen3 > -1){
		Overtake = true;
	}
	
	cerr << "OVERTAKE '" << Overtake << "'" << endl;
	//END OF OUR CODE HERE



if(Overtake == false){

//JONATHAN, JOHAN
    
    //check the distance to the right and goes in the right if-statement and set the SteeringData for the driver
        if(distance > -231 && distance < -219){
        	sd.setDriveStraight(1);
        	s1 = true;
        	r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = false;

	}else if(distance > -220 && distance < -181) {
        	sd.setTurnLeft(1);
        	s1 = false;
        	r1 = false;
	 		l1 = true;
			hr1 = false;
			hl1 = false;
        }else if (distance >= -180 && distance <= -171){
        	sd.setTurnHarderLeft(1);
        	s1 = false;
        	r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = true;
        }else if (distance >= -170 ){
        	sd.setTurnSuperHardLeft(1);
        	s1 = false;
        	r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = false;
        }else if (distance < -230 && distance > -261){
        	sd.setTurnRight(1);
        	s1 = false;
        	r1 = true;
	 		l1 = false;
			hr1 = false;    
			hl1 = false;
        }else if (distance < -260 && distance > -319){
        	sd.setTurnHarderRight(1);
        	s1 = false;
        	r1 = false;
	 		l1 = false;
			hr1 = true;
			hl1 = false;

        }else if (distance < -319){// if not find and stop on the line to the right, if the previous was driveStraigh, TurnLeft or TurnRight drive straight, else continue with the previous steering.
        	if (hr1 == true){
        		sd.setTurnHarderRight(1);
        	}
        	else if (hl1 == true){
        		sd.setTurnHarderLeft(1);
        	}
        	else {
        		sd.setDriveStraight(1);
        	}
        }//if stopline is found
        if (distanceToStop > -50 && sd.getTurnHarderLeft() != true && sd.getTurnHarderRight() != true && sd.getTurnSuperHardRight() != true &&  sd.getTurnSuperHardLeft() != true &&  sd.getTurnLeft() != true &&  sd.getTurnRight() != true   ){
        	sd.setStopAtIntersect(1);
        	sd.setDriveStraight(0);  

   	} 
}

	//OVERTAKING WRITTEN BY WILLIAM BERGENDAHL, CARL-OSCAR PERSSON


 else if (Overtake == true){
	if(firstTimeLaneSwitch)
	{
		firstTimeLaneSwitch = false;
		LaneSwitch = true;

	
	}
		
		
	cerr << "Time '" << baseTimorMillis << "'" << endl;

     	if(LaneSwitch){
		LaneSwitchBack = false;
		clock_gettime(CLOCK_REALTIME, &LaneSwitchStart);
		if(baseTimor == 0){
			 baseTimor = LaneSwitchStart.tv_sec;
			 baseTimorMillis = LaneSwitchStart.tv_nsec;
		
		}
		//else if(LaneSwitchStart.tv_sec - baseTimor <= 4.4 && baseTimor != 0)
		else if(!hasTimePassed(baseTimor + 4, baseTimorMillis + 70000000, LaneSwitchStart.tv_sec, LaneSwitchStart.tv_nsec))
		{
			sd.setTurnSuperHardLeft(1);
			s1 = false;
        		r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = true;
			cerr << "BaseTimor '" << LaneSwitchStart.tv_sec - baseTimor << "'" << endl;
			
		
		}
		else
		{
			LaneSwitch = false;
			baseTimor = 0;
 			baseTimorMillis = 0;
		}

 		cerr << "Most recent time '" << LaneSwitch << "'" << endl;
 		
		

		
	}
	else if(LaneSwitchBack){
		LaneSwitch = false;
		clock_gettime(CLOCK_REALTIME, &LaneSwitchStart2);

		if(baseTimor2 == 0){ 
			baseTimor2 = LaneSwitchStart2.tv_sec;
			baseTimorMillis2 = LaneSwitchStart2.tv_nsec;

		}
		//else if(LaneSwitchStart2.tv_sec - baseTimor2 <= 4 && baseTimor2 != 0)
		else if(!hasTimePassed(baseTimor2 + 5, baseTimorMillis2 + 0, LaneSwitchStart2.tv_sec, LaneSwitchStart2.tv_nsec))
		{

			sd.setTurnSuperHardRight(1);
			s1 = false;
        		r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = false;
			cerr << "BaseTimor2 '" << LaneSwitchStart2.tv_sec - baseTimor2 << "'" << endl;
		
		}
		else
		{
			LaneSwitchBack = false;
			firstTimeLaneSwitch = true;
			baseTimorMillis2 = 0;
			Overtake = false;
			cerr << "STOP OVERTAKING!" << endl;
			baseTimor2 = 0;
		}

 		cerr << "Most recent time '" << LaneSwitch << "'" << endl;
 		
		
		
	}
        else if(leftDistance < 231 && leftDistance > 219){
        	sd.setDriveStraight(1);
        	s1 = true;
        	r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = false;
        }
        else if(leftDistance < 220 && leftDistance > 180) {
        	sd.setTurnRight(1);
        	s1 = false;
        	r1 = true;
	 		l1 = false;
			hr1 = false;
			hl1 = false;
        }else if (leftDistance < 180 && leftDistance > 171){
        	sd.setTurnHarderRight(1);
        	s1 = false;
        	r1 = false;
	 		l1 = false;
			hr1 = true;
			hl1 = false;
        }else if (leftDistance < 171){
        	sd.setTurnSuperHardRight(1);
        	s1 = false;
        	r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = false;
        }else if (leftDistance > 230 && leftDistance < 261){
        	sd.setTurnLeft(1);
        	s1 = false;
        	r1 = false;
	 		l1 = true;
			hr1 = false;    
			hl1 = false;
        }else if (leftDistance > 260 && leftDistance <319){
        	sd.setTurnHarderLeft(1);
        	s1 = false;
        	r1 = false;
	 		l1 = false;
			hr1 = false;
			hl1 = true;	
        	
        }else if (leftDistance > 319){
		sd.setTurnLeft(1);
        	if (hr1 == true){
        		sd.setTurnHarderRight(1);
        	}
        	else if (hl1 == true){
        		sd.setTurnHarderLeft(1);
        	}
        	else {
        		sd.setDriveStraight(1);
        	}
        }

	if(sen2 < 0 && sen6 < 0 && sen4 < 0) {
	LaneSwitchBack = true;	
	}

	
}
   //END OF OVERTAKING







        // 2. Calculate desired steering commands from your image features to be processed by driver.

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


