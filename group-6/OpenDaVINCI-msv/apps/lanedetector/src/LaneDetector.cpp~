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

    void vertical_Line(Mat img, Point start_point, Point end_Point) 
    {
      line(img, start_point, end_Point, Scalar(255, 0, 0), 2, 8);
    }

    
    void horizental_green_Line( Mat img, Point start_point, Point end_Point ) 
    {
      line( img, start_point, end_Point, Scalar( 255, 0, 0),  2, 8);
    }
/* void p5_6( Mat img, Point start_point, Point end_Point ) 
    {
      line( img, start_point, end_Point, Scalar( 255, 0, 250),  2, 8);
    }
void p6_7( Mat img, Point start_point, Point end_Point ) 
    {
      line( img, start_point, end_Point, Scalar( 128, 0, 128),  2, 8);
   }  */
void p5_7( Mat img, Point start_point, Point end_Point ) 
    {
      line( img, start_point, end_Point, Scalar( 128, 0, 128),  2, 8);
    }
// distance between p5  and p7
double Distance(double dX0, double dY0, double dX1, double dY1)
{
    return sqrt((dX1 - dX0)*(dX1 - dX0) + (dY1 - dY0)*(dY1 - dY0));
}
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
// OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
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
  // double   count_inter=0.0;// for intersection
  // double  count_inter2=0.0;
    double count_green2 = 0.0;
     double count_red = 0.0;
    // Examp le: Show the image.
        if (m_debug) {
            if (m_image != NULL) {
               // cvShowImage("WindowShowImage", m_image);
                cvWaitKey(10);
            }
        }

        //TODO: Start here.

        Mat img, dst, cdst;
        img = cvarrToMat(m_image, true);
        
        //int centerLine = img.cols/2;
        int rows = img.rows;
        int cols = img.cols;

        Point p1, p2;
        Point p3, p4;
        Point p5,p6,p7 ;  // for angle

        p1.x = cols/2;
        p1.y = 0;
        p2.x = cols/2;
        p1.y = rows;

        p3.x = cols/2;
        p3.y = rows/1.4;
        p4.x = cols;
        p4.y = rows/1.4;

        //Edge Detection
        Canny(img, dst, 50, 200, 3);
        cvtColor(dst, cdst, CV_GRAY2BGR);
        cdst.setTo(Scalar::all(0));

        //Line Detection
        vector<Vec4i> lines;
       HoughLinesP(dst, lines, 1, CV_PI/180, 8, 10, 10);

        //Remove lines 
        for(size_t i = lines.size(); i > 0; i--) {
            Vec4i l = lines[i -1];
            if(l[1] < img.rows/2 && l[3] < img.rows/2) {
                lines.erase(lines.begin() + i -1);
            }
        }

        //Draw the lines
        for(size_t i = 0; i < lines.size(); i++) {

            Vec4i l = lines[i];
            line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 2, 8);
        }

        //Draw center line
       // line(cdst, Point(center, 0), Point(center, img.rows), Scalar(255,0,0), 2, 8);

        vertical_Line(cdst, p1, p2);


        for (int j=p3.x; j<img.cols; j++)
        {  int i=p3.y/1.5;
  // start drawing the horizental_green_Line until it detect a red color(long lane),by then stop drawing
        if(cdst.at<Vec3b>(i,j)[0] == 255 && cdst.at<Vec3b>(i,j)[1] == 255 && cdst.at<Vec3b>(i,j)[2]  ==255)
   {
   p5.x=j;
   p5.y=i;
        break; }

                else{cdst.at<Vec3b>(i,j)[0] = 0;
                cdst.at<Vec3b>(i,j)[1] = 0;
                cdst.at<Vec3b>(i,j)[2] = 255;
        
        count_red++; }  }


        for (int j=p3.x; j<img.cols; j++)
        {  int i=p3.y/1.2;
  // start drawing the horizental_green_Line until it detect a red color(long lane),by then stop drawing
        if(cdst.at<Vec3b>(i,j)[0] == 255 && cdst.at<Vec3b>(i,j)[1] == 255 && cdst.at<Vec3b>(i,j)[2]  ==255)
   {
   p5.x=j;
   p5.y=i;
        break; }

                else{cdst.at<Vec3b>(i,j)[0] = 0;
                cdst.at<Vec3b>(i,j)[1] = 0;
                cdst.at<Vec3b>(i,j)[2] = 255;
        
        count_red++; }  }
        //horizental_green_Line

        for (int j=p3.x; j<img.cols; j++)
        {  int i=p3.y/0.85;
  // start drawing the horizental_green_Line until it detect a red color(long lane),by then stop drawing
        if(cdst.at<Vec3b>(i,j)[0] == 255 && cdst.at<Vec3b>(i,j)[1] == 255 && cdst.at<Vec3b>(i,j)[2]  ==255)
   {
   p5.x=j;
   p5.y=i;
        break; }

                else{cdst.at<Vec3b>(i,j)[0] = 0;
                cdst.at<Vec3b>(i,j)[1] = 200;
                cdst.at<Vec3b>(i,j)[2] = 0;
        
        count_green++; }
  
//cut<<count_green<<endl; 
           
        }
