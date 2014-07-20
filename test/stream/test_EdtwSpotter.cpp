//
//  test_EdtwSpotter.cpp
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <math.h>
#include <assert.h>
#include "test_EdtwSpotter.hpp"
#include "DDWSpotter.hpp"
#include "StreamMonitor.hpp"
#include "array_utils.h"
#include "type_defs.h"
#include "basic_structs.hpp"
#include "testUtils.hpp"

static const SpotterType kSPOT_TYPE = DDW_OPTIMAL;

void test_EdtwSpotter_all() {
	
	printf("================================ Testing Edtw Spotter\n");
	
	EdtwSpotter_NoWarp_IdenticalSubsequence();
	EdtwSpotter_NoWarp_IdenticalSubsequenceInitialXOfZero();
	EdtwSpotter_NoWarp_IdenticalSubsequenceInitialQOfZero();
	EdtwSpotter_NoWarp_IdenticalSubsequenceInitialQandXOfZero();
	EdtwSpotter_NoWarp_PositiveOffset();
	EdtwSpotter_NoWarp_NegativeOffset();
	EdtwSpotter_NoWarp_AmplitudeScaling();
	EdtwSpotter_NoWarp_PositiveOffsetAndAmplitudeScaling();
	EdtwSpotter_NoWarp_NegativeOffsetAndAmplitudeScaling();
	EdtwSpotter_NoWarp_OffsetAndAmplitudeScalingMultipleMatches();
	
	EdtwSpotter_Warp1_IdenticalSubsequence();
	EdtwSpotter_Warp1_IdenticalSubsequenceInitialXOfZero();
	EdtwSpotter_Warp1_IdenticalSubsequenceInitialQOfZero();
	EdtwSpotter_Warp1_IdenticalSubsequenceInitialQandXOfZero();
	EdtwSpotter_Warp1_PositiveOffset();
	EdtwSpotter_Warp1_NegativeOffset();
	EdtwSpotter_Warp1_AmplitudeScaling();
	EdtwSpotter_Warp1_PositiveOffsetAndAmplitudeScaling();
	EdtwSpotter_Warp1_NegativeOffsetAndAmplitudeScaling();
	EdtwSpotter_Warp1_OffsetAndAmplitudeScalingMultipleMatches();
	
	EdtwSpotter_Warp2_IdenticalSubsequence();
	EdtwSpotter_Warp2_IdenticalSubsequenceInitialXOfZero();
	EdtwSpotter_Warp2_IdenticalSubsequenceInitialQOfZero();
	EdtwSpotter_Warp2_IdenticalSubsequenceInitialQandXOfZero();
	EdtwSpotter_Warp2_PositiveOffset();
	EdtwSpotter_Warp2_NegativeOffset();
	EdtwSpotter_Warp2_AmplitudeScaling();
	EdtwSpotter_Warp2_PositiveOffsetAndAmplitudeScaling();
	EdtwSpotter_Warp2_NegativeOffsetAndAmplitudeScaling();
	EdtwSpotter_Warp2_OffsetAndAmplitudeScalingMultipleMatches();
	
	EdtwSpotter_Warp1_HorzInherit();
	EdtwSpotter_Warp2_HorzInherit();
	EdtwSpotter_Warp3_HorzInherit();
	EdtwSpotter_Warp0_RestrictedHorzInherit();
	EdtwSpotter_Warp1_RestrictedHorzInherit();
	
	EdtwSpotter_Warp1_VertInherit();
	
	printf("================================ End Edtw Spotter Test: Success\n");
}

