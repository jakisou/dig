//
//  Subsequence.h
//  StreamMain
//
//  Created by Davis blalock on 8/2/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef StreamMain_Subsequence_h
#define StreamMain_Subsequence_h

#include <string>
#include <vector>
#include "math.h"
#include "type_defs.h"

static const data_t kMIN_VALID_DIST = -.0001; // not 0 cuz of floating pt error
static const tick_t kINVALID_LATENCY = -99999;
//static const char* kDEFAULT_LABEL = "DEFAULT_LBL";
static const char* kDEFAULT_LABEL = "";

/** @class A class encapsulating a subsequence of a time series */
class Subsequence {
public:
	tick_t ts;		///< The start time (index) of the subsequence
	tick_t te;		///< The stop time (index) of the subsequence
	tick_t latency; ///< The latency with which this subsequence was reported
	data_t dist;	///< The distance between this subsequence and its nearest neighbor
	std::string label; ///< The class label of this subsequence
	
	Subsequence();
	Subsequence(tick_t ts, tick_t te, data_t dist, std::string label);
	
	/** Whether or not this subsequence "matches" another subsequence; two
	 * sequences that overlap sufficiently and have the same class match.
	 */
	bool matches(const Subsequence& referenceSeq, double startThresh=.5, 
				 double endThresh=.5);
	
	/** Whether this subsequence could be used as an "answer" when running
	 * tests to assess the accuracy of different algorithms */
	inline bool isValidAnswer() const {
		return (! label.empty() ) && te > ts && te > 0 && dist < INFINITY;
	}

	/** Whether this subsequence is meaningful (i.e., has a class, nonnegative
	 * start time, etc.) */
	inline bool isValid() const {
		return (! label.empty() ) && te > ts && te > 0 &&
			dist >= kMIN_VALID_DIST && dist < INFINITY;
	}
	
	bool operator==(const Subsequence &other) const;
	bool operator!=(const Subsequence &other) const;
	Subsequence& operator=(const Subsequence &other);
};

// File I/O
Subsequence subsequenceFromString(std::string line);
std::vector<Subsequence> readSubsequences(std::string fileName);
void writeSubsequences(std::string filename, std::vector<Subsequence> subs);

//operator overloads
std::ostream& operator<<(std::ostream &strm, const Subsequence &s);

#endif
