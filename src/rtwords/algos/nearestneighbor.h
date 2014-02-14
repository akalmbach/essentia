#ifndef ESSENTIA_STREAMING_NEARESTNEIGHBOR_H
#define ESSENTIA_STREAMING_NEARESTNEIGHBOR_H

#include "algorithm.h"

using namespace std;

namespace essentia {
namespace standard {

class NearestNeighbor : public Algorithm {

 protected:
  Input<vector<Real> > _sample;
  Output<int> _label;

  vector<vector<Real> > _vocab;
  
  Real getDistance(vector<Real> s1, vector<Real> s2);
  
 public:
  NearestNeighbor() {
    declareInput(_sample, "sample", "the input samples");
    declareOutput(_label, "label", "the closest label from the vocabulary");
  }
  
  ~NearestNeighbor() {}


  void declareParameters() {
    declareParameter("vocab", "the set of samples from which to extract the labels", "", vector<vector<Real> >());
  }

  void configure() {
    _vocab = parameter("vocab").toVectorVectorReal();
  }

  void compute();

  static const char* name;
  static const char* description;

};

} // namespace standard
} // namespace essentia




#include "streamingalgorithm.h"

namespace essentia {
namespace streaming {

class NearestNeighbor : public Algorithm {
 protected:
  Sink<vector<Real> > _sample;
  Source<int> _label;
  
  vector<vector<Real> > _vocab;
  Real getDistance(vector<Real> s1, vector<Real> s2);

 public:
  NearestNeighbor() : Algorithm() {
    declareInput(_sample, "sample", "the input samples");
    declareOutput(_label, "label", "the closest label from the vocabulary");
  }

  ~NearestNeighbor() {}

  AlgorithmStatus process();

  void declareParameters() {
    declareParameter("vocab", "the set of samples from which to extract the labels", "", vector<vector<Real> >());
  }

  void configure() {
    _vocab = parameter("vocab").toVectorVectorReal();
  }

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_STREAMING_MONOMIXER_H
