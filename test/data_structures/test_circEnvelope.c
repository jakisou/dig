//
//  test_circEnvelope.c
//  edtw
//
//  Created By <Anonymous> on 2/5/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "assert.h"
#include "test_circEnvelope.h"
#include "circEnvelope.h"
#include "type_defs.h"
#include "array_utils.h"

//#define PRINT_TEST_NAMES

void test_circEnvelope_all() {
	printf("================================ Testing circEnvelope\n");

	circEnvelope_AddTooFewPoints_EnvelopesEmpty();
	circEnvelope_Warp0_MonotonicallyIncreasingData_EnvelopeCorrect();
	circEnvelope_Warp0_MonotonicallyDecreasingData_EnvelopeCorrect();
	circEnvelope_Warp1_MonotonicallyIncreasingData_EnvelopeCorrect();
	circEnvelope_Warp1_MonotonicallyDecreasingData_EnvelopeCorrect();
	circEnvelope_Warp2_NonMonotonicData_EnvelopeCorrect();
	
	circEnvelope_Resize_EnvelopeCorrect();
	circEnvelope_Reset_EnvelopeCorrect();
	circEnvelope_SetRAtStart_EnvelopeCorrect();
	
	printf("================================ End circEnvelope Test: Success\n");
}

static inline void printTestName(const char *testName) {
#ifdef PRINT_TEST_NAMES
	printf("--------%s--------\n", testName);
#endif
}

static inline void check_identical(circEnvelope *env,
								   const data_t *data, int dataLen,
								   const data_t *u, const data_t *l,
								   const char* testName) {
	unsigned int envLen = (unsigned int) env->time - env->r + 1;
	
	printf("checking last %d points in envelope\n", envLen);
	
	data_t *uEnd = env->u->head;
	data_t *lEnd = env->l->head;
	data_t *uStart = uEnd - envLen + 1;
	data_t *lStart = lEnd - envLen + 1;
	
	short int uSame = array_equal(uStart, u, envLen);
	short int lSame = array_equal(lStart, l, envLen);
	
	if (! (uSame && lSame) ) {
		printf("TEST FAILED: %s\n", testName);
		printf("u: \t");		array_print(uStart, envLen);
		printf("uTruth: \t");	array_print(u, envLen);
		printf("l: \t");		array_print(lStart, envLen);
		printf("lTruth: \t");	array_print(l, envLen);
		printf("data: \t");		array_print(data, dataLen);
		assert(0);
	}
}

static inline void execute_test(unsigned int logLen, unsigned int dataLen,
								steps_t r, const data_t *data,
								const data_t* u, const data_t *l,
								const char* testName) {
	printTestName(testName);
//	unsigned int envLen = dataLen - r;
	
	circEnvelope *env = circEnvelope_new(logLen, r);
	circEnvelope_appendAll(env, data, dataLen);
	
	check_identical(env, data, dataLen, u, l, testName);
	
	circEnvelope_free(env);
}

void circEnvelope_AddTooFewPoints_EnvelopesEmpty() {
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 3;
	steps_t r = 3;
	data_t data[] = {1,2,3};
	circEnvelope *env = circEnvelope_new(logLen, r);
	circEnvelope_appendAll(env, data, dataLen);
	
	idx_t uHead = env->u->headIdx;
	idx_t lHead = env->l->headIdx;
	
	if (uHead != -1 || lHead != -1) {
		printf("TEST FAILED: circEnvelope_AddTooFewPoints_EnvelopesEmpty\n");
		printf("upper head idx = %d, lower head idx = %d\n", uHead, lHead );
		assert(0);
	}
	
	circEnvelope_free(env);
}

void circEnvelope_Warp0_MonotonicallyIncreasingData_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Warp0_MonotonicallyIncreasingData_EnvelopeCorrect";
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 5;
	steps_t r = 0;
	data_t data[]	= {1,2,3,4,5};
	data_t u[]		= {1,2,3,4,5};
	data_t l[]		= {1,2,3,4,5};
	execute_test(logLen, dataLen, r, data, u, l, testName);
}

void circEnvelope_Warp0_MonotonicallyDecreasingData_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Warp0_MonotonicallyDecreasingData_EnvelopeCorrect";
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 5;
	steps_t r = 0;
	data_t data[]	= {5,4,3,2,1};
	data_t u[]		= {5,4,3,2,1};
	data_t l[]		= {5,4,3,2,1};

	execute_test(logLen, dataLen, r, data, u, l, testName);
}

