/*
 * This file was created by Arnold Kalmbach - Hopefully it is useful to
 * someone, but don't blame if it breaks anything!
 * 
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
 * 
 */

#ifndef ESSENTIA_POOLMATAGGREGATOR_H
#define ESSENTIA_POOLMATAGGREGATOR_H

// TODO: Include OpenCV
#include <set>
#include <opencv/cv.h>
#include "algorithm.h"
#include "pool.h"

namespace essentia {
namespace standard {

class PoolMatAggregator : public Algorithm {

 protected:
  Input<Pool> _input;
  Output<cv::Mat> _output;

 public:
  PoolMatAggregator() {
    declareInput(_input, "input", "the input pool");
    declareOutput(_output, "output", "a Mat where each row is a vector from the pool");
  }

  void declareParameters() {
	declareParameter("field", "The name of the input pool's field which will be used in the Mat.", "", "");
	//declareParameter("matType", "the OpenCV type for the Mat to be created.", CV_32FC1);
  }

  void compute();
  void configure();

  static const char* name;
  static const char* description;

};

} // namespace standard
} // namespace essentia
#endif //ESSENTIA_POOLMATAGGREGATOR_H
