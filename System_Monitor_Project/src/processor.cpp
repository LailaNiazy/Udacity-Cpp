#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>
#include "unistd.h"
#include <iostream>
using std::string;
using std::vector;
using std::stof;

//Constructor for the processor
Processor::Processor(){
    prevTotal_ = LinuxParser::Jiffies();
    prevActive_= LinuxParser::ActiveJiffies();
}

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    long curActive = LinuxParser::ActiveJiffies();
    long curTotal = LinuxParser::Jiffies();
    float CPU_Percentage = float(curActive - prevActive_)/(curTotal - prevTotal_);
    prevTotal_ = curTotal;
    prevActive_ = curActive;
    return CPU_Percentage;}