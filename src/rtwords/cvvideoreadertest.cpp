#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

#include "essentia/essentia.h"
#include "essentia/essentiautil.h"
#include "essentia/algorithmfactory.h"
#include "essentia/essentiamath.h"
#include "essentia/streaming/streamingalgorithm.h"
#include "essentia/streaming/sourcebase.h"
#include "essentia/scheduler/network.h"
#include "essentia/streaming/algorithms/poolstorage.h"
#include "algos/visualalgorithm.h"

using namespace std;
using namespace essentia;
using namespace streaming;
using namespace scheduler;

cv::Mat test(const cv::Mat& f1, const cv::Mat& f2, const vector<Real>& signal) {
  float avg = 0;
  for (int i = 0; i < signal.size(); i++) {
    avg += signal[i];
    cout << signal[i] << endl;
  }
  avg /= (float) signal.size();
  return f1*avg;
}

int main(int argc, char *argv[]) {
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  Algorithm* fc = factory.create("FrameCutter", "frameSize", 2048, "hopSize", 1024, "silentFrames", "noise");
  Algorithm* dy = factory.create("Loudness");
  Algorithm* reader = factory.create("CvVideoReader", "filename", argv[1]);
  Algorithm* imshow = factory.create("CvShow", "window_name", "test", "rate", 40);
  VisualAlgorithm* visual = (VisualAlgorithm *) factory.create("VisualAlgorithm");

  Pool pool;
  
  connect(audio->output("signal"), fc->input("signal"));
  connect(fc->output("frame"), dy->input("signal"));
  connect(dy->output("loudness"), pool, "loudness.loudness");
  
  visual->proc = (* test);
  
  connect(audio->output("time"), reader->input("time"));
  connect(reader->output("frame"), visual->input("Image In 1"));
  connect(reader->output("frame"), visual->input("Image In 2"));
  connect(dy->output("loudness"), visual->input("Signal"));
  connect(visual->output("Image Out"), imshow->input("frame"));
     
  Network network(audio, true);
  network.run();
  
  return 0;
}