for (int j=p3.x; j<img.cols; j++)
        {  int i=p3.y/0.8;
  // start drawing the horizental_green_Line until it detect a red color(long lane),by then stop drawing
        if(cdst.at<Vec3b>(i,j)[0] == 255 && cdst.at<Vec3b>(i,j)[1] == 255 && cdst.at<Vec3b>(i,j)[2]  ==255)
         { p6.x=j;
          p6.y=i;  
        break; }

                else{cdst.at<Vec3b>(i,j)[0] = 0;
                cdst.at<Vec3b>(i,j)[1] = 200;
                cdst.at<Vec3b>(i,j)[2] = 0;
        
        count_green2++;
   }
//cut<<count_green<<endl; 
           
        }
//horizental_yellow_Line
    for (int j=p3.x; j<img.cols; j++)
        {  int i=p3.y/0.75;
  // start drawing the horizental_green_Line until it detect a red color(long lane),by then stop drawing
      if(cdst.at<Vec3b>(i,j)[0] == 255 && cdst.at<Vec3b>(i,j)[1] == 255 && cdst.at<Vec3b>(i,j)[2]  ==255)
      {p7.x=j;
       p7.y=i;break; }

               else{ cdst.at<Vec3b>(i,j)[0] = 0;
                cdst.at<Vec3b>(i,j)[1] = 255;
                cdst.at<Vec3b>(i,j)[2] = 255;
        
        count_yellow++; }
  
//cut<<count_green<<endl; 
           
        }

// counter_inter 
 
p5_7(cdst, p5, p7 ) ;
   //  p5_6(cdst, p5, p6 ) ;
    // p6_7(cdst, p6, p7 ) ;
        //Show Image
        imshow("Lane Detection", cdst);



        // 1. Do something with the image m_image here, for example: find lane marking features, optimize quality, ...



        // 2. Calculate desired steering commands from your image features to be processed by driver.



        // Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.
        SteeringData sd;





/*left_ curve
   else if ((count_green/count_yellow)>0.35 &&(count_green/count_yellow)<0.67 &&count_green > 166 && count_yellow > 245  ) {
    speed=1.0;
    angle=-3.5;

    cout<<"1"<<endl;
    
    }else if((count_green/count_yellow)<0.1) {
    speed=2.0;
    angle=-4.0;
    
    cout<<"2"<<endl;
    
    }else if ((count_green/count_yellow)>0.35 &&(count_green/count_yellow)<0.58) {
    speed=0.5;
    angle=-10.0;

    cout<<"3"<<endl;
    
    }else if(count_green<10) {
    speed=2;
    angle=-6.0;

    cout<<"4"<<endl;
    
    }else if((count_green/count_yellow)>0.7&&(count_green/count_yellow)<1.0) {
    speed=2.0;
    angle=12.0;
    
    cout<<"5"<<endl;
    
    }else if((count_green/count_yellow)>1.3 && count_yellow<200 && count_yellow>30) {
    speed=2;
    angle=16;
    
    cout<<"6"<<endl;
    
    }else if((count_green/count_yellow)>1.3&& (count_green/count_yellow)<20.0 && count_yellow<30) {
    speed=2;
    angle=12;
    
    cout<<"7"<<endl;
    
    }else if((count_green/count_yellow)>20.0 && count_yellow<30) {
    speed=2;
    angle=-6;

    cout<<"7"<<endl;*/  

double Pink_line =Distance(p5.x, p5.y, p7.x, p7.y);

cout << "######################## " <<  endl;       
cout<<count_green/count_yellow<<endl;
cout<<count_red/count_green2<<endl;
cout<<count_green<<endl;
cout<<count_yellow<<endl;
cout<<angle<<endl;
cout << "PINK = " << Distance(p5.x, p5.y, p7.x, p7.y) << endl;
cout << " red = " << count_red << endl;
  ///////////////////////////////////////////////////////////////////////                ////////////////////////77


if(( count_green/count_yellow)> 0.801 && (count_green/count_yellow)< 0.92 ) {
     speed=8;
    angle=0;

    cout<<"111111111111111111111111"<<endl;}

else if(Pink_line>100 || Pink_line< 55){
    speed=2;
    angle=0;

    cout<<"int 2"<<endl;}

else if (count_green/count_yellow< 0.801 && count_green/count_yellow > 0.69) {  
      speed=0.5;
      angle=-10.0;  
       
  cout<<" LEFT "<<endl;}

//else if((count_green>319 && count_yellow>319)){
  //  speed=2;
    //angle=0;

    //cout<<"int 2"<<endl;}

else  {
     //usleep(100000);
   // double m=(atan2(p5.y-p7.y, p5.x-p7.x)* 180 / PI);
  //  double n=(atan2(p6.y-p7.y, p6.x-p7.x)* 360 / PI);
      angle=9.5;  
      speed=0.5;
    cout<<" RIGHT "<<endl;
     }




  /*  angle=(m+n)/2-4.33 ;
    cout<<"8"<<endl;*/
  
  
    sd.setSpeedData(speed);


    sd.setExampleData(angle);
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
