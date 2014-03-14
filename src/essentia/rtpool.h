#include "pool.h"

#ifndef ESSENTIA_RTPOOL_H
#define ESSENTIA_RTPOOL_H

namespace essentia {

class RTPool : public Pool {
  public:
    std::ostream* os;
    virtual void add(const std::string& name, const Real& value, bool validityCheck = false);
    virtual void add(const std::string& name, const std::vector<Real>& value, bool validityCheck = false);
    virtual void add(const std::string& name, const std::string& value, bool validityCheck = false);
    virtual void add(const std::string& name, const std::vector<std::string>& value, bool validityCheck = false);
};

}

#endif //ESSENTIA_RTPOOL_H
