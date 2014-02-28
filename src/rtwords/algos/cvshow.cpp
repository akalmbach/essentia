#include "cvshow.h"

using namespace std;
using namespace cv;
using namespace essentia;

namespace essentia {
namespace standard {

const char* CvShow::name = "CvShow";
const char* CvShow::description = DOC("Given a Mat source this shows it in a window");

void CvShow::compute() {
  const Mat& frame = _frame.get();
  if (_width >= 1 && _height >= 1) {
    resize(frame, resized, resized.size(), 0, 0, INTER_LINEAR);
    imshow(_window_name, resized);
  }
  else {
	imshow(_window_name, frame);
  }
  waitKey(_wait);
  
}

} // namespace standard
} // namespace essentia

namespace essentia {
namespace streaming {

const char* CvShow::name = standard::CvShow::name;
const char* CvShow::description = standard::CvShow::description;

AlgorithmStatus CvShow::process() {

  AlgorithmStatus status = acquireData();
  
  
  if (status != OK) {
    return status;
  }
    
  Mat frame = _frame.firstToken();

  if (_width >= 1 && _height >= 1) {
    resize(frame, resized, resized.size(), 0, 0, INTER_LINEAR);
    imshow(_window_name, resized);
  }
  else {
	imshow(_window_name, frame);
  }
  
  waitKey(_wait);
  
  releaseData();

  return OK;
}


} // namespace streaming
} // namespace essentia
