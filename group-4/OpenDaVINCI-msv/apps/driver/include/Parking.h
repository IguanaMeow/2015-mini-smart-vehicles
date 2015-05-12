#ifndef PARKING_H_
#define PARKING_H_


namespace msv {

	class Parking{
        public:
            Parking();
            bool hasGap(float, float, float);
            bool canTurnRight(float, float,float);
            bool canTurnLeft(float,float);
            bool stop(float,float);
        private:
            int step;
   
    };

}
#endif /*PARKING_H_*/