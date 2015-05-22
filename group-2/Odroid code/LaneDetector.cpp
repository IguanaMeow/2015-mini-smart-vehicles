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

bool r1 = false;
bool l1 = false;
bool s1 = false;
bool hr1 = false;
bool hl1 = false;
int rightDistance;
int leftDistance;
int forwardDistance;

//draws a line between two Points
void DrawLine( cv::Mat img, cv::Point start, cv::Point end, cv::Scalar color)
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
        //if (m_debug) {
        // Create an OpenCV-window.
        //cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
        //cvMoveWindow("WindowShowImage", 300, 100);
        //}
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
                    const uint32_t numberOfChannels = 1;
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
                // F U cvFlip(m_image, 0, -1);
                
                retVal = true;
            }
        }
        return retVal;
    }
    
    // You should start your work in this method.
    void LaneDetector::processImage() {
        
        //double steeringAngle;
        // Create a Mat image, define points and draw lines between them
        if (!m_debug) {
            cv::Mat img(m_image);
            
            cv::Mat contours, thr;
            
            cv::Canny(img ,contours,35,90);
            cv::threshold(contours, thr, 180, 240, cv::THRESH_BINARY);
            
            //cv::namedWindow("Image");
            //cv::imshow("Image",img);
            
            //cv::namedWindow("Gray");
            //cv::imshow("Gray",gray_image);
            
            
            //cv::namedWindow("Threshold");
            //cv::imshow("Threshold",thr);
            
            
            
            
            
            //int rows = thr.rows;
            int cols = thr.cols;
            cv::Size s = thr.size();
            
            //rows = s.height;
            cols = s.width;
            
            cv::Point p1, p2, p3, p4, p5, p6;
            
            p1.x=cols/2;
            p1.y=400;
            p2.x=cols/2;
            p2.y=400;
            p3.x=cols/2;
            p3.y=400;
            p4.x=cols/2;
            p4.y=400;
            p5.x=cols/2;
            p5.y=400;
            p6.x=cols/2;
            p6.y=400;
            
            //we find the white lines by checking the intensity of a pixel
            //cv::Vec3b intensity = contours.at<cv::Vec3b>(p3);
            /* while (p3.x != 0){
             intensity = contours.at<cv::Vec3b>(p3);
             //when intensity is white, we have found the end point for our line
             if (intensity[0] == 255 && intensity[1] == 255 && intensity[2] == 255){
             break;
             }
             p3.x = p3.x -1;
             }
             DrawLine(contours, p3, p4, cv::Scalar(255,0,0));
             */
            
            
            
            //trying to find the left lane marking
            cv::Scalar intensity = thr.at<uchar>(p3);
            while (p3.x != 0){
                intensity = thr.at<uchar>(p3);
                if (intensity[0] > 127){
                    break;
                }
                p3.x = p3.x -1;
            }
            
            //trying to find the right lane marking
            cv::Scalar intensity2 = thr.at<uchar>(p5);
            while (p5.x != 640){
                intensity2 = thr.at<uchar>(p5);
                if (intensity2[0] > 127){
                    break;
                }
                p5.x = p5.x +1;
            }
            
            
            //we also need to find stop lines in front of the car
            cv::Scalar intensity3 = thr.at<uchar>(p2);
            
            while (p2.y != 0){
                intensity3 = thr.at<uchar>(p2);
                if (intensity3[0] > 127){
                    break;
                }
                p2.y = p2.y -1;
                
            }
            DrawLine(thr, p2, p1, cv::Scalar(255,255,255));
            DrawLine(thr, p3, p6, cv::Scalar(255,255,255));
            DrawLine(thr, p5, p4, cv::Scalar(255,255,255));
            
            
            //cvShowImage("WindowShowImage", m_image);
            //cv::namedWindow("Lane-det");
            //cv::imshow("Lane-det",thr);
            //cv::waitKey(30);
            //cvWaitKey(10);
        
        //From this point on, Jonathan Klemetz and Johan Hermansson, started implementing the logic
        //that had been planned for the lane following algorithm.
            
            rightDistance = p5.x - p4.x;
            leftDistance = p6.x - p3.x;
            forwardDistance = p1.y - p2.y;
            //cerr << rightDistance<< " Right distance 1"<<endl;
            //cerr << leftDistance<< " Left distance"<<endl;
            //cerr << forwardDistance<< " Fwd distance"<<endl;
            
            
            
            
        }
        SteeringData sd;
        //cerr << rightDistance<< " Right distance 2"<<endl;
        sd.setDistanceToRight(rightDistance);
        if(rightDistance > 132.5  && rightDistance < 187.5){
            sd.setDriveStraight(1);
            s1 = true;
            r1 = false;
            l1 = false;
            hr1 = false;
            hl1 = false;
        }else if(rightDistance > 62.5 && rightDistance < 132.5) {
            sd.setTurnLeft(1);
            s1 = false;
            r1 = false;
            l1 = true;
            hr1 = false;
            hl1 = false;
        }else if (rightDistance < 62.5){
            sd.setTurnHarderLeft(1);
            s1 = false;
            r1 = false;
            l1 = false;
            hr1 = false;
            hl1 = true;
        }else if (rightDistance > 187.5 && rightDistance <252.5){
            sd.setTurnRight(1);
            s1 = false;
            r1 = true;
            l1 = false;
            hr1 = false;
            hl1 = false;
        }else if (rightDistance > 252.5 && rightDistance < 319.5){
            sd.setTurnHarderRight(1);
            s1 = false;
            r1 = false;
            l1 = false;
            hr1 = true;
            hl1 = false;
        }else if (rightDistance > 319.5){
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



