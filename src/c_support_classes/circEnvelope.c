//
//  circEnvelope.c
//  edtw
//
//  Created By <Anonymous> on 2/4/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "type_defs.h"
#include "deque.h"
#include "circEnvelope.h"
#include "circBuffer.h"
#include "array_utils.h"
#include "ucr_funcs.h"


circEnvelope *circEnvelope_new(unsigned int log2len, steps_t r) {
	
	circEnvelope *env= (circEnvelope*)malloc(sizeof(circEnvelope));
	env->maxLen = 1 << log2len; // length = 2 ^ log2len, done via bit shift
	int width = 2 * r + 1;
	float padding = kMAX_WARPING_WIDTH_R;
	
	env->recentData = circBuffer_new(ceil(log2(width+1))); // don't think the +1
//	env->u			= circBuffer_new(log2len);	//is needed, but little point in
//	env->l			= circBuffer_new(log2len);	//taking chances
	env->u			= circBuffer_with_end_padding_new(log2len, padding);
	env->l			= circBuffer_with_end_padding_new(log2len, padding);
	
//	deq_new(env->dl, width + 1);
//	deq_new(env->du, width + 1);
	
	env->r = r;
	env->time = -1;
	
	return env;
}

void circEnvelope_free(circEnvelope *env) {
	circBuffer_free(env->recentData);
	circBuffer_free(env->l);
	circBuffer_free(env->u);
//	deq_free(env->dl);
//	deq_free(env->du);
}

void circEnvelope_reset(circEnvelope *env) {
	circBuffer_reset(env->recentData);
	circBuffer_reset(env->l);
	circBuffer_reset(env->u);
//	deq_clear(env->dl);
//	deq_clear(env->du);
	env->time = -1;
}

void circEnvelope_resize(circEnvelope *env, unsigned int log2len) {
	circBuffer_resize(env->u, log2len);
	circBuffer_resize(env->l, log2len);
	env->maxLen = 1 << log2len;
}

//NOTE: this function *does not* recompute the envelope when r changes,
//so results using the unmodified envelope will be incorrect; officially,
//this function produces undefined behavior when the envelope already has
//data in it
void circEnvelope_setR(circEnvelope *env, steps_t r) {
	if (env->r == r) return;
	
	env->r = r;
	int newWidth = 2 * r + 1;
	
	if (newWidth >= env->recentData->maxLen) {
		circBuffer_resize(env->recentData, ceil(log2(newWidth+1)));
	}
}

void circEnvelope_append(circEnvelope* env, data_t x) {
	env->time++;
	steps_t r = env->r;
	int width = 2 * r + 1;
	circBuffer_append(env->recentData, x);
	
//	printf("env width, time = %d, %ld\n", width, env->time);
	
	// if we haven't gotten enough data yet to determine the
	// max/min for the first window, just return
	if (env->time < r) return;

	data_t *recentDataEnd = env->recentData->head;
	int offset;

	if (env->time < width -1) {
		offset = (int) env->time;
	} else {
		offset = width - 1;
	}
	data_t *recentDataStart = recentDataEnd - offset;
	
//	printf("t=%ld, tstart=%ld\n", env->time, env->time - offset);
//	array_print_with_name(recentDataStart, offset+1, "recentData");
	
	int recentDataLen = offset +1;
//	printf("recent data len = %d\n", recentDataLen);
	data_t max = array_max(recentDataStart, recentDataLen);
	data_t min = array_min(recentDataStart, recentDataLen);
//	printf("pushing u,l = %.1f, %.1f\n", max, min);
	
	circBuffer_append(env->u, max);
	circBuffer_append(env->l, min);
	
	// if r > 0, we need to fill out the rest of the buffer, which we aren't sure
	// is optimal
	if (r <= 0) return;
	
	data_t uTmp[width];
	data_t lTmp[width];
	build_envelope(recentDataStart,recentDataLen, r, lTmp, uTmp);

	data_t *uEnd = env->u->head;
	data_t *lEnd = env->l->head;
	
	
	data_t uShouldBeTheSame = uTmp[r];
	data_t lShouldBeTheSame = lTmp[r];
	if (uShouldBeTheSame != *uEnd || lShouldBeTheSame != *lEnd) {
		printf("Bro, this doesn't work how you think it does\n");
		printf("uHead, lHead = %.3f, %.3f\n", *uEnd, *lEnd);
		array_print_with_name(uTmp, recentDataLen, "uTmp");
		array_print_with_name(lTmp, recentDataLen, "lTmp");
		exit(1);
	}
//
//	array_print_with_name(uTmp, recentDataLen, "uTmp");
//	array_print_with_name(lTmp, recentDataLen, "lTmp");
	
	memcpy(uEnd+1, uTmp+r+1, r*sizeof(data_t));
	memcpy(lEnd+1, lTmp+r+1, r*sizeof(data_t));
	
//	array_print_with_name(uEnd, r+1, "uPastEnd");
//	array_print_with_name(lTmp, r+1, "lPastEnd");
	
	//TODO create an online version of build_envelope() function using
	//deques to get constant-time updates instead of O(r)
}

void circEnvelope_appendAll(circEnvelope* env, const data_t *ar, length_t len) {
	for (length_t i = 0; i < len; i++) {
		circEnvelope_append(env, ar[i]);
	}
}
