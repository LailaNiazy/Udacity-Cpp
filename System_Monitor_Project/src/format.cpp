#include <string>
#include <iostream>
#include "format.h"
#include <iomanip>
using std::string;

 
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long second) { 
   // second = 8000;
    int hours = second/3600;
    int minutes = (second/60)-(hours*60);
    int seconds = second - (minutes*60)-(hours*3600);
    std::stringstream stream;
    stream << std::setw(2) << std::setfill('0') << hours << ":"
           << std::setw(2) << std::setfill('0') << minutes << ":"
           << std::setw(2) << std::setfill('0') << seconds << std::endl;
    return stream.str();}