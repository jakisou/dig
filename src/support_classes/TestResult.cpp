//
//  TestResult.cpp
//  StreamMain
//
//  Created by Davis blalock on 8/5/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "TestResult.hpp"
#include "type_defs.h"
#include "basic_structs.hpp"

//double meanLatencyFromStart;
//double meanLatencyFromEnd;
//double stdLatencyFromStart;
//double stdLatencyFromEnd;


/** Default constructor: initialize everything to 0 */
//TestResult::TestResult()
//	: name("Unkown Test"), cpuTime(0), distCalls(0), 
//	pruningPower(0), precision(0), recall(0)
////	meanLatencyFromStart(0),
////	meanLatencyFromEnd(0),
////	stdLatencyFromStart(0),
////	stdLatencyFromEnd(0)
//{
//	f1score = calcF1score(precision, recall);
//}

/** add together each field; this is useful when we subsequently divide
 them and get an average */
TestResult & TestResult::operator+=(const TestResult &rhs) {
	cpuTime += rhs.cpuTime;
	distCalls += rhs.distCalls;
	pruningPower += rhs.pruningPower;
	meanLatency += rhs.meanLatency;
	precision += rhs.precision;
	recall += rhs.recall;
	f1score += rhs.f1score;
	
	return *this;
}
//
///** add together each field; this is useful when we subsequently divide
// them and get an average */
//TestResult TestResult::operator+(const TestResult &other) const {
//	TestResult result = *this;
//	result += other;
//	return result;
//}
//
/** elementwise multiplication of all fields */
TestResult & TestResult::operator*=(double d) {
	cpuTime *= d;
	distCalls *= d;
	pruningPower *= d;
	meanLatency *= d;
	precision *= d;
	recall *= d;
	f1score *= d;
	
	return *this;
}
//
///** elementwise multiplication of all fields */
//TestResult TestResult::operator*(double d) const {
//	TestResult result = *this;
//	result *= d;
//	return result;
//}

std::ostream& operator<<(std::ostream &strm, const TestResult & t) {
	char buff[256];
	sprintf(buff, "%ld, %lld, %.3f, %.3f, %.3f, %.3f",
			t.cpuTime, t.distCalls, t.pruningPower,
			t.getPrecision(), t.getRecall(),
			t.getF1score() );
//			t.meanLatency, t.stdLatency);
	return strm << SpotterParams_toString(t.params) << ", " << buff << ", " << t.name;
}

void writeTestResults(std::string fileName, std::vector<TestResult> seqs) {
	if (seqs.empty()) {
		printf("WARNING: writeSubsequences: received empty vector of subsequences\n");
		return;
	}
	
	std::ofstream outFile;
	outFile.open(fileName, std::ios::app);
	if (! outFile.is_open()) {
		printf("WARNING: couldn't open output file %s\n", fileName.c_str());
		return;
	}
	
	for (unsigned int i = 0; i < seqs.size(); i++) {
		outFile << seqs[i] << "\n";
	}
//	outFile << seqs[seqs.size() -1]; // no trailing newline
	outFile.close();
}

