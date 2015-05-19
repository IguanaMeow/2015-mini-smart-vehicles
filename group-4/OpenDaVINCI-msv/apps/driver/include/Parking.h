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
            bool hasGap(float, float, float);
            bool canTurnRight(float, float,float);
            bool canTurnLeft(float,float);
            bool stop(float,float);
    };
}
#endif /*PARKING_H_*/
