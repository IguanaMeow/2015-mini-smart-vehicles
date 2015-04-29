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
        rightLine1(0, 50, 0),
        rightLine2(0, 70, 0),

        leftLine1(0, 90, 0),
        leftLine2(0, 115, 0),
        leftLine3(0, 140, 0),
        leftLine4(0, 165, 0),
        
        upline1(300, 0, 120),
        upline2(340, 0, 120),
        state(1),
        counter(0),
        critCounter(0),

        inputAngle1(0.0),
        inputAngle2(0.0),
        inputAngle3(0.0)
         {
          rightList.push_back(rightLine1);
          rightList.push_back(rightLine2);

          leftList.push_back(leftLine1);
          leftList.push_back(leftLine2);
          leftList.push_back(leftLine3);
          leftList.push_back(leftLine4);
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
      return retVal;
    }

    void LaneDetector::processImage() {
      
      upline1.setYPos(measureDistance(300, 2, m_image));
      upline2.setYPos(measureDistance(340, 2, m_image));

      rightLine1.setXPos(measureDistance(50, 1, m_image));
      rightLine2.setXPos(measureDistance(70, 1, m_image));
      leftLine1.setXPos(measureDistance(90, 0, m_image));
      leftLine2.setXPos(measureDistance(115, 0, m_image));
      leftLine3.setXPos(measureDistance(140, 0, m_image));
      leftLine4.setXPos(measureDistance(165, 0, m_image));

      //std::cout << "rightline 1 " << rightLine1.getXPos() << "\n rightLine2 " << rightLine2.getXPos() << std::endl;

       if (critCounter < 6) {
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

        leftLine1.setCritical(leftList[0].getCritical());
        leftLine2.setCritical(leftList[1].getCritical());
        leftLine3.setCritical(leftList[2].getCritical());
        leftLine4.setCritical(leftList[3].getCritical());
        
      }


      vector<Lines> valid = validateLines(&leftList);

      SteeringData sd;
      LaneData ld;      

      ld.setRightLine1(measureAngle(m_image->height - rightLine2.getYPos(), rightLine2.getXPos(), m_image->height - rightLine1.getYPos(), rightLine1.getXPos()));

      switch (state) {
        case 1: // Lanedetection state
          std::cout << "state 1" << std::endl;
          sd.setSpeedData(3);
          
          // Following upper right lines
          if(rightLine1.getXPos() > 270 && rightLine2.getXPos() > 270 && leftLine1.getXPos() > 270 && leftLine2.getXPos() > 270)
      {
        state = 3;
        break;
      }
      // Follow left lines
      else if (rightLine1.getXPos() > 270 && rightLine2.getXPos() > 270)
      {
     
        // Get two valid lines to base steering on
        //vector<Lines> valid = validateLines(&leftList);
        // Steer to the right
        if (valid.begin()->getXPos() < valid.begin()->getCritical() - 2) {
          sd.setHeadingData(-measureAngle(m_image->height - valid.end()->getYPos(), valid.end()->getXPos(), m_image->height - valid.begin()->getYPos(), valid.begin()->getXPos()));
          std::cout<<-measureAngle(m_image->height - valid.end()->getYPos(), valid.end()->getXPos(), m_image->height - valid.begin()->getYPos(), valid.begin()->getXPos())<< std::endl;
        } 
        // Steer to the left
        else if (valid.begin()->getXPos() > valid.begin()->getCritical() + 2) {
          sd.setHeadingData(measureAngle(m_image->height - valid.end()->getYPos(), valid.end()->getXPos(), m_image->height - valid.begin()->getYPos(), valid.begin()->getXPos()));
          std::cout<<measureAngle(m_image->height - valid.end()->getYPos(), valid.end()->getXPos(), m_image->height - valid.begin()->getYPos(), valid.begin()->getXPos())<<std::endl;
        } 
        // Steer straight
        else {
           sd.setHeadingData(0.0);
        }   
      }
      // Follow the lower right lines
      else
      {
       
        // Steer to the left
        if (rightLine1.getXPos() < rightLine1.getCritical() - 2) {
            sd.setHeadingData(-adjustAngle(m_image->height - 70, rightLine2.getXPos(), m_image->height - 50, rightLine1.getXPos()));
            std::cout<<-adjustAngle(m_image->height - 70, rightLine2.getXPos(), m_image->height - 50, rightLine1.getXPos())<<std::endl;
        } 
        // Steer to the right
        else if (rightLine1.getXPos() > rightLine1.getCritical() + 2) {
            sd.setHeadingData(adjustAngle(m_image->height - 70, rightLine2.getXPos(), m_image->height - 50, rightLine1.getXPos()));
            std::cout<<adjustAngle(m_image->height - 70, rightLine2.getXPos(), m_image->height - 50, rightLine1.getXPos())<<std::endl;
        } 
        // Steer straight
        else {
            sd.setHeadingData(0.0);
          }
        if((abs(upline1.getYPos()-upline2.getYPos())<2)&& upline1.getYPos()<upline1.getCritical()){
          state = 2;
        }

      }
      break;
      case 2:
        std::cout << "state 2" << std::endl;
        sd.setSpeedData(0);
        counter++;
        if (counter > 50) {
          counter = 0;
          state = 3;
        }
        std::cout << counter << std::endl;
        break;
      case 3:
        std::cout << "state 3" << std::endl;
        
        sd.setSpeedData(2);
        if (upline1.getYPos()>upline1.getCritical()){
          state = 1;
        }
        break;
      } //switch end

      /*
      && (rightLine1.getXPos() < 270 || 
          (valid.begin()->getXPos() < 270 && valid.end()->getXPos() < 270))
          */

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
        Container c1(Container::USER_DATA_3, ld);

        // Send container.
        getConference().send(c);
        getConference().send(c1);

    }

    // This method will do the main data processing job.
    // Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
    ModuleState::MODULE_EXITCODE LaneDetector::body() {
      // Get configuration data.
      KeyValueConfiguration kv = getKeyValueConfiguration();
      m_debug = kv.getValue<int32_t> ("lanedetector.debug") == 1;

        Player *player = NULL;

        uint32_t lanecounter = 0;
        clock_t start;
        start = clock();
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
        cumduration = ( clock() - start ) / (double) CLOCKS_PER_SEC;
        cout << "LaneDetector: processed " << lanecounter << " frames." << endl;
        cout << "LaneDetector: processed " << lanecounter/cumduration << " frames per sec." << endl;

        OPENDAVINCI_CORE_DELETE_POINTER(player);

      return ModuleState::OKAY;
    }
    
