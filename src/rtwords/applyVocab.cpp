#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "essentia/scheduler/network.h"
#include "essentia/streaming/sourcebase.h"
#include "essentia/streaming/streamingalgorithm.h"
#include "essentia/streaming/algorithms/poolstorage.h"
#include "essentia/essentiautil.h"
#include "essentia/essentia.h"
#include "essentia/labelpool.h"
#include "rtlowleveldescriptors.h"
#include "3rdparty/jsonxx/jsonxx.h"

using namespace std;
using namespace essentia;
using namespace streaming;
using namespace scheduler;


int main(int argc, char* argv[]) {
  bool computeMeans = false;
  if (argc > 1 && string(argv[1]).compare("--computeMeans") == 0) {
    if (argc < 4){
      cout << "Not enough arguments" << endl;
      return -1;
    }
    else {
      computeMeans = true;
    }
  }

  
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  SourceBase& source = audio->output("signal");
  connect(audio->output("time"), NOWHERE);

  RTLowlevelDescriptors *lowlevel = new RTLowlevelDescriptors();
  lowlevel->createNetwork(source);
  LabelPool pool;
  pool.setVocab(argv[1]);
  lowlevel->pool = pool;
  
  Network network(audio, true);
  network.run();

  return 0;
}
