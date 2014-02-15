#include "visualalgorithm.h"
#include "sourcebase.h"

using namespace std;
using namespace cv;
using namespace essentia;


namespace essentia {
namespace streaming {
    
const char* VisualAlgorithm::name = "VisualAlgorithm";
const char* VisualAlgorithm::description = DOC(
"This algorithm can be used to generate images based on audio features"
);

void VisualAlgorithm::configure() {
	
}

AlgorithmStatus VisualAlgorithm::process() {
  AlgorithmStatus status = acquireData();
  if (status == OK) {
	const vector<Mat>& f1 = _image_in_1.tokens();
	const vector<Mat>& f2 = _image_in_2.tokens();
	const vector<Real>& features = _signal.tokens();
	
	vector<Mat>& output = _image_out.tokens();
	
	output.push_back(proc(f1[0], f2[0], features));
	
	return OK;
  }
  else {
	return status;
  }
}

}
}


