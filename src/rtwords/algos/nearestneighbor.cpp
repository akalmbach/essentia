#include "nearestneighbor.h"
#include "essentiamath.h"

using namespace std;

namespace essentia {
namespace standard {

const char* NearestNeighbor::name = "NearestNeighbor";
const char* NearestNeighbor::description = DOC("Given a vocabulary and an input sample, returns the label of the closest sample in the vocabulary to the input sample");

Real NearestNeighbor::getDistance(vector<Real> s1, vector<Real> s2){
  // TODO: Use FLANN or something else smarter here
  if (s1.size() != s2.size()) {
    throw EssentiaException("NearestNeighbor: Vocab sample and input sample must have the same size");
  }
  
  Real dist = 0;
  Real diff;
  for (unsigned int i = 0; i < s1.size(); i++) {
	  diff = s1[i] - s2[i];
	  dist += (diff * diff);
  }
  
  return dist;
}


void NearestNeighbor::compute() {
  const vector<Real>& input = _sample.get();
  int& output = _label.get();

  Real bestDist = -1;
  int bestLabel = -1;
  Real curDist;
  for (unsigned int i = 0; i < _vocab.size(); i++) {
    curDist = getDistance(input, _vocab[i]);
    if (curDist < bestDist || bestDist < 0) {
	  bestDist = curDist;
      bestLabel = i;
    }
  }
  if (bestLabel > 0) {
	output = bestLabel; 
  }
  else {
    throw EssentiaException("NearestNeighbor: No label was ever chosen");
  }
}

} // namespace standard
} // namespace essentia

namespace essentia {
namespace streaming {

const char* NearestNeighbor::name = standard::NearestNeighbor::name;
const char* NearestNeighbor::description = standard::NearestNeighbor::description;

Real NearestNeighbor::getDistance(vector<Real> s1, vector<Real> s2){
  // TODO: Use FLANN or something else smarter here
  if (s1.size() != s2.size()) {
    throw EssentiaException("NearestNeighbor: Vocab sample and input sample must have the same size");
  }
  
  Real dist = 0;
  Real diff;
  for (unsigned int i = 0; i < s1.size(); i++) {
	  diff = s1[i] - s2[i];
	  dist += (diff * diff);
  }
  
  return dist;
}

AlgorithmStatus NearestNeighbor::process() {

  AlgorithmStatus status = acquireData();

  if (status != OK) {
	// if shouldStop is true, that means there is no more audio coming
    if (!shouldStop()) return status;
    return NO_INPUT;
  }


  vector<Real> input = _sample.lastTokenProduced();
  vector<int>& output = _label.tokens();

  Real bestDist = -1;
  int bestLabel = -1;
  Real curDist;
  for (unsigned int i = 0; i < _vocab.size(); i++) {
    curDist = getDistance(input, _vocab[i]);
    if (curDist < bestDist || bestDist < 0) {
	  bestDist = curDist;
      bestLabel = i;
    }
  }
  if (bestLabel > 0) {
	output.push_back(bestLabel); 
  }
  else {
    throw EssentiaException("NearestNeighbor: No label was ever chosen");
  }

  releaseData();

  return OK;
}

} // namespace streaming
} // namespace essentia
