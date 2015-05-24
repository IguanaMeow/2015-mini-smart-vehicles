#ifndef _H_OVERTAKING
#define _H_OVERTAKING

#include <iostream>
namespace msv {

	class Overtaking {
		public:
			Overtaking();
			void doOvertaking(float, float, float, float, double&);	
			bool hasObstacle(float, float);
			int getOvertakingState();
			bool overtaking_start(float);
			bool overtaking_uk_mode(float);
			bool overtaking_turn_right(float, float, float);
			bool overtaking_straighten(float);
			bool overtaking_finish(float);
		private:
			int overtaking_state;
			int curveType;
			float distance;
	};

}

#endif
