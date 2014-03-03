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
  string vocabfilename;
  if (argc == 2) {
	  vocabfilename = string(argv[1]);
  }
  else {
    cerr << "Please enter a filename to use as the vocab" << endl;
    return -1;
  }
  
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  SourceBase& source = audio->output("signal");
  connect(audio->output("time"), NOWHERE);

  RTLowlevelDescriptors *lowlevel = new RTLowlevelDescriptors();
  LabelPool pool;
  pool.setVocab(vocabfilename);
  lowlevel->pool = pool;
  lowlevel->createNetwork(source);
  
  Network network(audio, true);
  cout << "Running network" << endl;
  network.run();

  return 0;
}
