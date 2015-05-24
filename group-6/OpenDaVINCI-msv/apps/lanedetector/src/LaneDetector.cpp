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

//Jose & Saleh

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/image/SharedImage.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include <unistd.h>  // timer

#include "tools/player/Player.h"

#include "GeneratedHeaders_Data.h"

#include "LaneDetector.h"
#define PI 3.14159265

namespace msv {


    using namespace cv;
    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::image;
    using namespace tools::player;


    double angle;
    double speed;

    void vertical_Line(Mat img, Point start_point, Point end_Point) {
        line(img, start_point, end_Point, Scalar(255, 0, 0), 2, 8);
    }


    void horizental_green_Line(Mat img, Point start_point, Point end_Point) {
        line(img, start_point, end_Point, Scalar(255, 0, 0), 2, 8);
    }

    void p5_7(Mat img, Point start_point, Point end_Point) {
        line(img, start_point, end_Point, Scalar(128, 0, 128), 2, 8);
    }

    // distance between p5  and p7
    double Distance(double dX0, double dY0, double dX1, double dY1) {
        return sqrt((dX1 - dX0) * (dX1 - dX0) + (dY1 - dY0) * (dY1 - dY0));
    }

    LaneDetector::LaneDetector(const int32_t &argc, char **argv) : ConferenceClientModule(argc, argv, "lanedetector"),
                                                                   m_hasAttachedToSharedImageMemory(false),
                                                                   m_sharedImageMemory(),
                                                                   m_image(NULL),
                                                                   m_debug(false) {

    }

    LaneDetector::~LaneDetector() {

    }


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

            SharedImage si = c.getData<SharedImage>();

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
        double count_green1 = 0.0;
        double count_green2 = 0.0;
        double count_green3 = 0.0;
        double count_green4 = 0.0;
        double count_green_medel = 0.0;
        double count_green7 = 0.0;
        double count_green8 = 0.0;


        // Example: Show the image.
        if (m_debug) {
            if (m_image != NULL) {
                // cvShowImage("WindowShowImage", m_image);
                cvWaitKey(10);
            }
        }

        Mat img, dst, cdst;
        img = cvarrToMat(m_image, true);  //The IplImage is converted to Mat

        int rows = img.rows;
        int cols = img.cols;

        Point p1, p2, p3, p4, p5, p6, p7;  // for angle

        p1.x = cols / 2;
        p1.y = 0;
        p2.x = cols / 2;
        p1.y = rows;

        p3.x = cols / 2;
        p3.y = rows / 1.4;
        p4.x = cols;
        p4.y = rows / 1.4;

        //Edge Detection
        Canny(img, dst, 50, 200, 3);
        cvtColor(dst, cdst, CV_GRAY2BGR);
        cdst.setTo(Scalar::all(0));

        //Line Detection
        vector<Vec4i> lines;
        HoughLinesP(dst, lines, 1, CV_PI / 180, 8, 10, 10);

        //Remove lines upper lines 
        for (size_t i = lines.size(); i > 0; i--) {

            Vec4i l = lines[i - 1];
            if (l[1] < img.rows / 2 && l[3] < img.rows / 2) {

                lines.erase(lines.begin() + i - 1);

            }
        }

