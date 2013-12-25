/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
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

#include <iostream>
#include <fstream>
#include <essentia/algorithmfactory.h>
#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/scheduler/network.h>

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace essentia::scheduler;

int main(int argc, char* argv[]) {
  // register the algorithms in the factory(ies)
  essentia::init();

  /////// PARAMS //////////////
  int frameSize = 2048;
  int hopSize = 1024;
  
  // we want to compute the MFCC of a file: we need the create the following:
  // audioloader -> framecutter -> windowing -> FFT -> MFCC -> PoolStorage
  // we also need a DevNull which is able to gobble data without doing anything
  // with it (required otherwise a buffer would be filled and blocking)

  Pool pool;
  AlgorithmFactory& factory = streaming::AlgorithmFactory::instance();

  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384, "autoconnect", true);

  Algorithm* fc    = factory.create("FrameCutter",
                                    "frameSize", frameSize,
                                    "hopSize", hopSize,
                                    "silentFrames", "noise");

  Algorithm* w     = factory.create("Windowing",
                                    "type", "blackmanharris62");

  Algorithm* spec  = factory.create("Spectrum");
  Algorithm* mfcc  = factory.create("MFCC");
  Algorithm* gfcc  = factory.create("GFCC");
  Algorithm* lpc   = factory.create("LPC", "sampleRate", 48000);

  /////////// CONNECTING THE ALGORITHMS ////////////////
  cout << "-------- connecting algos --------" << endl;

  // Audio -> FrameCutter
  audio->output("signal")    >>  fc->input("signal");
  audio->output("Jack Time") >>  PC(pool, "clock", &cout);                        

  // FrameCutter -> Windowing -> Spectrum
  fc->output("frame")       >>  w->input("frame");
  w->output("frame")        >>  spec->input("frame");
  w->output("frame")        >>  lpc->input("frame");

  // Spectrum -> MFCC -> Pool
  spec->output("spectrum")  >>  mfcc->input("spectrum");
  spec->output("spectrum")  >>  gfcc->input("spectrum");

  mfcc->output("bands")     >>  NOWHERE;                          // we don't want the mel bands
  mfcc->output("mfcc")      >>  PC(pool, "lowlevel.mfcc", &cout); // store only the mfcc coeffs and spit them 
  
  gfcc->output("bands")     >>  NOWHERE;                          // we don't want the mel bands
  gfcc->output("gfcc")      >>  PC(pool, "lowlevel.gfcc"); // store only the mfcc coeffs and spit them out
  
  lpc->output("lpc")        >>  PC(pool, "lowlevel.lpc", &cout);
  lpc->output("reflection") >>  PC(pool, "lowlevel.reflections");


  // create a network with our algorithms...
  Network n(audio);
  // ...and run it, easy as that!
  n.run();
  cout << "finished running" << endl;
  
  
  n.clear();

  essentia::shutdown();

  return 0;
}
