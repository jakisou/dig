//
//  test_DtwSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//


#include <math.h>
#include <assert.h>
#include "test_DtwSearcher.hpp"
#include "DtwSearcher.hpp"
#include "StreamMonitor.hpp"
#include "array_utils.h"
#include "type_defs.h"
#include "basic_structs.hpp"
#include "testUtils.hpp"

#define PRINT_TEST_NAMES

static const SpotterType kSPOT_TYPE_AGGRESSIVE = DTW_AGGRESSIVE;
static const SpotterType kSPOT_TYPE_CAUTIOUS = DTW_CAUTIOUS;

void test_DtwSearcher_all() {
	
	printf("================================ Testing Dtw Searcher\n");
	
	DtwSearcher_Aggressive_SubsequenceAtStart();
	DtwSearcher_Cautious_SubsequenceAtStart();
	DtwSearcher_Aggressive_SubsequenceAfterStart();
	DtwSearcher_Cautious_SubsequenceAfterStart();
	DtwSearcher_Aggressive_MatchesGreedily();
	DtwSearcher_Cautious_MatchesOptimally();
	DtwSearcher_Aggressive_MultipleConsecutiveMatches();
	DtwSearcher_Cautious_MultipleConsecutiveMatches();
	DtwSearcher_Aggressive_MultipleMatches();
	DtwSearcher_Cautious_MultipleMatches();
	
	DtwSearcher_Warp3();
	
	printf("================================ End Dtw Searcher Test: Success\n");
}

inline void printTestName(const char * testName) {
#ifdef PRINT_TEST_NAMES
	printf("--------%s--------\n", testName);
#endif
}

void DtwSearcher_Aggressive_SubsequenceAtStart() {
	length_t queryLen = 3;
	length_t streamLen = 4;
	data_t q[] = {1,2,3};
	data_t x[] = {-1,0,1,0};
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Cautious_SubsequenceAtStart() {
	printTestName("DtwSearcher_Cautious_SubsequenceAtStart");
	length_t queryLen = 3;
	length_t streamLen = 6;
	data_t q[] = {1,2,3};
	data_t x[] = {3,4,5,-2,4,1};
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_CAUTIOUS);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Aggressive_SubsequenceAfterStart() {
	printTestName("DtwSearcher_Aggressive_SubsequenceAfterStart");
	length_t queryLen = 3;
	length_t streamLen = 6;
	data_t q[] = {3,1,2};
	data_t x[] = {-1,4,3,1,2,-2};
	data_t cutoff = 1;
	
	tick_t trueTs = 2;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Cautious_SubsequenceAfterStart() {
	length_t queryLen = 3;
	length_t streamLen = 8;
	data_t q[] = {2,1,3};
	data_t x[] = {-1,4, 0,-1,1.1, -2,-3,91};
	data_t cutoff = 10;
	
	tick_t trueTs = 2;
	tick_t trueTe = 4;
	data_t trueDist = 0.000755;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_CAUTIOUS, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Aggressive_MatchesGreedily() {
	printTestName("DtwSearcher_Aggressive_MatchesGreedily");
	length_t queryLen = 4;
	length_t streamLen = 7;
	data_t q[] = {2,1,3,1};
	data_t x[] = {13,11,12,11,13,11, 5};
	data_t cutoff = 2;
	
	tick_t trueTs = 0;
	tick_t trueTe = 3;
	data_t trueDist = .72727;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Cautious_MatchesOptimally() {
	length_t queryLen = 4;
	length_t streamLen = 13;
	data_t q[] = {2,1,3,1};
	data_t x[] = {4,-7,2, 13,11,12,11,13,11, 5,7,-2,6};
	data_t cutoff = 10;
	
	tick_t trueTs = 5;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_CAUTIOUS, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Aggressive_MultipleConsecutiveMatches() {
	printTestName("DtwSearcher_Aggressive_MultipleConsecutiveMatches");
	length_t queryLen = 4;
	length_t streamLen = 11;
	data_t q[] = {2,1,3,1};
	data_t x[] = {2,1,3,1, 12,11,13,11, 13,11,5};
	data_t cutoff = 3;
	
	tick_t trueTs = 4;
	tick_t trueTe = 7;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Cautious_MultipleConsecutiveMatches() {
	printTestName("DtwSearcher_Aggressive_MultipleConsecutiveMatches");
	length_t queryLen = 4;
	length_t streamLen = 15;
	data_t q[] = {2,1,3,1};
	data_t x[] = {4,-7,-1, 2,1,3,1, 12,11,13,11, 5,-2,4,1};
	data_t cutoff = 4;
	
	tick_t trueTs = 7;
	tick_t trueTe = 10;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_CAUTIOUS, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Aggressive_MultipleMatches() {
	printTestName("DtwSearcher_Aggressive_MultipleConsecutiveMatches");
	length_t queryLen = 4;
	length_t streamLen = 17;
	data_t q[] = {2,1,3,1};
	data_t x[] = {4,-7,-1, 2,1,3,1, 7,4,1, 13,11,12,11,13,11, 5};
	data_t cutoff = 2;
	
	tick_t trueTs = 10;
	tick_t trueTe = 13;
	data_t trueDist = .72727;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Cautious_MultipleMatches() {
	printTestName("DtwSearcher_Cautious_MultipleMatches");
	length_t queryLen = 4;
	length_t streamLen = 18;
	data_t q[] = {2,1,3,1};
	data_t x[] = {4,-7,-1, 2,1,3,1, 7,4,1, 12,11,13,11, 5,-2,4,1};
	data_t cutoff = 4;
	
	tick_t trueTs = 10;
	tick_t trueTe = 13;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_CAUTIOUS, cutoff);
	if (! pass) {
		assert(0);
	}
}

void DtwSearcher_Warp3() {
	printTestName("DtwSearcher_Warp3");
	length_t queryLen = 9;
	length_t streamLen = 14;
	data_t q[] = {1, 3,3,3,3, 4,4,1,2};
	data_t x[] = {9,-7, 11,11,11,11,13,14,11,11,12, 10,11,-13};
	data_t cutoff = 1;
	steps_t warp = 33;
	
	tick_t trueTs = 0;
	tick_t trueTe = 8;		 //it doesn't actually match up the way you think it
	data_t trueDist = .9514; //should because q and x are different shapes
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}
