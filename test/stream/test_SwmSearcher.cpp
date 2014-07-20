//
//  test_SwmSearcher.c
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include "test_SwmSearcher.hpp"
#include "type_defs.h"
#include "testUtils.hpp"
#include "array_utils.h"

#define PRINT_TEST_NAMES

static const SpotterType kSPOT_TYPE_AGGRESSIVE = SWM_AGGRESSIVE;
static const SpotterType kSPOT_TYPE_CAUTIOUS = SWM_CAUTIOUS;

void test_SwmSearcher_all() {
	
	printf("================================ Testing SWM Searcher\n");
	
	SwmSearcher_Aggressive_ScaleFactor0_SubsequenceAtStart();
	SwmSearcher_Cautious_ScaleFactor0_SubsequenceAtStart();
	SwmSearcher_Aggressive_ScaleFactor0_SubsequenceAfterStart();
	SwmSearcher_Cautious_ScaleFactor0_SubsequenceAfterStart();
	
	SwmSearcher_Agressive_ScaleFactor0p5_SubsequenceAtStart();
	SwmSearcher_Agressive_ScaleFactor0p5_ShorterSubsequenceAfterStart();
	SwmSearcher_Agressive_ScaleFactor0p67_MaxLenSubsequenceAfterStart();
	
	printf("================================ End SWM Searcher Test: Success\n");
}

inline void printTestName(const char * testName) {
#ifdef PRINT_TEST_NAMES
	printf("%s\n", testName);
#endif
}


void SwmSearcher_Aggressive_ScaleFactor0_SubsequenceAtStart() {
	length_t queryLen = 3;
	length_t streamLen = 3;
	data_t q[] = {1,2,3};
	data_t x[] = {-1,0,1};
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist,
							  kSPOT_TYPE_AGGRESSIVE);
	if (! pass) {
		printf("FAILED\n");
		exit(1);
		assert(0);
	}
}

void SwmSearcher_Cautious_ScaleFactor0_SubsequenceAtStart() {
	printTestName("SwmSearcher_Cautious_ScaleFactor0_SubsequenceAtStart");
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

void SwmSearcher_Aggressive_ScaleFactor0_SubsequenceAfterStart() {
	length_t queryLen = 3;
	length_t streamLen = 6;
	data_t q[] = {3,1,2};
	data_t x[] = {-1,4,3,1,2,-2};
	data_t cutoff = .8;
	
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

void SwmSearcher_Cautious_ScaleFactor0_SubsequenceAfterStart() {
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

void SwmSearcher_Agressive_ScaleFactor0p5_SubsequenceAtStart() {
	printf("--------SwmSearcher_Agressive_ScaleFactor0p5_SubsequenceAtStart--------\n");
	length_t queryLen = 4;
	length_t streamLen = 4;
	data_t q[] = { 3,1,3,4};
	data_t x[] = {-2,-6,-2,5};
	data_t cutoff = .1;
	steps_t warp = 0;
	float scaleFactor = 1;
	
	tick_t trueTs = 0;
	tick_t trueTe = 2;
	data_t trueDist = 0;
	
	SpotterParams p(kSPOT_TYPE_AGGRESSIVE, AGGRESSIVE, cutoff, warp, scaleFactor);
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist, p);
	if (! pass) {
		assert(0);
	}
}

void SwmSearcher_Agressive_ScaleFactor0p5_ShorterSubsequenceAfterStart() {
	printf("--------SwmSearcher_Agressive_ScaleFactor0p5_ShorterSubsequenceAfterStart--------\n");
	length_t queryLen = 6;
	length_t streamLen = 11;
	data_t q[] = { 3,1,2,4,5,4}; //3,1,4,5 at len 4
	data_t x[] = {5,-1,0,13,11, 13,11,14,15, 19,12};
	data_t cutoff = .07;
	steps_t warp = 0;
	float scaleFactor = 1;
	
	int correctAnswerLen = 4;
	data_t s[correctAnswerLen];
	array_resample(q,s,queryLen,correctAnswerLen);
	array_print_with_name(s,correctAnswerLen,"s correct");
	
	tick_t trueTs = 5;
	tick_t trueTe = 8;
	data_t trueDist = 0;
	
	SpotterParams p(kSPOT_TYPE_AGGRESSIVE,AGGRESSIVE,
											  cutoff, warp, scaleFactor);
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist, p);
	if (! pass) {
		assert(0);
	}
}

void SwmSearcher_Agressive_ScaleFactor0p67_MaxLenSubsequenceAfterStart() {
	printf("--------SwmSearcher_Agressive_ScaleFactor0p67_LongerSubsequenceAfterStart--------\n");
	length_t queryLen = 6;
	length_t streamLen = 19;
	data_t q[] = { 3,1,2,4,5,4}; //3,3,1,2,2,4,5,5,4 at len=9
	data_t x[] = {5,-1,0,13,11, 13,13,11,12,12,14,15,15,14, 19,12,7,6,12};
	data_t cutoff = .05;
	steps_t warp = 0;
	float scaleFactor = .5;
	
	int correctAnswerLen = 9;
	data_t s[correctAnswerLen];
	array_resample(q,s,queryLen,correctAnswerLen);
	array_print_with_name(s,correctAnswerLen,"s");
	
	tick_t trueTs = 5;
	tick_t trueTe = 13;
	data_t trueDist = 0;
	
	SpotterParams p(kSPOT_TYPE_AGGRESSIVE,AGGRESSIVE,cutoff, warp, scaleFactor);
	
	bool pass = runSimpleTest(q, x, queryLen, streamLen,
							  trueTs, trueTe, trueDist, p);
	if (! pass) {
		assert(0);
	}
}

