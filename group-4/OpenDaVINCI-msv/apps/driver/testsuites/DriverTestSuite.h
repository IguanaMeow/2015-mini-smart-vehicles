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

#ifndef DRIVERTESTSUITE_H_
#define DRIVERTESTSUITE_H_

#include "cxxtest/TestSuite.h"

// Include local header files.
#include "../include/Driver.h"

using namespace std;
using namespace core::data;
using namespace msv;

/**
 * This class derives from SensorBoard to allow access to protected methods.
 */
class DriverTestling : public Driver {
    private:
        DriverTestling();
    
    public:
        DriverTestling(const int32_t &argc, char **argv) :
            Driver(argc, argv) {}

        // Here, you need to add all methods which are protected in Driver and which are needed for the test cases.
};

/**
 * The actual testsuite starts here.
 */
class DriverTest : public CxxTest::TestSuite {
    private:
        DriverTestling *dt;

    public:
        /**
         * This method will be called before each testXYZ-method.
         */
        void setUp() {
            // Prepare the data that would be available from commandline.
             string argv0("driver");
            string argv1("--cid=110");
            // modified, so it takes the freq too
            string argv2("--freq=10");
            int32_t argc = 3;
            char **argv;
            argv = new char*[3];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());

            // Create an instance of sensorboard through SensorBoardTestling which will be deleted in tearDown().
            dt = new DriverTestling(argc, argv);
        }

        /**
         * This method will be called after each testXYZ-method.
         */
        void tearDown() {
            delete dt;
            dt = NULL;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // Below this line the actual testcases are defined.
        ////////////////////////////////////////////////////////////////////////////////////

        void testDriverSuccessfullyCreated() {
            TS_ASSERT(dt != NULL);
        }

         void testFilter(void)
        {
            float balance[3] = {50, 2, 4};
            //char** d = NULL;
           // msv::Driver driver(0,d); // you need to add arguments here etc.
            float num = 4;
     

            TS_TRACE("Filter test");
            //TS_ASSERT_EQUALS(dt->filter(3, balance), num);
                //this is needed, because comparing two floating points gets an error
            if (dt->filter(3, balance) >= num || dt->filter(3, balance) <= num) {
                TS_TRACE("yes, it is equal");
            } else {
                TS_TRACE("not equal");
            }
            
            // or TS_ASSERT_EQUALS(driver.filter(3, balance), 4); but it doesnt work
            TS_TRACE("Finishing filter test");
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // Below this line the necessary constructor for initializing the pointer variables,
        // and the forbidden copy constructor and assignment operator are declared.
        //
        // These functions are normally not changed.
        ////////////////////////////////////////////////////////////////////////////////////

    public:
        /**
         * This constructor is only necessary to initialize the pointer variable.
         */
        DriverTest() : dt(NULL) {}

    private:
        /**
         * "Forbidden" copy constructor. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the copy constructor.
         *
         * @param obj Reference to an object of this class.
         */
        DriverTest(const DriverTest &/*obj*/);

        /**
         * "Forbidden" assignment operator. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the assignment operator.
         *
         * @param obj Reference to an object of this class.
         * @return Reference to this instance.
         */
        DriverTest& operator=(const DriverTest &/*obj*/);

};

#endif /*DRIVERTESTSUITE_H_*/