std::vector<Lines> LaneDetector::validateLines(std::vector<Lines>* lines)
{
  std::vector<Lines> line;
  int j = 0;

  // Iterates through the vector of lines and stops when two valid lines have been found.
  for(std::vector<Lines>::iterator it = lines->begin(); it != lines->end() && j < 2; it++ )
  {
    // As long as the lines X-position isn't extremely out of bounds...
    if(it->getXPos() < 270)
    {
      // ...add it to the vector.
      line.push_back(*it);
      j++;
    }
  }
  return line;

}

void LaneDetector::calculateCritical(const vector<Lines>::iterator& line, int dir, IplImage* image) {
  double result;

  result = measureDistance(line->getYPos(), dir, image);
  std::cout << "line pointer" << line->getYPos() << std::endl;

  if (result < 270) {
    line->setCritical(result);
    critCounter++;
  }
  
}

double LaneDetector::measureAngle(int yPos1, int xPos1, int yPos2, int xPos2) {
  double deltaY = yPos2 - yPos1;
  double deltaX = xPos2 - xPos1;

  double angle = atan2(deltaY, deltaX)/2;

  return angle;
}

double LaneDetector::adjustAngle(int yPos1, int xPos1, int yPos2, int xPos2){
  inputAngle1 = inputAngle2;
  inputAngle2 = inputAngle3;
  inputAngle3 = measureAngle(yPos1, xPos1, yPos2, xPos2);
  if((inputAngle3-inputAngle2)<0.00005 && (inputAngle2-inputAngle1)<0.00005){
    return 0.1*inputAngle3;
  }
  else if ((inputAngle3 - inputAngle2) < (inputAngle2 - inputAngle1)){
    return 0.3*inputAngle3;
  }else if ((inputAngle3 - inputAngle2)> (inputAngle2 - inputAngle1)){
    return 3*inputAngle3;
  }else {
    return inputAngle3;
  }
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
    for(i = x/2; i<x; i++){
      int r = data[(y-yPos)*step + i*channel + 0];
      int g = data[(y-yPos)*step + i*channel + 1];
      int b = data[(y-yPos)*step + i*channel + 2];
          
      if (r == 255 && g == 255 && b == 255){
        ptRight.x = i;
        break;
      }
      distance++;
    }
    line(newImage, ptMiddle, ptRight, cvScalar(127,255,0), 3, 8);
  }
  // Scans for full-white line to the left 
  else if (dir==0){
    for(i = x/2; i>0; i--){
      int r = data[(y-yPos)*step + i*channel + 0];
      int g = data[(y-yPos)*step + i*channel + 1];
      int b = data[(y-yPos)*step + i*channel + 2];
          
      if (r == 255 && g == 255 && b == 255){
      ptLeft.x = i;
        break;
      }
      distance++;
    }
    line(newImage, ptMiddle, ptLeft, cvScalar(139,0,139), 3, 8);
  }
  // Scans for upper full-white line
  else {
    for(i = 0; i< y-1; i++){
      int r = data[step*(y-1)+ yPos*channel + 0 -i*step];
      int g = data[step*(y-1)+ yPos*channel + 1 -i*step];
      int b = data[step*(y-1)+ yPos*channel + 2 -i*step];
          
      if (r == 255 && g == 255 && b == 255){
        ptUp.y = y-i;
        break;
      }
      distance++;
    }
    
    line(newImage, ptDown, ptUp, cvScalar(0,133,0), 1, 8);
  }
  return distance;
}

} // msv