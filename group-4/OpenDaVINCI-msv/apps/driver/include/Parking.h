#ifndef PARKING_H_
#define PARKING_H_

namespace msv {
    class Parking
    {
        public:
            Parking();
            void doParking(double&, double&, float[]);
        private:
            int step;
            bool hasGap(float, float);
            bool canTurnRight(float);
            bool canTurnLeft(float,float);
            bool goForward(float);
            bool stop(float,float,float);
    };
}
#endif /*PARKING_H_*/
