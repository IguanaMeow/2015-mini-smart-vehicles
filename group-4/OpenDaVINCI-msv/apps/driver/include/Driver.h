/**
 * driver - Sample application for calculating steering and acceleration commands.
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

#ifndef DRIVER_H_
#define DRIVER_H_

#include "core/base/ConferenceClientModule.h"
#include "core/data/environment/Point3.h"
#include "core/data/environment/VehicleData.h"
#include "core/data/control/VehicleControl.h"
#include "Parking.h"
#include "Overtaking.h"


namespace msv {

    using namespace std;
    using namespace core::data::environment;
    using namespace core::data::control;

    /**
     * This class is a skeleton to send driving commands to Hesperia-light's vehicle driving dynamics simulation.
     */
    class Driver : public core::base::ConferenceClientModule {
        private:
            /**
             * "Forbidden" copy constructor. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the copy constructor.
             *
             * @param obj Reference to an object of this class.
             */
            Driver(const Driver &/*obj*/);

            /**
             * "Forbidden" assignment operator. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the assignment operator.
             *
             * @param obj Reference to an object of this class.
             * @return Reference to this instance.
             */
            Driver& operator=(const Driver &/*obj*/);
            enum SENSOR_ID {
                IR_FrontRight = 0,
                IR_RearRight = 1,
                IR_Rear = 2,
                US_FrontCenter = 3,
                US_FrontRight = 4,
                US_RearRight = 5
            };

            Parking parking;
            Overtaking overtaking;
            virtual void setUp();
            virtual void tearDown();

            

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            Driver(const int32_t &argc, char **argv);
            float filter(float,float*);
            
            virtual ~Driver();

            core::base::ModuleState::MODULE_EXITCODE body();

    };

} // msv

#endif /*DRIVER_H_*/
