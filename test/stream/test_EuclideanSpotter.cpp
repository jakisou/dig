//
//  test_EuclideanSpotter.cpp
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <math.h>
#include <assert.h>
#include "test_EuclideanSpotter.hpp"
#include "EuclideanSpotter.hpp"
#include "StreamMonitor.hpp"
#include "array_utils.h"
#include "type_defs.h"
#include "basic_structs.hpp"
#include "testUtils.hpp"
#include "ucr_funcs.h"

static const SpotterType kSPOT_TYPE = EDW_OPTIMAL;

void test_EucSpotter_all() {
	
	printf("================================ Testing Euclidean Spotter\n");
	
	EucSpotter_IdenticalSubsequence();
	EucSpotter_IdenticalSubsequenceInitialXOfZero();
	EucSpotter_IdenticalSubsequenceInitialQOfZero();
	EucSpotter_IdenticalSubsequenceInitialQandXOfZero();
	EucSpotter_PositiveOffset();
	EucSpotter_NegativeOffset();
	EucSpotter_AmplitudeScaling();
	EucSpotter_PositiveOffsetAndAmplitudeScaling();
	EucSpotter_NegativeOffsetAndAmplitudeScaling();
	EucSpotter_OffsetAndAmplitudeScalingMultipleMatches();
	
	printf("================================ End Euclidean Spotter Test: Success\n");
}

void EucSpotter_IdenticalSubsequence() {
	length_t queryLen = 3;
	length_t streamLen = 4;
	data_t q[] = {1,2,3};
	data_t x[] = {1,2,3,0};
//	znormalize(x, streamLen); // this un-breaks it...
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
//	exit(1);
	if (! pass) {
		assert(0);
	}
}


void EucSpotter_IdenticalSubsequenceInitialXOfZero() {
	length_t queryLen = 3;
	length_t streamLen = 8;
	data_t q[] = {1,2,3};
	data_t x[] = {0,-1, 1,2,3 ,0,1,1};
	
	tick_t trueTs = 2;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_IdenticalSubsequenceInitialQOfZero() {
	length_t queryLen = 3;
	length_t streamLen = 9;
	data_t q[] = {0,1,2};
	data_t x[] = {3,2,2, 0,1,2 ,4,-1,0};
	
	tick_t trueTs = 3;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}
void EucSpotter_IdenticalSubsequenceInitialQandXOfZero() {
	length_t queryLen = 4;
	length_t streamLen = 6;
	data_t q[] = {0,1,2,3};
	data_t x[] = {0,1,2,3 ,4,-1,0};
	
	tick_t trueTs = 0;
	tick_t trueTe = 3;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_PositiveOffset() {
	length_t queryLen = 4;
	length_t streamLen = 9;
	data_t q[] = {0,3,2,1};
	data_t x[] = {0, 5,8,7,6 ,6,5,7,0};
	
	tick_t trueTs = 1;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_NegativeOffset() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {0,3,3,0};
	data_t x[] = {0,-5, -5,-2,-2,-5 ,-4,-3,-3,-5,-6,0};
	data_t cutoff = 0.001;
	
	tick_t trueTs = 2;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_AmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,3,2,-1};
	data_t x[] = {1,3,2, 3,9,6,-3 ,1,3,2,0,5,0};
	
	tick_t trueTs = 3;
	tick_t trueTe = 6;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_PositiveOffsetAndAmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 14;
	data_t q[] = {1,-1,0,1};
	data_t x[] = {1,-1,0,2, 3,9,6,-3 ,14,6,10,14, 5,0};
	
	tick_t trueTs = 8;
	tick_t trueTe = 11;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_NegativeOffsetAndAmplitudeScaling() {
	length_t queryLen = 5;
	length_t streamLen = 13;
	data_t q[] = {1,3,5,2,4};
	data_t x[] = {1,-1,0,2, -8,-4,0,-6,-2 ,1,0,9,0};
	
	tick_t trueTs = 4;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}

void EucSpotter_OffsetAndAmplitudeScalingMultipleMatches() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,2,3,4};
	data_t x[] = {5, 1,2,3,4, 1,2,3,4, -5,-10,-15};
	
	tick_t trueTs = 5;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE);
	if (! pass) {
		assert(0);
	}
}