void EdtwSpotter_NoWarp_IdenticalSubsequence() {
	length_t queryLen = 3;
	length_t streamLen = 4;
	data_t q[] = {1,2,3};
	data_t x[] = {1,2,3,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	
	if (! pass) {
		assert(0);
	}
}


void EdtwSpotter_NoWarp_IdenticalSubsequenceInitialXOfZero() {
	length_t queryLen = 3;
	length_t streamLen = 8;
	data_t q[] = {1,2,3};
	data_t x[] = {0,-1, 1,2,3 ,0,1,1};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 2;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_IdenticalSubsequenceInitialQOfZero() {
	length_t queryLen = 3;
	length_t streamLen = 9;
	data_t q[] = {0,1,2};
	data_t x[] = {3,2,2, 0,1,2 ,4,-1,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 3;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}
void EdtwSpotter_NoWarp_IdenticalSubsequenceInitialQandXOfZero() {
	length_t queryLen = 4;
	length_t streamLen = 6;
	data_t q[] = {0,1,2,3};
	data_t x[] = {0,1,2,3 ,4,-1,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 0;
	tick_t trueTe = 3;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_PositiveOffset() {
	length_t queryLen = 4;
	length_t streamLen = 9;
	data_t q[] = {0,3,2,1};
	data_t x[] = {0, 5,8,7,6 ,6,5,7,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 1;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_NegativeOffset() {
	length_t queryLen = 4;
	length_t streamLen = 10;
	data_t q[] = {0,3,3,0};
	data_t x[] = {0,-5, -5,-2,-2,-5 ,-4,-5,-6,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 2;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_AmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,3,2,-1};
	data_t x[] = {1,3,2, 3,9,6,-3 ,1,3,2,0,5,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 3;
	tick_t trueTe = 6;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_PositiveOffsetAndAmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 14;
	data_t q[] = {1,-1,0,1};
	data_t x[] = {1,-1,0,2, 3,9,6,-3 ,14,6,10,14, 5,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 8;
	tick_t trueTe = 11;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_NegativeOffsetAndAmplitudeScaling() {
	length_t queryLen = 5;
	length_t streamLen = 14;
	data_t q[] = {1,3,5,2,4};
	data_t x[] = {1,-1,0,2, -8,-4,0,-6,-2 ,2,3,0,9,0};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 4;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_NoWarp_OffsetAndAmplitudeScalingMultipleMatches() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,2,3,4};
	data_t x[] = {5, 1,2,3,4, 1,2,3,4, -5,-10,-15};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 5;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_IdenticalSubsequence() {
	length_t queryLen = 3;
	length_t streamLen = 4;
	data_t q[] = {1,2,3};
	data_t x[] = {1,2,3,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	
	if (! pass) {
		assert(0);
	}
}


void EdtwSpotter_Warp1_IdenticalSubsequenceInitialXOfZero() {
	length_t queryLen = 3;
	length_t streamLen = 8;
	data_t q[] = {1,2,3};
	data_t x[] = {0,-1, 1,2,3 ,0,1,1};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 2;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_IdenticalSubsequenceInitialQOfZero() {
	length_t queryLen = 3;
	length_t streamLen = 9;
	data_t q[] = {0,1,2};
	data_t x[] = {3,2,2, 0,1,2 ,4,-1,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 3;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}
void EdtwSpotter_Warp1_IdenticalSubsequenceInitialQandXOfZero() {
	length_t queryLen = 4;
	length_t streamLen = 6;
	data_t q[] = {0,1,2,3};
	data_t x[] = {0,1,2,3 ,4,-1,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 0;
	tick_t trueTe = 3;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_PositiveOffset() {
//	printf("--------EdtwSpotter_Warp1_PositiveOffset--------\n");
	length_t queryLen = 4;
	length_t streamLen = 9;
	data_t q[] = {0,3,2,1};
	data_t x[] = {0, 5,8,7,6 ,6,5,7,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 1;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_NegativeOffset() {
	length_t queryLen = 4;
	length_t streamLen = 10;
	data_t q[] = {0,3,3,0};
	data_t x[] = {0,-5, -5,-2,-2,-5 ,-4,-5,-6,0};
	data_t cutoff = .5;
	steps_t warp = 1;
	
	tick_t trueTs = 2;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_AmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,3,2,-1};
	data_t x[] = {1,3,2, 3,9,6,-3 ,1,3,2,0,5,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 3;
	tick_t trueTe = 6;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_PositiveOffsetAndAmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 14;
	data_t q[] = {1,-1,0,1};
	data_t x[] = {1,-1,0,2, 3,9,6,-3 ,14,6,10,14, 5,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 8;
	tick_t trueTe = 11;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_NegativeOffsetAndAmplitudeScaling() {
	length_t queryLen = 5;
	length_t streamLen = 17;
	data_t q[] = {1,3,5,2,4};
	data_t x[] = {1,-1,0,2, -8,-4,0,-6,-2 ,1,3,-1,2,3, 0,9,0};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 4;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_OffsetAndAmplitudeScalingMultipleMatches() {
	length_t queryLen = 4;
	length_t streamLen = 10;
	data_t q[] = {1,2,3,4};
	data_t x[] = {5, 1,2,3,4, 1,2,3,4, -5,-10,-15};
	data_t cutoff = 2;
	steps_t warp = 1;
	
	tick_t trueTs = 5;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_IdenticalSubsequence() {
//	printf("EdtwSpotter_Warp2_IdenticalSubsequence\n");
	length_t queryLen = 3;
	length_t streamLen = 4;
	data_t q[] = {1,2,3};
	data_t x[] = {1,2,3,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	
	if (! pass) {
		assert(0);
	}
}


void EdtwSpotter_Warp2_IdenticalSubsequenceInitialXOfZero() {
//	printf("EdtwSpotter_Warp2_IdenticalSubsequenceInitialXOfZero\n");
	length_t queryLen = 3;
	length_t streamLen = 8;
	data_t q[] = {1,2,3};
	data_t x[] = {0,-1, 1,2,3 ,0,1,1};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 2;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_IdenticalSubsequenceInitialQOfZero() {
//	printf("EdtwSpotter_Warp2_IdenticalSubsequenceInitialQOfZero\n");
	length_t queryLen = 3;
	length_t streamLen = 9;
	data_t q[] = {0,1,2};
	data_t x[] = {3,2,2, 0,1,2 ,4,-1,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 3;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}
void EdtwSpotter_Warp2_IdenticalSubsequenceInitialQandXOfZero() {
//	printf("EdtwSpotter_Warp2_IdenticalSubsequenceInitialQOfZero\n");
	length_t queryLen = 4;
	length_t streamLen = 6;
	data_t q[] = {0,1,2,3};
	data_t x[] = {0,1,2,3 ,4,-1,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 0;
	tick_t trueTe = 3;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_PositiveOffset() {
//	printf("--------EdtwSpotter_Warp2_PositiveOffset\n--------");
	length_t queryLen = 4;
	length_t streamLen = 9;
	data_t q[] = {0,3,2,1};
	data_t x[] = {0, 5,8,7,6 ,6,5,7,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 1;
	tick_t trueTe = 4;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_NegativeOffset() {
	length_t queryLen = 4;
	length_t streamLen = 10;
	data_t q[] = {0,3,3,0};
	data_t x[] = {0,-5, -5,-2,-2,-5 ,-3,-5,-6,0};
	data_t cutoff = .5;
	steps_t warp = 2;
	
	tick_t trueTs = 2;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_AmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,3,2,-1};
	data_t x[] = {1,3,2, 3,9,6,-3 ,1,3,2,0,5,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 3;
	tick_t trueTe = 6;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_PositiveOffsetAndAmplitudeScaling() {
	length_t queryLen = 4;
	length_t streamLen = 14;
	data_t q[] = {1,-1,0,1};
	data_t x[] = {1,-1,0,2, 3,9,6,-3 ,14,6,10,14, 5,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 8;
	tick_t trueTe = 11;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_NegativeOffsetAndAmplitudeScaling() {
	length_t queryLen = 5;
	length_t streamLen = 14;
	data_t q[] = {1,3,5,2,4};
	data_t x[] = {1,-1,0,2, -8,-4,0,-6,-2 ,1,-2,0,9,0};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 4;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_OffsetAndAmplitudeScalingMultipleMatches() {
	length_t queryLen = 4;
	length_t streamLen = 12;
	data_t q[] = {1,2,3,4};
	data_t x[] = {5, 1,2,3,4, 1,2,3,4, -5,-10,-15};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 5;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_HorzInherit() {
	length_t queryLen = 3;
	length_t streamLen = 9;
	data_t q[] = {1,2,3};
	data_t x[] = {0, 2,2,3,3,4, 0,1,3};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 2;
	tick_t trueTe = 5;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp2_HorzInherit() {
//	printf("--------EdtwSpotter_Warp2_HorzInherit--------\n");
	length_t queryLen = 3;
	length_t streamLen = 10;
	data_t q[] = {3,1,2};
	data_t x[] = {9, 13,13,11,11,11,12, 10,11,13};
	data_t cutoff = INFINITY;
	steps_t warp = 2;
	
	tick_t trueTs = 2;
	tick_t trueTe = 6;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp3_HorzInherit() {
	length_t queryLen = 4;
	length_t streamLen = 16;
	data_t q[] = {1,3,2,4};
	data_t x[] = {9,-7, 6,6,10,10,10,10,8,8,8,8,12, 10,11,-13};
	data_t cutoff = INFINITY;
	steps_t warp = 3;
	
	tick_t trueTs = 3;
	tick_t trueTe = 12;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp0_RestrictedHorzInherit() {
	length_t queryLen = 3;
	length_t streamLen = 7;
	data_t q[] = {1,2,3};
	data_t x[] = {3, 2,2,3,3,4, 3};
	data_t cutoff = INFINITY;
	steps_t warp = 0;
	
	tick_t trueTs = 1;
	tick_t trueTe = 3;
	data_t trueDist = .25; // not a perfect match
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_RestrictedHorzInherit() {
//	printf("========EdtwSpotter_Warp1_RestrictedHorzInherit\n");
	length_t queryLen = 3;
	length_t streamLen = 10;
	data_t q[] = {3,2,3};
	data_t x[] = {3, -4,-4,-5,-5,-5,-4, 3,1,-4};
	data_t cutoff = INFINITY;
	steps_t warp = 1;
	
	tick_t trueTs = 4;
	tick_t trueTe = 6;
	data_t trueDist = .75; // not a perfect match...at all
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

void EdtwSpotter_Warp1_VertInherit() {
//	length_t queryLen = 4;
//	length_t streamLen = 6;
//	data_t q[] = {1,2,2,3};
//	data_t x[] = {-1, 2,3,4, 0,1};
//	data_t cutoff = INFINITY;
//	steps_t warp = 1;
//	
//	tick_t trueTs = 1;
//	tick_t trueTe = 3;
//	data_t trueDist = 0;
//	
//	bool pass = runSimpleTest(q, x, queryLen, streamLen,
//							  trueTs, trueTe, trueDist,
//							  kSPOT_TYPE, cutoff, warp);
//	if (! pass) {
//		exit(1);
//		assert(0);
//	}
	//FAILS without horizontal accumulation because [2,2] inherits
	//a horizontal distance of 0 instead of a vertical one of 0
	
	
	//This version passes because the initial 5,2 can't be made
	//to resemble the query
	length_t queryLen = 4;
	length_t streamLen = 6;
	data_t q[] = {1,2,2,3};
	data_t x[] = {5, 2,3,4, 0,1};
	data_t cutoff = INFINITY;
	steps_t warp = 1;

	tick_t trueTs = 1;
	tick_t trueTe = 3;
	data_t trueDist = 0;

	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE, cutoff, warp);
	if (! pass) {
		assert(0);
	}
}

