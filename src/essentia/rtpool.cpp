#include "rtpool.h"

using namespace std;

namespace essentia {
  void RTPool::add(const std::string& name, const Real& value, bool validityCheck){
    if (os != 0) {
      *os << name << ":" << value << endl;
    }
    Pool::add(name, value, validityCheck);
  }
  
  void RTPool::add(const std::string& name, const std::vector<Real>& value, bool validityCheck){
    if (os != 0) {
      *os << name << ":";
      for (int i = 0; i < value.size(); i++) *os << value[i] << ",";
      *os << endl;
    }
    Pool::add(name, value, validityCheck);
  }
  
  void RTPool::add(const std::string& name, const std::string& value, bool validityCheck){
    if (os != 0) {
      *os << name << ":" << value << endl;
    }
    Pool::add(name, value, validityCheck);
  }
  
  void RTPool::add(const std::string& name, const std::vector<std::string>& value, bool validityCheck){
    if (os != 0) {
      *os << name << ":";
      for (int i = 0; i < value.size(); i++) *os << value[i] << ",";
      *os << endl;
    }
    Pool::add(name, value, validityCheck);
  }
}
