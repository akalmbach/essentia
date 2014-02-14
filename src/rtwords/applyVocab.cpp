/*
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <fstream>

#include "essentia/scheduler/network.h"
#include "essentia/streaming/sourcebase.h"
#include "essentia/streaming/streamingalgorithm.h"
#include "essentia/streaming/algorithms/poolstorage.h"
#include "essentia/essentiautil.h"
#include "essentia/essentia.h"
#include "rtlowleveldescriptors.h"


using namespace std;
using namespace essentia;
using namespace streaming;
using namespace scheduler;


int main(int argc, char* argv[]) {
	
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Pool features;
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  SourceBase& source = audio->output("signal");
  connect(audio->output("time"), NOWHERE);

  RTLowlevelDescriptors *lowlevel = new RTLowlevelDescriptors();
  lowlevel->createNetwork(source, features);
   
  for (int i = 0; i < lowlevel->namespaces.size(); i++) {
  
  
  Network network(audio, true);
  network.run();
  
  ofstream vocabfile;
  vocabfile.open(argv[1]);

  return 0;
}
