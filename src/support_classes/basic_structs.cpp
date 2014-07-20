//
//  basic_structs.cpp
//  edtw
//
//  Created By <Anonymous> on 1/24/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include "basic_structs.hpp"

//static const data_t kSPOTPARAMS_DEFAULT_CUTOFF				= INFINITY;
//static const data_t kSPOTPARAMS_DEFAULT_WARP				= 0;
//static const SpotterType kSPOTPARAMS_DEFAULT_SPOTTER_TYPE	= EUCLIDEAN;
//static const dims_t kSPOTPARAMS_DEFAULT_DIMENSIONS			= 1;

//SpotterParams SpotterParams_construct() {
//	SpotterParams p;
//	p.cutoff = kSPOTPARAMS_DEFAULT_CUTOFF;
//	p.scaleFactor = kSPOTPARAMS_DEFAULT_SCALE_FACTOR;
//	p.maxWarp = kSPOTPARAMS_DEFAULT_WARP;
//	p.spotType = kSPOTPARAMS_DEFAULT_SPOTTER_TYPE;
//	p.nDims = kSPOTPARAMS_DEFAULT_DIMENSIONS;
//	p.timeMatchThresh = kSPOTPARAMS_DEFAULT_TIME_MATCH_THRESH;
//	return p;
//}

std::string SpotterType_toString(SpotterType t) {
	switch(t) {
		case EDW_OPTIMAL:
			return "EDW_Opt";
		case EDW_AGGRESSIVE:
			return "EDW";
		case DDW_OPTIMAL:
			return "DDW_Opt";
		case DDW_AGGRESSIVE:
			return "DDW";
		case ED_AGGRESSIVE:
			return "ED";
		case ED_CAUTIOUS:
			return "ED_Cau";
		case DTW_AGGRESSIVE:
			return "DTW";
		case DTW_CAUTIOUS:
			return "DTW_Cau";
		case US_AGGRESSIVE:
			return "US";
		case US_CAUTIOUS:
			return "US_Cau";
		case SWM_AGGRESSIVE:
			return "SWM";
		case SWM_CAUTIOUS:
			return "SWM_Cau";
		case SPRING_OPTIMAL:
			return "SPRING_Opt";
		case SPRING_AGGRESSIVE:
			return "SPRING";
		default:
			return "INVALID_SPOTTER_TYPE";
	}
}

SpotterParams::SpotterParams(SpotterType spotType,
							 ReportCriteria reportCriteria,
							 data_t cutoff,
							 steps_t maxWarp,
							 float scaleFactor,
							 float timeMatchThresh,
							 dims_t nDims) :
	spotType(spotType),
	reportCriteria(reportCriteria),
	cutoff(cutoff),
	maxWarp(maxWarp),
	scaleFactor(scaleFactor),
	timeMatchThresh(timeMatchThresh),
	nDims(nDims)
{}

//SpotterParams SpotterParams_construct(SpotterType spotType,
//									  ReportCriteria reportCriteria,
//									  data_t cutoff, steps_t maxWarp,
//									  float scaleFactor, float timeMatchThresh,
//									  dims_t nDims) {
//	SpotterParams p;
//	p.spotType = spotType;
//	p.reportCriteria = reportCriteria;
//	p.cutoff = cutoff;
//	p.maxWarp = maxWarp;
//	p.scaleFactor = scaleFactor;
//	p.timeMatchThresh = timeMatchThresh;
//	p.nDims = nDims;
//	return p;
//}

std::string SpotterParams_toString(SpotterParams p) {
	char buff[256];
	sprintf(buff, "%d, %.3f, %d, %.3f",
//			SpotterType_toString(p.spotType).c_str(),
			p.spotType,
			p.cutoff, p.maxWarp, p.scaleFactor);
	std::string retVal(buff);
	return retVal;
}

//TODO write more comprehensive serialization rather than
//relying on default values
SpotterParams SpotterParams_fromString(std::string line) {
	SpotterParams p;
	sscanf(line.c_str(), "%d, %lf, %hd, %f",
//			SpotterType_toString(p.spotType).c_str(),
		   &p.spotType,
		   &p.cutoff, &p.maxWarp, &p.scaleFactor);
	return p;
}

std::string SpotterParams_toFileName(SpotterParams p) {
	char buff[256];
	sprintf(buff, "t%dt_w%dw_s%.2fs_c%.2fc.csv", //wrap fields with unique char for easy segmentation
			p.spotType, p.maxWarp, p.scaleFactor, p.cutoff);
	std::string retVal(buff);
	return retVal;
}

std::vector<SpotterParams> readSpotterParams(std::string fileName) {
	std::vector<SpotterParams> params = std::vector<SpotterParams>();
	
	printf("searching for SpotterParams in file: %s\n", fileName.c_str());
	
	// create a SpotterParams from each line in the file. If the lines
	// aren't valid representations of SpotterParams, this will completely
	// crash and burn
	std::ifstream file(fileName.c_str());
	std::string line;
	while(getline(file, line) ) {
		SpotterParams p = SpotterParams_fromString(line);
		params.push_back(p);
	}
	
	file.close();
	return params;
}
void writeSpotterParams(std::string fileName, std::vector<SpotterParams> objs) {
	std::ofstream outFile;
	outFile.open(fileName);
	if (! outFile.is_open()) {
		printf("WARNING: couldn't open output file %s\n", fileName.c_str());
		return;
	}
	
	if (objs.empty()) {
		printf("WARNING: writeSpotterParams: received empty vector of objects\n");
		outFile << "NONE";
	} else {
		for (unsigned int i = 0; i < objs.size()-1; i++) {
			outFile << SpotterParams_toString(objs[i]) << "\n";
		}
		outFile << SpotterParams_toString(objs[objs.size() -1]); // no trailing newline
	}
	outFile.close();
}

bool isSequenceValid(Sequence s) {
	return (s.nDims > 0) && (s.nSamples > 0) && (s.data.size() > 0);
}

void Sequence_populate(data_t *data, length_t nSamples, dims_t nDims,
					   Sequence *seq) {
	seq->data = std::vector<data_t>(data, data+(nSamples*nDims));
	seq->nSamples = nSamples;
	seq->nDims = nDims;
	seq->isValid = isSequenceValid(*seq);
}

Sequence Sequence_construct(data_t *data, length_t nSamples, dims_t nDims) {
	Sequence seq;
	Sequence_populate(data, nSamples, nDims, &seq);
	return seq;
}

Template Template_construct(data_t *data, length_t nSamples, dims_t nDims, std::string label) {
	Template ref;
	Sequence_populate(data, nSamples, nDims, &ref);
	ref.label = label;
	return ref;
}

Stream Stream_construct(data_t *data, length_t nSamples, dims_t nDims,
						std::string name, std::vector<Subsequence> answers) {
	Stream s;
	Sequence_populate(data, nSamples, nDims, &s);
	s.name = name;
	s.patterns = answers;
	return s;
}