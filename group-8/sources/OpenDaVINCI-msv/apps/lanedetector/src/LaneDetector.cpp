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
#include <vector>
#include <opencv/cv.h>
#include <opencv/highgui.h>
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
#include "Lines.h"




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
	merge_image(NULL),
	m_debug(false),

	upline1(0.0, 0.0, 0.0),
	upline2(0.0, 0.0, 0.0),
	state(1),
	counter(0),
	critCounter(0),
  	critAngleCounter(0),
	yCount(0),

	imgWidth(0),
	imgHeight(0),

	SPEED(2),
	SIZE(10),

	tempAngle(0.0),

	critAngleRight(0.0),
	critAngleLeft(0.0),
	rightError(0.0),
	leftError(0.0),

	leftLength(0),
	rightLength(0),

	rightList(10, Lines(0.0, 0.0, 0.0)),
	leftList(10, Lines(0.0, 0.0, 0.0))

{}

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
		const uint32_t numberOfChannels = si.getBytesPerPixel();
		//Single channel version of image copy
		if (m_sharedImageMemory->isValid()){
			if(numberOfChannels == 1) {
				// Lock the memory region to gain exclusive access. REMEMBER!!! DO NOT FAIL WITHIN lock() / unlock(), otherwise, the image producing process would fail.
				m_sharedImageMemory->lock();
				{
					// For example, simply show the image.
					if (merge_image == NULL) {
						merge_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
					}
					if (m_image == NULL){
						m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, 3);
					}

					// Copying the image data is very expensive...
					if (merge_image != NULL) {
						memcpy(merge_image->imageData,
								m_sharedImageMemory->getSharedMemory(),
								si.getWidth() * si.getHeight() * numberOfChannels);
					}
				}
			}
			// Check if we could successfully attach to the shared memory.
			else {
				// Lock the memory region to gain exclusive access. REMEMBER!!! DO NOT FAIL WITHIN lock() / unlock(), otherwise, the image producing process would fail.
				m_sharedImageMemory->lock();
				{
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
			}
			// Release the memory region so that the image produce (i.e. the camera for example) can provide the next raw image data.
			m_sharedImageMemory->unlock();

			if(numberOfChannels == 1){
				cvSmooth( merge_image, merge_image, CV_GAUSSIAN, 11, 11 );
				cvCanny( merge_image, merge_image, 100, 50, 3 );
				cvDilate(merge_image, merge_image,NULL,1);
				cvMerge(merge_image, merge_image, merge_image, NULL, m_image);

			}



			// Mirror the image.
			if(numberOfChannels != 1){
				cvFlip(m_image, 0, -1);
			}

			retVal = true;
		}
	}
	imgWidth = m_image->width;
	imgHeight = m_image->height;
	return retVal;
}

