//
//  DtwSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 2/2/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include "DtwSearcher.hpp"

#include <stdio.h>
#include "DtwSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"
#include "circEnvelope.h"
//#include "array_utils.h"

circEnvelope *DtwSearcher::sStreamEnvelope = circEnvelope_new(
					kDEFAULT_ENVELOPE_LEN_LOG2, kDEFAULT_WARPING_WIDTH);

// ================================================================
// Lifecycle
// ================================================================

DtwSearcher::DtwSearcher(Template const& ref, SpotterParams const& p):
	WindowSpotter(ref, p),
	mMaxWarp(M * p.maxWarp/100.0f +.5),
	mMaxWarpR(p.maxWarp/100.0f)
{
//	mCumBound1 = new data_t[M];
//	mCumBound2 = new data_t[M];
//	mNormalizedData = new data_t[M];
	mDtwQuery = dtw_query_new(refData, M, mMaxWarpR);//((double) mMaxWarp) / M);
	
	// ensure the shared envelope object is long enough
	if (DtwSearcher::sStreamEnvelope->maxLen < M) {
		circEnvelope_resize(DtwSearcher::sStreamEnvelope, ceil(log2(M)));
	}
	
	circEnvelope_setR(DtwSearcher::sStreamEnvelope, mMaxWarp);
}

DtwSearcher::DtwSearcher(const DtwSearcher& other):
	WindowSpotter(other),
	mMaxWarp(other.mMaxWarp),
	mMaxWarpR(other.mMaxWarpR)
{
	if (mDtwQuery != nullptr) {
		dtw_query_free(mDtwQuery);
	}
	mDtwQuery = dtw_query_new(refData, M, mMaxWarpR);
}

DtwSearcher::~DtwSearcher() {
//	delete [] mCumBound1;
//	delete [] mCumBound2;
//	delete [] mNormalizedData;
	dtw_query_free(mDtwQuery);
}

// ================================================================
// Methods
// ================================================================

//tick_t DtwSearcher::getCurrentStartTime() const {
//	return getCurrentEndTime() - M + 1;
//}
//
//tick_t DtwSearcher::getCurrentEndTime() const {
////	return mTime - mMaxWarp;
//	return mTime;
//}

//inline DtwSearcher::refreshQueryStruct() {
//	 dtw_query_new(refData, M, (double mMaxWarp) / M);
//}

// NOTE: this assumes the maxWarp value given is a percentage, not an absolute
// number of steps
// NOTE 2: this renderes the results invalid until the envelope calculations
// using the old R are no longer in the sliding window
void DtwSearcher::setMaxWarp(steps_t maxWarp) {
	double newMaxWarpR = maxWarp / 100.0f;
	if (newMaxWarpR == mMaxWarpR) return;
	
	mMaxWarpR = newMaxWarpR;
	mMaxWarp = mMaxWarpR * M +.5;
	dtw_query_free(mDtwQuery);
	mDtwQuery = dtw_query_new(refData, M, mMaxWarpR);
	circEnvelope_setR(DtwSearcher::sStreamEnvelope, mMaxWarp);
}

void DtwSearcher::onDataReceived(tick_t t, data_t x) {
	tick_t envelopeTime = DtwSearcher::sStreamEnvelope->time;
	if (t == 0 && envelopeTime > 0) {
		long len = DtwSearcher::sStreamEnvelope->maxLen;
		circEnvelope_free(DtwSearcher::sStreamEnvelope);
		DtwSearcher::sStreamEnvelope = circEnvelope_new(ceil(log2(len)), mMaxWarp);
	}
	
//	printf("DtwSearcher received data %.3f at time %ld; envelope time = %ld\n",
//		   x,t, DtwSearcher::sStreamEnvelope->time);
	// ensure that the envelope incorporates each data point
	if (t > DtwSearcher::sStreamEnvelope->time) {
//		printf("DtwSearcher appended data %.3f\n", x);
		circEnvelope_append(DtwSearcher::sStreamEnvelope, x);
	}
}

data_t DtwSearcher::computeDistance(const data_t* buffEnd,
									tick_t t, tick_t cantStartUntilAfter,
									data_t meanX, data_t stdX, data_t bsf) {
	
	//TODO getting messed up cuz warping envelope way behind cuz this doesn't
	//even get called until the buffer is full
	
	
//	printf("searching with r, rSteps = %.3f, %d\n", mMaxWarpR, mMaxWarp);
//	exit(1);
	
	// check if we have enough data (since the last reported sequence) to
	// compute a distance and return if not
//	idx_t startOffset = M +  mMaxWarp - 1;
	idx_t startOffset = M - 1;
	tick_t startIdx = t - startOffset;
	if ( startIdx <= cantStartUntilAfter ) return INFINITY;
	
	// get pointers the start of the data and data envelope
	const data_t *dataBuffStart = buffEnd - startOffset;
	
//	const data_t *lBuffEnd = DtwSearcher::sStreamEnvelope->l->head;
//	const data_t *uBuffEnd = DtwSearcher::sStreamEnvelope->u->head;
	const data_t *lBuffEnd = DtwSearcher::sStreamEnvelope->l->head + mMaxWarp;
	const data_t *uBuffEnd = DtwSearcher::sStreamEnvelope->u->head + mMaxWarp;
	const data_t *lBuffStart = lBuffEnd - startOffset;
	const data_t *uBuffStart = uBuffEnd - startOffset;

//	data_t dist = dtw_distance(mDtwQuery, dataBuffStart,
//							   mNormalizedData,
//							   mCumBound1, mCumBound2,
//							   lBuffStart, uBuffStart,
//							   meanX, stdX, bsf);
	data_t dist = dtw_distance(mDtwQuery, dataBuffStart,
							   lBuffStart, uBuffStart,
							   meanX, stdX, bsf);
//	printf("dist = %.3f\n", dist);
	return dist;
}