#ifndef PARKING_H_
#define PARKING_H_

namespace msv {
    class Parking
    {
        public:
            Parking();
            void doParking(double&, double&, float, float[]);

        private:


            int step;
            float path_gapStart;
            float path_gapEnd;
            float path_middlePark;
            const float carSize;
            const float parkingSpeed;



            bool isInRange(float, int, int);
            bool outsideRange(float, int, int);
            bool hasGap(float, float, float);
            bool hasReady(float);
            bool canBackRight(float);
            bool canBackLeft(float,float);
            bool canGoRight(float);
            bool temp(int);



/*            bool canBackRight(float,float);
            bool canGoBack(float);
            bool canForwardRight(float);
            bool stop(float);*/
    };
}
#endif /*PARKING_H_*/