void LaneDetector::processImage() {

  setLines(m_image);

	validLines(leftList,0);
  validLines(rightList,1);
    ///***********************************************************************CHAGE THIS)
            // critAngleRight = (atan2(rightLine2.getYPos() - rightLine1.getYPos(), rightLine1.getXPos() - rightLine2.getXPos()) * Constants::RAD2DEG);
  if (critAngleCounter < 2 ) {
    if (validLeft.begin()->getYPos() < validLeft[1].getYPos()) {
      critAngleLeft = (atan2(validLeft[1].getYPos() - validLeft.begin()->getYPos(), validLeft[1].getXPos()- validLeft.begin()->getXPos() ) * Constants::RAD2DEG);
    // cout << "End Y "<< validLeft[1].getYPos() << " Begin Y: " << validLeft.begin()->getYPos() << " Begin X: " << validLeft.begin()->getXPos() << " End X: " << validLeft[1].getXPos() << endl;
    // cout << "111 critical LEFT angle is"<< critAngleLeft<<endl;
    critAngleCounter += 1;

    } else {
      critAngleLeft = (atan2(validLeft.begin()->getYPos() - validLeft[1].getYPos(),validLeft.begin()->getXPos()- validLeft[1].getXPos()) * Constants::RAD2DEG);
    // cout << "Begin Y: "<< validLeft.begin()->getYPos() << " End Y: " << validLeft[1].getYPos() << " End X: " << validLeft[1].getXPos() << " Begin X: " << validLeft.begin()->getXPos() << endl;
    //     cout << "222critical LEFT angle is"<< critAngleLeft<<endl;
        critAngleCounter += 1;

    }

    if (validRight.begin()->getYPos() < validRight[1].getYPos()) {
      critAngleRight = (atan2(validRight[1].getYPos() - validRight.begin()->getYPos(), validRight.begin()->getXPos()-validRight[1].getXPos()) * Constants::RAD2DEG);
    // cout << "End Y "<< validRight[1].getYPos() << " Begin Y: " << validRight.begin()->getYPos() << " Begin X: " << validRight.begin()->getXPos() << " End X: " << validRight[1].getXPos() << endl;
    // cout << "111 critical LEFT angle is"<< critAngleRight<<endl;
    critAngleCounter += 1;

    } else {
      critAngleRight = (atan2(validRight.begin()->getYPos() - validRight[1].getYPos(),validRight[1].getXPos()-validRight.begin()->getXPos()) * Constants::RAD2DEG);
    // cout << "Begin Y: "<< validRight.begin()->getYPos() << " End Y: " << validRight[1].getYPos() << " End X: " << validRight[1].getXPos() << " Begin X: " << validRight.begin()->getXPos() << endl;
    //     cout << "222critical LEFT angle is"<< critAngleRight<<endl;
        critAngleCounter += 1;

    }

  }

	SteeringData sd;
	LaneData ld;
	ld.setRightLine1(rightLength);
	ld.setLeftLine(leftLength);
	
  if((abs(upline1.getYPos()-upline2.getYPos())<10)&& upline1.getYPos()<upline1.getCritical()){
    sd.setIntersectionLine(1);
  } else {
    sd.setIntersectionLine(0);
  }

	rightError = rightList[0].getCritical() - rightList[0].getXPos();
  leftError = validLeft.begin()->getXPos()-validLeft.begin()->getCritical();

  sd.setHeadingData(measureAngle(m_image));


	// Shows the image.
	if (m_debug) {
		if (m_image != NULL) {
			cvShowImage("LaneDetector", m_image);
			cvWaitKey(10);
		}
	}


	// Create container for finally sending the data.
	Container c(Container::USER_DATA_1, sd);
	Container c2(Container::USER_DATA_3, ld);

	// Send container.
	getConference().send(c);
	getConference().send(c2);
}


