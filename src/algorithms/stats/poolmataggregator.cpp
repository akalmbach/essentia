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
	
	int rows = getNumSamples(input);
	int cols = 0;
	for (int i = 0; i < _fields.size(); i++) {
		cols += getColsForField(input, _fields[i]);
	}
  cout << "rows: " << rows << ", cols: " << cols << endl;

	Mat& output = _output.get();
	output = Mat::zeros(rows, cols, CV_32FC1);
  cout << output.rows << " " << output.cols << endl;

	// TODO: Better names for index vars
	int col = 0;
	for (int i = 0; i < _fields.size(); i++) {
		if (input.contains< vector <vector <Real> > >(_fields[i])) {
			vector< vector<Real> > input_data = input.value< vector <vector<Real> > >(_fields[i]);
			for (int row = 0; row < rows; row++) {
				for (int j = 0; j < input_data[row].size(); j++) {
					output.at<float>(row, col + j) = input_data[row][j];
				}
			}
			col += input_data[0].size();
		}
		else if (input.contains< vector <Real> >(_fields[i])) {
			vector<Real> input_data = input.value< vector<Real> >(_fields[i]);
			for (int row = 0; row < rows; row++) {
				output.at<float>(row, col) = input_data[row];
			}
			col++;
		}
	}

}

int PoolMatAggregator::getColsForField(const Pool& input, string field) {
	int cols = -1;
	if (input.contains< vector <vector <Real> > >(field)) {
		// Values were added as vector<Real>
		vector< vector<Real> > input_data = input.value< vector <vector<Real> > >(field);
		cols = input_data[0].size();
		for (int i = 0; i < input_data.size(); i++) {
			if (input_data[i].size() != cols) {
				throw EssentiaException("PoolMatAggregator requires each intance of a given field to have constant width: " + field);
			}
		}
	}
	else if (input.contains< vector <Real> >(field)) {
		cols = 1;
	}
	else {
		throw EssentiaException("PoolMatAggregator tried to use a field with the wrong type: " + field);
	}
	return cols;
}

int PoolMatAggregator::getNumSamples(const Pool& input) {
	int num_samples = -1;
	for (int i = 0; i < _fields.size(); i++) {
		if (input.contains< vector <vector <Real> > >(_fields[i])) {
			// Values were added as vector<Real>
			if ( (input.value< vector <vector <Real> > >(_fields[i])).size() != num_samples && num_samples != -1 ) 	{
				throw EssentiaException("PoolMatAggregator requires all fields to have the same number of samples: " + _fields[i]);
			}
			if (num_samples == -1) {
				num_samples = input.value< vector <vector <Real> > >(_fields[i]).size();
			}
		}
		else if (input.contains< vector <Real> >(_fields[i])) {
			// Values were added as <Real>
			if ( (input.value< vector <Real> >(_fields[i])).size() != num_samples && num_samples != -1 ) 	{
				throw EssentiaException("PoolMatAggregator requires all fields to have the same number of samples: " + _fields[i]);
			}
			if (num_samples == -1) {
				num_samples = input.value< vector <Real> >(_fields[i]).size();
			}
		}
		else {
			throw EssentiaException("PoolMatAggregator tried to use a field with the wrong type: " + _fields[i]);
		}
	}
	return num_samples;
}

void PoolMatAggregator::configure() {
	_fields = parameter("fields").toVectorString();
}
