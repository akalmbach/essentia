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

#include "poolmataggregator.h"
#include "algorithmfactory.h"
#include "essentiamath.h"
#include "essentiautil.h"
#include "tnt/tnt2essentiautils.h"

using namespace std;
using namespace essentia;
using namespace standard;
using namespace cv;

const char* PoolMatAggregator::name = "PoolMatAggregator";

const char* PoolMatAggregator::description = DOC("This algorithm performs aggregation on a pool and places the results of the aggregation into an OpenCV Mat, as an interface to the built-in machine learning and linear algebra facilities contained there.\n"
  "The pool must be a vector <Real> Pool, and each vector <Real> will be placed into a row of the output Mat.\n");

void PoolMatAggregator::compute() {
        const Pool& input = _input.get();
	Mat& output = _output.get();
        // TODO: Does this really have to be only a vector<Real> pool?
        if ( input.contains< vector <vector <Real> > >(_field) ) {
            // Get value in input(_field) as a vector< vector<Real> >
            vector< vector<Real> > input_data = input.value< vector <vector<Real> > >(_field);
            // Initialize output to num rows = outer vector size, num cols = inner vector size
            int rows = input_data.size();
            int cols = input_data[0].size();
            output(rows, cols, CV_32FC1);
        
            // Iterate over the elements, cast them as float
            // and put them in the right place in the output
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    // TODO: Handle badly formatted fields where this will go out of range!
                    output.at<float>(r, c) = (float) input_data[r][c];
                }
            }
        }
        else {
            // TODO: Throw an exception of some kind
        }
}

void PoolMatAggregator::configure() {
	_field = parameter("field").toString();
}
