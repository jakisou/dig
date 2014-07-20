//
//  USUSEuclideanSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "USEuclideanSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"
#include "array_utils.h"

// ================================================================
// Lifecycle
// ================================================================

USEuclideanSearcher::USEuclideanSearcher(Template const& ref, SpotterParams const& p):
	USSearcher(ref, p),
	mQuery(nullptr)	//explicitly set to null so check in constructor will work
{
	setScaleFactor(p.scaleFactor);
	
	// if the scale factor didn't change from the default of 0 scaling, the
	// query won't be initialized, so ensure that we have a valid query
	if (mQuery == nullptr) {
		mQuery = us_query_new(refData, M, mMinWindowLen, mMaxWindowLen);
	}
}

USEuclideanSearcher::USEuclideanSearcher(const USEuclideanSearcher& other):
	USSearcher(other)
{
	if (mQuery != nullptr) {
		us_query_free(mQuery);
	}
	mQuery = us_query_new(refData, M, mMinWindowLen, mMaxWindowLen);
}

USEuclideanSearcher::~USEuclideanSearcher() {
	us_query_free(mQuery);
}

// ================================================================
// Methods
// ================================================================

void USEuclideanSearcher::onScalingsChanged() {
	if (mQuery != nullptr) {
		us_query_free(mQuery);
	}
	mQuery = us_query_new(refData, M, mMinWindowLen, mMaxWindowLen);
}

data_t USEuclideanSearcher::computeDistance(const data_t* buffEnd,
										  tick_t t, tick_t cantStartUntilAfter,
										  data_t meanX, data_t stdX,
										  data_t bsf) {
//	printf("----t=%ld, meanX=%.3f, stdX = %.3f\n", t, meanX, stdX);
	
	// check if any scaling is viable
	tick_t minLenStartIdx = t - mMinWindowLen +1;
	if ( minLenStartIdx <= cantStartUntilAfter ) return INFINITY;
	
	tick_t maxLenStartIdx = t - mMaxWindowLen +1;
//	printf("minLenStartIdx, maxLenStartIdx = %ld, %ld\n", minLenStartIdx, maxLenStartIdx);
	
	idx_t maxLen = mMaxWindowLen;
	if (maxLenStartIdx <= cantStartUntilAfter) {
		maxLen = (idx_t) (t - cantStartUntilAfter);
	}

//	if (stdX == 0) {
//		printf("ERROR: USEuclideanSearcher: got stdX of 0");
//		exit(1);
//	}
	
	const data_t *buffStart = buffEnd - maxLen + 1;
//	printf("called USEuclideanSearcher#computeDistance(); searching arrays:\n");
//	array_print_with_name(refData, M, "refData");
//	array_print_with_name(buffStart, maxLen, "buff");
	
	mQuery->maxLen = maxLen;
	data_t dist = us_distance_and_len_envelope(mQuery, buffStart, meanX, stdX, bsf,
											   mUseEnvelopes, &mBestLen);
//											   false, &mBestLen);	//TODO remove after test
//	if (! mUseEnvelopes) {
//		printf("not using envelopes...\n");
//	}
	mQuery->maxLen = mMaxWindowLen;
	
	// determine if the best length found is actually a better match (and not just
	// whatever garbage got placed in the integer when no better match was found)
	mBestLenValid = dist < bsf;
	
//	if (dist == INFINITY) {
//		printf("ERROR: USEuclideanSearcher: computed infinite dist");
//		exit(1);
//	}
	
	//	printf("t=%ld, dist = %.2f\n",t, dist);
	return dist;
}
