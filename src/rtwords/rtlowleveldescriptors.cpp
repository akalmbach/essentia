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

#include "rtlowleveldescriptors.h"
using namespace std;
using namespace essentia;
using namespace essentia::streaming;

const string RTLowlevelDescriptors::nameSpace="rt.";  	

void RTLowlevelDescriptors::createNetwork(SourceBase& source, Pool& pool){

  Real analysisSampleRate =  44100;// TODO: unify

  AlgorithmFactory& factory = AlgorithmFactory::instance();


  Real sampleRate = 44100;
  int frameSize =   2048;
  int hopSize =     1024;
  int zeroPadding = 0;

  string silentFrames ="noise";
  string windowType = "hann";
  
  // FrameCutter
  Algorithm* fc = factory.create("FrameCutter",
                                 "frameSize", frameSize,
                                 "hopSize", hopSize,
                                 "silentFrames", silentFrames);
  connect(source, fc->input("signal"));


  // Silence Rate
  Real thresholds_dB[] = { -20, -30, -60 };

  vector<Real> thresholds(ARRAY_SIZE(thresholds_dB));
  for (uint i=0; i<thresholds.size(); i++) {
    thresholds[i] = db2lin(thresholds_dB[i]/2.0);
  }

  Algorithm* sr = factory.create("SilenceRate","thresholds", thresholds);
  connect(fc->output("frame"), sr->input("frame"));
  connect(sr->output("threshold_0"), pool, "silence_rate.20dB", &cout);
  connect(sr->output("threshold_1"), pool, "silence_rate.30dB", &cout);
  connect(sr->output("threshold_2"), pool, "silence_rate.60dB", &cout);
  namespaces.push_back("silence_rate");


  // Windowing
  Algorithm* w = factory.create("Windowing",
                                "type", windowType,
                                "zeroPadding", zeroPadding);
  connect(fc->output("frame"), w->input("frame"));


  // Spectrum
  Algorithm* spec = factory.create("Spectrum");
  connect(w->output("frame"), spec->input("frame"));

  
  // Temporal Descriptors
  Algorithm* zcr = factory.create("ZeroCrossingRate");
  connect(zcr->input("signal"), fc->output("frame"));
  connect(zcr->output("zeroCrossingRate"), pool, "zcr.zerocrossingrate", &cout);
  namespaces.push_back("zcr");

  // MFCC
  Algorithm* mfcc = factory.create("MFCC");
  connect(spec->output("spectrum"), mfcc->input("spectrum"));
  connect(mfcc->output("bands"), NOWHERE);
  connect(mfcc->output("mfcc"), pool, "mfcc.mfcc", &cout);
  namespaces.push_back("mfcc");

  // Spectral Decrease
  Algorithm* square = factory.create("UnaryOperator", "type", "square");
  Algorithm* decrease = factory.create("Decrease",
                                       "range", analysisSampleRate * 0.5);
  connect(spec->output("spectrum"), square->input("array"));
  connect(square->output("array"), decrease->input("array"));
  connect(decrease->output("decrease"), pool, "spectral.decrease", &cout);
  namespaces.push_back("spectral");
  
  // Spectral Energy
  Algorithm* energy = factory.create("Energy");
  connect(spec->output("spectrum"), energy->input("array"));
  connect(energy->output("energy"), pool, "spectral.energy", &cout);

  // Spectral Energy Band Ratio

  Algorithm* ebr_low = factory.create("EnergyBand",
                                      "startCutoffFrequency", 20.0,
                                      "stopCutoffFrequency", 150.0);
  connect(spec->output("spectrum"), ebr_low->input("spectrum"));
  connect(ebr_low->output("energyBand"), pool, "spectral.energyband_low",&cout);

  Algorithm* ebr_mid_low = factory.create("EnergyBand",
                                          "startCutoffFrequency", 150.0,
                                          "stopCutoffFrequency", 800.0);
  connect(spec->output("spectrum"), ebr_mid_low->input("spectrum"));
  connect(ebr_mid_low->output("energyBand"), pool, "spectral.energyband_middle_low",&cout);

  Algorithm* ebr_mid_hi = factory.create("EnergyBand",
                                         "startCutoffFrequency", 800.0,
                                         "stopCutoffFrequency", 4000.0);
  connect(spec->output("spectrum"), ebr_mid_hi->input("spectrum"));
  connect(ebr_mid_hi->output("energyBand"), pool, "spectral.energyband_middle_high",&cout);


  Algorithm* ebr_hi = factory.create("EnergyBand",
                                     "startCutoffFrequency", 4000.0,
                                     "stopCutoffFrequency", 20000.0);
  connect(spec->output("spectrum"), ebr_hi->input("spectrum"));
  connect(ebr_hi->output("energyBand"), pool, "spectral.energyband_high",&cout);
 


  // Spectral HFC
  Algorithm* hfc = factory.create("HFC");
  connect(spec->output("spectrum"), hfc->input("spectrum"));
  connect(hfc->output("hfc"), pool, "hfc.hfc", &cout);
  namespaces.push_back("hfc");


  // Spectral Frequency Bands
  Algorithm* fb = factory.create("FrequencyBands",
                                 "sampleRate", analysisSampleRate);
  connect(spec->output("spectrum"), fb->input("spectrum"));
  connect(fb->output("bands"), pool, "fb.frequency_bands",&cout);
  namespaces.push_back("fb");


  // Spectral RMS
  Algorithm* rms = factory.create("RMS");
  connect(spec->output("spectrum"), rms->input("array"));
  connect(rms->output("rms"), pool, "spectral.rms",&cout);


  // Spectral Flux
  Algorithm* flux = factory.create("Flux");
  connect(spec->output("spectrum"), flux->input("spectrum"));
  connect(flux->output("flux"), pool, "spectral.flux",&cout);


  // Spectral Roll Off
  Algorithm* ro = factory.create("RollOff");
  connect(spec->output("spectrum"), ro->input("spectrum"));
  connect(ro->output("rollOff"), pool, "spectral.rolloff",&cout);


  // Spectral Strong Peak
  Algorithm* sp = factory.create("StrongPeak");
  connect(spec->output("spectrum"), sp->input("spectrum"));
  connect(sp->output("strongPeak"), pool, "spectral.strongpeak",&cout);


  // BarkBands
  uint nBarkBands = 27;
  Algorithm* barkBands = factory.create("BarkBands",
                                        "numberBands", nBarkBands);
  connect(spec->output("spectrum"), barkBands->input("spectrum"));
  connect(barkBands->output("bands"), pool, "bb.barkbands", &cout);
  namespaces.push_back("bb");


  // Spectral Crest
  Algorithm* crest = factory.create("Crest");
  connect(barkBands->output("bands"), crest->input("array"));
  connect(crest->output("crest"), pool, "spectral.crest",&cout);


  // Spectral Flatness DB
  Algorithm* flatness = factory.create("FlatnessDB");
  connect(barkBands->output("bands"), flatness->input("array"));
  connect(flatness->output("flatnessDB"), pool, "spectral.flatness_db",&cout);


  // Spectral Centroid
  Algorithm* square2 = factory.create("UnaryOperator", "type", "square");
  Algorithm* centroid = factory.create("Centroid",
                                       "range", analysisSampleRate * 0.5);
  connect(spec->output("spectrum"), square2->input("array"));
  connect(square2->output("array"), centroid->input("array"));
  connect(centroid->output("centroid"), pool, "spectral.centroid",&cout);


  // Spectral Central Moments Statistics
  Algorithm* cm = factory.create("CentralMoments",
                                 "range", analysisSampleRate * 0.5);
  Algorithm* ds = factory.create("DistributionShape");
  connect(spec->output("spectrum"), cm->input("array"));
  connect(cm->output("centralMoments"), ds->input("centralMoments"));
  connect(ds->output("kurtosis"), pool, "spectral.kurtosis",&cout);
  connect(ds->output("spread"), pool, "spectral.spread",&cout);
  connect(ds->output("skewness"), pool, "spectral.skewness",&cout);


  // Spectral Dissonance
  Algorithm* peaks = factory.create("SpectralPeaks",
                                    "orderBy", "frequency");
  Algorithm* diss = factory.create("Dissonance");
  connect(spec->output("spectrum"), peaks->input("spectrum"));
  connect(peaks->output("frequencies"), diss->input("frequencies"));
  connect(peaks->output("magnitudes"), diss->input("magnitudes"));
  connect(diss->output("dissonance"), pool, "spectral.dissonance",&cout);

  // Spectral Contrast
  Algorithm* sc = factory.create("SpectralContrast",
                                 "frameSize", frameSize,
                                 "sampleRate", analysisSampleRate,
                                 "numberBands", 6,
                                 "lowFrequencyBound", 20,
                                 "highFrequencyBound", 11000,
                                 "neighbourRatio", 0.4,
                                 "staticDistribution", 0.15);

  connect(spec->output("spectrum"), sc->input("spectrum"));
  connect(sc->output("spectralContrast"), pool, "spectral.contrast",&cout);
  connect(sc->output("spectralValley"), pool, "spectral.scvalleys",&cout);


  // Spectral BarkBands Central Moments Statistics
  Algorithm* bbcm = factory.create("CentralMoments",
                                 "range", nBarkBands-1);
  Algorithm* ds2 = factory.create("DistributionShape");
  connect(barkBands->output("bands"), bbcm->input("array"));
  connect(bbcm->output("centralMoments"), ds2->input("centralMoments"));
  connect(ds2->output("kurtosis"), pool, "bb.barkbands_kurtosis",&cout);
  connect(ds2->output("spread"), pool, "bb.barkbands_spread",&cout);
  connect(ds2->output("skewness"), pool, "bb.barkbands_skewness",&cout);


  // Spectral Complexity
  Algorithm* tc = factory.create("SpectralComplexity",
                                 "magnitudeThreshold", 0.005);
  connect(spec->output("spectrum"), tc->input("spectrum"));
  connect(tc->output("spectralComplexity"), pool, "spectral.complexity",&cout);


  // Pitch Detection
  Algorithm* pitch = factory.create("PitchYinFFT",
                                    "frameSize", frameSize);
  connect(spec->output("spectrum"), pitch->input("spectrum"));
  connect(pitch->output("pitch"), pool, nameSpace + "pitch",&cout);
  connect(pitch->output("pitchConfidence"), pool, "pitch.instantaneous_confidence",&cout);
  namespaces.push_back("pitch");


  // Pitch Salience
  Algorithm* ps = factory.create("PitchSalience");
  connect(spec->output("spectrum"), ps->input("spectrum"));
  connect(ps->output("pitchSalience"), pool, "pitch.salience",&cout);


  // Loudness
  Algorithm* dy = factory.create("Loudness");
  connect(fc->output("frame"), dy->input("signal"));
  connect(dy->output("loudness"), pool, "l.loudness",&cout);
  namespaces.push_back("l");

} 

