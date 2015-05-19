#include <iostream>
#include "Overtaking.h"
namespace msv {

	using namespace std;

	Overtaking::Overtaking() : overtaking_state(0), curveType(0), distance(0)
	{
		// hello
	}
	
	void Overtaking::doOvertaking(float pathTraveled, 
				      float us_front_right, 
				      float ir_front_right, 
				      float us_front,
				      double &desiredSteeringWheelAngle) {
		if(getOvertakingState() == 0) {
			hasObstacle(us_front, pathTraveled);
			desiredSteeringWheelAngle = desiredSteeringWheelAngle;
		}
		else if (getOvertakingState() == 1) {
			desiredSteeringWheelAngle = -22;
			overtaking_start(pathTraveled);
		}
		else if(getOvertakingState() == 2){
			overtaking_uk_mode(pathTraveled);
			desiredSteeringWheelAngle = 20;
		}							
		else if(getOvertakingState() == 3){
			desiredSteeringWheelAngle = desiredSteeringWheelAngle;
			overtaking_turn_right(pathTraveled, us_front_right, ir_front_right);
		}
		else if(getOvertakingState() == 4) {
			desiredSteeringWheelAngle = 25;
			overtaking_straighten(pathTraveled);
		}
		else if(getOvertakingState() == 5) {
			desiredSteeringWheelAngle = -26;
			overtaking_finish(pathTraveled);
		}
		else
		{
			desiredSteeringWheelAngle = desiredSteeringWheelAngle;
			cout << "Do we get here?" << endl;
		}
	}
	
	int Overtaking::getOvertakingState(){
		return overtaking_state;
	}

	bool Overtaking::hasObstacle(float usfront, float path)
	{
		// usfront > 8 (worked 30% of the time)
		bool success = (usfront < 15) && (usfront > 0); 
		if(success){
			overtaking_state = 1;
			distance = path;
			cout << "step 1, turning left" << endl;
		}
		return success;
	}

	bool Overtaking::overtaking_start(float path) {	
		//cout << "front IR: " << frontirsensor << endl;
		bool success = path > distance + 6; //4.8 
		if (success){
			overtaking_state = 2;
			distance = path;
			cout << "step 2, turning right" << endl;	
		}
		return success;
	}
	
	bool Overtaking::overtaking_uk_mode(float path) {
		bool success = path > distance + 4;
		if(success){
			overtaking_state = 3;
			cout << "step 3, Lane Detection started" << endl;
		}
		return success;
	}
	
	bool Overtaking::overtaking_turn_right(float path, float us_front, float frontirsensor) {
		bool success = (us_front < 0 || us_front > 10) && (frontirsensor < 0); 
		if(success) {
			distance = path;
			overtaking_state = 4;
			cout << "step 4, turning right " << endl;
		}
		return success;
	}
	
	bool Overtaking::overtaking_straighten(float path) {
		bool success = path > distance + 5; //4.8
		if(success) {
			overtaking_state = 5;
			distance = path;
			cout << "step 5, straightening out left " << endl;
		}
		return success;
	}
	
	bool Overtaking::overtaking_finish(float path) {
		bool success = path > distance + 4; //4.5
		if(success){
			overtaking_state = 0;
			cout << "step 0, overtaking finished" << endl;
		}
		return success;
	}
	
	


}
