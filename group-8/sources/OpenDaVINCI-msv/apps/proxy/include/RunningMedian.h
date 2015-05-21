/**
 * Running Median - a (not very portable) class to handle a buffered running median, storing values in a 77
 * 
 *
 */

#include <stdint.h>

#ifndef RUNNING_MEDIAN_H_
#define RUNNING_MEDIAN_H_

    using namespace std;

    class RunningMedian{
    	private:

            RunningMedian(const RunningMedian &/*obj*/);

            RunningMedian& operator=(const RunningMedian &/*obj*/);

        public:
	       	
	       	//Constructor
	        RunningMedian();

	        /*---Add a value to the sliding window and sorted array---*/
	        void addValue(uint16_t value);

	        /*---"Insert" the new value into the sorted array using a kind of 
	        	insertion sort then redurn the median---*/        
			uint16_t getMedian();

        private:
        	//set size of sample
        	static const int BUFFSIZE = 5;
   			uint16_t circBuffer[BUFFSIZE];
			uint16_t sortedArray[BUFFSIZE];


    };


#endif /*RUNNING_MEDIAN_H_*/