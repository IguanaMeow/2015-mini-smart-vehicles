/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe 
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

#ifndef __SERIAL_KILLER__H__
#define __SERIAL_KILLER__H__

#include "core/base/ConferenceClientModule.h"
#include "core/base/Mutex.h"
#include "GeneratedHeaders_Data.h"

namespace msv {

    using namespace std;

    /**
     * This class demonstrates how to receive sent data over a serial port.
     */
    class SerialKiller : public core::base::ConferenceClientModule, public core::wrapper::StringListener, public core::wrapper::ConnectionListener {
        private:
            /**
             * "Forbidden" copy constructor. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the copy constructor.
             *
             * @param obj Reference to an object of this class.
             */
            SerialKiller(const SerialKiller &/*obj*/);

            /**
             * "Forbidden" assignment operator. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the assignment operator.
             *
             * @param obj Reference to an object of this class.
             * @return Reference to this instance.
             */
            SerialKiller& operator=(const SerialKiller &/*obj*/);

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            SerialKiller(const int32_t &argc, char **argv);

            virtual ~SerialKiller();

            core::base::ModuleState::MODULE_EXITCODE body();

            virtual void handleConnectionError();

            virtual void nextString(const string &s);

            std::map<uint32_t, double> sensorValues;
            SensorBoardData sbd;
        private:
            virtual void setUp();

            virtual void tearDown();
            void takeAction(const string&, const string&);

            enum SENSOR_ID {
                IR_FrontRight = 0,
                IR_RearRight = 1,
                IR_Rear = 2,
                US_FrontCenter = 3,
                US_FrontRight = 4,
                US_RearRight = 5
            };

            core::base::Mutex m_bufferMutex;
            string m_buffer;
    };

} // examples

#endif /*SERIAL_KILLER_H_*/
