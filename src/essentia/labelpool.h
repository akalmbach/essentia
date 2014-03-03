#ifndef ESSENTIA_LABELPOOL_H
#define ESSENTIA_LABELPOOL_H

#include "types.h"
#include "threading.h"
#include "utils/tnt/tnt.h"
#include "essentiautil.h"
#include "pool.h"
#include "3rdparty/jsonxx/jsonxx.h"

using namespace std;

namespace essentia {
  class LabelPool : public Pool {
    public:
      void setVocab(string filename);
      
      virtual void add(const std::string& name, const Real& value, bool validityCheck = false);
      virtual void add(const std::string& name, const std::vector<Real>& value, bool validityCheck = false);
      
    private:
      void add(const std::string& name);
      int nearestNeighborEuclidean(vector<Real> sample, vector<vector<Real> > vocab);
      int addVocabNamespace(string ns);
      map<string,vector<vector<Real> > > _vocab_map;
      map<string,bool> _fresh_fields;
      jsonxx::Object _json_object;
  };
}

#endif // ESSENTIA_LABELPOOL_H

