#include "cvvideoreader.h"

using namespace std;
using namespace cv;
using namespace essentia;

namespace essentia {
namespace standard {

const char* CvVideoReader::name = "CvVideoReader";
const char* CvVideoReader::description = DOC("Reads frames in order from a video file and outputs them as Mats");

void CvVideoReader::configure() {
  _filename = parameter("filename").toString();
  _video_capture.open(_filename);
}

void CvVideoReader::compute() {
  // get a frame from the video reader
  // and put it in the Source.
}

} // namespace standard
} // namespace essentia

namespace essentia {
namespace streaming {

const char* CvVideoReader::name = standard::CvVideoReader::name;
const char* CvVideoReader::description = standard::CvVideoReader::description;

void CvVideoReader::configure() {
  _filename = parameter("filename").toString();
  cout << "Opening video file " << _filename << endl;
  _video_capture.open(_filename);
}

AlgorithmStatus CvVideoReader::process() {
  AlgorithmStatus status = acquireData();
  
  if (status != OK) {
    return status;
  }
  
  // get a frame from the video reader
  // and put it in the Source.
  Mat& output = _frame.firstToken();
  _video_capture >> output;
    
  releaseData();
  
  return OK;
}


} // namespace streaming
} // namespace essentia

