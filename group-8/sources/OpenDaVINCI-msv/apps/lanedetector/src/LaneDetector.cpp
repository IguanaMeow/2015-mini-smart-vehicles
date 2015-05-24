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
    SIZE(9),

    tempAngle(0.0),

    critAngleRight(0.0),
    critAngleLeft(0.0),
    vanY(0),
    rightError(0.0),
    leftError(0.0),

    leftLength(0),
    rightLength(0),

    rightList(SIZE, Lines(0.0, 0.0, 0.0)),
    leftList(SIZE, Lines(0.0, 0.0, 0.0))

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
                    cvCanny( merge_image, merge_image, 110, 55, 3 );
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
        /*---Critical angle is determined from vlid, drawn lines to establish the level of skew from the camera angle ---*/
        if (critAngleCounter < 2 ) {
          /*---the left critical angle is not currently used (only the right) but we left it here 
              because we'd like to use it (call it experimental) to average out with the right critical angle---*/ 
            if (validLeft.begin()->getYPos() < validLeft[1].getYPos()) {
                critAngleLeft = (atan2(validLeft[1].getYPos() - validLeft.begin()->getYPos(), 
                    validLeft[1].getXPos()- validLeft.begin()->getXPos() ) * Constants::RAD2DEG);
                critAngleCounter += 1;
            } else {
                critAngleLeft = (atan2(validLeft.begin()->getYPos() - validLeft[1].getYPos(),
                    validLeft.begin()->getXPos()- validLeft[1].getXPos()) * Constants::RAD2DEG);
                critAngleCounter += 1;
            }
            if (validRight.begin()->getYPos() < validRight[1].getYPos()) {
                critAngleRight = (atan2(validRight[1].getYPos() - validRight.begin()->getYPos(), 
                    validRight.begin()->getXPos()-validRight[1].getXPos()) * Constants::RAD2DEG);
                //vanY is vanishing point
                vanY = (validRight.begin()->getYPos() + (tan(critAngleRight * Constants::DEG2RAD) 
                    * (validRight.begin()->getXPos() - imgWidth/2)));
                critAngleCounter += 1;
            } else {
                critAngleRight = (atan2(validRight.begin()->getYPos() - validRight[1].getYPos(),
                    validRight[1].getXPos()-validRight.begin()->getXPos()) * Constants::RAD2DEG);
                vanY = (validRight.begin()->getYPos() + (tan(critAngleRight * Constants::DEG2RAD) 
                    * (validRight.begin()->getXPos() - imgWidth/2)));
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

        cout << "Create Containers" << endl;
        // Create container for finally sending the data.
        Container c(Container::USER_DATA_1, sd);
        Container c2(Container::USER_DATA_3, ld);
        cout << "Send Containers" << endl;
        // Send container.
        getConference().send(c);
        getConference().send(c2);
        cout << "Processed image" << endl;
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
            cout << "has_next_frame = "<< has_next_frame << endl;
            // Process the read image.
            if (true == has_next_frame) {
                processImage();
                lanecounter++;
            }
            cout << "lanecounter =" << lanecounter << endl;
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
        double distance = 0.04;
        const bool headless = getKeyValueConfiguration().getValue<uint32_t>("global.headless") == 1;
        if(yCount < 1) {

            /*---If running on Odroid (though is also ugly but functional on sim) 
                use higher lines to handle acute camera tilt---*/

            if(headless){

                for(int i = 0; i < SIZE; ++i){
                    rightList[i].setYPos(round(imgHeight * distance *(i+6)));
                    leftList[i].setYPos(round(imgHeight * distance *(i+6)));
                    upline1.setCritical(imgHeight * 0.36);
                    upline2.setCritical(imgHeight * 0.36);
                }
            }else{
                for(int i = 0; i < SIZE; ++i){
                    rightList[i].setYPos(round(imgHeight * distance *(i+2)));
                    leftList[i].setYPos(round(imgHeight * distance *(i+2)));
                }
                upline1.setCritical(imgHeight * 0.2);
                upline2.setCritical(imgHeight * 0.2);
            }

            upline1.setXPos((imgWidth / 2) - (imgWidth * 0.04));
            upline2.setXPos((imgWidth / 2) + (imgWidth * 0.04));
            yCount = 1;
        }
        upline1.setYPos(measureDistance(upline1.getXPos(), 2, m_image));
        upline2.setYPos(measureDistance(upline2.getXPos(), 2, m_image));
        

        //calculate critical distance for each line once
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

        for (int i = 0; i < SIZE; ++i){
            leftList[i].setXPos(measureDistance(leftList[i].getYPos(), 0, m_image));
            rightList[i].setXPos(measureDistance(rightList[i].getYPos(), 1, m_image));
        }
    }

    //check if the gien vector is empty. Empty means (-1, -1, -1)
    bool LaneDetector::isEmpty(std::vector<Lines>& lines){
        for(int i=0; i<SIZE; ++i){
            if (lines.at(i).getYPos()>0)
                return false;
        }
        return true;
    }

    void LaneDetector::validLines(std::vector<Lines>& lines, int LorR){
        if (LorR==0){
            validLeft.clear();
            leftLength = 0;
        }else{
            validRight.clear();
            rightLength = 0;
        }

        int crop = (imgWidth-(imgHeight *1.333 ))/2;
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
                	//when its invalid, put place holder (-1, -1, -1) to that position
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

        /*---for extreme cases of tight turns set opposing max angle to 
        bring car back towards centre---*/

        if(rightList[1].getXPos() < (imgWidth * 0.62)){
            angle = -26* Constants::DEG2RAD;
            tempAngle = angle;
        }else if (rightList[1].getXPos() > (imgWidth * 0.88)){
            angle = 25* Constants::DEG2RAD;
            tempAngle = angle;
        }

        /*---Calculate the midpoint of valid right and left lines, translate around 
        (nearly) the middle of the image using a slightly offset vanishing point and 
        measure angle from close to the wheels to the average of the nearest two midpoints---*/

        else if(!isEmpty(validLeft) && !isEmpty(validRight)){
            tempListLeft.clear();
            tempListRight.clear();
            for(int i=0; i<SIZE; ++i){
            	//when the ith element in both vectors are valid, push them back to temporary lists
                if (validLeft[i].getXPos()>0 && validRight[i].getXPos()>0){
                    tempListLeft.push_back(validLeft[i]);
                    tempListRight.push_back(validRight[i]); 
                }
            }
            int length = tempListLeft.size();
            //when there are more than two valid lines, only keep the first two
            if (length >0){
                if(length >1){
                    length = 2;
                }
                //draw lines
                for(int j=0; j<length; ++j){
                    ptBegin.x = x/2;
                    ptBegin.y= y;
                    ptEnd.x =((tempListRight[j].getXPos()-tempListLeft[j].getXPos())/2)
                    +tempListLeft[j].getXPos();
                    ptEnd.y =y-tempListLeft[j].getYPos();
                    line(newImage, ptBegin, ptEnd, cvScalar(255, 100, 255), 2, 8);
                }

                double sum = 0.0;
                double array[length];

                for (int i = 0; i < length; ++i){
                	//difference between the midponit of the image and the mid of the road

                    int alphaX =(((tempListRight[i].getXPos()-tempListLeft[i].getXPos())/2)
                        +tempListLeft[i].getXPos() + 5)-x/2;        
                    int alphaY = tempListLeft[i].getYPos();
                    if(alphaX != 0){
                        alphaX = translatePoint(((int)tempListLeft[i].getYPos()), alphaX);
                    }
                    ptEnd.x = alphaX+x/2;
                    line(newImage, ptBegin, ptEnd, cvScalar(255, 100, 0), 2, 8);
                    if(alphaX == 0 ){
                        array[i] = 0.0;
                    }else{
                        array[i]= atan2(alphaX,alphaY-10);
                    }
                    sum+=array[i];
                }
                angle = (sum/length);
                //due to hardware restriction, angles have to be between -26 and 25
                if (angle* Constants::RAD2DEG > 25)
                    angle = 25* Constants::DEG2RAD;
                else if (angle* Constants::RAD2DEG < -26)
                    angle = -26* Constants::DEG2RAD;

                cout << "THE ANGLE WILL BE ----------->>>>>>>>>>    " << angle 
                << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;
                tempAngle = angle;
            }else {
                angle = tempAngle;
            }
        }else{
            /*---follow tempangle in case of no data---*/
            angle = tempAngle;
            cout << "THE TEMP ANGLE WILL BE ----------->>>>>>>>>>    " << angle 
            << " <-radius   degree->"<< angle* Constants::RAD2DEG <<endl;
        }
        return angle;
    }

    /*---Translate a given point around (nearly) the middle of the image using a slightly offset 
            vanishing point---*/

    int LaneDetector::translatePoint(int yPos, int xPos){
        //xPos must be relative to middle!
        int newXpos;
        //angle from vanishing point with offset
        int critAngle = atan2(vanY+150 - yPos, xPos)* Constants::RAD2DEG;
        //translate point around the rotation axis *0.04*14
        if(yPos>(imgHeight*0.04*14)){
            newXpos = xPos + (tan((90 - critAngle)* Constants::DEG2RAD) * yPos);
        }else{
            newXpos = xPos - (tan((90 - critAngle)* Constants::DEG2RAD) * yPos);
        }
        return newXpos;
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
