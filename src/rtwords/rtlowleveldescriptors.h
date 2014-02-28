#ifndef RT_LOWLEVEL_DESCRIPTORS_H
#define RT_LOWLEVEL_DESCRIPTORS_H

#include "essentia/streaming/sourcebase.h"
#include "essentia/pool.h"
#include "essentia/types.h"
#include "essentia/algorithm.h"
#include "essentia/scheduler/network.h"
#include "essentia/streaming/streamingalgorithm.h"
#include "essentia/algorithmfactory.h"
#include "essentia/streaming/algorithms/poolstorage.h"

#include "essentia/essentiamath.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;
using namespace scheduler;

class Field {
public:
 Field(string name, int start, int size) {
   name = name;
   start = start;
   size = size;
 }
 int end() {return start + size}
 string name;
 int start;
 int size;
}

class NameSpace {
public:
 NameSpace(string s) {
   name = s;
 }
 string name;
 vector<Field> fields;
}

class RTLowlevelDescriptors {

 public:
	RTLowlevelDescriptors() {}
	
 	vector<NameSpace> namespaces; 

 	void createNetwork(SourceBase& source);
    Pool pool;
 	
 private:
	string vocabfile;
	void addOutput(SourceBase& feature, string nameSpace, string name);
 };

 #endif
