#ifndef ESSENTIA_STREAMING_CVSHOW_H
#define ESSENTIA_STREAMING_CVSHOW_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "algorithm.h"

using namespace std;
using namespace cv;

namespace essentia {
namespace standard {

class CvShow : public Algorithm {

 protected:
  Input<Mat> _frame;
  string _window_name;
  Real _wait;
  int _width, _height;
  Mat resized;

 public:
  CvShow() {
    declareInput(_frame, "frame", "the stream of Mat's to show");
  }

  void declareParameters() {
    declareParameter("window_name", "the name of the window", "", "window");
    declareParameter("rate", "update rate for showing frames", "0-50", 20.0);
    declareParameter("width", "pixel width of the output. -1 uses the width of the input image", "-1-1920", -1);
    declareParameter("height", "pixel height of the output. -1 uses the height of the input image", "-1-1080", -1);
  }

  ~CvShow() {}

  void configure() {
    _window_name = parameter("window_name").toString();
    _wait = 1.0/parameter("rate").toReal();
    _width = parameter("width").toInt();
    _height = parameter("height").toInt();
    if (_width >= 1 && _height >= 1) {
      resized = Mat::zeros(_width, _height, CV_8UC3);
	}
    
    namedWindow(_window_name);
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

class CvShow : public Algorithm {

 protected:
  Sink<Mat> _frame;
  string _window_name;
  Real _wait;
  int _width, _height;
  Mat resized;

 public:
  CvShow() {
    declareInput(_frame, 1, 1, "frame", "the stream of Mat's to show");
  }

  void declareParameters() {
    declareParameter("window_name", "the name of the window", "", "window");
    declareParameter("rate", "update rate for showing frames", "[0,100]", 20.0);
    declareParameter("width", "pixel width of the output. -1 uses the width of the input image", "[-1,1920]", -1);
    declareParameter("height", "pixel height of the output. -1 uses the height of the input image", "[-1-,1080]", -1);
  }

  ~CvShow() {}

  void configure() {
    _window_name = parameter("window_name").toString();
    _wait = 1000.0/parameter("rate").toReal();
    _width = parameter("width").toInt();
    _height = parameter("height").toInt();
    if (_width >= 1 && _height >= 1) {
      resized = Mat::zeros(_width, _height, CV_8UC3);
	}
    
    namedWindow(_window_name);
  }

  AlgorithmStatus process();

  static const char* name;
  static const char* description;

};

} // namespace streaming
} // namespace essentia


#endif // ESSENTIA_STREAMING_CVSHOW_H
