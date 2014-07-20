//
//  test_EuclideanSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <math.h>
#include <assert.h>
#include "test_EuclideanSearcher.hpp"
#include "EuclideanSearcher.hpp"
#include "StreamMonitor.hpp"
#include "array_utils.h"
#include "type_defs.h"
#include "basic_structs.hpp"
#include "testUtils.hpp"

static const SpotterType kSPOT_TYPE_AGGRESSIVE = ED_AGGRESSIVE;
static const SpotterType kSPOT_TYPE_CAUTIOUS = ED_CAUTIOUS;

void test_EucSearcher_all() {
	
	printf("================================ Testing Euclidean Searcher\n");
	
	EucSearcher_Aggressive_SubsequenceAtStart();
	EucSearcher_Cautious_SubsequenceAtStart();
	EucSearcher_Aggressive_SubsequenceAfterStart();
	EucSearcher_Cautious_SubsequenceAfterStart();
	EucSearcher_Aggressive_MatchesGreedily();
	EucSearcher_Cautious_MatchesOptimally();
	EucSearcher_Aggressive_MultipleConsecutiveMatches();
	EucSearcher_Cautious_MultipleConsecutiveMatches();
	EucSearcher_Aggressive_MultipleMatches();
	EucSearcher_Cautious_MultipleMatches();
	
	printf("================================ End Euclidean Searcher Test: Success\n");
}

void EucSearcher_Aggressive_SubsequenceAtStart() {
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

void EucSearcher_Cautious_SubsequenceAtStart() {
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

void EucSearcher_Aggressive_SubsequenceAfterStart() {
//	printf("--------EucSearcher_Aggressive_SubsequenceAfterStart--------\n");
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

void EucSearcher_Cautious_SubsequenceAfterStart() {
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

void EucSearcher_Aggressive_MatchesGreedily() {
//	printf("--------EucSearcher_Aggressive_MatchesGreedily--------\n");
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

void EucSearcher_Cautious_MatchesOptimally() {
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

void EucSearcher_Aggressive_MultipleConsecutiveMatches() {
//	printf("--------EucSearcher_Aggressive_MultipleConsecutiveMatches--------\n");
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

void EucSearcher_Cautious_MultipleConsecutiveMatches() {
//	printf("--------EucSearcher_Aggressive_MultipleConsecutiveMatches--------\n");
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

void EucSearcher_Aggressive_MultipleMatches() {
//	printf("--------EucSearcher_Aggressive_MultipleConsecutiveMatches--------\n");
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

void EucSearcher_Cautious_MultipleMatches() {
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
