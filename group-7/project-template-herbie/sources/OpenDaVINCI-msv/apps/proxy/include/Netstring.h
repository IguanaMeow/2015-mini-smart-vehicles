#ifndef NETSTRING_H_ 
#define NETSTRING_H_

#include <iostream>
#include <string>

using namespace std; 

string encodeNetstring(string payload);
string decodeNetstring(string netstring);

#endif
