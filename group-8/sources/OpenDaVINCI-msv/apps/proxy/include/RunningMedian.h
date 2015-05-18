#include <stdint.h>

#ifndef RUNNING_MEDIAN_H_
#define RUNNING_MEDIAN_H_

    using namespace std;

    class RunningMedian{
        public:
	       
	        RunningMedian();
	        void addValue(uint16_t value);
			uint16_t getMedian();

        private:
        	static const int BUFFSIZE = 5;
   			uint16_t circBuffer[BUFFSIZE];
			uint16_t sortedArray[BUFFSIZE];


    };


#endif /*RUNNING_MEDIAN_H_*/