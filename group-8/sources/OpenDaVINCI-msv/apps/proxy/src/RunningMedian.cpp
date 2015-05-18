#include "RunningMedian.h"

RunningMedian::RunningMedian():
	circBuffer(),
	sortedArray()
	{}
	
void RunningMedian::addValue(uint16_t value){
	int i,j;
	bool notReplaced = true;
	uint16_t finalNum = circBuffer[BUFFSIZE -1];
	for(i = (BUFFSIZE-1); i > 0; --i){
		if(sortedArray[i] == finalNum && notReplaced){
			for(j = i; j > 0; --j){
				sortedArray[j] = sortedArray[j-1];
			}
			notReplaced = false;		
		}
		circBuffer[i] = circBuffer[i-1];
	}
	circBuffer[0] = value;
	sortedArray[0] = value;
}

uint16_t RunningMedian::getMedian(){
		int i = 1;
	uint16_t tmp;
	if(!(sortedArray[0] >= sortedArray[1])){
		tmp = sortedArray[0]; 
	 	while(i < BUFFSIZE && sortedArray[i] != tmp) {
	 		 //cout << "test " << sortedArray[0] << " : ";
	 		 //cout << sortedArray[i] << endl;
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