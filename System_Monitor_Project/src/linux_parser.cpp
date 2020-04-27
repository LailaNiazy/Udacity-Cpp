#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <typeinfo>
#include <unistd.h>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          filestream.close();
          return value;
        }
      }
    }
  }
  return value;
}

// read the version of kernel from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version, line = {};
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    //made minor change
    linestream >> os >> version >> kernel; 
    stream.close();
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids = {};
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
//total used memory = Memtotal - (MemFree + Buffers)
float LinuxParser::MemoryUtilization() { 
  float memory[4] = {0,0,0,0};
  string name, line = {};
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    for(int i = 0; i < 4;i++){
      std::getline(stream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> memory[i];}
      float used = memory[0]-(memory[1]+memory[3]);
      stream.close();
      return used/memory[0];}
  return 0;}

//Read and return the system uptime in seconds
long LinuxParser::UpTime(){ 
  long uptime = 0.0;
  string line, space = {};
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    stream.close();}
  return uptime;}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpu = CpuUtilization();
  long jiffies = 0.0;
  for(int i = 0; i < 10; i++){
    jiffies += stol(cpu[i]);
  }
  return jiffies;}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  long utime, stime, cutime, cstime = 0.0;
  string value, line = {};
   std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 13; i++) {
      linestream >> value;
    }
    linestream >> utime >> stime >> cutime >> cstime; 
    stream.close();
  }
  return (utime+stime+cutime+cstime);}

//read and return the start time of a process
long LinuxParser::StartTime(int pid) {  
  string value, line = {};
  long starttime =0.0;
   std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++) {
      linestream >> value;
    }
    linestream >> starttime;
    stream.close(); 
  }
  return starttime;}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  //user + nice + system + irq + softirq + steal
  return Jiffies() - IdleJiffies();}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> cpu = CpuUtilization();
  //idle + iowait
  return (stol(cpu[3]) + stol(cpu[4]));}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string key, value, line = {};
  vector<string> cpu = {};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    if (key == "cpu"){
      for(int i = 0; i <= 9; i ++){
        linestream >> value;
        cpu.push_back(value);
      }
    stream.close();
    return cpu;
    } 
  }
  return cpu;}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, line ={};
  int value = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes"){
          stream.close();
          return value;}}}}
  return value;}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key, line = {};
  int value = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)){
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running"){
          stream.close();
          return value;}}}}
  return value;}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {  
  string cmd, line = {};
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
    stream.close();}
  return cmd;}

//Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line,key = {}; 
  string ram = {};
  int value = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "VmSize"){
          value /= 1000;
          ram = to_string(value);
          stream.close();
          return ram;
        }
      }
    }
  }
  return ram;}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line = {}; 
  string uid, key = {};
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> key >> uid){
        if(key == "Uid"){
          stream.close();
          return uid;
        }
      }
    }
  }
  return uid;}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string line, x, user, key = {};
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> user >> x >> key){
        if(key == uid){
          stream.close();
          return user;
        }
      }
    }
  }
  return user;}

//Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line = {};
  long time = 0.0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    for(int i = 0; i < 22; i++){
        std::istringstream linestream(line);
        linestream >> time;
    }
    time = time/sysconf(_SC_CLK_TCK);
    stream.close();
  }
  return time;}