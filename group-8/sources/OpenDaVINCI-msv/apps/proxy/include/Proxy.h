/**
 * proxy - Sample application to encapsulate HW/SW interfacing with embedded systems.
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

#ifndef PROXY_H_
#define PROXY_H_

#include <map>

#include "core/base/ConferenceClientModule.h"
#include "core/data/Container.h"
#include "tools/recorder/Recorder.h"
#include "serial/serial.h"

#include "Camera.h"
#include "RunningMedian.h"

namespace msv {

    using namespace std;

    /**
     * This class wraps the software/hardware interface board.
     */
    class Proxy : public core::base::ConferenceClientModule {
        private:
            /**
             * "Forbidden" copy constructor. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the copy constructor.
             *
             * @param obj Reference to an object of this class.
             */
            Proxy(const Proxy &/*obj*/);

            /**
             * "Forbidden" assignment operator. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the assignment operator.
             *
             * @param obj Reference to an object of this class.
             * @return Reference to this instance.
             */
            Proxy& operator=(const Proxy &/*obj*/);

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            Proxy(const int32_t &argc, char **argv);

            virtual ~Proxy();

            core::base::ModuleState::MODULE_EXITCODE body();

        private:
            virtual void setUp();

            virtual void tearDown();

            void distribute(core::data::Container c);
            int getSerial();
            void distSerial();
            void sendSerial();

        private:
            static const int INSERIAL = 17;
            static const int OUTSERIAL = 7;
            tools::recorder::Recorder *m_recorder;
            Camera *m_camera;
            serial::Serial *this_serial;
            uint8_t endByte;
            uint8_t startByte;
            uint8_t outSer[OUTSERIAL];
            uint8_t incomingSer[INSERIAL];
            uint8_t oldIncomingSer[OUTSERIAL];
            uint16_t speedOut;
            uint16_t steeringOut;
            RunningMedian irFrontRightMedian;
            RunningMedian irMiddleRightMedian;
            RunningMedian irBackMedian;
            RunningMedian usFrontMedian;
            RunningMedian usFrontRightMedian;


    };

} // msv

#endif /*PROXY_H_*/
