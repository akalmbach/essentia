#include "rtpool.h"
#include "3rdparty/jsonxx/jsonxx.h"

#ifndef ESSENTIA_LABELPOOL_H
#define ESSENTIA_LABELPOOL_H

namespace essentia {

class LabelPool : public RTPool {
  public:
    void setVocab(std::string filename);
};

}

#endif //ESSENTIA_LABELPOOL_H
