/*
 * ChibiOS/RT Application Layer for Miniature Cars
 * Copyright (C) 2013 - 2015 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "DiscoveryBoard.h"

#include "Example/RPNCalculator.h"

///////////////////////////////////////////////////////////////////////////////
// Data structures and configuation.
///////////////////////////////////////////////////////////////////////////////

DataT myData;
static int8_t dataFeed = 0;

///////////////////////////////////////////////////////////////////////////////
// Interface methods.
///////////////////////////////////////////////////////////////////////////////

void initializeUserProtocol(void) {
    int i = 0;
    int bufferLength = 1024;
    for (i = 0; i < bufferLength; i++) {
        myData.payload[i] = 0;
        myData.length = 0;
    }
}

void consumeDataFromHost(DataT *ptrToDataFromHost) {
    if (ptrToDataFromHost != NULL) {
        if (ptrToDataFromHost->length == 1) {
            int value = 0;
            rsscanf(ptrToDataFromHost->payload, "%d", &value);
            dataFeed = value;
            prepareDataFeed();
        } else if (ptrToDataFromHost->length > 3) {
            if (ptrToDataFromHost->payload[0] == '[' && ptrToDataFromHost->payload[ptrToDataFromHost->length - 1] == ']') {
                if (ptrToDataFromHost->payload[1] == 'V' && ptrToDataFromHost->payload[2] == 'A') {
                    int i = 0;
                    int speed = 0;
                    int direction = 0;
                    int value = 0;
                    for (i = 4; i < ptrToDataFromHost->length; ++i) {
                        if (ptrToDataFromHost->payload[i] >= '0' && ptrToDataFromHost->payload[i] <= '9') {
                            if (value == 0) {
                                speed = (speed*10) + (ptrToDataFromHost->payload[i] - '0');
                            }
                            else if (value == 1) {
                                direction = (direction*10) + (ptrToDataFromHost->payload[i] - '0');
                            }
                        }
                        else if (ptrToDataFromHost->payload[i] == ';') {
                            ++value;
                        } else if (ptrToDataFromHost->payload[i] == ')') {
                            break;
                        }
                    }
                    setMotorData(direction,speed);
                }
            }
        }
    }
}

void produceDataForHost_AfterConsumption(DataT *ptrToDataForHost, int maxBufferLength) {
    int i = 0;
    if (ptrToDataForHost != NULL && maxBufferLength > 0) {
        // Copy data to send buffer.
        if (myData.length < maxBufferLength) {
            for(i = 0; i < myData.length; i++) {
                ptrToDataForHost->payload[i] = myData.payload[i];
            }

            ptrToDataForHost->length = myData.length;
        }
    }
}

void prepareDataFeed(void) {
    char buffer[50];
    int i = 0;
    myData.length = 0;

    if (dataFeed & ONBOARD_ACCELEROMETER_FEED) {
        OnboardAccelerometerDataT accel;
        accel.x = accel.y = accel.z = 0;

        getOnboardAccelerometerData(&accel);

        chsprintf(buffer, "[OA(%d;%d;%d)]$", accel.x, accel.y, accel.z);

        for(i = 0; i < 50; i++) {
            if (buffer[i] != '$')
                myData.payload[myData.length + i] = buffer[i];
            else
                break;
        }
        myData.length += i;
    }

    if (dataFeed & ONBOARD_TEMPERATURE_FEED) {
        TemperatureDataT temp;
        temp.T = 0;

        getTemperatureData(&temp);
        
        chsprintf(buffer, "[T(%d)]$", temp.T);

        for(i = 0; i < 50; i++) {
            if (buffer[i] != '$')
                myData.payload[myData.length + i] = buffer[i];
            else
                break;
        }
        myData.length += i;
    }

    if (dataFeed & INFRARED_FEED) {
        InfraredDataT IR0, IR1, IR2;
        IR0.address = 0;
        IR0.distance = 0;
        IR0.next = &IR1;

        IR1.address = 0;
        IR1.distance = 0;
        IR1.next = &IR2;

        IR2.address = 0;
        IR2.distance = 0;
        IR2.next = NULL;

        getInfraredData(&IR0);
        
        chsprintf(buffer, "[IR((%d;%d);(%d;%d);(%d;%d))]$", IR0.address, IR0.distance,
                                                            IR1.address, IR1.distance,
                                                            IR2.address, IR2.distance);
        for(i = 0; i < 50; i++) {
            if (buffer[i] != '$')
                myData.payload[myData.length + i] = buffer[i];
            else
                break;
        }
        myData.length += i;
    }

    if (dataFeed & ULTRASONIC_FEED) {
        UltrasonicDataT US0, US1, US2;
        US0.address = 0;
        US0.distance = 0;
        US0.next = &US1;

        US1.address = 0;
        US1.distance = 0;
        US1.next = &US2;

        US2.address = 0;
        US2.distance = 0;
        US2.next = NULL;

        getUltrasonicData(&US0);
        
        chsprintf(buffer, "[US((%d;%d);(%d;%d);(%d;%d))]$", US0.address, US0.distance,
                                                            US1.address, US1.distance,
                                                            US2.address, US2.distance);
        for(i = 0; i < 50; i++) {
            if (buffer[i] != '$')
                myData.payload[myData.length + i] = buffer[i];
            else
                break;
        }
        myData.length += i;
    }

    if (dataFeed & WHEELENCODER_FEED) {
        WheelEncoderDataT WC;
        WC.drivenDistanceLeftWheel = 0;
        WC.drivenDistanceRightWheel = 0;
        WC.speedLeftWheel = 0;
        WC.speedRightWheel = 0;

        getWheelEncoderData(&WC);

        /*if (WC.speedRightWheel > 8 || WC.speedRightWheel < -8) {
            setCutSpeed(1);
        } else {
            setCutSpeed(0);
        }*/
        
        chsprintf(buffer, "[WC(%d;%d;%d;%d)]$", WC.drivenDistanceLeftWheel,
                                                WC.drivenDistanceRightWheel,
                                                WC.speedLeftWheel,
                                                WC.speedRightWheel);
        for(i = 0; i < 50; i++) {
            if (buffer[i] != '$')
                myData.payload[myData.length + i] = buffer[i];
            else
                break;

        }

        myData.length += i;
    }

    if (0) {
        int8_t data[3];
        getRCReceiverData(data);

        chsprintf(buffer, "[RC(%d;%d)]$", data[0], data[1]);
        for(i = 0; i < 50; i++) {
            if (buffer[i] != '$')
                myData.payload[myData.length + i] = buffer[i];
            else
                break;

            }
        myData.length += i;
    }
}

void produceDataForHost_WithoutConsumption(DataT *ptrToDataForHost, int maxBufferLength) {
    if (ptrToDataForHost != NULL && maxBufferLength > 0) {
        myData.length = 0;

    	dataFeed = INFRARED_FEED | ULTRASONIC_FEED | WHEELENCODER_FEED;
        prepareDataFeed();

        myData.payload[myData.length] = '\0';

        // Copy data to send buffer.
        if (myData.length < maxBufferLength) {
            int i = 0;
            for(i = 0; i < myData.length; i++) {
                ptrToDataForHost->payload[i] = myData.payload[i];
            }

            ptrToDataForHost->length = myData.length;
        }
    }
}

