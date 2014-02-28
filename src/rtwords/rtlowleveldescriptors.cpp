#include "rtlowleveldescriptors.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;

void RTLowlevelDescriptors::addOutput(SourceBase& feature, string nameSpace, string name) {
	
	bool newnamespace = true;
	for (unsigned int i = 0; i < namespaces.size(); i++) {
		if (nameSpace.compare((namespaces[i])) == 0) {
			newnamespace = false;
			break;
		}
	}
	if (newnamespace) namespaces.push_back(nameSpace);

	connect(feature, pool, nameSpace+"."+name, &cout);
}

void RTLowlevelDescriptors::createNetwork(SourceBase& source){

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
  addOutput(sr->output("threshold_0"), "silence_rate", "20dB");
  addOutput(sr->output("threshold_1"), "silence_rate", "30dB");
  addOutput(sr->output("threshold_2"), "silence_rate", "60dB");


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
  addOutput(zcr->output("zeroCrossingRate"), "zcr", "zeroCrossingRate");

  // MFCC
  Algorithm* mfcc = factory.create("MFCC");
  connect(spec->output("spectrum"), mfcc->input("spectrum"));
  connect(mfcc->output("bands"), NOWHERE);
  addOutput(mfcc->output("mfcc"), "mfcc", "mfcc");

  // Spectral Decrease
  Algorithm* square = factory.create("UnaryOperator", "type", "square");
  Algorithm* decrease = factory.create("Decrease",
                                       "range", analysisSampleRate * 0.5);
  connect(spec->output("spectrum"), square->input("array"));
  connect(square->output("array"), decrease->input("array"));
  addOutput(decrease->output("decrease"), "spectral", "decrease");
  
  // Spectral Energy
  Algorithm* energy = factory.create("Energy");
  connect(spec->output("spectrum"), energy->input("array"));
  addOutput(energy->output("energy"), "spectral", "energy");

  // Spectral Energy Band Ratio
  Algorithm* ebr_low = factory.create("EnergyBand",
                                      "startCutoffFrequency", 20.0,
                                      "stopCutoffFrequency", 150.0);
  connect(spec->output("spectrum"), ebr_low->input("spectrum"));
  addOutput(ebr_low->output("energyBand"), "spectral", "energyband_low");

  Algorithm* ebr_mid_low = factory.create("EnergyBand",
                                          "startCutoffFrequency", 150.0,
                                          "stopCutoffFrequency", 800.0);
  connect(spec->output("spectrum"), ebr_mid_low->input("spectrum"));
  addOutput(ebr_mid_low->output("energyBand"), "spectral", "energyband_middle_low");

  Algorithm* ebr_mid_hi = factory.create("EnergyBand",
                                         "startCutoffFrequency", 800.0,
                                         "stopCutoffFrequency", 4000.0);
  connect(spec->output("spectrum"), ebr_mid_hi->input("spectrum"));
  addOutput(ebr_mid_hi->output("energyBand"), "spectral", "energyband_middle_high");


  Algorithm* ebr_hi = factory.create("EnergyBand",
                                     "startCutoffFrequency", 4000.0,
                                     "stopCutoffFrequency", 20000.0);
  connect(spec->output("spectrum"), ebr_hi->input("spectrum"));
  addOutput(ebr_hi->output("energyBand"), "spectral", "spectral.energyband_high");
 


  // Spectral HFC
  Algorithm* hfc = factory.create("HFC");
  connect(spec->output("spectrum"), hfc->input("spectrum"));
  addOutput(hfc->output("hfc"), "hfc", "hfc");


  // Spectral Frequency Bands
  Algorithm* fb = factory.create("FrequencyBands",
                                 "sampleRate", analysisSampleRate);
  connect(spec->output("spectrum"), fb->input("spectrum"));
  addOutput(fb->output("bands"), "fb", "bands");

  // Spectral RMS
  Algorithm* rms = factory.create("RMS");
  connect(spec->output("spectrum"), rms->input("array"));
  addOutput(rms->output("rms"),"spectral", "rms");


  // Spectral Flux
  Algorithm* flux = factory.create("Flux");
  connect(spec->output("spectrum"), flux->input("spectrum"));
  addOutput(flux->output("flux"), "spectral", "flux");


  // Spectral Roll Off
  Algorithm* ro = factory.create("RollOff");
  connect(spec->output("spectrum"), ro->input("spectrum"));
  addOutput(ro->output("rollOff"), "spectral", "rolloff");


  // Spectral Strong Peak
  Algorithm* sp = factory.create("StrongPeak");
  connect(spec->output("spectrum"), sp->input("spectrum"));
  addOutput(sp->output("strongPeak"), "spectral", "strongpeak");


  // BarkBands
  uint nBarkBands = 27;
  Algorithm* barkBands = factory.create("BarkBands",
                                        "numberBands", nBarkBands);
  connect(spec->output("spectrum"), barkBands->input("spectrum"));
  addOutput(barkBands->output("bands"), "bb", "barkbands");

  // Spectral Crest
  Algorithm* crest = factory.create("Crest");
  connect(barkBands->output("bands"), crest->input("array"));
  addOutput(crest->output("crest"), "spectral", "crest");


  // Spectral Flatness DB
  Algorithm* flatness = factory.create("FlatnessDB");
  connect(barkBands->output("bands"), flatness->input("array"));
  addOutput(flatness->output("flatnessDB"), "spectral", "flatness_db");


  // Spectral Centroid
  Algorithm* square2 = factory.create("UnaryOperator", "type", "square");
  Algorithm* centroid = factory.create("Centroid",
                                       "range", analysisSampleRate * 0.5);
  connect(spec->output("spectrum"), square2->input("array"));
  connect(square2->output("array"), centroid->input("array"));
  addOutput(centroid->output("centroid"), "spectral", "centroid");


  // Spectral Central Moments Statistics
  Algorithm* cm = factory.create("CentralMoments",
                                 "range", analysisSampleRate * 0.5);
  Algorithm* ds = factory.create("DistributionShape");
  connect(spec->output("spectrum"), cm->input("array"));
  connect(cm->output("centralMoments"), ds->input("centralMoments"));
  addOutput(ds->output("kurtosis"), "spectral", "kurtosis");
  addOutput(ds->output("spread"), "spectral", "spread");
  addOutput(ds->output("skewness"), "spectral","skewness");


  // Spectral Dissonance
  Algorithm* peaks = factory.create("SpectralPeaks",
                                    "orderBy", "frequency");
  Algorithm* diss = factory.create("Dissonance");
  connect(spec->output("spectrum"), peaks->input("spectrum"));
  connect(peaks->output("frequencies"), diss->input("frequencies"));
  connect(peaks->output("magnitudes"), diss->input("magnitudes"));
  addOutput(diss->output("dissonance"), "spectral", "dissonance");

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
  addOutput(sc->output("spectralContrast"), "spectral", "contrast");
  addOutput(sc->output("spectralValley"), "spectral", "scvalleys");


  // Spectral BarkBands Central Moments Statistics
  Algorithm* bbcm = factory.create("CentralMoments",
                                 "range", nBarkBands-1);
  Algorithm* ds2 = factory.create("DistributionShape");
  connect(barkBands->output("bands"), bbcm->input("array"));
  connect(bbcm->output("centralMoments"), ds2->input("centralMoments"));
  addOutput(ds2->output("kurtosis"), "bb", "kurtosis");
  addOutput(ds2->output("spread"), "bb", "spread");
  addOutput(ds2->output("skewness"), "bb", "skewness");


  // Spectral Complexity
  Algorithm* tc = factory.create("SpectralComplexity",
                                 "magnitudeThreshold", 0.005);
  connect(spec->output("spectrum"), tc->input("spectrum"));
  addOutput(tc->output("spectralComplexity"), "spectral", "complexity");


  // Pitch Detection
  Algorithm* pitch = factory.create("PitchYinFFT",
                                    "frameSize", frameSize);
  connect(spec->output("spectrum"), pitch->input("spectrum"));
  addOutput(pitch->output("pitch"), "pitch", "pitch");
  addOutput(pitch->output("pitchConfidence"), "pitch", "confidence");

  // Pitch Salience
  Algorithm* ps = factory.create("PitchSalience");
  connect(spec->output("spectrum"), ps->input("spectrum"));
  addOutput(ps->output("pitchSalience"), "pitch", "salience");


  // Loudness
  Algorithm* dy = factory.create("Loudness");
  connect(fc->output("frame"), dy->input("signal"));
  addOutput(dy->output("loudness"), "loudness", "loudness");
} 



