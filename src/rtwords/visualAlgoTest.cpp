#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>

#include "essentia/scheduler/network.h"
#include "essentia/streaming/streamingalgorithm.h"
#include "essentia/essentiautil.h"
#include "essentia/essentia.h"

using namespace std;
using namespace jsonxx;
using namespace essentia;
using namespace streaming;

int main(int argc, char *argv[]) {
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  Algorithm* visual = factory.create("VisualAlgorithm", 
  
  return 0;
}