void circEnvelope_Warp1_MonotonicallyIncreasingData_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Warp1_MonotonicallyIncreasingData_EnvelopeCorrect";
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 5;
	steps_t r = 1;
	data_t data[]	= {1,2,3,4,5};
	data_t u[]		= {2,3,4,5};
	data_t l[]		= {1,1,2,3};
	execute_test(logLen, dataLen, r, data, u, l, testName);
}

void circEnvelope_Warp1_MonotonicallyDecreasingData_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Warp1_MonotonicallyDecreasingData_EnvelopeCorrect";
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 5;
	steps_t r = 1;
	data_t data[]	= {5,4,3,2,1};
	data_t u[]		= {5,5,4,3};
	data_t l[]		= {4,3,2,1};
	
	execute_test(logLen, dataLen, r, data, u, l, testName);
				 
}

void circEnvelope_Warp2_NonMonotonicData_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Warp2_NonMonotonicData_EnvelopeCorrect";
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 8;
	steps_t r = 2;
	data_t data[]	= {-3, 1, 2, 3,-2, 5, 2, 3};
	data_t u[]		= { 2, 3, 3, 5, 5, 5};
	data_t l[]		= {-3,-3,-3,-2,-2,-2};
	
	execute_test(logLen, dataLen, r, data, u, l, testName);
}

void circEnvelope_Resize_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Resize_EnvelopeCorrect";
	unsigned int logLen1 = 3;//3;
	unsigned int logLen2 = 5; // size = 2^3 = 8
	unsigned int logLen3 = 4;
	unsigned int dataLen = 11;
	unsigned int initialDataLen = 5;
	unsigned int remainingDataLen = dataLen - initialDataLen;
	steps_t r = 1;
	data_t data[]	= {-3, 1, 2, 3,-2, 5, 2, 3, 6,-1, 4};
	data_t u[]		= { 1, 2, 3, 3, 5, 5, 5, 6, 6, 6};
	data_t l[]		= {-3,-3, 1,-2,-2,-2, 2, 2,-1,-1};
	
	printTestName(testName);
	
	circEnvelope *env = circEnvelope_new(logLen1, r);
	circEnvelope_resize(env, logLen2);
	circEnvelope_appendAll(env, data, initialDataLen);
	circEnvelope_resize(env, logLen3);
	circEnvelope_appendAll(env, data+initialDataLen, remainingDataLen);
	
	check_identical(env, data, dataLen, u, l, testName);
	
	circEnvelope_free(env);
}

void circEnvelope_Reset_EnvelopeCorrect() {
	const char * testName = "circEnvelope_Reset_EnvelopeCorrect";
	unsigned int logLen = 3;//3;
	unsigned int dataLen = 11;
	unsigned int initialDataLen = 5;
	unsigned int remainingDataLen = dataLen - initialDataLen;
	steps_t r = 1;
	data_t data[]	= {-3, 1, 2, 3,-2, 5, 2, 3, 6,-1, 4};
	data_t u[]		= {				   5, 5, 6, 6, 6};
	data_t l[]		= {				   2, 2, 2,-1,-1};
	
	printTestName(testName);
	
	circEnvelope *env = circEnvelope_new(logLen, r);
	circEnvelope_appendAll(env, data, initialDataLen);
	circEnvelope_reset(env);
//	printf("----Reset Envelope----\n");
	circEnvelope_appendAll(env, data+initialDataLen, remainingDataLen);
	
	check_identical(env, data+initialDataLen, remainingDataLen, u, l, testName);
	
	circEnvelope_free(env);
}

void circEnvelope_SetRAtStart_EnvelopeCorrect() {
	const char * testName = "circEnvelope_SetRAtStart_EnvelopeCorrect";
	unsigned int logLen = 4; // size = 2^4 = 16
	unsigned int dataLen = 8;
	steps_t r1 = 0;
	steps_t r2 = 2;
	steps_t r3 = 1;
	data_t data[]	= {-3, 1, 2, 3,-2, 5, 2, 3};
	data_t u[]		= { 1, 2, 3, 3, 5, 5, 5};
	data_t l[]		= {-3,-3, 1,-2,-2,-2, 2};
	
	printTestName(testName);
	
	circEnvelope *env = circEnvelope_new(logLen, r1);
	circEnvelope_setR(env, r2);
	circEnvelope_setR(env, r3);
	circEnvelope_appendAll(env, data, dataLen);
	
	check_identical(env, data, dataLen, u, l, testName);
	
	circEnvelope_free(env);
}