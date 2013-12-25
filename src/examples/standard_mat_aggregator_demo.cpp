///*
 //* Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 //*
 //* This file is part of Essentia
 //*
 //* Essentia is free software: you can redistribute it and/or modify it under
 //* the terms of the GNU Affero General Public License as published by the Free
 //* Software Foundation (FSF), either version 3 of the License, or (at your
 //* option) any later version.
 //*
 //* This program is distributed in the hope that it will be useful, but WITHOUT
 //* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 //* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 //* details.
 //*
 //* You should have received a copy of the Affero GNU General Public License
 //* version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 //*/

//#include <iostream>
//#include <fstream>
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//#include <opencv/ml.h>
//#include <essentia/algorithmfactory.h>
//#include <essentia/essentiamath.h>
//#include <essentia/pool.h>

//using namespace std;
//using namespace essentia;
//using namespace essentia::standard;

//void audioFeatures(string audioFilename, Pool& main_pool) {
  //Pool pool;

  ///////// PARAMS //////////////
  //int sampleRate = 44100;
  //int frameSize = 2048;
  //int hopSize = 1024;

  //AlgorithmFactory& factory = standard::AlgorithmFactory::instance();

  //Algorithm* audio = factory.create("MonoLoader",
                                    //"filename", audioFilename,
                                    //"sampleRate", sampleRate);

  //Algorithm* fc    = factory.create("FrameCutter",
                                    //"frameSize", frameSize,
                                    //"hopSize", hopSize);

  //Algorithm* w     = factory.create("Windowing",
                                    //"type", "blackmanharris62");
                                    
  //Algorithm* loudness = factory.create("Loudness");

  //Algorithm* spec  = factory.create("Spectrum");
  //Algorithm* mfcc  = factory.create("MFCC");
  //Algorithm* flux  = factory.create("Flux");
  
  //Algorithm* centroid = factory.create("Centroid",
										//"range", sampleRate/2.0);
										
  //Algorithm* d     = factory.create("Derivative");


  ///////////// CONNECTING THE ALGORITHMS ////////////////
  //cout << "-------- connecting algos ---------" << endl;

  //// Audio -> FrameCutter
  //vector<Real> audioBuffer;

  //audio->output("audio").set(audioBuffer);
  //fc->input("signal").set(audioBuffer);

  //// FrameCutter -> Windowing -> Spectrum
  //vector<Real> frame, windowedFrame;

  //fc->output("frame").set(frame);
  //w->input("frame").set(frame);

  //w->output("frame").set(windowedFrame);
  //spec->input("frame").set(windowedFrame);
  //loudness->input("signal").set(windowedFrame);

  //// Spectrum -> MFCC
  //vector<Real> spectrum, mfccCoeffs, mfccBands;
  //Real flux_out, centroid_out, loudness_out;

  //spec->output("spectrum").set(spectrum);
  //mfcc->input("spectrum").set(spectrum);
  //flux->input("spectrum").set(spectrum);
  //centroid->input("array").set(spectrum);

  //mfcc->output("bands").set(mfccBands);
  //mfcc->output("mfcc").set(mfccCoeffs);
  
  //flux->output("flux").set(flux_out);
  
  //centroid->output("centroid").set(centroid_out);
  
  //loudness->output("loudness").set(loudness_out);


  ///////////// STARTING THE ALGORITHMS //////////////////
  //cout << "-------- start processing " << audioFilename << " --------" << endl;

  //audio->compute();

  //while (true) {

    //// compute a frame
    //fc->compute();

    //// if it was the last one (ie: it was empty), then we're done.
    //if (!frame.size()) {
      //break;
    //}

    //// if the frame is silent, just drop it and go on processing
    //if (isSilent(frame)) continue;

    //w->compute();
    //loudness->compute();
    //spec->compute();
    //mfcc->compute();
    //flux->compute();
   
    //pool.add("lowlevel.mfcc", mfccCoeffs);
    //pool.add("lowlevel.flux", flux_out);
	//pool.add("lowlevel.centroid", centroid_out);
	//pool.add("lowlevel.loudness", loudness_out);
  //}
  
  ////vector<Real> d_loud;
  ////d->input("signal").set(pool.value< vector<Real> >("lowlevel.loudness"));
  ////d->output("signal").set(d_loud);
  ////d->compute();
  ////pool.set("lowlevel.dloudness", d_loud);
  
  //main_pool.merge(pool, "append");
//}

//int main(int argc, char* argv[]) {
  //if (argc != 3) {
    //cout << "ERROR: incorrect number of arguments." << endl;
		//cout << "Usage: " << argv[0] << " train_playlist test_playlist" << endl;
    //exit(1);
  //}
  
  //essentia::init();
  
  //std::ifstream train_file(argv[1]);
  //string audioDirectory = "/home/arnold/Projects/blogscraper/";
  //string audioFilename;
  //Pool pool;
  //while (train_file >> audioFilename) {
	  //audioFeatures(audioDirectory + audioFilename, pool);
  //}

   
  //vector<string> fields;
  //fields.push_back("lowlevel.loudness");
  ////fields.push_back("lowlevel.dloudness");
  //fields.push_back("lowlevel.centroid");
  //fields.push_back("lowlevel.flux");
  //fields.push_back("lowlevel.mfcc");

  //Algorithm* aggr_mat = AlgorithmFactory::create("PoolMatAggregator", "fields", fields);

  //aggr_mat->input("input").set(pool);
  //cv::Mat aggr;
  //aggr_mat->output("output").set(aggr);
  //aggr_mat->compute();
  
  //cv::namedWindow("window", CV_WINDOW_NORMAL);
  //int wh = 50;
  //int i = wh;
  //while (i < aggr.rows - wh) {
	  //cv::Mat window(aggr, cv::Rect(0, i, aggr.cols, wh));
	  //cv::imshow("window", window);
	  //char k = cv::waitKey(0);
	  //if (k == 'z') i += 25;
	  //if (k == 'x') i += 100;
	  //if (k == 'c') i += wh;
	  //if (k == 'q') break;
	  //else i++;
  //}
    
  //essentia::shutdown();

  //return 0;
//}

int main(int argc, char * argv[]){
    return 0;
}
