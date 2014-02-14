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

void RTLowlevelDescriptors::addOutput(SourceBase& feature, string nameSpace, string name, Pool& pool) {
	
	bool newnamespace = true;
	for (unsigned int i = 0; i < namespaces.size(); i++) {
		if (nameSpace.compare((namespaces[i])) == 0) {
			newnamespace = false;
			break;
		}
	}
	if (newnamespace) namespaces.push_back(nameSpace);

	connect(feature, pool, nameSpace+"."+name, &cout);
	
	
	//connect(feature, 
}

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
  addOutput(sr->output("threshold_0"), "silence_rate", "20dB", pool);
  addOutput(sr->output("threshold_1"), "silence_rate", "30dB", pool);
  addOutput(sr->output("threshold_2"), "silence_rate", "60dB", pool);


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
  connect(fc->output("frame"), zcr->input("signal"));
  addOutput(zcr->output("zeroCrossingRate"), "zcr", "zeroCrossingRate", pool);

  // MFCC
  Algorithm* mfcc = factory.create("MFCC");
  connect(spec->output("spectrum"), mfcc->input("spectrum"));
  connect(mfcc->output("bands"), NOWHERE);
  addOutput(mfcc->output("mfcc"), "mfcc", "mfcc", pool);

  // Spectral Decrease
  Algorithm* square = factory.create("UnaryOperator", "type", "square");
  Algorithm* decrease = factory.create("Decrease",
                                       "range", analysisSampleRate * 0.5);
  connect(spec->output("spectrum"), square->input("array"));
  connect(square->output("array"), decrease->input("array"));
  addOutput(decrease->output("decrease"), "spectral", "decrease", pool);
  
  // Spectral Energy
  Algorithm* energy = factory.create("Energy");
  connect(spec->output("spectrum"), energy->input("array"));
  addOutput(energy->output("energy"), "spectral", "energy", pool);

  // Spectral Energy Band Ratio
  Algorithm* ebr_low = factory.create("EnergyBand",
                                      "startCutoffFrequency", 20.0,
                                      "stopCutoffFrequency", 150.0);
  connect(spec->output("spectrum"), ebr_low->input("spectrum"));
  addOutput(ebr_low->output("energyBand"), "spectral", "energyband_low", pool);

  Algorithm* ebr_mid_low = factory.create("EnergyBand",
                                          "startCutoffFrequency", 150.0,
                                          "stopCutoffFrequency", 800.0);
  connect(spec->output("spectrum"), ebr_mid_low->input("spectrum"));
  addOutput(ebr_mid_low->output("energyBand"), "spectral", "energyband_middle_low", pool);

  Algorithm* ebr_mid_hi = factory.create("EnergyBand",
                                         "startCutoffFrequency", 800.0,
                                         "stopCutoffFrequency", 4000.0);
  connect(spec->output("spectrum"), ebr_mid_hi->input("spectrum"));
  addOutput(ebr_mid_hi->output("energyBand"), "spectral", "energyband_middle_high", pool);


  Algorithm* ebr_hi = factory.create("EnergyBand",
                                     "startCutoffFrequency", 4000.0,
                                     "stopCutoffFrequency", 20000.0);
  connect(spec->output("spectrum"), ebr_hi->input("spectrum"));
  addOutput(ebr_hi->output("energyBand"), "spectral", "spectral.energyband_high", pool);
 


  // Spectral HFC
  Algorithm* hfc = factory.create("HFC");
  connect(spec->output("spectrum"), hfc->input("spectrum"));
  addOutput(hfc->output("hfc"), "hfc", "hfc", pool);


  // Spectral Frequency Bands
  Algorithm* fb = factory.create("FrequencyBands",
                                 "sampleRate", analysisSampleRate);
  connect(spec->output("spectrum"), fb->input("spectrum"));
  addOutput(fb->output("bands"), "fb", "bands", pool);

  // Spectral RMS
  Algorithm* rms = factory.create("RMS");
  connect(spec->output("spectrum"), rms->input("array"));
  addOutput(rms->output("rms"),"spectral", "rms", pool);


  // Spectral Flux
  Algorithm* flux = factory.create("Flux");
  connect(spec->output("spectrum"), flux->input("spectrum"));
  addOutput(flux->output("flux"), "spectral", "flux", pool);


  // Spectral Roll Off
  Algorithm* ro = factory.create("RollOff");
  connect(spec->output("spectrum"), ro->input("spectrum"));
  addOutput(ro->output("rollOff"), "spectral", "rolloff", pool);


  // Spectral Strong Peak
  Algorithm* sp = factory.create("StrongPeak");
  connect(spec->output("spectrum"), sp->input("spectrum"));
  addOutput(sp->output("strongPeak"), "spectral", "strongpeak", pool);


  // BarkBands
  uint nBarkBands = 27;
  Algorithm* barkBands = factory.create("BarkBands",
                                        "numberBands", nBarkBands);
  connect(spec->output("spectrum"), barkBands->input("spectrum"));
  addOutput(barkBands->output("bands"), "bb", "barkbands", pool);

  // Spectral Crest
  Algorithm* crest = factory.create("Crest");
  connect(barkBands->output("bands"), crest->input("array"));
  addOutput(crest->output("crest"), "spectral", "crest", pool);


  // Spectral Flatness DB
  Algorithm* flatness = factory.create("FlatnessDB");
  connect(barkBands->output("bands"), flatness->input("array"));
  addOutput(flatness->output("flatnessDB"), "spectral", "flatness_db", pool);


  // Spectral Centroid
  Algorithm* square2 = factory.create("UnaryOperator", "type", "square");
  Algorithm* centroid = factory.create("Centroid",
                                       "range", analysisSampleRate * 0.5);
  connect(spec->output("spectrum"), square2->input("array"));
  connect(square2->output("array"), centroid->input("array"));
  addOutput(centroid->output("centroid"), "spectral", "centroid", pool);


  // Spectral Central Moments Statistics
  Algorithm* cm = factory.create("CentralMoments",
                                 "range", analysisSampleRate * 0.5);
  Algorithm* ds = factory.create("DistributionShape");
  connect(spec->output("spectrum"), cm->input("array"));
  connect(cm->output("centralMoments"), ds->input("centralMoments"));
  addOutput(ds->output("kurtosis"), "spectral", "kurtosis", pool);
  addOutput(ds->output("spread"), "spectral", "spread", pool);
  addOutput(ds->output("skewness"), "spectral","skewness", pool);


  // Spectral Dissonance
  Algorithm* peaks = factory.create("SpectralPeaks",
                                    "orderBy", "frequency");
  Algorithm* diss = factory.create("Dissonance");
  connect(spec->output("spectrum"), peaks->input("spectrum"));
  connect(peaks->output("frequencies"), diss->input("frequencies"));
  connect(peaks->output("magnitudes"), diss->input("magnitudes"));
  addOutput(diss->output("dissonance"), "spectral", "dissonance", pool);

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
  addOutput(sc->output("spectralContrast"), "spectral", "contrast", pool);
  addOutput(sc->output("spectralValley"), "spectral", "scvalleys", pool);


  // Spectral BarkBands Central Moments Statistics
  Algorithm* bbcm = factory.create("CentralMoments",
                                 "range", nBarkBands-1);
  Algorithm* ds2 = factory.create("DistributionShape");
  connect(barkBands->output("bands"), bbcm->input("array"));
  connect(bbcm->output("centralMoments"), ds2->input("centralMoments"));
  addOutput(ds2->output("kurtosis"), "bb", "kurtosis", pool);
  addOutput(ds2->output("spread"), "bb", "spread", pool);
  addOutput(ds2->output("skewness"), "bb", "skewness", pool);


  // Spectral Complexity
  Algorithm* tc = factory.create("SpectralComplexity",
                                 "magnitudeThreshold", 0.005);
  connect(spec->output("spectrum"), tc->input("spectrum"));
  addOutput(tc->output("spectralComplexity"), "spectral", "complexity", pool);


  // Pitch Detection
  Algorithm* pitch = factory.create("PitchYinFFT",
                                    "frameSize", frameSize);
  connect(spec->output("spectrum"), pitch->input("spectrum"));
  addOutput(pitch->output("pitch"), "pitch", "pitch", pool);
  addOutput(pitch->output("pitchConfidence"), "pitch", "confidence", pool);

  // Pitch Salience
  Algorithm* ps = factory.create("PitchSalience");
  connect(spec->output("spectrum"), ps->input("spectrum"));
  addOutput(ps->output("pitchSalience"), "pitch", "salience", pool);


  // Loudness
  Algorithm* dy = factory.create("Loudness");
  connect(fc->output("frame"), dy->input("signal"));
  addOutput(dy->output("loudness"), "loudness", "loudness", pool);
} 



