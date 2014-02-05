#include "const.h"

using namespace essentia;
using namespace standard;

const char* Const::name = "Const";
const char* Const::description = DOC("This algorithm outputs 1");

void Const::configure() {
  _value = parameter("value").toReal();
}

void Const::compute() {
  Real& c = _const.get();
  c = _value;
}