        //Draw the lines
        for (size_t i = 0; i < lines.size(); i++) {
            Vec4i l = lines[i];
            line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 2, 8);
        }

        vertical_Line(cdst, p1, p2);

        //horizontal green_line7
        for (int j = p3.x; j > = 0; j--) {
            int i = p3.y / 0.87;
           
            // start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;

            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green7++;
            }

        }

        //horizontal green_line8
        for (int j = p3.x; j > = 0; j--) {
            int i = p3.y / 0.77;
            
            // start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;

            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green8++;
            }

        }

        //horizental_green_Line1
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 1.5;
            
            // start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;

            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green1++;
            }
        }

        //horizontal green_line2
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 1.4;
           
            // start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;

            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green2++;
            }
        }

        //horizontal green_line3
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 1.2;
            
            // start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;

            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green3++;
            }
        }

        //horizontal green_line4
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 1.1;
            
            // start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;
            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green4++;
            }
        }

        //horizontal green_line
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 0.85;
            
            //start drawing the horizental_green_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {

                p5.x = j;
                p5.y = i;
                break;
            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green++;
            }
        }

        //horizontal green_medel
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 0.8;
           
            // start drawing the horizental_green_medel until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {
                p6.x = j;
                p6.y = i;
                break;
            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 200;
                cdst.at<Vec3b>(i, j)[2] = 0;

                count_green_medel++;

            }
        }

        //horizental_yellow_Line
        for (int j = p3.x; j < img.cols; j++) {
            int i = p3.y / 0.75;
           
            // start drawing the horizental_yellow_Line until it detect a white color(long lane),by then stop drawing
            if (cdst.at<Vec3b>(i, j)[0] == 255 && cdst.at<Vec3b>(i, j)[1] == 255 && cdst.at<Vec3b>(i, j)[2] == 255) {
                p7.x = j;
                p7.y = i;
                break;
            } else {
                cdst.at<Vec3b>(i, j)[0] = 0;
                cdst.at<Vec3b>(i, j)[1] = 255;
                cdst.at<Vec3b>(i, j)[2] = 255;

                count_yellow++;
            }
        }


        // counter_inter

        p5_7(cdst, p5, p7);
        imshow("Lane Detection", cdst);


        SteeringData sd;
        double Pink_line = Distance(p5.x, p5.y, p7.x, p7.y);

        cout << "################################ " << endl;
        cout << "count_green/count_yellow " << count_green / count_yellow << endl;
        cout << "count_green1/count_green2 " << count_green1 / count_green2 << endl;
        cout << "count_green3/count_green4 " << count_green3 / count_green4 << endl;
        cout << "LEFT LANE " << count_green7 / count_green8 << endl;
        cout << count_green << endl;
        cout << count_yellow << endl;
        cout << angle << endl;

        cout << "Pink_line " << Pink_line << endl;


        Container containerSteeringData = getKeyValueDataStore().get(Container::USER_DATA_1);
        SteeringData sd_old = containerSteeringData.getData<SteeringData>();
        double critical_dist = sd_old.getDistanceData();
        ///////////////////////////////////////////////////////////////////////

        int w = img.cols/2;

        if((int)count_green_medel == w && (int)count_green != w && (int)count_yellow != w ){

            speed = 0.1;
            angle = 0;

            cout<<"Right side is middle lane"<<endl;

        } else if ((count_green / count_yellow) > 0.81 && (count_green / count_yellow) < 0.911)   {

            speed = 1;

            if (critical_dist < 0.1) {
                angle = 0;
                critical_dist = count_green;
            } else {
                angle = (count_green - critical_dist)/20;
            }

            cout << "STRAIGHT" << endl;
        } else if ((count_green / count_yellow) < 0.811 && (count_green / count_yellow) > 0.64) {

            speed = 0.5;
            angle = (count_green - critical_dist)/7;
            cout << "LEFT" << endl;
        } else if (((count_green / count_yellow) > 0.999 || (count_green3 / count_green4) > 0.999) &&
                 (count_green1 / count_green2 < 1)) {


            speed = 0.5;
            angle = 0;
            cout << "INTERSECTION" << endl;
        } else if ((count_green / count_yellow) > 0.91 && (count_green / count_yellow) < 0.999) {

            speed = 0.5;
            angle = (count_green - critical_dist)/5;
            cout << "RIGHT" << endl;

        } else {
            speed = sd_old.getSpeedData();
            angle = sd_old.getExampleData();

            cout<<"LOST LANE"<<endl;
        }

    sd.setDistanceData(critical_dist);
    sd.setSpeedData(speed);
    sd.setExampleData(angle);

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



    }
    // msv