// This method will do the main data processing job.
// Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
ModuleState::MODULE_EXITCODE LaneDetector::body() {
	// Get configuration data.
	KeyValueConfiguration kv = getKeyValueConfiguration();
	m_debug = kv.getValue<int32_t> ("lanedetector.debug") == 1;

	Player *player = NULL;

	uint32_t lanecounter = 0;
	time_t startTime = time(0);
	double cumduration;
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
			lanecounter++;
		}
	}
	cout << "LaneDetector: processed " << lanecounter << " frames." << endl;
	time_t endTime = time(0);
	cumduration = difftime(endTime, startTime);
	cout << "LaneDetector: processed " << lanecounter/cumduration << " frames per sec." << endl;

	OPENDAVINCI_CORE_DELETE_POINTER(player);

	return ModuleState::OKAY;
}
void  LaneDetector::setLines(IplImage* image)
{
    imgWidth = image->width;
    imgHeight = image->height;
    if(yCount < 1) {

	    rightList[0].setYPos(round(imgHeight * 0.104));
	    rightList[1].setYPos(round(imgHeight * 0.146));
	    rightList[2].setYPos(round(imgHeight * 0.187));
	    rightList[3].setYPos(round(imgHeight * 0.24));
	    rightList[4].setYPos(round(imgHeight * 0.292));
	    rightList[5].setYPos(round(imgHeight * 0.36));
	    rightList[6].setYPos(round(imgHeight * 0.41));
	    rightList[7].setYPos(round(imgHeight * 0.46));
	    rightList[8].setYPos(round(imgHeight * 0.51));
	    rightList[9].setYPos(round(imgHeight * 0.56));


	    leftList[0].setYPos(round(imgHeight * 0.104));
	    leftList[1].setYPos(round(imgHeight * 0.146));
	    leftList[2].setYPos(round(imgHeight * 0.187));
	    leftList[3].setYPos(round(imgHeight * 0.24));
	    leftList[4].setYPos(round(imgHeight * 0.292));
	    leftList[5].setYPos(round(imgHeight * 0.36));
	    leftList[6].setYPos(round(imgHeight * 0.41));
	    leftList[7].setYPos(round(imgHeight * 0.46));
	    leftList[8].setYPos(round(imgHeight * 0.51));
	    leftList[9].setYPos(round(imgHeight * 0.56));


	    upline1.setXPos((imgWidth / 2) - (imgWidth * 0.04));
	    upline2.setXPos((imgWidth / 2) + (imgWidth * 0.04));
	    upline1.setCritical(imgHeight * 0.2);
	    upline2.setCritical(imgHeight * 0.2);

	    yCount = 1;
    }

    upline1.setYPos(measureDistance(upline1.getXPos(), 2, m_image));
    upline2.setYPos(measureDistance(upline2.getXPos(), 2, m_image));

    if (critCounter < 20) {
    	for(int i = 0; i < SIZE; ++i) {
            if (leftList[i].getCritical() < 1){
                calculateCritical(leftList[i], 0, m_image);
            }
        }
        for(int i = 0; i < SIZE; ++i) {
            if (rightList[i].getCritical() < 1){
                calculateCritical(rightList[i], 1, m_image);

            }

        }

    }

    for (int i = 0; i < SIZE; ++i)
    {
    	leftList[i].setXPos(measureDistance(leftList[i].getYPos(), 0, m_image));
    	rightList[i].setXPos(measureDistance(rightList[i].getYPos(), 1, m_image));
    }
}

bool LaneDetector::isEmpty(std::vector<Lines>& lines){
    for(int i=0; i<SIZE; ++i){
            if (lines.at(i).getYPos()>0)
                return false;
        }
        return true;
}

bool LaneDetector::isObject() 
{
  SensorBoardData sbd;
  KeyValueConfiguration kv = getKeyValueConfiguration();
  double carLength = kv.getValue<double> ("global.carLength");

  if (sbd.getValueForKey_MapOfDistances(3) > 0 && sbd.getValueForKey_MapOfDistances(3) < carLength * 3) {
    return true;
  } else if (sbd.getValueForKey_MapOfDistances(4) > 0 && sbd.getValueForKey_MapOfDistances(4) < carLength * 3){
    return true;
  } else {
    return false;
  }
}

void LaneDetector::validLines(std::vector<Lines>& lines, int LorR)
{

    if (LorR==0){
        validLeft.clear();
        leftLength = 0;
    }else{
        validRight.clear();
		rightLength = 0;
    }
	
	int crop = (imgWidth-(imgHeight *1.333 ))/2;
	//cout << crop << endl;
	// Iterates through the vector of lines and stops when two validLeft lines have been found.
	for(int i = 0; i < SIZE; ++i)
	{
		if (LorR==0){
        // As long as the lines X-position isn't extremely out of bounds...
		if(lines[i].getXPos() > crop+1 && lines[i].getXPos() < (imgWidth/2-4))
		{
			// ...add it to the vector.
			validLeft.push_back(lines[i]);
			++leftLength;

		}else{
            validLeft.push_back(Lines(-1,-1,-1));
        }

    }else{
        if(lines[i].getXPos() < imgWidth-1 && lines[i].getXPos() > (imgWidth/2+4))
        {
            // ...add it to the vector.
            validRight.push_back(lines[i]);
			++rightLength;

        }else{
            validRight.push_back(Lines(-1,-1,-1));
        }

    }

	}

}

