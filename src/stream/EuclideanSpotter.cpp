//
//  RefMonitor.cpp
//  StreamMain
//
//  Created by Davis blalock on 7/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "EuclideanSpotter.hpp"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "global_test_vals.h"
#include "flags.h"

// initialize streamStats object
streamStats *EuclideanSpotter::sStats = streamStats_new(kSTREAM_STATS_DEFAULT_LEN);

#ifdef ABANDON_PREFIX_DISTS
static const short int kDIST_PER_LEN_COEFF = 1000;	//wow, this has to be really high to avoid reducing recall
#endif
#ifdef ABANDON_BAD_REGRESSION
//static const data_t kHUGE_DIST = 32700; //just under 2^15, in case we ever switch to int16s
#endif
#ifdef ABANDON_BETA1
static const float kMIN_BETA1 = .2;
static const short int kMAX_BETA1 = 5;
#endif

static inline data_t optimal_dist(data_t Sxx, data_t Syy, data_t Sxy) {
	
	//store calls to distance function for profiling
	global_dist_calls++;
	
	data_t beta1 = Sxy / Sxx;
	
#ifdef ABANDON_BETA1
	if (beta1 < kMIN_BETA1 || beta1 > kMAX_BETA1) {
#else
	if (Sxy <= 0) {
#endif
	#ifdef ABANDON_BAD_REGRESSION
//		return kHUGE_DIST; //makes it insanely fast (almost identical to UCR_ED), but much less accurate
//		return Sxx - Sxy + Syy;
		return Syy;
	#else
		// the more negative Sxy, the worse the match; the more positive
		// Sxx and Syy, the farther apart the points are (with the offset
		// normalized)
//		return (Sxx - Sxy + Syy);
		
		return Syy;
	#endif
	}

	return Syy - Sxy*beta1;
}

// ================================================================
// Lifecycle
// ================================================================

EuclideanSpotter::EuclideanSpotter(Template const& ref, SpotterParams const& p):
	PrefixSpotter(ref, p)
{
	// allocate needed arrays
	D = new data_t[M];
	Sxy = new data_t[M];
	
	// ensure the shared statistics object keeps a long enough history
	if (EuclideanSpotter::sStats->len < M) {
		streamStats_setHistoryLength(EuclideanSpotter::sStats, M);
	}
	
	// allocate storage for our lists of relevant indices
	validIdxs	 = new idx_t[M+1];
	validIdxs[0] = 0; // 1st index always valid since first dist always 0
	
	// initially no valid indices at previous time step
	numParentIdxs = 0;
}

EuclideanSpotter::EuclideanSpotter(const EuclideanSpotter& other):
	PrefixSpotter(other),
	numParentIdxs(other.numParentIdxs)
{
	D			= (data_t*) realloc(D,			other.M*sizeof(data_t));
	Sxy			= (data_t*) realloc(Sxy,		other.M*sizeof(data_t));
	validIdxs	= (idx_t*)  realloc(validIdxs,	other.M*sizeof(idx_t));
	memcpy(D,			other.D,			other.M*sizeof(data_t));
	memcpy(Sxy,			other.Sxy,			other.M*sizeof(data_t));
	memcpy(validIdxs,	other.validIdxs,	other.M*sizeof(idx_t));
}

EuclideanSpotter::~EuclideanSpotter() {
	delete [] D;
	delete [] Sxy;
	delete [] validIdxs;
}

// ================================================================
// Distance Calculation
// ================================================================

void EuclideanSpotter::updateMatrix(const tick_t t, const data_t* sample,
									const data_t rawCutoff) {
	// -------------------------------
	// compute necessary scalars
	// -------------------------------
	mTime = t;
	data_t x = *sample;
	
	//determine normalized cutoff from raw cutoff
	data_t cutoff = denormalize(rawCutoff);
	
#ifdef ABANDON_PREFIX_DISTS
	data_t cutoffPerLen = kDIST_PER_LEN_COEFF * rawCutoff;
#endif
	
	// -------------------------------
	// ensure that we don't try to continue past the end of the array
	// -------------------------------
	if (numParentIdxs > 0 && validIdxs[numParentIdxs-1] == M-1) {
		numParentIdxs--;
	}
	
	// -------------------------------
	// update sufficient statistics
	// -------------------------------
	
	//ensure shared X statistics updated; if this is the first time step,
	//ensure that the stream statistics don't include data from previous
	//runs
	if (t == 0 && EuclideanSpotter::sStats->t > 0) {
		streamStats_reset(EuclideanSpotter::sStats);
	}
	streamStats_update(EuclideanSpotter::sStats, t, x);
	
	data_t *dx		= EuclideanSpotter::sStats->dx;	  // store these as locals
	data_t *Sxx		= EuclideanSpotter::sStats->Sxx;  // for legibility
	
	//update Sxy
	idx_t index;
	for (idx_t i = numParentIdxs - 1; i >= 0; i-- ) {
		index = validIdxs[i];
		Sxy[index+1] = Sxy[index] + dx[index+1] * yStats[index+1].deltaY;
	}
	Sxy[0] = 0;
	
	//TODO combine these loops
		// wait, nevermind; can't do that cuz validIdxs need to go in
		// ascending order and Sxy needs to go in descending order
			// although Sxy could do ascending with a tmp variable
	
	// -------------------------------
	// calculate distances
	// -------------------------------
	
	D[M-1] = INFINITY;		// if last index is invalid, infinite dist
	D[0] = 0;
	validIdxs[0] = 0;
	idx_t numValidIdxs = 1; // idx 0 always included since always zero dist
	idx_t nextIdx = validIdxs[0] + 1;
	tick_t firstConflictIdx = t - te;
	for (idx_t i = 0; i < numParentIdxs; i++) {
		index = nextIdx;
		nextIdx = validIdxs[i+1] + 1; // size M+1, so can't be out of bounds

		D[index] = optimal_dist(Sxx[index], yStats[index].Syy, Sxy[index]);

#ifdef ABANDON_PREFIX_DISTS
		if (D[index] <= cutoff && D[index] < (cutoffPerLen * yStats[index].Syy) &&
			(index < firstConflictIdx ||  D[index] < dMin) ) {
#else
		if (D[index] <= cutoff &&
			(index < firstConflictIdx ||  D[index] < dMin) ) {
#endif
			validIdxs[numValidIdxs++] = index;
		}
	}
	
	// TODO split this into 2 loops: one before overlap with the best-so-far
	// sequence and one after it; have the commented-out check before adding
	// stuff to validIdxs only check if index < firstConflictIdx in 1st loop
	// and only check if D[index] < dMin in 2nd loop
		// -have to enforce elsewhere that dMin is always <= cutoff then
			// make updateMatrix not take in a cutoff; have a setCutoff()
			// function instead
		// -also write that darn function to find the index of the first element
		// of an array greater than or equal to some value so we can avoid
		// checking if it's < firstConflictIdx each time
	
	// -------------------------------
	// update number of parent indices
	// -------------------------------
	numParentIdxs = numValidIdxs;
}

// ================================================================
// Other Methods
// ================================================================

bool EuclideanSpotter::sequenceLocallyOptimal(tick_t tend, data_t dist) const {
	// if there's any path that overlaps what we think is the optimal
	// one that still has a distance less than dist, we can't be sure yet
	// that we've found an optimal subsequence
	idx_t i, index;
	tick_t firstConflictingIdx = mTime - tend;
	for (i = 0; i < numParentIdxs; i++) {
		index = validIdxs[i];
		if (index >= firstConflictingIdx) {
			break;
		}
	}
	// now that we've found the first overlapping index, start checking for
	// subsequences with lower distances
	for ( ; i < numParentIdxs; i++) {
		index = validIdxs[i];
		if ( D[index] < dist ) {
			return false;
		}
	}
	return true;
}

/** To be called _before_ or _after_ processSample() to remove subsequences
 from the STWM that conflict with the start and end times provided */
void EuclideanSpotter::purgeAllOverlap(tick_t tstart, tick_t tend) {
	// discard sequences that overlap with the specified interval
	//
	// We carry out this discarding by examining each valid index,
	// and possibly adding it to a revised list of valid indices
	// if its corresponding start time doesn't conflict
	idx_t index;
	idx_t newNumValidIdxs = 0;
	idx_t maxIdx = (idx_t)((mTime - tend) - 1); // inheritance is always diagonal, so we can
												// compute where it conflicts directly
	for (idx_t i=0; i < numParentIdxs; i++) {
		index = validIdxs[i];
		if (index <= maxIdx) { 	//TODO binary search for first idx above maxIdx instead
			newNumValidIdxs++;
		} else {
			// once we get past maxIdx, we're done, since idxs are nondecreasing
			break;
		}
	}
	
	//if final index isn't valid after purge, ensure dist of infinity is stored
	if ( newNumValidIdxs < 1 || validIdxs[newNumValidIdxs-1] != (M-1) ) {
		D[M-1] = INFINITY;
	}

	// store the revised list of valid indices
	numParentIdxs = newNumValidIdxs;
}

void EuclideanSpotter::purgeInferiorOverlap(tick_t tstart, tick_t tend, data_t dist) {
	idx_t i, index;
	idx_t newNumValidIdxs = 0;
	idx_t maxIdx = (idx_t)((mTime - tend) - 1); // inheritance is always diagonal, so we can
												// compute where it conflicts directly
	// if no overlap at all, don't have to worry about distance
	for (i=0; i < numParentIdxs; i++) {
		index = validIdxs[i];
		if (index <= maxIdx) {
			newNumValidIdxs++;
		} else {
			break;
		}
	}
	// once start times overlap, only keep idxs with sufficiently small distances
	for ( ; i < numParentIdxs; i++) {
		index = validIdxs[i];
		if (D[index] < dist) {
			validIdxs[newNumValidIdxs++] = index;
		}
	}
	
	//if final index isn't valid after purge, ensure dist of infinity is stored
	if ( newNumValidIdxs < 1 || validIdxs[newNumValidIdxs-1] != (M-1) ) {
		D[M-1] = INFINITY;
	}
	
	// store the revised list of valid indices
	numParentIdxs = newNumValidIdxs;
}

data_t EuclideanSpotter::getCurrentDist() const {
	return normalize(D[M-1]);
}
tick_t EuclideanSpotter::getCurrentStartTime() const {
	return mTime - M + 1;
}
