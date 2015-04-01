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
#include <opencv2/imgproc/imgproc.hpp>

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/image/SharedImage.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"
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
    using namespace core::data::control;
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

    void LaneDetector::processImage() {
        if (m_debug) {
            if (m_image != NULL) {
                cvWaitKey(10);
            }
        }

        // CONFIGURATION
        // Portion of the screen used for lane detection
        const float portion = 0.35;
        // Amount of lines checked
        const int rows = 20;
        // Total value to turning degrees ratio
        const float ratio = 0.0006;
        // Increased weight of later lines (1 + weight * line#)
        const float weight = 0.5;
        // Max turn degrees per tick
        const float maxturn = 2.5;

        Mat src(m_image);
        Mat dst, cdst;
        const int center = src.cols / 2;
        const int rowdist = (src.rows * portion) / rows;
        const int lnull = -1;
        const int rnull = src.cols + 1;
        int lcount = 0;
        int total = 0;
        int left[rows];
        int right[rows];

        // Edge detection
        Canny(src, dst, 50, 200, 3);
        cvtColor(dst, cdst, CV_GRAY2BGR);
        cdst.setTo(Scalar::all(0));

        // Line detection
        vector<Vec4i> lines;
        HoughLinesP(dst, lines, 1, CV_PI/180, 8, 10, 10);

        // Remove lines on upper half of screen
        for(size_t i = lines.size(); i > 0; i--)
        {
            Vec4i l = lines[i-1];
            if(l[1] < src.rows/2 && l[3] < src.rows/2)
            {
                lines.erase(lines.begin() + i - 1);
            }
        }

        // Draw lines
        for(size_t i = 0; i < lines.size(); i++)
        {
            Vec4i l = lines[i];
            line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
        }
        
        // Draw center line
        line(cdst, Point(center, 0), Point(center, src.rows), Scalar(255,0,0), 1, CV_AA);

        // Calculate distance to lines
        for(int i = 0; i < rows; i++)
        {
            int lx = lnull;
            int rx = rnull;
            int y = src.rows - ((i + 1) * rowdist);
            for(size_t j = 0; j < lines.size(); j++)
            {
                Vec4i l = lines[j];

                if((l[1] < y && l[3] > y) || (l[1] > y && l[3] < y))
                {
                    float ytotal = l[1] - l[3];
                    float ypart = l[1] - y;
                    float percent = ypart / ytotal;

                    int x = (l[2] - l[0]) * percent + l[0];

                    if(x < center)
                    {
                        if(x > lx)
                        {
                            lx = x;
                        }
                    }
                    else
                    {
                        if(x < rx)
                        {
                            rx = x;
                        }
                    }
                }
            }
            left[i] = lx;
            right[i] = rx;
            if(lx != lnull)
            {
                lcount++;
            }
        }

        // Predict lines in front
        for(int i = 1; i < rows; i++)
        {
            if(left[i] == lnull && left[i - 1] != lnull)
            {
                int j = i;
                while(++j < rows && left[j] == lnull);
                if(j != rows)
                {
                    left[i] = left[i - 1] + (left[j] - left[i - 1]) / (j - (i - 1));
                }
                else
                {
                    left[i] = left[i - 1] + (left[i - 1] - left[i - 2]);
                }
            }
            if(right[i] == rnull && right[i - 1] != rnull)
            {
                int j = i;
                while(++j < rows && right[j] == rnull);
                if(j != rows)
                {
                    right[i] = right[i - 1] + (right[j] - right[i - 1]) / (j - (i - 1));
                }
                else
                {
                    right[i] = right[i - 1] + (right[i - 1] - right[i - 2]);
                }
            }
        }

        // Predict lines backwards
        for(int i = rows - 2; i >= 0; i--)
        {
            if(left[i] == lnull && left[i + 1] != lnull)
            {
                int j = i;
                while(--j < rows && left[j] == lnull);
                if(j != -1)
                {
                    left[i] = left[i + 1] + (left[j] - left[i + 1]) / (j - (i + 1));
                }
                else
                {
                    left[i] = left[i + 1] + (left[i + 1] - left[i + 2]);
                }
            }
            if(right[i] == rnull && right[i + 1] != rnull)
            {
                int j = i;
                while(--j < rows && right[j] == rnull);
                if(j != -1)
                {
                    right[i] = right[i + 1] + (right[j] - right[i + 1]) / (j - (i + 1));
                }
                else
                {
                    right[i] = right[i + 1] + (right[i + 1] - right[i + 2]);
                }
            }
        }

        // Calculate sum of distances
        for(int i = 0; i < rows; i++)
        {
            int y = src.rows - ((i + 1) * rowdist);
            if(lcount > 3)
            {
                /*if(i == 0 ||
                    (left[i] - left[i - 1] > -100 &&
                    right[i] - right[i - 1] < 100))*/
                {
                    line(cdst, Point(left[i], y), Point(center, y), Scalar(0, 255, 255), 1, CV_AA);
                    line(cdst, Point(right[i], y), Point(center, y), Scalar(0, 255, 0), 1, CV_AA);
                    total += (-left[i] + center - (right[i] - center)) * (i * weight + 1);
                }
                /*else
                {
                    printf("%f\n", (float)left[i - 1] / (float)left[i]);
                    break;
                }*/
            }
        }

        // Show image
        imshow("Lane Detection", cdst);

        // Retrieve previous steeringdata
        Container vehicleControlData = getKeyValueDataStore().get(Container::VEHICLECONTROL);
        VehicleControl vc = vehicleControlData.getData<VehicleControl>();

        // Calculate new angle
        double prevAngle = vc.getSteeringWheelAngle() * Constants::RAD2DEG;
        double diff = (total * -ratio) - prevAngle;
        diff = fmin(maxturn, diff);
        diff = fmax(-maxturn, diff);
        double newAngle = prevAngle + diff;

        // Create SteeringData
        SteeringData sd;
        sd.setExampleData(newAngle);

        printf("Total: %i, %f, Heading: %f\n", total, prevAngle, diff);

        // Create container for sending the data
        Container c(Container::USER_DATA_1, sd);
        // Send container
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