void LaneDetector::calculateCritical(Lines& line, int dir, IplImage* image) {
	double result;
	result = measureDistance(line.getYPos(), dir, image);
	
	if(dir == 0){
		if (result > 1) {
		line.setCritical(result);
		++critCounter;
		}
	}
	else{
		if (result < (imgWidth-1)) {
		line.setCritical(result);
		++critCounter;
	}
	}

}

double LaneDetector::measureAngle(IplImage *image) {
    double angle = 0.00;
    cv::Mat newImage = cv::cvarrToMat(image);
    vector<Lines> tempListLeft;
    vector<Lines> tempListRight;
    cv::Point ptBegin;
    cv::Point ptEnd;
    int y = image->height;
    int x = image->width;

    if(!isEmpty(validLeft) && !isEmpty(validRight)){
        tempListLeft.clear();
        tempListRight.clear();
        for(int i=0; i<SIZE; ++i){
            if (validLeft[i].getXPos()>0 && validRight[i].getXPos()>0){
                tempListLeft.push_back(validLeft[i]);
                tempListRight.push_back(validRight[i]); 

            }
        }
        int length = tempListLeft.size();
        if (length >0){


        if(length >1){
    		length = 2;
    	}
         //draw lines
    for(int j=0; j<length; ++j){
        
        ptBegin.x = x/2;
      //  ptBegin.y = y-tempListLeft[0].getYPos();
        ptBegin.y= y;
        ptEnd.x =((tempListRight[j].getXPos()-tempListLeft[j].getXPos())/2)+tempListLeft[j].getXPos();
        ptEnd.y =y-tempListLeft[j].getYPos();
        //cout << "THE baseline WILL BE ----------->>>>>>>>>>    " << tempListLeft[0].getXPos()<< "  ,  " <<tempListLeft[0].getYPos() <<endl;
        //cout << "Right X temp IS ----------->>>>>>>>>>    " << tempListRight[j].getXPos() <<endl;
        //cout << "Left X IS ----------->>>>>>>>>>    " << tempListLeft[j].getXPos() <<endl;
        //cout << "Left Y IS ----------->>>>>>>>>>    " << tempListLeft[j].getYPos() <<endl;
        line(newImage, ptBegin, ptEnd, cvScalar(255, 100, 255), 2, 8);

     }

    double sum = 0.0;
    double array[length];
    
    	
   		for (int i = 0; i < length; ++i){
        //int alphaX =(((tempListRight[i].getXPos()-tempListLeft[i].getXPos())/2)+tempListLeft[i].getXPos())-(((tempListRight[0].getXPos()-tempListLeft[0].getXPos())/2)+tempListLeft[0].getXPos());
        int alphaX =(((tempListRight[i].getXPos()-tempListLeft[i].getXPos())/2)+tempListLeft[i].getXPos())-x/2;
        
        int alphaY = tempListLeft[i].getYPos()-y;

        // cout << "THE DELTA X  ----------->>>>>>>>>>    " << alphaX <<endl;
        // cout << "THE DELTA Y  ----------->>>>>>>>>>    " << -alphaY <<endl;
        if(alphaX == 0 ){
            array[i] = 0.0;
        }else{
            array[i]= atan2(alphaX,-alphaY);
        }
        sum+=array[i];
    }
    angle = (sum/length);
    if (angle* Constants::RAD2DEG > 25)
        angle = 25* Constants::DEG2RAD;
    else if (angle* Constants::RAD2DEG < -26)
        angle = -26* Constants::DEG2RAD;
    
    //cout << "THE ANGLE WILL BE ----------->>>>>>>>>>    " << angle << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;
    tempAngle = angle;

    
    	


	}else {
		angle = tempAngle;
	}

}else if (isEmpty(validLeft)&& !isEmpty(validRight)){

		//follow right lane
		tempListLeft.clear();
        tempListRight.clear();
        for(int i=0; i<SIZE; ++i){
            if (validRight[i].getXPos()>0 ){
                tempListRight.push_back(validRight[i]);
            }
        }
        int length = tempListRight.size();

		if(length >1){
    		length = 2;
    	}	

    	for(int j=0; j<length; ++j){
        ptBegin.x = x/2;
      //  ptBegin.y = y-tempListLeft[0].getYPos();
        ptBegin.y= y;
        ptEnd.x =tempListRight[j].getXPos() - tempListRight[j].getCritical()+x/2;
        ptEnd.y =y-tempListRight[j].getYPos();
        //cout << "THE baseline WILL BE ----------->>>>>>>>>>    " << tempListLeft[0].getXPos()<< "  ,  " <<tempListLeft[0].getYPos() <<endl;
        //cout << "Right X temp IS ----------->>>>>>>>>>    " << tempListRight[j].getXPos() <<endl;
        //cout << "Left X IS ----------->>>>>>>>>>    " << tempListLeft[j].getXPos() <<endl;
        // cout << "Left X IS ----------->>>>>>>>>>    " << tempListRight[j].getXPos() << "it's critical is " << tempListRight[j].getCritical()<<endl;
        // cout << "Left Y IS ----------->>>>>>>>>>    " << tempListRight[j].getYPos() <<endl;

        line(newImage, ptBegin, ptEnd, cvScalar(255, 100, 255), 2, 8);

    }
    double sum = 0.0;
    double array[length];
    
    	
   		for (int i = 0; i < length; ++i){
        //int alphaX =(((tempListRight[i].getXPos()-tempListLeft[i].getXPos())/2)+tempListLeft[i].getXPos())-(((tempListRight[0].getXPos()-tempListLeft[0].getXPos())/2)+tempListLeft[0].getXPos());
        int alphaX = tempListRight[i].getXPos() - tempListRight[i].getCritical();
        
        int alphaY = tempListRight[i].getYPos()-y;

         cout << "THE DELTA X  ----------->>>>>>>>>>    " << alphaX <<endl;
         cout << "THE DELTA Y  ----------->>>>>>>>>>    " << -alphaY <<endl;
        if(alphaX == 0 ){
            array[i] = 0.0;
        }else{
            array[i]= atan2(alphaX,-alphaY);
        }
        sum+=array[i];
    }
    angle = (sum/length);
    if (angle* Constants::RAD2DEG > 25)
        angle = 25* Constants::DEG2RAD;
    else if (angle* Constants::RAD2DEG < -26)
        angle = -26* Constants::DEG2RAD;
    
    cout << "THE ANGLE WILL BE ----------->>>>>>>>>>    " << angle << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;
    tempAngle = angle;
	}else if (!isEmpty(validLeft)&& isEmpty(validRight)){
		
		//follow left lane
		tempListLeft.clear();
        tempListRight.clear();
        for(int i=0; i<SIZE; ++i){
            if (validLeft[i].getXPos()>0 ){
                tempListLeft.push_back(validLeft[i]);
            }
        }
        int length = tempListLeft.size();
        if(length >1){
    		length = 2;
    	}
	for(int j=0; j<length; ++j){
        ptBegin.x = x/2;
      //  ptBegin.y = y-tempListLeft[0].getYPos();
        ptBegin.y= y;
        ptEnd.x =tempListLeft[j].getXPos()-tempListLeft[j].getCritical()+x/2;
        ptEnd.y =y-tempListLeft[j].getYPos();
        //cout << "THE baseline WILL BE ----------->>>>>>>>>>    " << tempListLeft[0].getXPos()<< "  ,  " <<tempListLeft[0].getYPos() <<endl;
        //cout << "Right X temp IS ----------->>>>>>>>>>    " << tempListRight[j].getXPos() <<endl;
       // cout << "Left X IS ----------->>>>>>>>>>    " << tempListLeft[j].getXPos() << "it's critical is " << tempListLeft[j].getCritical()<<endl;
       // cout << "Left Y IS ----------->>>>>>>>>>    " << tempListLeft[j].getYPos() <<endl;
        line(newImage, ptBegin, ptEnd, cvScalar(255, 100, 255), 2, 8);

    }
    double sum = 0.0;
    double array[length];
    
    	
   		for (int i = 0; i < length; ++i){
        //int alphaX =(((tempListRight[i].getXPos()-tempListLeft[i].getXPos())/2)+tempListLeft[i].getXPos())-(((tempListRight[0].getXPos()-tempListLeft[0].getXPos())/2)+tempListLeft[0].getXPos());
        int alphaX =tempListLeft[i].getXPos()-tempListLeft[i].getCritical();
        
        int alphaY = tempListLeft[i].getYPos()-y;

          cout << "THE DELTA X  ----------->>>>>>>>>>    " << alphaX <<endl;
          cout << "THE DELTA Y  ----------->>>>>>>>>>    " << -alphaY <<endl;
        if(alphaX == 0 ){
            array[i] = 0.0;
        }else{
            array[i]= atan2(alphaX,-alphaY);
        }
        sum+=array[i];
    }
    angle = (sum/length);
    if (angle* Constants::RAD2DEG > 25)
        angle = 25* Constants::DEG2RAD;
    else if (angle* Constants::RAD2DEG < -26)
        angle = -26* Constants::DEG2RAD;
    
    cout << "THE ANGLE WILL BE ----------->>>>>>>>>>    " << angle << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;
    tempAngle = angle;

    
    	


	}
   else{
   		//follow tempangle
   	angle = tempAngle;
    cout << "THE TEMP ANGLE WILL BE ----------->>>>>>>>>>    " << angle << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;

   }
    
    
    return angle;
	
	}



