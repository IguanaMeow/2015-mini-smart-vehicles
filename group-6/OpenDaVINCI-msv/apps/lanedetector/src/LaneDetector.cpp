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
//Saleh and Jose
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/image/SharedImage.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

 using namespace cv;

#include "tools/player/Player.h"

#include "GeneratedHeaders_Data.h"

#include "LaneDetector.h"

void vertical_Line( cv::Mat image,cv::Point start_point, cv::Point end_Point )
{
line( image, start_point, end_Point, Scalar( 255, 0, 0),  2, 8);
}

void horizental_green_Line( cv::Mat image,cv::Point start_point, cv::Point end_Point )
{
line( image, start_point, end_Point, Scalar( 255, 0, 0),  2, 8);
}

namespace msv {
  
    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::image;
    using namespace tools::player;

	double angle;
	double speed;

    LaneDetector::LaneDetector(const int32_t &argc, char **argv) : ConferenceClientModule(argc, argv, "lanedetector"),
        m_hasAttachedToSharedImageMemory(false),
        m_sharedImageMemory(),
        m_image(NULL),
        m_debug(false) {}

    LaneDetector::~LaneDetector() {}
//





//
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

	double count_green = 0.0;
	double count_yellow = 0.0;
    
        // Example: Show the image.
        if (m_debug) {
            if (m_image != NULL) {
             	cvShowImage("WindowShowImage", m_image);
            	cvWaitKey(10);
            }
        }

        //TODO: Start here.

		Mat img,gray,edge, draw,dst,color_dst; 
        img = cvarrToMat(m_image, true);



		int rows = img.rows;
		int columns =img.cols;
		
		cv::Point p1,p2;    //Vertical_line,s starting and ending point
		cv::Point p3, p4;    //blue_line's starting and ending point
		p1.x=columns/2;
		p1.y=0;
		p2.x=columns/2;
		p1.y=rows;

		p3.x=columns/2;
		p3.y=rows/1.4;
		p4.x=columns;
		p4.y=rows/1.4;

  

      // if(img==NULL){
      // cerr << "ERROR! image not found" << endl;
      //  	return;
//
//}

       // 1. Do something with the image m_image here, for example: find lane marking features, optimize quality, ...

    	 // cvtColor( img, gray, CV_BGR2GRAY );
           //Canny( gray, edge, 50, 150, 3);
          //edge.convertTo(draw, CV_8U);


		Canny( img, dst, 50, 200, 3 );
     	cvtColor( dst, color_dst, CV_GRAY2BGR );

//		vector<Vec4i> lines;
 //  	HoughLinesP( dst, lines, 1, CV_PI/180, 90, 150,90);
   // 	for( size_t i = 0; i < lines.size(); i++ )
  //  	{
    //    line( color_dst, Point(lines[i][0], lines[i][1]),
 //           Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
   // 	}


		vertical_Line( img, p1, p2);

//		Vec3b color = color_dst.at<Vec3b>(p1.x, p1.y);
//		if(color.val[0]==255 && color.val[1]==255 && color.val[2]==255) {
//		HoughLinesP( dst, lines, 1, CV_PI/180, 90, 300,50);
  //  	for( size_t i = 0; i < lines.size(); i++ )
   // 	{
     //   line( color_dst, Point(lines[i][0], lines[i][1]),
       //     Point(lines[i][2], lines[i][3]), Scalar(255,255,255), 3, 8 );
    //	}
//	}

//horizental_green_Line
        for (int j=p3.x; j<img.cols; j++)
       	{  int i=p3.y;
  // start drawing the horizental_green_Line until it detect a red color(long lane),by then stop drawing
  		if(img.at<Vec3b>(i,j)[0] == 255 && img.at<Vec3b>(i,j)[1] == 255 &&img.at<Vec3b>(i,j)[2]  ==255)
			break;

               	img.at<Vec3b>(i,j)[0] = 0;
                img.at<Vec3b>(i,j)[1] = 200;
                img.at<Vec3b>(i,j)[2] = 0;
 				
 				count_green++;
	
//cut<<count_green<<endl;	
           
        }
  // }
//horizental_yellow_Line
		for (int j=p3.x; j<img.cols; j++)
       	{  int i=p3.y/0.8;
  // start drawing the horizental_yellow_Line until it detect a red color(long lane),by then stop drawing
  		if(img.at<Vec3b>(i,j)[0] == 255 && img.at<Vec3b>(i,j)[1] == 255 && img.at<Vec3b>(i,j)[2]  ==255)
			break;

                img.at<Vec3b>(i,j)[0] = 0;
                img.at<Vec3b>(i,j)[1] = 255;
                img.at<Vec3b>(i,j)[2] = 255;
				
				count_yellow++;
	//cout<<count_yellow<<endl;
           
        }
   

//		for (int j=p3.x; j<img.cols; j--)
  //    {  int i=(p3.y)/0.9;
  // start drawing the horizental_yellow_Line until it detect a red color(long lane),by then stop drawing
  //	if(img.at<Vec3b>(i,j)[0] == 0 && img.at<Vec3b>(i,j)[1] == 0 && img.at<Vec3b>(i,j)[2]  ==255)
	//		break;

