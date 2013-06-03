/*
 * Copyright (C) 2006-2008 Music Technology Group (MTG)
 *                         Universitat Pompeu Fabra
 *
 */

#ifndef ESSENTIA_STREAMING_MONOWRITER_H
#define ESSENTIA_STREAMING_MONOWRITER_H

#include "streamingalgorithm.h"
#include "audiocontext.h"
#include "network.h"

namespace essentia {
namespace streaming {

class MonoWriter : public Algorithm {
 protected:
  Sink<AudioSample> _audio;
  AudioContext _audioCtx;
  bool _configured;

 public:
  MonoWriter() : Algorithm(), _configured(false) {
    declareInput(_audio, 4096, "audio", "the input audio");
  }

  void declareParameters() {
    declareParameter("filename", "the name of the encoded file", "", Parameter::STRING);
    declareParameter("sampleRate", "the audio sampling rate [Hz]","(0,inf)", 44100.);
    declareParameter("format", "the audio output format","{wav,aiff,aif,mp3,ogg,flac}", "wav");
    declareParameter("bitrate", "the audio bit rate for compressed formats [kbps]",
		     "{32,40,48,56,64,80,96,112,128,144,160,192,224,256,320}", 192);
  }

  void configure();
  AlgorithmStatus process();
  void reset();

  static const char* name;
  static const char* description;
};

} // namespace streaming
} // namespace essentia

#include "algorithm.h"
#include "vectorinput.h"
#include "network.h"

namespace essentia {
namespace standard {

// Standard non-streaming algorithm comes after the streaming one as it
// depends on it

class MonoWriter : public Algorithm {
 protected:
  Input<std::vector<AudioSample> > _audio;

  Real _duration;
  int _nChannels;
  int _sampleRate;

  bool _configured;

  streaming::Algorithm* _writer;
  streaming::VectorInput<AudioSample, 1024>* _audiogen;
  scheduler::Network* _network;

  void createInnerNetwork();

 public:
  MonoWriter() : _configured(false) {
    declareInput(_audio, "audio", "the audio signal");

    createInnerNetwork();
  }

  ~MonoWriter() {
    delete _network;
  }

  void declareParameters() {
    declareParameter("filename", "the name of the encoded file", "", Parameter::STRING);
    declareParameter("format", "the audio output format","{wav,aiff,aif,mp3,ogg,flac}", "wav");
    declareParameter("sampleRate", "the audio sampling rate [Hz]","(0,inf)", 44100.);
    declareParameter("bitrate", "the audio bit rate for compressed formats [kbps]",
		     "{32,40,48,56,64,80,96,112,128,144,160,192,224,256,320}", 192);
  }

  void configure();

  void compute();

  static const char* name;
  static const char* description;
};

} // namespace standard
} // namespace essentia

#endif // ESSENTIA_STREAMING_MONOWRITER_H