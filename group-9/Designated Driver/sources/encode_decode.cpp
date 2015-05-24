//Peili Ge

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <iostream>
#include <regex>

     int port;
     std::vector<std::string> explode(std::string const & s, char delim);
int main() {
	//void write_int(int y);
    //For speed and angle
    std::string s = "<0.12,0.45>";
    // Good examples
    std::string s1 = "<0.123,0.456>";
    std::string s2 = "<0.10,0.20>";
    // Bad examples 
    std::string s3 = "<0.1230.456>"; // No comma 
    std::string s4 = "<0>"; // Too short 
    std::string s5 = "<0.1234567890,0.123456789>"; // Too long 
    std::string s6 = "what's up"; // No number 
    std::string s7 = "0.123,0.456"; // No opening or closing '<', '>'

     const int N = s.size();
     //Make sure the recived string of data is in right size. Minimum size is 3. Maximum size is 20
     if (N < 3) {
        std::cout << "Wrong data recived 3" << std::endl;
        return 0;
     } 
     if (N >= 20) {
        std::cout << "Wrong data recived 2 " << std::endl;
        return 0;
     }
     //Make sure the string contans ',' which split 2 datas from speed and angle.
     std::size_t findT = s.find(s);
     findT =s.find( ',');
     if (findT == std::string::npos){
        std::cout << "Wrong data recived 4 " << std::endl;
        return 0;
     }
        std::size_t findA = s.find(s);
     findA =s.find( '<');
     if (findA == std::string::npos){
        std::cout << "Wrong data recived: no openning < " << std::endl;
        return 0;
     }
        std::size_t findB = s.find(s);
     findB =s.find( '>');
     if (findB == std::string::npos){
        std::cout << "Wrong data recived: no ending > " << std::endl;
        return 0;
     }
    char chars[] = "()<>"; // remove < >
    for (unsigned int i = 0; i < strlen(chars); ++i) {
     s.erase (std::remove(s.begin(), s.end(), chars[i]), s.end());
   }

    int i = 0; 
    char tab2[1024];
    strncpy(tab2, s.c_str(), sizeof(tab2));
    //tab2[sizeof(tab2) - 1] = 0;
    while (tab2[i]){
    if ((tab2[i] <= 'z' && tab2[i] >= 'a') || (tab2[i] <= 'Z' && tab2[i] >= 'A')){
       std::cout << "Wrong data recived, it shouldn't be letters" << std::endl;
        return 0;
     }
     else {
     std::vector<std::string> result = explode(s, ',');                   
     std::cout << "Speed:" << result.at(0) << std::endl;
     std::cout << "Angle:" << result.at(1) << std::endl;
 return 0;
     }
   }
}
std::vector<std::string> explode(std::string const & s, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); ) {
        result.push_back(std::move(token));
    }
    return result;
}

// when tesing docoing, comment out the encoding part
void enocding(int s, int a) {
     
     // when connected in Proxy, to print the data received by using:
     cerr << "Speed data: " << vc.getSpeed() << endl;
     cout << "Angle : " << vc.getSteeringWheelAngle()<<endl;
    
    int speedData= (int)vc.getSpeed();/ when connected in Proxy // when connected in Proxy
    int speed = 0.8; // example data to test when is not in proxy
    int angleFromDriver= (int)vc.getSteeringWheelAngle(); // when connected in Proxy
    int angle = 30; // example data to test when is not in proxy
    if (angle < 0) {
        angle = (angle * (-1));
    } else {
        angle = (angle + 90);
    }
        stringstream ss;
        sa << angle;
        convertedAngle=sa.str();
        stringstream ss;
        ss << speed;
        convertedSpeed=ss.str();
        int L = convertedAngle.size() + convertedSpeed.size();

    cout << "<" << L << ":" << convertedAngle << "," << convertedSpeed << ">" <<endl;
   return 0;
} 
