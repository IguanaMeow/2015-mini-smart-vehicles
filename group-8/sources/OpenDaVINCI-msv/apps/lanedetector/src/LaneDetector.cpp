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
		rightLine1(0, 0, 0),
		rightLine2(0, 0, 0),
        rightLine3(0, 0, 0),
        rightLine4(0, 0, 0),
        rightLine5(0, 0, 0),
        rightLine6(0, 0, 0),

		leftLine1(0, 0, 0),
		leftLine2(0, 0, 0),
		leftLine3(0, 0, 0),
		leftLine4(0, 0, 0),
        leftLine5(0, 0, 0),
        leftLine6(0, 0, 0),

		upline1(0, 0, 130),
		upline2(0, 0, 130),
		state(1),
		counter(0),
		critCounter(0),
        critAngleCounter(0),
		yCount(0),



		imgWidth(0),
		imgHeight(0),

		SPEED(2),

		tempAngle(0.0),

		critAngleRight(0.0),
		critAngleLeft(0.0),
    rightError(0.0),
    leftError(0.0)
{
	rightList.push_back(rightLine1);
	rightList.push_back(rightLine2);
    rightList.push_back(rightLine3);
    rightList.push_back(rightLine4);
    rightList.push_back(rightLine5);
    rightList.push_back(rightLine6);

	leftList.push_back(leftLine1);
	leftList.push_back(leftLine2);
	leftList.push_back(leftLine3);
	leftList.push_back(leftLine4);
    leftList.push_back(leftLine5);
    leftList.push_back(leftLine6);
}

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
	
