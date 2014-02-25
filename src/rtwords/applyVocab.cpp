#include <iostream>
#include <sstream>
#include <ifstream>

#include "essentia/scheduler/network.h"
#include "essentia/streaming/sourcebase.h"
#include "essentia/streaming/streamingalgorithm.h"
#include "essentia/streaming/algorithms/poolstorage.h"
#include "essentia/essentiautil.h"
#include "essentia/essentia.h"
#include "rtlowleveldescriptors.h"
#include "3rdparty/jsonxx/jsonxx.h"

using namespace std;
using namespace jsonxx;
using namespace essentia;
using namespace streaming;
using namespace scheduler;

int main(int argc, char* argv[]) {
  // Load the whole vocab file into a string
  ifstream vocabfile(argv[1], ios::in, ios::binary);
  if (vocabfile) {
    string vocabstring;
    vocabfile.seekg(0, ios::end);
    vocabstring.resize(vocabfile.tellg());
    vocabfile.seekg(0, ios::beg);
    vocabfile.read(&vocabstring[0], vocabstring.size());
    vocabfile.close();
  }
  else {
    cerr << "Could not load file " << argv[1] << endl;
    return -1;
  }
  
  Object o;
  o.parse(vocabstring);
    
  
  // Init the network
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  SourceBase& source = audio->output("signal");
  connect(audio->output("time"), NOWHERE);

  RTLowlevelDescriptors *lowlevel = new RTLowlevelDescriptors();
  lowlevel->createNetwork(source);
  
  
  // Run the network
  Network network(audio, true);
  network.run();

  return 0;
}
