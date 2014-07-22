//
//  WindowSpotter.cpp
//  edtw
//
//  Created By <Anonymous> on 2/1/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <math.h>
#include <float.h>
#include "WindowSpotter.hpp"
#include "Subsequence.hpp"
#include "circBuffer.h"
#include "basic_structs.hpp"
#include "array_utils.h"
#include "type_defs.h"
#include "ucr_funcs.h"

// initialize streamStats object
circBuffer* WindowSpotter::sBuff = circBuffer_new(kDEFAULT_LOG_BUFFLEN);
tick_t WindowSpotter::sBufferUpdatedTime = -1;

// ================================================================
// Lifecycle
// ================================================================

WindowSpotter::WindowSpotter(Template const& ref, SpotterParams const& p):
	PatternSpotter(ref, p),
	mCurrentDist(INFINITY),
	mEpochCounter(0),
	mSumX(0),
	mSumX2(0),
	mMaxWindowLen(M),
	mMinWindowLen(M),
	mLastMatchEndTime(-1)
{
	ensureBufferLongEnough(mMaxWindowLen);
	mOrder = new idx_t[M];
	mSortedY = new data_t[M];

	// normalize reference data (query)
	znormalize(refData, M);

	// create sorted version of reference data
	array_copy(refData, mSortedY, M);
	sort_abs_decreasing(mSortedY, mOrder, M);
}

WindowSpotter::WindowSpotter(const WindowSpotter& other):
	PatternSpotter(other),
	mCurrentDist(other.mCurrentDist),
	mEpochCounter(other.mEpochCounter),
	mSumX(other.mSumX),
	mSumX2(other.mSumX2),
	mMaxWindowLen(other.mMaxWindowLen),
	mMinWindowLen(other.mMinWindowLen),
	mLastMatchEndTime(other.mLastMatchEndTime)
{
	ensureBufferLongEnough(mMaxWindowLen);
	mOrder	 = (idx_t*)	realloc(mOrder,		other.M*sizeof(idx_t));
	mSortedY = (data_t*)realloc(mSortedY,	other.M*sizeof(data_t));
	memcpy(mOrder,		other.mOrder,	other.M*sizeof(idx_t));
	memcpy(mSortedY,	other.mSortedY,	other.M*sizeof(data_t));
}

WindowSpotter::~WindowSpotter() {
	delete [] mOrder;
	delete [] mSortedY;
}

// ================================================================
// Methods
// ================================================================

inline void WindowSpotter::ensureBufferLongEnough(idx_t len) {
	if (len > WindowSpotter::sBuff->maxLen) {
		circBuffer_resize(WindowSpotter::sBuff, ceil(log2(len)));
	}
}

inline void WindowSpotter::resetBufferIfNeeeded(tick_t currentTime) {
	if (currentTime == 0 && (WindowSpotter::sBufferUpdatedTime > 0) ) {

//		printf("reseting buffer: current time, static time = %ld, %ld\n",
//			   currentTime, WindowSpotter::sBufferUpdatedTime);
		circBuffer_reset(WindowSpotter::sBuff);
		WindowSpotter::sBufferUpdatedTime = -1;
	}
}

inline void WindowSpotter::updateBuffer(tick_t time, data_t x) {
	resetBufferIfNeeeded(time);
	if (time > WindowSpotter::sBufferUpdatedTime) {
		circBuffer_append(WindowSpotter::sBuff, x);
		WindowSpotter::sBufferUpdatedTime = time;
	}
}

void WindowSpotter::updateMatrix(const tick_t t, const data_t* sample, data_t cutoff) {
	mTime = t;
	data_t x = *sample;		//TODO don't assume 1D input

//	printf("--------, t=%ld, x=%.2f\n", t, x);

	// add data to buffer if another WindowSpotter hasn't already
	updateBuffer(t,x);

	// update running statistics
	mSumX += x;
	mSumX2 += x*x;
	mEpochCounter++;

	// let child classes do preprocessing before calling the distance
	// function
	onDataReceived(t,x);

	// if don't have enough data to fill the sliding window yet, just return
	tick_t dataStartIdx = t - mMinWindowLen + 1;
	if (dataStartIdx < 0) return;

	// pick out most recent data in buffer
	data_t *dataEnd = WindowSpotter::sBuff->head;
	data_t* dataStart = dataEnd - mMinWindowLen + 1;

	// if the running sums have taken in too many points, reset them
	// fresh to avoid floating point errors
	if (mEpochCounter >= kRESET_SUMS_AFTER) {
		mSumX = array_sum(dataStart,mMinWindowLen);
		mSumX2 = array_sum_squared(dataStart,mMinWindowLen);
		mEpochCounter = 0;
	}

	// compute mean and standard deviation for normalization
	mMeanX = mSumX / mMinWindowLen;
	mStdX = mSumX2 / mMinWindowLen;
	mStdX = sqrt(mStdX - mMeanX*mMeanX);
	if (mStdX == 0) mStdX = DBL_MIN;	//don't divide by 0

	// if we just found a match, then we can't report subsequences that
	// overlap with that match
	tick_t latestPossibleStartTime = t - mMinWindowLen + 1;
	if (latestPossibleStartTime > mLastMatchEndTime) {

		double localCutoff = denormalize(cutoff);
		double bsf = dMin < localCutoff ? dMin : localCutoff;
//		double bsf = dMin == INFINITY ? localCutoff : dMin;
//		double bsf = dMin;

		// if we could report a non-overlaping subsequence, see if a good
		// one (ie, one with dist less that of the best one found so far) is present
		mCurrentDist = computeDistance(dataEnd, t, mLastMatchEndTime, mMeanX, mStdX, bsf);
		if (mCurrentDist < bsf) {
			dMin = mCurrentDist;
		} else {
			mCurrentDist = INFINITY;
		}
	}

	// subtract off points that just exited the sliding window
	data_t oldX = *(dataStart);
	mSumX -= oldX;
	mSumX2 -= oldX*oldX;
}

bool WindowSpotter::sequenceLocallyOptimal(Subsequence seq) const {
	tick_t earliestPossibleStart;
	switch (mReportCriteria) {
		case AGGRESSIVE:
			return true;
			break;
		case CAUTIOUS:
			//if same class, possibly just count it as optimal regardless
			if (mSameClassMeansOptimal && seq.label == mLabel) return true;
			//otherwise, check whether there's any possible way we could do better
			earliestPossibleStart = mTime - mMaxWindowLen + 1;
			return seq.te < earliestPossibleStart;
		default:
			printf("Error: WindowSpotter: sequenceLocallyOptimal: unsupported ReportCriteria\n");
			exit(1);
			break;
	}
}

void WindowSpotter::purgeAllOverlap(Subsequence seq) {
	mLastMatchEndTime = seq.te;
	dMin = INFINITY;
//	mCurrentDist = INFINITY;

	// recalculate distance without overlap
	data_t *dataEnd = WindowSpotter::sBuff->head;
	mCurrentDist = computeDistance(dataEnd, mTime, mLastMatchEndTime,
								   mMeanX, mStdX, dMin);

//	printf("called purgeAllOverlap; currentStartTime = %ld, seqEndTime = %ld\n",
//		   getCurrentStartTime(), mLastMatchEndTime);
//	if (getCurrentStartTime() > mLastMatchEndTime) {
//		printf("reset mCurrentDist\n");
//
//		// recalculate distance
//		data_t *dataEnd = WindowSpotter::sBuff->head;
//		mCurrentDist = computeDistance(dataEnd, mTime, mLastMatchEndTime,
//									   mMeanX, mStdX, dMin);
//	}
}
