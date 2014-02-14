#include <iostream>
#include <sstream>
#include <fstream>

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
  // Load the vocab
  
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
