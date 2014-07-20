//
//  TestResult.h
//  StreamMain
//
//  Created by Davis blalock on 8/5/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef StreamMain_TestResult_h
#define StreamMain_TestResult_h

#include <string.h>
#include "type_defs.h"
#include "basic_structs.hpp"

//forward declarations
inline double calcF1score(double precision, double recall);

/** @class A class encupsulating the results of a test, meaning a search through
 * a stream with known events */
class TestResult {
public:
	SpotterParams params;	///< The parameters used to search the stream
	std::string name;		///< The (user-defined) name of the test
	long cpuTime;			///< How long the test took in ms of CPU time
	dist_calls_t distCalls;	///< How many times the distance subroutine was called
	double meanLatency;		///< The average latency between a match first being seen and reported
	double stdLatency;		///< The std dev of how long between a match first being seen and reported
	double pruningPower;	///< The actual calls to the distance function / possible calls
	
	// override addition and multiplication so we can easily take averages
	TestResult & operator+=(const TestResult &rhs);
//	TestResult operator+(const TestResult &other) const;
	TestResult & operator*=(double d);
//	TestResult operator*(double d) const;
	
//	TestResult();
	
	void setPrecision(double p) {
		precision = p;
		updateF1score();
	}
	void setRecall(double r) {
		recall = r;
		updateF1score();
	}
	double getPrecision() const {
		return precision;
	}
	double getRecall() const {
		return recall;
	}
	double getF1score() const {
		return f1score;
	}
private:
	double precision;		///< Precision in reporting matches
	double recall;			///< Recall in reporting matches
	double f1score;			///< F1 Score for combined precision and recall
	
	void updateF1score() {
		f1score = calcF1score(precision, recall);
	}
};

inline double calcF1score(double precision, double recall) {
	if (precision > 0 && recall > 0) {
		return 2*precision*recall / (precision + recall);
	} else {
		return 0;
	}
}

std::ostream& operator<<(std::ostream &strm, const TestResult & tr);

/** Serializes (by appending) a vector of TestResults to a file */
void writeTestResults(std::string fileName, std::vector<TestResult> seqs);

#endif
