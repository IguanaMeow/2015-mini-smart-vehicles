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

#ifndef LANEDETECTOR_H_
#define LANEDETECTOR_H_

#include <opencv/cv.h>
#include "core/SharedPointer.h"
#include "core/base/ConferenceClientModule.h"
#include "core/wrapper/SharedMemory.h"

#include "Lines.h"

namespace msv {

    using namespace std;

    /**
     * This class is an exemplary skeleton for processing video data.
     */
    class LaneDetector: public core::base::ConferenceClientModule {
        private:
	        /**
	         * "Forbidden" copy constructor. Goal: The compiler should warn
	         * already at compile time for unwanted bugs caused by any misuse
	         * of the copy constructor.
	         *
	         * @param obj Reference to an object of this class.
	         */
	        LaneDetector(const LaneDetector &/*obj*/);

	        /**
	         * "Forbidden" a ssignment operator. Goal: The compiler should warn
	         * already at compile time for unwanted bugs caused by any misuse
	         * of the assignment operator.
	         *
	         * @param obj Reference to an object of this class.
	         * @return Reference to this instance.
	         */
	        LaneDetector& operator=(const LaneDetector &/*obj*/);

        public:
	        /**
	         * Constructor.
	         *
	         * @param argc Number of command line arguments.
	         * @param argv Command line arguments.
	         */
	        LaneDetector(const int32_t &argc, char **argv);

	        virtual ~LaneDetector();

	        core::base::ModuleState::MODULE_EXITCODE body();

        protected:
	        /**
	         * This method is called to process an incoming container.
	         *
	         * @param c Container to process.
	         * @return true if c was successfully processed.
	         */
	        bool readSharedImage(core::data::Container &c);

        private:
	        bool m_hasAttachedToSharedImageMemory;
	        core::SharedPointer<core::wrapper::SharedMemory> m_sharedImageMemory;
	        IplImage *m_image;
            bool m_debug;

			/* Scans for two valid lines in a vector of lines */
			std::vector<Lines> validateLines(std::vector<Lines>* lines);
            
            /* Measures the distance to full-white lines */
			double measureDistance(int yPos, int dir, IplImage* image);

			/* Measures the angle between delta X and delta Y */
			double measureAngle(int yPos1, int xPos1, int yPos2, int xPos2);

			double adjustAngle(int yPos1, int xPos1, int yPos2, int xPos2);

	        virtual void setUp();

	        virtual void tearDown();

            void processImage();
            
			Lines rightLine1;
        	Lines rightLine2;

        	Lines leftLine1;
        	Lines leftLine2;
        	Lines leftLine3;
        	Lines leftLine4;
        	Lines upline1;
        	Lines upline2;

        	int state;
        	int counter;

        	double inputAngle1;
        	double inputAngle2;
        	double inputAngle3;


	};

} // msv

#endif /*LANEDETECTOR_H_*/