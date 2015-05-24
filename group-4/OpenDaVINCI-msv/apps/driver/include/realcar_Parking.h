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
            bool canBackRight(float,float);
            bool canGoBack(float);
            bool canForwardRight(float);
            bool stop(float);
            //bool stop(float, float);
    };
}
#endif /*PARKING_H_*/
