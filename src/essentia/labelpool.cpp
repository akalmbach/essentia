#include "labelpool.h"
#include <fstream>
#include <string>

using namespace std;

namespace essentia{
  void LabelPool::setVocab(string filename) {
    // Read the whole file into a buffer
    ifstream vocabfile(filename.c_str(), ios::in | ios::binary);
    if (vocabfile) {
      string json_buffer;
      vocabfile.seekg(0, ios::end);
      json_buffer.resize(vocabfile.tellg());
      vocabfile.seekg(0, ios::beg);
      vocabfile.read(&json_buffer[0], json_buffer.size());
      vocabfile.close();
      // Use jsonxx to parse it
      _json_object.parse(json_buffer);
    }
  }
  
  int LabelPool::addVocabNamespace(string ns) {
    vector<vector<Real> > V;
    if (!_json_object.has<jsonxx::Object>(ns)) {
      cerr << "VOCAB DID NOT CONTAIN NS: " << ns << endl;
      return -1;
    }
    else {
      jsonxx::Object o_ns = _json_object.get<jsonxx::Object>(ns);
      int i = 0;
      ostringstream ss;
      ss << i;
      while (o_ns.has<jsonxx::Array>(ss.str())) {
        vector<Real> entry;
        jsonxx::Array a = o_ns.get<jsonxx::Array>(ss.str());
        vector<jsonxx::Value*> values = a.values();
        for (unsigned int j = 0; j < values.size(); j++) {
          if (values[i]->is<jsonxx::Number>()) {
            entry.push_back(values[i]->get<jsonxx::Number>());
          }
        }
        V.push_back(entry);
        i++;
        ss << i;
      }      
      _vocab_map[ns] = V;
    }
    return 0;
  }
  
  void LabelPool::add(const string& name) {
     // get the namespace out of the name
    int dotidx = name.find(".");
    string ns = name.substr(0, dotidx);
    
    // get the descriptor names for the namespace
    // GUARANTEE ON ORDER?!?!
    vector<string> fields = descriptorNames(ns);
    
    // for each one, check if there is fresh data
    bool allFresh = true;
    for (unsigned int i = 0; i < fields.size(); i++) {
      if (_fresh_fields.find(fields[i]) == _fresh_fields.end() ||
          _fresh_fields[fields[i]] == false ) {
        allFresh = false;
        break;
      }
    }
    
    if (allFresh) {
      // make sure the namespace is parsed in the vocab
      if (_vocab_map.find(ns) == _vocab_map.end()) {
        addVocabNamespace(ns);
      }
      // get the right vector<vector<Real>> out of the vocab_map
      vector<vector<Real> > vocab = _vocab_map[ns];
      vector<Real> sample;
      for (unsigned int i = 0; i < fields.size(); i++) {
        if (_poolReal.find(fields[i]) != _poolReal.end()) {
          vector<Real> allsamples = _poolReal.find(fields[i])->second;
          Real singlesample = allsamples.back();
          sample.push_back(singlesample);
        }
        else if (_poolVectorReal.find(fields[i]) != _poolVectorReal.end() ) {
          vector<vector<Real > > allsamples = _poolVectorReal.find(fields[i])->second;
          vector<Real> vectorsample = allsamples.back();
          sample.insert(sample.end(), vectorsample.begin(), vectorsample.end());
        }
        else {
          // WHAT NOW?
        }
      }
      int label = nearestNeighborEuclidean(sample, vocab);
      Pool::add("labels."+ns, label);
      for (unsigned int i = 0; i < fields.size(); i++) {
        _fresh_fields[fields[i]] = false;
      }
    }
    else {
      _fresh_fields[name] = true;
    }
  }
  
  void LabelPool::add(const std::string& name, const Real& value, bool validityCheck) {
    Pool::add(name, value);
    LabelPool::add(name);
  }
  
  void LabelPool::add(const std::string& name, const std::vector<Real>& value, bool validityCheck) {
    Pool::add(name, value);
    LabelPool::add(name);
  }
  
  int LabelPool::nearestNeighborEuclidean(vector<Real> sample, vector<vector<Real> > vocab) {
    if (sample.size() != vocab[0].size()) {
      return -1;
    }
    int best_i = -1;
    double best_dist = -1;
    for (unsigned int i = 0; i < vocab.size(); i++) {
      double dist = 0;
      for (unsigned int j = 0; j < sample.size(); j++) {
        double diff = sample[j] - vocab[i][j];
        dist += diff*diff;
      }
      if (best_dist < 0 || dist < best_dist) {
        best_i = i;
        best_dist = dist;
      }
    }
    return best_i;
  }
}
