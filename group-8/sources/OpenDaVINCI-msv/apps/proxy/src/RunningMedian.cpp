#include "RunningMedian.h"

RunningMedian::RunningMedian():
	circBuffer(),
	sortedArray()
	{}

/*---Add a value to the sliding window and sorted array---*/

void RunningMedian::addValue(uint16_t value){
	int i,j;
	bool notReplaced = true;
	uint16_t finalNum = circBuffer[BUFFSIZE -1];

	//Iterate through circBuffer to move all elements up one place
	for(i = (BUFFSIZE-1); i > 0; --i){
		//remove old value from sorted array and close the gap
		if(sortedArray[i] == finalNum && notReplaced){
			for(j = i; j > 0; --j){
				sortedArray[j] = sortedArray[j-1];
			}
			notReplaced = false;		
		}
		circBuffer[i] = circBuffer[i-1];
	}
	//Put new value at start of both
	circBuffer[0] = value;
	sortedArray[0] = value;
}

/*---"Insert" the new value into the sorted array using a kind of 
	insertion sort then redurn the median---*/

uint16_t RunningMedian::getMedian(){
	int i = 1;
	uint16_t tmp;

	/*---insert the first element of sort list in place (rest can be assumed to be sorted), 
		making room by pushing greater values back one place---*/

	if(!(sortedArray[0] >= sortedArray[1])){
		tmp = sortedArray[0]; 
	 	while(i < BUFFSIZE && sortedArray[i] != tmp) {
	 		if(tmp > sortedArray[i]){
				break;
			}else{
				sortedArray[i-1] = sortedArray[i];
			}
			++i;
	 	}
	 	sortedArray[i - 1] = tmp;
	}
 	return sortedArray[BUFFSIZE/2];
}