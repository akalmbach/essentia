#ifndef ESSENTIA_STREAMING_VISUALALGORITHM_H
#define ESSENTIA_STREAMING_VISUALALGORITHM_H

#include <opencv/cv.h>
#include "streamingalgorithm.h"

using namespace cv;

namespace essentia {
namespace streaming {
	

class VisualAlgorithm : public Algorithm {

protected:
  Sink<Mat> _image_in_1;
  Sink<Mat> _image_in_2;
  Sink<Real> _signal;
  Source<Mat> _image_out;
   
 public:
  VisualAlgorithm(){
    declareInput(_image_in_1, 1, "Image In 1", "");
	declareInput(_image_in_2, 1, "Image In 2", "");
	declareInput(_signal, "Image In 1", "");
	declareOutput(_image_out, 1, "Image Out", "");
  };
  
  Mat & (*proc)(const Mat& f1, const Mat& f2, const vector<Real>& signal);


  void declareParameters() { 
	//declareParameter("autoconnect", "Connect to system input/output defaults automatically", "", true);  
  }

  AlgorithmStatus process();
  void configure();

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia
#endif //ESSENTIA_STREAMING_VISUALALGORITHM_H
