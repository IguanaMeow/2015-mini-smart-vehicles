//arthur Fredric Eidsvik
#include <stdio.h>
#include <stdlib.h>

        char sens1[10],sens2[10],sens3[10],sens4[10], sens5[10];
        int s1,s2,s3,s4,s5, totalSens1, totalSens2, totalSens3, totalSens4, totalSens5;
        char sum[10];
        char inData[15] = "";
        int k = 0;
        while (inData[k] != ':' && k < 3) {
            sum[k] = inData[k];
            k++;
        }
        int result = atoi(sum);
    
        // double checkSum = getCheckSum(inData);
        int checkSum = result;
        /*if (checkSum > 99){*/
        sens1[0] = inData[4]; 
        sens1[1] = inData[5];
        sens2[0] = inData[6];
        sens2[1] = inData[7];
        sens3[0] = inData[8];
        sens3[1] = inData[9];
        sens4[0] = inData[10];
        sens4[1] = inData[11];
        sens5[0] = inData[12];
        sens5[1] = inData[13];
        /*} else {
        sens1[0] = inData[3];
        sens1[1] = inData[4];
        sens2[0] = inData[5];
        sens2[1] = inData[6];
        sens3[0] = inData[7];
        sens3[1] = inData[8];
        sens4[0] = inData[9];
        sens4[1] = inData[10];
        sens5[0] = inData[11];
        sens5[1] = inData[12];
    }*/
    s1 = atoi(sens1);
    s2 = atoi(sens2);
    s3 = atoi(sens3);
    s4 = atoi(sens4);
    s5 = atoi(sens5);
    totalSens1 = s1;
    totalSens2 = totalSens1 + s2;
    totalSens3 = totalSens2 + s3;
    totalSens4 = totalSens3 + s4;
    totalSens5 = totalSens4 + s5;
    if (totalSens5 == checkSum) {
        cout << "good match:" << endl;
        cout << "check:" << checkSum << endl;
        cout << "total:" << totalSens5 << endl;

        sbd.putTo_MapOfDistances(0, s1);
        sbd.putTo_MapOfDistances(1, s2);
        sbd.putTo_MapOfDistances(2, s3);
        sbd.putTo_MapOfDistances(3, s4);
        sbd.putTo_MapOfDistances(4, s5);
        //getConference().send(containerSensorBoardData);
        return 0;
    }
    else{
        cout << "bad match" << endl;
        cout << "check:" << checkSum << endl;
        cout << "total:" << totalSens5 << endl;
        return 0;
    }
