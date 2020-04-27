#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include "linux_parser.h"
#include "process.h"

using std::string;


// Constructor
Process::Process(int p){ 
    pid_ = p;
    cmd_ = LinuxParser::Command(Pid());
    user_ = LinuxParser::User(Pid());
    }

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
    //uptime of the system
    long upTime = LinuxParser::UpTime();
    long total_time = LinuxParser::ActiveJiffies(pid_);
    long start_time = LinuxParser::StartTime(pid_);
    float seconds = upTime - (start_time / sysconf(_SC_CLK_TCK));
return ((total_time /sysconf(_SC_CLK_TCK) ) / seconds);}

// Return the command that generated this process
string Process::Command() { return cmd_; }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid());}

// Return the user (name) that generated this process
string Process::User() {return user_;}

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid());}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return (this->CpuUtilization() > a.CpuUtilization()); }