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
	cout << "VA got data" << endl;
	const Mat& f1 = _image_in_1.firstToken();
	const Mat& f2 = _image_in_2.firstToken();
	const vector<Real>& features = _signal.tokens();
	
	Mat& output = _image_out.firstToken(); 
	output = proc(f1, f2, features);
	
	releaseData();
	
	return OK;
  }
  
  else {
	return status;
  }
}

}
}


