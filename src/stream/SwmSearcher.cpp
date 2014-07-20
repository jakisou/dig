//
//  SwmSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 2/9/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "SwmSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"
#include "array_utils.h"
#include "circEnvelope.h"

//static const float kDEFAULT_WARPING_R = .05;
static const float kDEFAULT_WARPING_WIDTH = 10;
static const unsigned int kDEFAULT_ENVELOPE_LEN_LOG2 = 5;	//len = 2^5 = 32

circEnvelope *SwmSearcher::sStreamEnvelope = circEnvelope_new(
					kDEFAULT_ENVELOPE_LEN_LOG2, kDEFAULT_WARPING_WIDTH);

// ================================================================
// Lifecycle
// ================================================================

SwmSearcher::SwmSearcher(Template const& ref, SpotterParams const& p):
	USSearcher(ref, p),
	mMaxWarp(mMaxWindowLen * p.maxWarp/100.0f + .5f),
	mMaxWarpR(p.maxWarp/100.0f),
	mQuery(nullptr)	//explicitly set to null so check in constructor will work
{
	setScaleFactor(p.scaleFactor);
	
	// if the scale factor didn't change from the default of 0 scaling, the
	// query won't be initialized, so ensure that we have a valid query
	if (mQuery == nullptr) {
		mQuery = swm_query_new(refData, M, mMinWindowLen, mMaxWindowLen, mMaxWarpR);
	}
	
	// ensure the shared envelope object is long enough
	if (SwmSearcher::sStreamEnvelope->maxLen < mMaxWindowLen) {
		circEnvelope_resize(SwmSearcher::sStreamEnvelope, ceil(log2(mMaxWindowLen)));
	}
}

SwmSearcher::SwmSearcher(const SwmSearcher& other):
	USSearcher(other),
	mMaxWarp(other.mMaxWarp),
	mMaxWarpR(other.mMaxWarpR)
{
	if (mQuery != nullptr) {
		swm_query_free(mQuery);
	}
	mQuery = swm_query_new(refData, M, mMinWindowLen, mMaxWindowLen, mMaxWarpR);
}

SwmSearcher::~SwmSearcher() {
	swm_query_free(mQuery);
}

// ================================================================
// Methods
// ================================================================

void SwmSearcher::setMaxWarp(steps_t maxWarp) {
//	printf("setMaxWarp to %d\n", maxWarp);
	
	//TODO combine duplicate code with DtwSearcher
	double newMaxWarpR = maxWarp / 100.0f;
	if (newMaxWarpR == mMaxWarpR) return;
	
//	printf("changed warpR to %.3f\n", newMaxWarpR);
	
	mMaxWarpR = newMaxWarpR;
//	mMaxWarp = round(mMaxWarpR*M);
	mMaxWarp = mMaxWarpR * mMaxWindowLen + .5f; //window wider for longer lens, so need to use longest
	swm_query_free(mQuery);
	mQuery = swm_query_new(refData, M, mMinWindowLen, mMaxWindowLen, mMaxWarpR);
	circEnvelope_setR(SwmSearcher::sStreamEnvelope, mMaxWarp);
}

void SwmSearcher::onScalingsChanged() {
//	printf("called SwmSearcher onScalingsChanged\n");
//	printf("minLen, maxLen = %d, %d\n", mMinWindowLen, mMaxWindowLen);
//	mQuery->maxLen = mMaxWindowLen;
//	mQuery->minLen = mMinWindowLen;
	if (mQuery != NULL) {
		swm_query_free(mQuery);
	}
	mQuery = swm_query_new(refData, M, mMinWindowLen, mMaxWindowLen, mMaxWarpR);
//	printf("built new swm_query due to scaling change\n");
}

void SwmSearcher::onDataReceived(tick_t t, data_t x) {
	// reset the envelope if this is the start of a new stream
	tick_t envelopeTime = SwmSearcher::sStreamEnvelope->time;
	if (t == 0 && envelopeTime > 0) {
		long len = SwmSearcher::sStreamEnvelope->maxLen;
		circEnvelope_free(SwmSearcher::sStreamEnvelope);
		SwmSearcher::sStreamEnvelope = circEnvelope_new(ceil(log2(len)), mMaxWarp);
	}
	
	// ensure that the envelope incorporates each data point
	if (t > SwmSearcher::sStreamEnvelope->time) {
		circEnvelope_append(SwmSearcher::sStreamEnvelope, x);
	}
}

data_t SwmSearcher::computeDistance(const data_t* buffEnd, tick_t t,
									tick_t cantStartUntilAfter,
									data_t meanX, data_t stdX,
									data_t bsf) {
	//	printf("----t=%ld, meanX=%.3f, stdX = %.3f\n", t, meanX, stdX);
	
	// check if any scaling is viable
	tick_t minLenStartIdx = t - mMinWindowLen +1;
	if ( minLenStartIdx <= cantStartUntilAfter ) return INFINITY;
	
	tick_t maxLenStartIdx = t - mMaxWindowLen +1;
	
	idx_t maxLen = mMaxWindowLen;
	if (maxLenStartIdx <= cantStartUntilAfter) {
		maxLen = (idx_t) (t - cantStartUntilAfter);
	}
	
//	if (fabs(mMaxWarpR-.05) >.0001 || mMaxWarp != (int)round(mMaxWarpR*M) ) {
//		printf("mMaxWarpR = %.9f, mMaxWarp = %d\n", mMaxWarpR, mMaxWarp);
//		printf("should be %.5f, %d\n", .05, (int)round(mMaxWarpR*M));
//		exit(1);
//	}
	
	idx_t startOffset = maxLen - 1;
	const data_t *buffStart = buffEnd - startOffset;
	
	const data_t *lBuffEnd = SwmSearcher::sStreamEnvelope->l->head + mMaxWarp;
	const data_t *uBuffEnd = SwmSearcher::sStreamEnvelope->u->head + mMaxWarp;
	const data_t *lBuffStart = lBuffEnd - startOffset;
	const data_t *uBuffStart = uBuffEnd - startOffset;
	
	mQuery->maxLen = maxLen;
	data_t dist = swm_distance_and_len_envelope(mQuery, buffStart,
												lBuffStart,uBuffStart,
												meanX, stdX, bsf,
												mUseEnvelopes,&mBestLen);
	mQuery->maxLen = mMaxWindowLen;
	
	
	// determine if the best length found is actually a better match (and not just
	// whatever garbage got placed in the integer when no better match was found)
	mBestLenValid = dist < bsf;
	
//	if (dist == INFINITY) {
//		printf("WARNING: SwmSearcher: computed infinite dist\n");
//	}
	
//	if (t >20) 	exit(1);	//TODO remove
	
	return dist;
}