/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */

#ifndef MSV_STEERINGDATA_H
#define MSV_STEERINGDATA_H

// core/platform.h must be included to setup platform-dependent header files and configurations.
#include "core/platform.h"


#include "core/data/SerializableData.h"


namespace msv {
	using namespace std;
	
	class SteeringData : public core::data::SerializableData {
		public:
			SteeringData();
	
			virtual ~SteeringData();
	
			/**
			 * Copy constructor.
			 *
			 * @param obj Reference to an object of this class.
			 */
			SteeringData(const SteeringData &obj);
	
			/**
			 * Assignment operator.
			 *
			 * @param obj Reference to an object of this class.
			 * @return Reference to this instance.
			 */
			SteeringData& operator=(const SteeringData &obj);
	
		public:
			/**
			 * @return exampleData.
			 */
			double getExampleData() const;
			
			/**
			 * This method sets exampleData.
			 *
			 * @param val Value for exampleData.
			 */
			void setExampleData(const double &val);
	




//TEST TA BORT!


		public:
			/**
			 * @return speedData.
			 */
			double getSpeedData() const;
			
			/**
			 * This method sets speedData.
			 *
			 * @param val Value for speedData.
			 */
			void setSpeedData(const double &val1);


/*TA BORT!*/


//TEST TA BORT!


		public:
			/**
			 * @return stopData.
			 */
			bool getStopData() const;
			
			/**
			 * This method sets stopData.
			 *
			 * @param val Value for speedData.
			 */
			void setStopData(const bool &val1);


/*TA BORT!*/










		public:
			virtual ostream& operator<<(ostream &out) const;
			virtual istream& operator>>(istream &in);
	
			virtual const string toString() const;
	
		private:
			double m_exampleData;
	     /*TA BORT!*/double m_speedData;
	     /*TA BORT!*/bool stopData;
	};
} // msv

#endif /*MSV_STEERINGDATA_H*/