  //              img.at<Vec3b>(i,j)[0] = 0;
    //            img.at<Vec3b>(i,j)[1] = 255;
      //          img.at<Vec3b>(i,j)[2] = 255;

	
           
        //}
 
//horizental_green_Line( color_dst, p3, p4);

           namedWindow( "Result window", CV_WINDOW_AUTOSIZE );  
           imshow( "Result window", img );  // instead img put imgROI
			cvWaitKey(10);




// herreeee

        // 2. Calculate desired steering commands from your image features to be processed by driver.

		SteeringData sd;
//		SpeedData speed;
//		Driver d;
//		d.desiredSteeringWheelAngle=15;
//		double angle = 0.0;
//		sd.setExampleData(angle);
//why not this does not change
//		for(int i=0;i<100;i++){

		cout<<count_green/count_yellow<<endl;
//cout<<count_green<<endl;
//cout<<count_yellow<<endl;


//left_ curve
		if ((count_green/count_yellow)>0.55 &&(count_green/count_yellow)<0.65 ) {
		speed=1.0;
		angle=-12.0;

		cout<<"1"<<endl;
		
		}else if((count_green/count_yellow)<0.1) {
		speed=2.0;
		angle=-4.0;
		
		cout<<"2"<<endl;
		
		}else if (count_green<30.0){
		speed=0.5;
		angle=-12.8;

		cout<<"3"<<endl;
		
		}else if(count_green<10) {
		speed=2;
		angle=-6.0;

		cout<<"4"<<endl;
		
		}else if((count_green/count_yellow)>0.7&&(count_green/count_yellow)<1.0) {
		speed=2.0;
		angle=12.0;
		
		cout<<"5"<<endl;
		
		}else if((count_green/count_yellow)>1.3&& count_yellow<200 && count_yellow>30) {
		speed=2;
		angle=16;
		
		cout<<"6"<<endl;
		
		}else if((count_green/count_yellow)>1.3&& (count_green/count_yellow)<20.0 && count_yellow<30) {
		speed=2;
		angle=12;
		
		cout<<"7"<<endl;
		
		}else if((count_green/count_yellow)>20.0&& count_yellow<30) {
		speed=2;
		angle=-6;

		cout<<"7"<<endl;
		
		}else{
		speed=2.4;
		angle=0;
		
		cout<<"8"<<endl;
	}
		
		sd.setSpeedData(speed);


		sd.setExampleData(angle);

/*
// right_curve
else if (((count_green/count_yellow)>0.98)&&((count_green/count_yellow)<1.1)&& count_green<250){
sd.setSpeedData(1);
sd.setExampleData(1);
cout<<"7"<<endl;
}else if (((count_green/count_yellow)>0.98)&&((count_green/count_yellow)<1.1)&& count_green>250 &&count_green<250 ){
sd.setSpeedData(0.5);
sd.setExampleData(10);
cout<<"8"<<endl;
}else if ((count_green/count_yellow)>1.0){
sd.setSpeedData(0.5);
sd.setExampleData(10);
cout<<"10"<<endl;
}else if (count_green>151&& count_green<230&&count_yellow>221&&count_yellow<290){
sd.setSpeedData(0.5);
sd.setExampleData(10.0);
cout<<"11"<<endl;
}



else if(((count_green/count_yellow)<0.60) &&((count_green/count_yellow)>0.50)){
sd.setSpeedData(3.0);
sd.setExampleData(-5);
cout<<"3"<<endl;
}
else if(((count_green/count_yellow)<0.65)&&((count_green/count_yellow)>0.50)){
sd.setExampleData(-5);
cout<<"4"<<endl;
}
else if(((count_green/count_yellow)<0.50)&&((count_green/count_yellow)>0.40)){
sd.setExampleData(-5);
cout<<"5"<<endl;
}
else if(((count_green/count_yellow)<0.40)&&((count_green/count_yellow)>0.30)){
sd.setExampleData(-0.5);
cout<<"6"<<endl;
}
else if(((count_green/count_yellow)<0.30)&&((count_green/count_yellow)>0.1)){
sd.setExampleData(-0.6);
cout<<"7"<<endl;
}*/


// if((count_green/count_yellow)<0.4 &&(count_green/count_yellow)>0.2){
//sd.setExampleData(-10);
//cout<<"4"<<endl;
//}
 //if((count_green/count_yellow)<0.2 &&(count_green/count_yellow)>0.0){
//sd.setExampleData(-15);
//cout<<"5"<<endl;
//}
//}

        // Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.
      //  SteeringData sd;
      // sd.setExampleData(1234.56);

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

