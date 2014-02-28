#include <opencv/cv.h>
#include <opencv/highgui.h>
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

vector<vector< double > > computeVocab(Pool pool, vector<string> fields, int k) {
  standard::Algorithm* mat_aggregator = standard::AlgorithmFactory::create("PoolMatAggregator", "fields", fields);
  mat_aggregator->input("input").set(pool);
  cv::Mat kmeans_data, labels, centers;
  mat_aggregator->output("output").set(kmeans_data);
  mat_aggregator->compute();
  
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
  bool computeMeans = false;
  if (argc > 1 && string(argv[1]).compare("--computeMeans") == 0) {
    if (argc < 4){
      cout << "Not enough arguments" << endl;
      return -1;
    }
    else {
      computeMeans = true;
    }
  }

  
  essentia::init();
	
  AlgorithmFactory& factory = AlgorithmFactory::instance();
  
  Algorithm* audio = factory.create("JackRingBuffer", "client_name", "essentia", "bufferSize", 16384);
  SourceBase& source = audio->output("signal");
  connect(audio->output("time"), NOWHERE);

  RTLowlevelDescriptors *lowlevel = new RTLowlevelDescriptors();
  lowlevel->createNetwork(source);
  Pool pool;
  lowlevel->pool = pool;
  
  Network network(audio, true);
  network.run();
  
  
  if (computeMeans) {
    cout << "Computing " << atoi(argv[3]) << " Means..." << endl;
    cout << "Writing to " << argv[2] << endl;
    ofstream vocabfile;
    vocabfile.open(argv[2]);
  
    Object vocab_json;
    for (unsigned int i = 0; i < lowlevel->namespaces.size(); i++) {
      Object feature_json;
        
      vector<vector<double> > vocab = computeVocab(lowlevel->pool,
        lowlevel->pool.descriptorNames(lowlevel->namespaces[i]), atoi(argv[3]));
      
      for (unsigned int j = 0; j < vocab.size(); j++) {
        Array sample_json;
        for (unsigned int k = 0; k < vocab[j].size(); k++) {
          sample_json << vocab[j][k];
        }
        stringstream convert;
        convert << j;
        feature_json << convert.str() << sample_json;
      }
      vocab_json << lowlevel->namespaces[i] << feature_json;
    }
      
    vocabfile << vocab_json.json() << endl;
    vocabfile.close();
  }

  return 0;
}
