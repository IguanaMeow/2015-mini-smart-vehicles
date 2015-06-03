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
double angle;
double turnangle;
int llineh1 = 350;
int state = 0;

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




bool IfWhite(Mat img, int y, int x){
    Vec3b intensity = img.at<Vec3b>(y,x);
    int blue = intensity.val[0];
    int green = intensity.val[1];
    int red = intensity.val[2];
    if(blue == 255 && green == 255 && red == 255){
        return true;
    }
return false;
}

int MeasureVertical(Mat img, int &yAxis, int lenght, int xAxis){
if(yAxis>640)
return yAxis;

    if(IfWhite(img, yAxis, lenght)==true){

        //yAxis = lenght;
        return yAxis;
        }else {
            yAxis++;
            MeasureVertical(img, yAxis, lenght, xAxis);
        } 
return yAxis;
}

void DrawLine( Mat img, Point start, Point end, Scalar color)
{
  int thickness = 2;
  int lineType = 8;
  line( img,
        start,
        end,
        color,
        thickness,
        lineType );
}



int MeasureLeft(Mat img, int yAxis, int lenght, int &leftLineLength){
     if(lenght<1)

        return leftLineLength;

    if(IfWhite(img, yAxis, lenght)==true){
        leftLineLength = lenght;

        return leftLineLength;

        }else {
            lenght--; 
            MeasureLeft(img, yAxis, lenght, leftLineLength);
        }
        return leftLineLength;
}

int MeasureRight(Mat img, int yAxis, int lenght, int &rightLineLength){
    if(lenght>640)
       
        return rightLineLength;
    
    if(IfWhite(img, yAxis, lenght)==true){
        rightLineLength = lenght;
        return rightLineLength;
    
        }else {
            lenght++;
            MeasureRight(img, yAxis, lenght, rightLineLength);
            
        } 
return rightLineLength;
}



    // You should start your work in this method.
    void LaneDetector::processImage() {
	
			
		SteeringData sd;
		Mat src,dst,color_dst,out_image,edge,blurred;
       		src=m_image;
       		blur(src , blurred, Size(3,3) );
       		cvtColor( blurred, color_dst, COLOR_RGB2GRAY );
     		Canny( color_dst, dst, 50, 170, 3);
    		dst.convertTo(edge, CV_8U);
      		cvtColor( edge, out_image, COLOR_GRAY2RGB );
		

		
		//int variables
		int w = m_image->width;
		int h = m_image->height;
		int lenght = w/2;
		//int weight = 2;
		
		CvPoint pt4 = cvPoint (w/2, h); //bottom center
		//CvPoint pt5 = cvPoint(w/2,0); //top center
		
		
		int lineheight1 = 350;
    		int rightLineLength1 =0;
		int leftLineLength1 =0;
		
		MeasureRight(out_image, lineheight1, lenght, rightLineLength1);
		MeasureLeft(out_image, lineheight1, lenght, leftLineLength1);
		double Line1left = lenght - leftLineLength1;
		double Line1right = rightLineLength1 - lenght;

		cout << "left:"<< Line1left<<endl;
		cout << "right:"<< Line1right<<endl;

		
		cout << "width"<< w/4+15<<endl;
		

		int lineheight2 = 400;
    		int rightLineLength2 =0;
		int leftLineLength2 =0;

		MeasureRight(out_image, lineheight2, lenght, rightLineLength2);
		MeasureLeft(out_image, lineheight2, lenght, leftLineLength2);
		double Line2left = lenght - leftLineLength2;
		double Line2right = rightLineLength2 - lenght;

		cout << "left2:"<< Line2left<<endl;
		cout << "right2:"<< Line2right<<endl;

		int lineheight3 = 450;
    		int rightLineLength3 =0;
		int leftLineLength3 =0;

		MeasureRight(out_image, lineheight3, lenght, rightLineLength3);
		MeasureLeft(out_image, lineheight3, lenght, leftLineLength3);
		double Line3left = lenght - leftLineLength3;
		double Line3right = rightLineLength3 - lenght;

		cout << "left3:"<< Line3left<<endl;
		cout << "right3:"<< Line3right<<endl;

		
		int verticalLineLength = 0;
		//int xAxis = w/2;
		MeasureVertical(out_image,verticalLineLength , lenght, 0);
		cout << "Height:"<< verticalLineLength<<endl;
		
		if(Line3right<=-320 && Line3left<=320 && state==0){
        	state=1;
		sd.setState(1.0);
		}
		


	//Draw lines
        if (m_debug) {
		if (m_image != NULL){
	DrawLine( out_image, Point( lenght, lineheight1 ), Point( leftLineLength1, lineheight1),Scalar(0,0,255)); 
    	DrawLine( out_image, Point( lenght, lineheight1 ), Point( rightLineLength1, lineheight1),Scalar(255, 0, 0 )); 
	//DrawLine( out_image, pt4, pt5,Scalar( 0, 255, 0 )); //draw vertical line
	DrawLine( out_image, Point( lenght, lineheight2 ), Point( rightLineLength2, lineheight2),Scalar(255,0,0)); 
	DrawLine( out_image, Point( lenght, lineheight2 ), Point( leftLineLength2, lineheight2),Scalar(0,0,255)); 
    	DrawLine( out_image, pt4, Point( w/2, verticalLineLength),Scalar( 255, 0, 0 )); 
	DrawLine( out_image, Point( lenght, lineheight3 ), Point( rightLineLength3, lineheight3),Scalar(255,0,0)); 
	DrawLine( out_image, Point( lenght, lineheight3 ), Point( leftLineLength3, lineheight3),Scalar(0,0,255)); 
		//cvShowImage("WindowShowImage", out_image);
		cvWaitKey(10);
            }
        }
	
imshow( "out_window", out_image );
	
	
double LeftAngle = atan(Line1left/300) * Constants::RAD2DEG;
double RightAngle = atan(Line1right/300) * Constants::RAD2DEG;


  if((Line1right > w/2-20 || Line1right <0)){
        angle=turnangle;
	state =0;
	sd.setState(0.0);
        //cout<<"1"<<endl;
}


    else if( Line1left + Line1right > (w/2)+30 ) //-50
       angle = RightAngle-(LeftAngle/(Line1left/(h/3)));
    else if( Line1left + Line1right < w/2+30 )
       angle = (RightAngle*(Line1left/(h/3)))- LeftAngle; //-20 180
    else
        angle= RightAngle-LeftAngle;
  
    if(Line1right > w/2-20){
        angle=0;
	//cout<<"4"<<endl;
    }
turnangle=angle;
std::cout<<"Angledata: "<< angle <<'\n';
sd.setExampleData(angle);
//sd.setExampleData(angle*weight);


	//Save angle to data.ovd
	Container c(Container::USER_DATA_1, sd);
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
