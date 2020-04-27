#include "processor.h"
#include "linux_parser.h"




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