Real squeezeRange(Real& x, Real& x1, Real& x2);

Real squeezeRange(Real& x, Real& x1, Real& x2) {
  return (0.5 + 0.5 * tanh(-1.0 + 2.0 * (x - x1) / (x2 - x1)));
}


void RTLowlevelDescriptors::computeAverageLoudness(Pool& pool){ // after computing network

  vector<Real> levelArray = pool.value<vector<Real> >(nameSpace + "loudness");
  pool.remove(nameSpace + "loudness");

  // Maximum dynamic
  Real EPSILON = 10e-5;
  Real maxValue = levelArray[argmax(levelArray)];
  if (maxValue <= EPSILON) {
    maxValue = EPSILON;
  }

  // Normalization to the maximum
  Real THRESHOLD = 0.0001; // this corresponds to -80dB
  for (uint i=0; i<levelArray.size(); i++) {
    levelArray[i] /= maxValue;
    if (levelArray[i] <= THRESHOLD) {
      levelArray[i] = THRESHOLD;
    }
  }

  // Average Level
  Real levelAverage = pow2db(mean(levelArray));

  // Re-scaling and range-control
  // This yields in numbers between
  // 0 for signals with  large dynamic variance and thus low dynamic average
  // 1 for signal with little dynamic range and thus
  // a dynamic average close to the maximum
  Real x1 = -5.0;
  Real x2 = -2.0;
  Real levelAverageSqueezed = squeezeRange(levelAverage, x1, x2);
  pool.set(nameSpace + "average_loudness", levelAverageSqueezed);
}