double LaneDetector::measureDistance(int yPos, int dir, IplImage* image) {

	int i = 0;
	int x = image->width;
	int y = image->height;
	int step = image->widthStep;
	int channel = image->nChannels;
	//pointer to aligned data
	uchar* data = (uchar*)image->imageData;

	// OpenCV variable declarations and instantiations
	cv::Mat newImage = cv::cvarrToMat(image);
	cv::Point ptMiddle;
	cv::Point ptRight;
	cv::Point ptLeft;
	cv::Point ptDown;
	cv::Point ptUp;
	ptMiddle.x = x/2;
	ptMiddle.y = y-yPos;
	ptRight.y = y-yPos;
	ptLeft.y = y-yPos;
	// The argument "yPos" is the xPos for ptDown
	ptDown.x = yPos;
	ptDown.y = y;
	ptUp.x = yPos;

	// Scans for full-white line to the right
	if (dir == 1){

		for(i = x/2; i<x-1; ++i){
			int r = data[(y-yPos)*step + i*channel + 0];
			int g = data[(y-yPos)*step + i*channel + 1];
			int b = data[(y-yPos)*step + i*channel + 2];

			if (r == 255 && g == 255 && b == 255){
				ptRight.x = i;
				break;
			}
		}
		if(ptRight.x > x/2){
			line(newImage, ptMiddle, ptRight, cvScalar(51,255,102), 3, 8);
		}else{
			ptRight.x = x-1;
			line(newImage, ptMiddle, ptRight, cvScalar(51,255,102), 3, 8);
		}
	}
	// Scans for full-white line to the left
	else if (dir==0){

		for(i = x/2; i>1; --i){
			int r = data[(y-yPos)*step + i*channel + 0];
			int g = data[(y-yPos)*step + i*channel + 1];
			int b = data[(y-yPos)*step + i*channel + 2];

			if (r == 255 && g == 255 && b == 255){
				ptLeft.x = i;
				break;
			}

		}
		//cout << "ptleft.x is     " << ptLeft.x << "and i is      " <<i <<endl;
		line(newImage, ptLeft,ptMiddle, cvScalar(204,51,255), 3, 8);
	}
	// Scans for upper full-white line
	else {
		for(i = 0; i< y-1; ++i){
			int r = data[step*(y-1)+ yPos*channel + 0 -i*step];
			int g = data[step*(y-1)+ yPos*channel + 1 -i*step];
			int b = data[step*(y-1)+ yPos*channel + 2 -i*step];

			if (r == 255 && g == 255 && b == 255){
				ptUp.y = y-i;
				break;
			}
		}
		ptDown.y -= rightList[0].getYPos();
		line(newImage, ptDown, ptUp, cvScalar(0,184,245), 1, 8);
	}

	return i;
}

} // msv
