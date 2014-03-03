#ifndef ESSENTIA_STREAMING_CVVIDEOREADER_H
#define ESSENTIA_STREAMING_CVVIDEOREADER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "algorithm.h"

using namespace std;
using namespace cv;

namespace essentia {
namespace standard {

class CvVideoReader : public Algorithm {
 
 protected:
  Input<Real> _time;
  Output<Mat> _frame;
  string _filename;
  VideoCapture _video_capture;
  
 public:
  CvVideoReader() {
	declareInput(_time, "time", "the time, in seconds of the video that should be displayed");
    declareOutput(_frame, "frame", "the stream of Mat's coming from the video reader");
  }

  void declareParameters() {
    declareParameter("filename", "the name of the video file to read", "", "in.avi");
  }

  ~CvVideoReader() {}

  void configure();
  void compute();

  static const char* name;
  static const char* description;

};

} // namespace standard
} // namespace essentia


#include "streamingalgorithm.h"

namespace essentia {
namespace streaming {

class CvVideoReader : public Algorithm {

 protected:
  Source<Mat> _frame;
  Sink<Real> _time;
  string _filename;
  VideoCapture _video_capture;
  Real _start_time;

 public:
  CvVideoReader() {
	declareInput(_time, 1, 1, "time", "the time, in seconds of the video that should be displayed");
    declareOutput(_frame, 1, 1, "frame", "the stream of Mat's coming from the video reader");
    _start_time = -1;
  }

  void declareParameters() {
    declareParameter("filename", "the name of the video file to read", "", "in.avi");
  }

  ~CvVideoReader() {}

  void configure();
  AlgorithmStatus process();

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_STREAMING_CV_VIDEOREADER_H
