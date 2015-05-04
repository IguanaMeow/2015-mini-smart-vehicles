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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"
#include "tools/player/Player.h"
#include "GeneratedHeaders_Data.h"
#include "LaneDetector.h"
#include <unistd.h>


void testArrow( cv::Mat img, cv::Point start, cv::Point end )
{
int thickness = 5;
int lineType = 6;
line( img,
start,
end,
cv::Scalar( 0, 0, 255 ),
thickness,
lineType );
}
void verticalLine( cv::Mat img, cv::Point start, cv::Point end )
{
int thickness = 1;
int lineType = 8;
line( img,
start,
end,
cv::Scalar( 0, 0, 255 ),
thickness,
lineType );
}
void rightArrow( cv::Mat img, cv::Point start, cv::Point end )
{
int thickness = 1;
int lineType = 8;
line( img,
start,
end,
cv::Scalar( 98, 206, 81 ),
thickness,
lineType );
}
void leftArrow( cv::Mat img, cv::Point start, cv::Point end )
{
int thickness = 1;
int lineType = 8;
line( img,
start,
end,
cv::Scalar(255,154,0 ),
thickness,
lineType );
}
namespace msv {
using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::data::image;
using namespace tools::player;
using namespace core::data::control;
using namespace core::data::environment;

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
if (m_debug) {
cv::Mat mat_img(m_image); //converts the IPL image to a mat image
int rows = mat_img.rows;
int cols = mat_img.cols;
cv::Size s = mat_img.size(); //gets rows and columns
rows = s.height;
cols = s.width;
cv::Point pt1; //initialize the startin and ending points for each arrow
cv::Point pt2;
cv::Point pt3;
cv::Point pt4;
cv::Point pt5;
cv::Point pt6;
pt1.x=cols/2; //starting points for first arrow
pt1.y=0;
pt2.x=cols/2; //ending points for first arrow
pt2.y=rows;

pt3.x=cols/2;
pt3.y=350;
pt4.x = pt3.x;
pt4.y = pt3.y;
pt5.x = pt3.x;
pt5.y = pt3.y;
pt6.x = 50;
pt6.y = 350;
cv::Vec3b rightLaneColor = mat_img.at<cv::Vec3b>(pt4); //defines the color at current positions
while(pt4.x != cols){
pt4.x = pt4.x +1; //extend the arrow
rightLaneColor = mat_img.at<cv::Vec3b>(pt4); //checks for color at current position
if(rightLaneColor.val[0] == 255 && rightLaneColor.val[1] == 255 && rightLaneColor.val[2] == 255){
break; //color is white at current position
}
}
rightArrow(mat_img, pt3, pt4);
cv::Vec3b leftLaneColor = mat_img.at<cv::Vec3b>(pt5);
while(pt5.x != 0){
pt5.x = pt5.x -1;
leftLaneColor = mat_img.at<cv::Vec3b>(pt5);
if(leftLaneColor.val[0] == 255 && leftLaneColor.val[1] == 255 && leftLaneColor.val[2] == 255){
break;
}
}
leftArrow(mat_img, pt3, pt5);

//vertical arrow
cv::Vec3b verticalColor = mat_img.at<cv::Vec3b>(pt1);
while(pt1.y != rows){
pt1.y = pt1.y +1;
verticalColor = mat_img.at<cv::Vec3b>(pt1);
if(verticalColor.val[0] == 255 && verticalColor.val[1] == 255 && verticalColor.val[2] == 255){
break;
}
}
verticalLine(mat_img, pt1, pt2);

imshow( "Display window", mat_img );
cvWaitKey(10);
SteeringData sd;
SensorBoardData sbd;
double angle = 0.0;
sd.setStopData(false);
//checks vertical line
if(pt1.y > 350 && pt1.y < 360){
//sends boolean to driver
sd.setStopData(true);

}
//checks right line
else if(pt4.x < 470 && pt4.x > 350 ){
angle = -10.0;
sd.setExampleData(angle); //sends the steering data to the driver.cpp code

}
//checks left line
else if(pt5.x > 170 ){
angle = +26.0;
sd.setExampleData(angle);

}


//TODO: Start here.
// 1. Do something with the image m_image here, for example: find lane marking features, optimize quality, ...
// 2. Calculate desired steering commands from your image features to be processed by driver.
// Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.

//Create container for finally sending the data.
Container c(Container::USER_DATA_1, sd);

// Send container.
getConference().send(c);
}
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

