/*
 * This software is open source. Please see COPYING and AUTHORS for further information.
 *
 * This file is auto-generated. DO NOT CHANGE AS YOUR CHANGES MIGHT BE OVERWRITTEN!
 */


#include "core/base/Hash.h"
#include "core/base/Deserializer.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"

#include "generated/msv/SteeringData.h"

namespace msv {
	using namespace std;
	using namespace core::base;
	
	SteeringData::SteeringData() :
	    SerializableData()
		, m_exampleData(0.0) , m_speedData(0.0), stopData(false)
	{}
	
	SteeringData::SteeringData(const SteeringData &obj) :
	    SerializableData()
		, m_exampleData(obj.m_exampleData), m_speedData(obj.m_speedData), stopData(obj.stopData)

	{}
	
	SteeringData::~SteeringData() {}
	
	SteeringData& SteeringData::operator=(const SteeringData &obj) {
		m_exampleData = obj.m_exampleData;
		m_speedData = obj.m_speedData;
	/*TA BORT!*/	stopData = obj.stopData;
		return (*this);
	}
	
	double SteeringData::getExampleData() const {
		return m_exampleData;
		
	}
	
	void SteeringData::setExampleData(const double &val) {
		m_exampleData = val;
	}



	double SteeringData::getSpeedData() const {
		return m_speedData;
	}
	
	void SteeringData::setSpeedData(const double &val1) {
		m_speedData = val1;
	}



/* TA BORT! */

	bool SteeringData::getStopData() const {
		return stopData;
	}
	
	void SteeringData::setStopData(const bool &val2) {
		stopData = val2;
	}

	
/*TA BORT!*/
	

	
	const string SteeringData::toString() const {
		stringstream s;
	
		s << "ExampleData: " << getExampleData() << " ";
	
		return s.str();
	}
	
	ostream& SteeringData::operator<<(ostream &out) const {
		SerializationFactory sf;
	
		Serializer &s = sf.getSerializer(out);
	
		s.write(CRC32 < CharList<'e', CharList<'x', CharList<'a', CharList<'m', CharList<'p', CharList<'l', CharList<'e', CharList<'D', CharList<'a', CharList<'t', CharList<'a', NullType> > > > > > > > > > >  >::RESULT,
				m_exampleData);

s.write(CRC32 < CharList<'s', CharList<'p', CharList<'e', CharList<'e', CharList<'d', CharList<'i', CharList<'n', CharList<'D', CharList<'a', CharList<'t', CharList<'a', NullType> > > > > > > > > > >  >::RESULT,
				m_speedData);

/*TA BORT*/

s.write(CRC32 < CharList<'s', CharList<'t', CharList<'o', CharList<'p', CharList<'p', CharList<'i', CharList<'n', CharList<'D', CharList<'a', CharList<'t', CharList<'a', NullType> > > > > > > > > > >  >::RESULT,
				stopData);

/*TA BORT*/
		return out;
	}
	
	istream& SteeringData::operator>>(istream &in) {
		SerializationFactory sf;
	
		Deserializer &d = sf.getDeserializer(in);
	
		d.read(CRC32 < CharList<'e', CharList<'x', CharList<'a', CharList<'m', CharList<'p', CharList<'l', CharList<'e', CharList<'D', CharList<'a', CharList<'t', CharList<'a', NullType> > > > > > > > > > >  >::RESULT,
				m_exampleData);

d.read(CRC32 < CharList<'s', CharList<'p', CharList<'e', CharList<'e', CharList<'d', CharList<'i', CharList<'n', CharList<'D', CharList<'a', CharList<'t', CharList<'a', NullType> > > > > > > > > > >  >::RESULT,
				m_speedData);
/*TA BORT*/
d.read(CRC32 < CharList<'s', CharList<'t', CharList<'o', CharList<'p', CharList<'p', CharList<'i', CharList<'n', CharList<'D', CharList<'a', CharList<'t', CharList<'a', NullType> > > > > > > > > > >  >::RESULT,
				stopData);
/*TA BORT*/

		return in;
	}
} // msv
