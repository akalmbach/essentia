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

#include <essentia/streaming/algorithms/poolstorage.h>
#include <essentia/essentiautil.h>
#include <essentia/essentia.h> 
#include  "essentia/scheduler/network.h"
#include  "essentia/streaming/sourcebase.h"
#include  "essentia/streaming/streamingalgorithm.h"
#include "freesound/FreesoundLowlevelDescriptors.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <fstream>


using namespace std;
using namespace essentia;
using namespace streaming;
using namespace scheduler;

vector<vector< double > > computeVocab(Pool features, vector<string> fields, int k) {
	standard::Algorithm* mat_aggregator = standard::AlgorithmFactory::create("PoolMatAggregator", "fields", fields);
  mat_aggregator->input("input").set(features);
  cv::Mat kmeans_data, labels, centers;
  mat_aggregator->output("output").set(kmeans_data);
  mat_aggregator->compute();
  
  cout << fields << ":" << kmeans_data.rows << "," << kmeans_data.cols << endl;
    
  cv::namedWindow("window", CV_WINDOW_NORMAL);
  cv::imshow("window", kmeans_data);
  cv::waitKey(0);
  
  cv::kmeans(kmeans_data, k, labels, cv::TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 0.000001),
      5, cv::KMEANS_RANDOM_CENTERS, centers);
      		
	vector<vector<double> > vocab;
	for (int i = 0; i < centers.rows; i++) {
		vector<double> center;
		for (int j = 0; j < centers.cols; j++) {
			center.push_back(centers.at<float>(i, j));
		}
		vocab.push_back(center);
	}
	return vocab;
}

int main(int argc, char* argv[]) {
	
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  Pool features;
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  SourceBase& source = audio->output("signal");
  connect(audio->output("time"), NOWHERE);

 	
  FreesoundLowlevelDescriptors *lowlevel = new FreesoundLowlevelDescriptors();
  lowlevel->createNetwork(source, features);
  
  Network network(audio, true);
  network.run();
  
  
  ofstream vocabfile;
  vocabfile.open(argv[1]);

  
  for (int i = 0; i < lowlevel->namespaces.size(); i++) {
	  // TODO: Magic number
	  vocabfile << lowlevel->namespaces[i] << ": \n  [";
      vector<vector<double> > vocab = computeVocab(features, features.descriptorNames(lowlevel->namespaces[i]), 100);
      for (int j = 0; j < vocab.size(); j++) {
		  vocabfile << "\n    [";
		  for (int k = 0; k < vocab[j].size()-1; k++) {
			  vocabfile << vocab[j][k] << ",";
		  }
		  vocabfile << vocab[j][vocab[j].size()] << "]";
	  }
	  vocabfile << "  ]" << endl;
  }
  
  vocabfile.close();

  return 0;
}