//	cout << "crit distance: " << rightLine1.getCritical() << "critAngle: " << critAngleRight << endl;
//	cout << "x1: " << rightLine1.getXPos() << " x2: " << rightLine2.getXPos() << " y1: " << rightLine1.getYPos() << " y2: " << rightLine2.getYPos() << endl;


	validLines(&leftList,0);
    validLines(&rightList,1);
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

	
	rightError = rightLine1.getCritical() - rightLine1.getXPos();
  leftError = validLeft.begin()->getXPos()-validLeft.begin()->getCritical();
  // cout<<"left validLeft begin xpos is" <<validLeft.begin()->getXPos()<<"  its critical is " << validLeft.begin()->getCritical()<<"    LEFT ERROR IS           "<<leftError<<endl; 
  // cout<<"left validLeft begin xpos is" <<validLeft[1].getXPos()<<"  its critical is " << validLeft[1].getCritical()<<"    LEFT ERROR 2 IS           "<<validLeft[1].getXPos()-validLeft[1].getCritical()<<endl; 

	
	switch (state) {
	case 1: // Lanedetection state

		sd.setSpeedData(SPEED);

        
        if (isEmpty(&validLeft) && isEmpty(&validRight)){
            state = 2;
        }else {
            //following both lanes
            sd.setHeadingData(measureAngle(m_image));

        }
        break;
		
	case 2:

		sd.setSpeedData(0);
		counter++;
		if (counter > 50) {
			counter = 0;
			state = 3;
		}
		 cout << counter << endl;
		break;
	case 3:

		sd.setSpeedData(SPEED);
		if (upline1.getYPos()>upline1.getCritical()){
			state = 1;
		}
		break;
	} //switch end


	// Shows the image.
	if (m_debug) {
		if (m_image != NULL) {
			cvShowImage("LaneDetector", m_image);
			cvWaitKey(10);
		}
	}


	// Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.

	//sd.setExampleData(1234.56);

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
        rightLine1.setYPos(round(imgHeight * 0.104));
        rightLine2.setYPos(round(imgHeight * 0.146));
        rightLine3.setYPos(round(imgHeight * 0.187));
        rightLine4.setYPos(round(imgHeight * 0.24));
        rightLine5.setYPos(round(imgHeight * 0.292));
        rightLine6.setYPos(round(imgHeight * 0.344));

        leftLine1.setYPos(round(imgHeight * 0.104));
        leftLine2.setYPos(round(imgHeight * 0.146));
        leftLine3.setYPos(round(imgHeight * 0.187));
        leftLine4.setYPos(round(imgHeight * 0.24));
        leftLine5.setYPos(round(imgHeight * 0.292));
        leftLine6.setYPos(round(imgHeight * 0.344));

        rightList[0].setYPos(rightLine1.getYPos());
        rightList[1].setYPos(rightLine2.getYPos());
        rightList[2].setYPos(rightLine3.getYPos());
        rightList[3].setYPos(rightLine4.getYPos());
        rightList[4].setYPos(rightLine5.getYPos());
        rightList[5].setYPos(rightLine6.getYPos());

        leftList[0].setYPos(leftLine1.getYPos());
        leftList[1].setYPos(leftLine2.getYPos());
        leftList[2].setYPos(leftLine3.getYPos());
        leftList[3].setYPos(leftLine4.getYPos());
        leftList[4].setYPos(leftLine5.getYPos());
        leftList[5].setYPos(leftLine6.getYPos());

        upline1.setXPos((imgWidth / 2) - (imgWidth * 0.04));
        upline2.setXPos((imgWidth / 2) + (imgWidth * 0.04));
        upline1.setCritical(imgHeight * 0.3);
        upline2.setCritical(imgHeight * 0.3);

        yCount = 1;
    }

    upline1.setYPos(measureDistance(upline1.getXPos(), 2, m_image));
    upline2.setYPos(measureDistance(upline2.getXPos(), 2, m_image));

    rightLine1.setXPos(measureDistance(rightLine1.getYPos(), 1, m_image));
    rightLine2.setXPos(measureDistance(rightLine2.getYPos(), 1, m_image));
    rightLine3.setXPos(measureDistance(rightLine3.getYPos(), 1, m_image));
    rightLine4.setXPos(measureDistance(rightLine4.getYPos(), 1, m_image));
    rightLine5.setXPos(measureDistance(rightLine5.getYPos(), 1, m_image));
    rightLine6.setXPos(measureDistance(rightLine6.getYPos(), 1, m_image));

    leftLine1.setXPos(measureDistance(leftLine1.getYPos(), 0, m_image));
    leftLine2.setXPos(measureDistance(leftLine2.getYPos(), 0, m_image));
    leftLine3.setXPos(measureDistance(leftLine3.getYPos(), 0, m_image));
    leftLine4.setXPos(measureDistance(leftLine4.getYPos(), 0, m_image));
    leftLine5.setXPos(measureDistance(leftLine5.getYPos(), 0, m_image));
    leftLine6.setXPos(measureDistance(leftLine6.getYPos(), 0, m_image));

    if (critCounter < 12) {
        for(vector<Lines>::iterator it = rightList.begin(); it != rightList.end(); it++) {
            if (it->getCritical() < 1){
                calculateCritical(it, 1, m_image);
            }
        }
        for(vector<Lines>::iterator it = leftList.begin(); it != leftList.end(); it++) {
            if (it->getCritical() < 1){
                calculateCritical(it, 0, m_image);
            }
        }
        rightLine1.setCritical(rightList[0].getCritical());
        rightLine2.setCritical(rightList[1].getCritical());
        rightLine3.setCritical(rightList[2].getCritical());
        rightLine4.setCritical(rightList[3].getCritical());
        rightLine5.setCritical(rightList[4].getCritical());
        rightLine6.setCritical(rightList[5].getCritical());

        leftLine1.setCritical(leftList[0].getCritical());
        leftLine2.setCritical(leftList[1].getCritical());
        leftLine3.setCritical(leftList[2].getCritical());
        leftLine4.setCritical(leftList[3].getCritical());
        leftLine5.setCritical(leftList[4].getCritical());
        leftLine6.setCritical(leftList[5].getCritical());

    }

        leftList[0].setXPos(leftLine1.getXPos());
        leftList[1].setXPos(leftLine2.getXPos());
        leftList[2].setXPos(leftLine3.getXPos());
        leftList[3].setXPos(leftLine4.getXPos());
        leftList[4].setXPos(leftLine5.getXPos());
        leftList[5].setXPos(leftLine6.getXPos());

        rightList[0].setXPos(rightLine1.getXPos());
        rightList[1].setXPos(rightLine2.getXPos());
        rightList[2].setXPos(rightLine3.getXPos());
        rightList[3].setXPos(rightLine4.getXPos());
        rightList[4].setXPos(rightLine5.getXPos());
        rightList[5].setXPos(rightLine6.getXPos());

       // cout << "rightList x " << rightList[0].getXPos() << "leftList x " << leftList[0].getXPos() << endl;
}

