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
			    //cvFlip(m_image, 0, -1);

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

        /////////////////////
        //  CONFIGURATION  //
        /////////////////////
        // Portion of the image cropped away
        const float crop = 0.5;
        // Total value exponent
        const float exp = 1.09;
        // Total value to turning degrees ratio
        const float ratio = 0.075;
        // Increased weight of each row (1 + weight * row#)
        const float weight = 0.25;
        // Minimum row length
        const int minLength = 10;

        // Create Mat from image
        Mat src(m_image);

        // Crop upper half of the image
        src = src(Rect(0, src.rows * crop, src.cols, src.rows * (1 - crop)));

        // Retrieve previous angle
        Container vehicleControlData = getKeyValueDataStore().get(Container::VEHICLECONTROL);
        VehicleControl vc = vehicleControlData.getData<VehicleControl>();
        const double prevAngle = vc.getSteeringWheelAngle() * Constants::RAD2DEG;

        // Variable declarations
        Mat dst, cdst;
        const int rows = 36 - (abs(prevAngle * 0.8) > 8 ? 8 : floor(abs(prevAngle * 0.8)));
        const int rowdist = src.rows * 0.025;
        const int center = src.cols / 2;
        const int wbot = center * 18 / 20;
        const int wtop = center * 8 / 20;
        int lnull[rows];
        int rnull[rows];
        for(int i = 0; i < rows; i++)
        {
            const int y = src.rows - ((i + 1) * rowdist);
            lnull[i] = ((center - wtop) - (center - wbot)) * ((src.rows - y) / (float)src.rows) + (center - wbot);
            rnull[i] = ((center + wtop) - (center + wbot)) * ((src.rows - y) / (float)src.rows) + (center + wbot);
        }
        int lcount = 0;
        int rcount = 0;
        int total = 0;
        int left[rows];
        int right[rows];
        float totalWeight = 0;

        // Edge detection
        Canny(src, dst, 100, 300, 3);
        cvtColor(dst, cdst, CV_GRAY2BGR);
        cdst.setTo(Scalar::all(0));

        // Line detection
        vector<Vec4i> lines;
        HoughLinesP(dst, lines, 1, CV_PI/180, 8, 10, 10);

        // Calculate angle of lines and filter horizontal ones
        for(size_t i = 0; i < lines.size(); i++)
        {
            Vec4i l = lines[i];
            int dy = l[3] - l[1];
            int dx = l[2] - l[0];
            float angle = 0;
            if(dx != 0)
            {
                float theta = atan2(dy, dx);
                angle = theta * 180/CV_PI;
            }
            else
            {
                if(dy > 0)
                    angle = 90;
                else
                    angle = -90;
            }

            angle *= -1;
            if(angle < 0)
                angle = 180 + angle;

            if(angle < 15 || angle > 165)
            {
                line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 64), 1, CV_AA);
                lines.erase(lines.begin() + i);
                i--;
            }
        }

        if (m_debug)
        {
            // Draw lines
            for(size_t i = 0; i < lines.size(); i++)
            {
                Vec4i l = lines[i];
                line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 1, CV_AA);
            }

            // Draw center line
            line(cdst, Point(center, 0), Point(center, src.rows), Scalar(255,0,0), 1, CV_AA);

            // Draw filter limit
            line(cdst, Point(center - wbot, src.rows), Point(center - wtop, 0), Scalar(255, 192, 0), 1, CV_AA);
            line(cdst, Point(center + wbot, src.rows), Point(center + wtop, 0), Scalar(255, 192, 0), 1, CV_AA);
        }

        // Calculate distance to lines
        for(int i = 0; i < rows; i++)
        {
            const int y = src.rows - ((i + 1) * rowdist);
            int lx = lnull[i];
            int rx = rnull[i];
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
                        if(x > lx && x < center - minLength)
                        {
                            lx = x;
                        }
                    }
                    else
                    {
                        if(x < rx && x > center + minLength)
                        {
                            rx = x;
                        }
                    }
                }
            }
            left[i] = lx;
            right[i] = rx;
            if(lx != lnull[i])
            {
                lcount++;
            }
            if(rx != rnull[i])
            {
                rcount++;
            }
        }

        // Predict rows forwards
        for(int i = 1; i < rows; i++)
        {
            if(left[i] == lnull[i] && left[i - 1] != lnull[i - 1] && lcount > 2)
            {
                int j = i;
                while(++j < rows && left[j] == lnull[j]);
                if(j != rows)
                {
                    left[i] = limit(left[i - 1] + (left[j] - left[i - 1]) / (j - (i - 1)), lnull[i], center);
                }
                else
                {
                    left[i] = limit(left[i - 1] + (left[i - 1] - left[i - 2]), lnull[i], center);
                }
            }
            if(right[i] == rnull[i] && right[i - 1] != rnull[i - 1] && rcount > 2)
            {
                int j = i;
                while(++j < rows && right[j] == rnull[j]);
                if(j != rows)
                {
                    right[i] = limit(right[i - 1] + (right[j] - right[i - 1]) / (j - (i - 1)), center, rnull[i]);
                }
                else
                {
                    right[i] = limit(right[i - 1] + (right[i - 1] - right[i - 2]), center, rnull[i]);
                }
            }
        }

        // Predict rows backwards
        for(int i = rows - 2; i >= 0; i--)
        {
            if(left[i] == lnull[i] && left[i + 1] != lnull[i + 1] && lcount > 2)
            {
                int j = i;
                while(--j >= 0 && left[j] == lnull[j]);
                if(j != -1)
                {
                    left[i] = limit(left[i + 1] + (left[j] - left[i + 1]) / (j - (i + 1)), lnull[i], center);
                }
                else
                {
                    left[i] = limit(left[i + 1] + (left[i + 1] - left[i + 2]), lnull[i], center);
                }
            }
            if(right[i] == rnull[i] && right[i + 1] != rnull[i + 1] && rcount > 2)
            {
                int j = i;
                while(--j >= 0 && right[j] == rnull[j]);
                if(j != -1)
                {
                    right[i] = limit(right[i + 1] + (right[j] - right[i + 1]) / (j - (i + 1)), center, rnull[i]);
                }
                else
                {
                    right[i] = limit(right[i + 1] + (right[i + 1] - right[i + 2]), center, rnull[i]);
                }
            }
        }

        // Calculate sum of distances
        for(int i = 0; i < rows; i++)
        {
            const int y = src.rows - ((i + 1) * rowdist);
            int value = (-left[i] + center - (right[i] - center)) * (i * weight + 1);
            total += value;
            totalWeight += (i * weight + 1);

            if (m_debug)
            {
                line(cdst, Point(left[i], y), Point(center, y), Scalar(0, 255, 255), 1, CV_AA);
                line(cdst, Point(right[i], y), Point(center, y), Scalar(0, 255, 0), 1, CV_AA);
            }
        }

        // Calculate new angle
        total /= totalWeight;
        const int sign = (total < 0) ? -1 : 1;
        total *= sign;
        total = pow(total, exp);
        total *= sign;
        const double newAngle = total * -ratio;

        // Create SteeringData
        SteeringData sd;
        sd.setWheelAngle(newAngle);

        // Show image
        if (m_debug)
        {
            imshow("Source", src);
            imshow("Canny", dst);
            imshow("Lane Detection", cdst);
        }
        printf("Total: %i, Angle: %f\n", total, newAngle);

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

    // Constrains input between low and high
    int LaneDetector::limit(int input, int low, int high)
    {
        return min(high, max(low, input));
    }
} // msv

