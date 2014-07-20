//
//  USUSSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "USSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"
#include "array_utils.h"

// ================================================================
// Lifecycle
// ================================================================

USSearcher::USSearcher(Template const& ref, SpotterParams const& p):
	WindowSpotter(ref, p),
	mBestLen(-1),
	mBestLenValid(false),
	mUseEnvelopes(false)
{
//	setScaleFactor(kDEFAULT_SCALE_FACTOR);	//original--can we replace this with the actual value?
//		setScaleFactor(p.scaleFactor);	//have to call in subclasses cuz onScalingsChanged() is virtual
}

USSearcher::USSearcher(const USSearcher& other):
	WindowSpotter(other),
	mBestLen(other.mBestLen),
	mBestLenValid(other.mBestLenValid),
	mUseEnvelopes(other.mUseEnvelopes)
{}

USSearcher::~USSearcher() {}

// ================================================================
// Methods
// ================================================================

void USSearcher::setUseScalingEnvelopes(bool useEnvelopes) {
	 mUseEnvelopes = useEnvelopes;
}

tick_t USSearcher::getCurrentStartTime() const {
	if (mBestLenValid) {
		return mTime - mBestLen + 1;
	}
	return -1;
}

void USSearcher::setMaxScaling(double r) {
	setScalings(mMinWindowLen / M, r);
}

void USSearcher::setMinScaling(double r) {
	setScalings(r, mMaxWindowLen / M);
}

//NOTE: this sets the min and max scalings to 1/(1+r),1+r respectively;
//ie a scale factor of 25% yields lengths from 80% to 125% and a scale
//factor of 100% yields lengths from 50% to 200%
void USSearcher::setScaleFactor(double r) {
	setScalings(1/(1+r), 1+r);
}

void USSearcher::setScalings(double rMin, double rMax) {
	if (rMin < 0 ||rMax < 0) {
		printf("ERROR: USSearcher: tried to set scalings < 0\n");
		exit(1);
	}
	if (rMax < rMin || rMin > 1 || rMax < 1) {
		printf("ERROR: USSearcher:tried to set bad rMin, rMax %.3f, %.3f\n",
			   rMin, rMax);
		exit(1);
	}
	
	int newMinLen = round(rMin * M);
	int newMaxLen = round(rMax * M);
	
	setMatchLenRange(newMinLen, newMaxLen);
}

void USSearcher::setMatchLenRange(length_t newMinLen, length_t newMaxLen) {
	if ( newMinLen == mMinWindowLen && newMaxLen == mMaxWindowLen ) return;
	
	// if we have to change the min or max scaling, we need to construct a
	// new us_query object, since it precomputes information based on the max
	// and min query lengths
	mMinWindowLen = newMinLen;
	mMaxWindowLen = newMaxLen;
	
	onScalingsChanged();
}