bool LaneDetector::isEmpty(std::vector<Lines>* lines){
    for(int i=0; i<6; ++i){
            if (lines->at(i).getYPos()>0)
                return false;
        }
        return true;
}



void LaneDetector::validLines(std::vector<Lines>* lines, int LorR)
{
    if (LorR==0){
        validLeft.clear();
    }else{
        validRight.clear();
    }
	

	// Iterates through the vector of lines and stops when two validLeft lines have been found.
	for(std::vector<Lines>::iterator it = lines->begin(); it != lines->end(); ++it )
	{
		if (LorR==0){
        // As long as the lines X-position isn't extremely out of bounds...
		if(it->getXPos() > 1)
		{
			// ...add it to the vector.
			validLeft.push_back(*it);

		}else{
            validLeft.push_back(Lines(-1,-1,-1));
        }
       // cout << "the left list is " << validLeft[0] -> getXPos() << "," << it -> getYPos()<<endl;
    }else{
        if(it->getXPos() < (imgWidth-2))
        {
            // ...add it to the vector.
            validRight.push_back(*it);
        }else{
            validRight.push_back(Lines(-1,-1,-1));
        }

    }

	}

}

void LaneDetector::calculateCritical(const vector<Lines>::iterator& line, int dir, IplImage* image) {
	double result;
	result = measureDistance(line->getYPos(), dir, image);
	
	if (result < (imgWidth-2)) {
		line->setCritical(result);
		critCounter++;
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

    if(!isEmpty(&validLeft) && !isEmpty(&validRight)){
        tempListLeft.clear();
        tempListRight.clear();
        for(int i=0; i<6; ++i){
            if (validLeft[i].getXPos()>0 && validRight[i].getXPos()>0){
                tempListLeft.push_back(validLeft[i]);
                tempListRight.push_back(validRight[i]); 

            }
        }
        int length = tempListLeft.size();
         //draw lines
    for(int j=0; j<2; ++j){
        
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
    if(length != 0 ){
    	if(length >1){
    		length = 2;
    	}
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

    }else{
    	angle = tempAngle;
    //    cout << "THE TEMP ANGLE WILL BE ----------->>>>>>>>>>    " << angle << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;

    }
    	


}
    
    
    return angle;
	
	}



double LaneDetector::measureDistance(int yPos, int dir, IplImage* image) {

	int i = 0, distance = 0;
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
        distance = x/2;

		for(i = x/2; i<x; ++i){
			int r = data[(y-yPos)*step + i*channel + 0];
			int g = data[(y-yPos)*step + i*channel + 1];
			int b = data[(y-yPos)*step + i*channel + 2];

			if (r == 255 && g == 255 && b == 255){
				ptRight.x = i;
				break;
			}
			++distance;
		}
		line(newImage, ptMiddle, ptRight, cvScalar(51,255,102), 3, 8);
	}
	// Scans for full-white line to the left
	else if (dir==0){
        distance = x/2;

		for(i = x/2; i>0; --i){
			int r = data[(y-yPos)*step + i*channel + 0];
			int g = data[(y-yPos)*step + i*channel + 1];
			int b = data[(y-yPos)*step + i*channel + 2];

			if (r == 255 && g == 255 && b == 255){
				ptLeft.x = i;
				break;
			}
			--distance;

		}

		line(newImage, ptMiddle, ptLeft, cvScalar(204,51,255), 3, 8);
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
			++distance;
		}
		ptDown.y -= rightLine1.getYPos();
		line(newImage, ptDown, ptUp, cvScalar(0,184,245), 1, 8);
	}

	return i;
}

} // msv
