//
//  Subsequence.cpp
//  StreamMain
//
//  Created by Davis blalock on 8/2/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <sstream>
#include <fstream>
#include <math.h>
#include "Subsequence.hpp"
#include "type_defs.h"

//#define MATCH_IGNORE_TIMING

Subsequence::Subsequence():
	ts(kINVALID),
	te(kINVALID),
	latency(kINVALID_LATENCY),
	dist(kINVALID),
	label(kDEFAULT_LABEL) {}

Subsequence::Subsequence(tick_t tstart, tick_t tend, data_t distance, std::string labelStr):
	ts(tstart),
	te(tend),
	latency(kINVALID_LATENCY),
	dist(distance),
	label(labelStr) {}

Subsequence& Subsequence::operator=(const Subsequence &other) {
	if (this != &other) {
		ts = other.ts;
		te = other.te;
		latency = other.latency;
		dist = other.dist;
		label = other.label;
	}
	return *this;
}


std::ostream& operator<<(std::ostream &strm, const Subsequence &s) {
	return strm << s.label
		<< " " << s.ts
		<< " " << s.te
		<< " " << s.dist
		<< " " << s.latency;
}

bool Subsequence::operator==(const Subsequence &other) const {
	return (label == other.label && 
		ts == other.ts &&
		te == other.te &&
		latency == other.latency &&
		dist == other.dist);
}
bool Subsequence::operator!=(const Subsequence &other) const {
	return !(*this == other);
}

/** Read in a subsequence written using this class's operator<<; note that
 this will crash horribly if the string provided isn't formatted properly,
 or if the label is for some reason longer than 512 characters */
Subsequence subsequenceFromString(std::string line) {
//	std::string sep = " ";
//
//	std::ostringstream ss;
//	ss << "%s" << sep << "%ld" << sep << "%ld" << sep << "%f";
//	std::string formatStr = ss.str();
	
	Subsequence seq;
	char label[512];
	double tsAsDouble, teAsDouble;
//	sscanf(line.c_str(), "%s %ld %ld %lf", label, &seq.ts, &seq.te, &seq.dist);
	sscanf(line.c_str(), "%s %lf %lf %lf", label, &tsAsDouble, &teAsDouble, &seq.dist);
	seq.ts = static_cast<tick_t>( round(tsAsDouble) );
	seq.te = static_cast<tick_t>( round(teAsDouble) );
	seq.label = std::string(label);
	
	return seq;
}


bool Subsequence::matches(const Subsequence& referenceSeq, double startThresh, 
						  double endThresh) {
//	std::cout << "testing if subseqs match using thresh " << startThresh << std::endl;
//	std::cout << *this << std::endl;
//	std::cout << referenceSeq << std::endl;
	
	// subsequences don't match if they don't have the same class
	if (! (label == referenceSeq.label) ) return false;
	
	//if both subsequences have a defined dist, these distances must match
	bool distsValid = (dist >= 0) && (referenceSeq.dist >= 0);
	bool distsDifferent = dist != referenceSeq.dist;
	if (distsValid && distsDifferent ) {
		printf("Subsequence::matches(): no match, distances different\n");
		return false;
	}

#ifdef MATCH_IGNORE_TIMING
//	printf("subsequences match at ts, te = %d, %d\n", ts, te);
	return true;
#endif

	//compute lengths of subsequences
	double refLen = referenceSeq.te - referenceSeq.ts + 1;
	
	//determine percent error for start time and end time
	double startErr = (ts - referenceSeq.ts) / refLen;
	double endErr	= (te - referenceSeq.te) / refLen;
	
//	printf("startErr, stopErr = %lf, %lf\n", startErr, endErr);
	
	// check if the percent errors are within the acceptable range
	bool timesMatch = (fabs(startErr) <= startThresh) && (fabs(endErr) <= endThresh);
	
	return timesMatch;
}


std::vector<Subsequence> readSubsequences(std::string fileName) {
	std::vector<Subsequence> seqs = std::vector<Subsequence>();
	
	printf("searching for Subsequences in file: %s\n", fileName.c_str());
	
	// create a subsequence from each line in the file. If the lines
	// aren't valid representations of subsequences, this will completely
	// crash and burn
	std::ifstream file(fileName.c_str());
	std::string line;
	while(getline(file, line) ) {
		Subsequence seq = subsequenceFromString(line);
		if (seq.isValidAnswer()) {
			seqs.push_back(seq);
		} else {
			std::cout << "read invalid subseq: " << seq << std::endl;
		}
	}
	
	file.close();
	return seqs;
}

void writeSubsequences(std::string fileName, std::vector<Subsequence> seqs) {
	std::ofstream outFile;
	outFile.open(fileName);
	if (! outFile.is_open()) {
		printf("WARNING: couldn't open output file %s\n", fileName.c_str());
		return;
	}
	
	if (seqs.empty()) {
		printf("WARNING: writeSubsequences: received empty vector of subsequences\n");
		outFile << "NONE";
	} else {
		for (unsigned int i = 0; i < seqs.size()-1; i++) {
			outFile << seqs[i] << "\n";
		}
		outFile << seqs[seqs.size() -1]; // no trailing newline
	}
	outFile.close();
}
