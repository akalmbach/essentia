#ifndef ESSENTIA_POOLMATAGGREGATOR_H
#define ESSENTIA_POOLMATAGGREGATOR_H

#include <set>
#include <opencv/cv.h>
#include "algorithm.h"
#include "pool.h"

namespace essentia {
namespace standard {

class PoolMatAggregator : public Algorithm {

 protected:
  Input<Pool> _input;
  Output<cv::Mat> _output;
  
  std::vector<std::string> _fields;
  
  int getColsForField(const Pool& input, std::string field);
  int getNumSamples(const Pool& input);

 public:
  PoolMatAggregator() {
    declareInput(_input, "input", "the input pool");
    declareOutput(_output, "output", "a Mat where each row is a vector from the pool");
  }

  void declareParameters() {
	declareParameter("fields", "The names of the fields to be concatenated in each sample",
		"", std::vector<std::string>());
	//declareParameter("matType", "the OpenCV type for the Mat to be created.", CV_32FC1);
  }

  void compute();
  void configure();

  static const char* name;
  static const char* description;

};

} // namespace standard
} // namespace essentia
#endif //ESSENTIA_POOLMATAGGREGATOR_H
