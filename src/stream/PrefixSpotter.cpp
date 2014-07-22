//
//  PrefixSpotter.cpp
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdlib.h>
#include "PrefixSpotter.hpp"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "array_utils.h"
#include "ucr_funcs.h"

// ================================================================
// Lifecycle
// ================================================================

PrefixSpotter::PrefixSpotter(Template const& ref, SpotterParams const& p):
	PatternSpotter(ref, p),
	bestPrefix(M-1),
	bestPrefixStartTime(kINVALID),
	bestPrefixDistance(kINVALID),
	minPrefixLen(ref.nSamples),
	maxPrefixLen(ref.nSamples)
{
	//allocate storage for sufficient statistics
	//	deltaY	= new data_t[M];
	//	meanY	= new data_t[M];
	//	Syy		= new data_t[M];
	yStats = new RefStats[M];

	//initialize sufficient statistics for y
	data_t y = refData[0];
	data_t meanY = y;
	data_t dy;
	yStats[0].deltaY = 0;
	yStats[0].deltaCoeff = 0;
	yStats[0].Syy = 0;
	for (idx_t i = 1; i < M; i++) {
		y = refData[i];
		dy = y - meanY;
		meanY = meanY + dy/(i+1);

		yStats[i].deltaY = y - meanY;
		yStats[i].Syy = yStats[i-1].Syy + dy*(yStats[i].deltaY);
		yStats[i].deltaCoeff = (1.0f - 1.0f/(i+1));
	}

	// intialize normalization factor
	normalizationFactor = yStats[M-1].Syy; // equivalent to z-normalization + length normalization
}

PrefixSpotter::PrefixSpotter(const PrefixSpotter& other):
	PatternSpotter(other),
	bestPrefix(other.bestPrefix),
	bestPrefixStartTime(other.bestPrefixStartTime),
	bestPrefixDistance(other.bestPrefixDistance),
	minPrefixLen(other.minPrefixLen),
	maxPrefixLen(other.maxPrefixLen)
{
	yStats	 = (RefStats*) realloc(yStats, other.M*sizeof(RefStats));
	memcpy(yStats, other.yStats, other.M*sizeof(RefStats));
}

PrefixSpotter::~PrefixSpotter() {
	delete [] yStats;
}

// ================================================================
// Shared methods
// ================================================================

bool PrefixSpotter::sequenceLocallyOptimal(Subsequence seq) const {

	// if just greedily reporting anything below the cutoff, matches are
	// always considered optimal
	if (mReportCriteria == AGGRESSIVE) {
		return true;
	}

	// same class -> can't change classification, so consider it
	// optimal if this flag is set
	if (mSameClassMeansOptimal && seq.label == mLabel) {
		return true;
	}

	return sequenceLocallyOptimal(seq.te, denormalize(seq.dist));
}

void PrefixSpotter::purgeAllOverlap(Subsequence seq) {
	purgeAllOverlap(seq.ts, seq.te);
}

void PrefixSpotter::purgeInferiorOverlap(Subsequence seq) {
	purgeInferiorOverlap(seq.ts, seq.te, seq.dist);
}