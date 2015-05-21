#include <iostream>
#include <string>
#include <stdint.h> 
#include <sstream> 
#include "Netstring.h"

using namespace::std;

string encodeNetstring(string payload) {
    unsigned long len = payload.length();
    if(!len) return "ERROR_EMPTY_STRING";
    std::stringstream ss;   //Convert len to string
    ss << len;
    string s= ss.str();     
    string netstring = s + ":" + payload + ",";
    
    return netstring;
}

string decodeNetstring(string netstring) {
    if (netstring.length() < 3) return "NETSTRING_ERROR"/*_TOO_SHORT"*/;
    
    long semicolonIndex = netstring.find(':');
    if (semicolonIndex < 0) return "NETSTRING_ERROR"/*_NO_COLON"*/;
    
    string getLength = netstring.substr(0, semicolonIndex);
    unsigned int payloadLength; 
    stringstream convert(getLength.c_str()); //Convert string to int
    if (!(convert >> payloadLength))       //give the value to payloadLength using the values in the string
        payloadLength = 0;               // if fails set value to zero  

    if (payloadLength < 1) return "NETSTRING_ERROR"/*_LEADING_ZERO"*/;
    
    string payload = netstring.substr(semicolonIndex + 1);
    if (!payload.length()) return "NETSTRING_ERROR"/*_NO_LENGTH"*/;
    
    if (payload.substr(payload.length() -1) == ",") payload.erase(payload.length()-1); //remove the comma
    
    if (payload.length() != payloadLength) return "NETSTRING_ERROR"/*_INVALID_LENGTH"*/;
      
    return payload;
}

