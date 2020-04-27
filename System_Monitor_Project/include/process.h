#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int p);
  Process();
  int Pid();                               
  std::string User();                      
  std::string Command();                   
  float CpuUtilization()  const;                  
  std::string Ram();                       
  long int UpTime();                       
  bool operator<(Process const& a) const;  
  
  // Declare any necessary private members
 private:
    int pid_ = 0;
    std::string cmd_ = {};
    std::string user_ = {};
};

#endif