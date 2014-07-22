//
//  DDWSpotter.cpp
//  edtw
//
//  Created By <Anonymous> on 1/26/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "DDWSpotter.hpp"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "array_utils.h"
#include "global_test_vals.h"
#include "flags.h"

// different acceleration / early abandoning strategies
//#define ABANDON_PREFIX_DISTS	//pp -= .1, recall -= 2%, unit test safe	//moved to flags.h
//#define ABANDON_BETA0			//TODO implement somehow
//#define ABANDON_BETA1			//pp -= .2, recall -= 1%, unit test unsafe
//#define ABANDON_NEG_SXX_SXY	//pp -= .05, prec += 1%, rec -=1%, unit test unsafe

#ifdef ABANDON_PREFIX_DISTS
static const short int kDIST_PER_LEN_COEFF = 5;
#endif
#ifdef ABANDON_BETA1
static const short int kMAX_BETA1 = 5;
static const float kMIN_BETA1 = 1/kMAX_BETA1;
#endif
#ifdef ABANDON_NEG_SXX_SXY
static const data_t kHUGE_DIST = 32700; //just under 2^15, in case we ever switch to int16s
#endif

static const uint8_t VALIDITY_SHIFT	= 0x4;
static const uint8_t DIST_VALID		= 0x1;
static const uint8_t DIAG_VALID		= 0x2;
static const uint8_t VERT_VALID		= 0x4;
static const uint8_t HORZ_VALID		= 0x8;
static const uint8_t POSSIBLY_VALID_MASK = VERT_VALID | DIAG_VALID | HORZ_VALID;
static const uint8_t NEW_DIST_VALID	= DIST_VALID << VALIDITY_SHIFT;
static const uint8_t NEW_DIAG_VALID	= DIAG_VALID << VALIDITY_SHIFT;
static const uint8_t NEW_HORZ_VALID	= HORZ_VALID << VALIDITY_SHIFT;

// ================================================================
// Inline utility functions
// ================================================================

inline uint8_t DDWSpotter::idxValidAt(idx_t idx) const {
	return (validityFlags[idx] & DIST_VALID);
}
inline void DDWSpotter::invalidateIdxAt(idx_t idx) {
	validityFlags[idx] &= ~(DIST_VALID | HORZ_VALID);
	validityFlags[idx+1] &= ~(DIAG_VALID);
}

inline data_t inheritAndCalcDist(SubseqStats* src, SubseqStats* dest, data_t x,
								 data_t deltaY, data_t Syy, float deltaCoeff,
								 idx_t n) {
	//store calls to distance function for profiling
	global_dist_calls++;

	//compute updated stats
	data_t dx = x - src->meanX;
	dest->Sxy = src->Sxy + dx*deltaY;
	dest->Sxx = src->Sxx + dx * dx * deltaCoeff;
	dest->meanX = src->meanX + dx / n;

	//ensure that inherited value will be sensible before (possibly) abandoning
	dest->S = src->S;
	dest->H = 0;

	data_t beta1 = dest->Sxy / dest->Sxx;

	// For linear regression, Y = b1*X + b0 + error; don't allow
	// negative b1, since this means Y resembles -X, not X. Since
	// b1 = Sxy / Sxx and Sxx is nonnegative, we can just check Sxy.
	// We can also just check if b1 is less than 0, since this happens
	// if and only if Sxy is less than 0.
#ifdef ABANDON_BETA1			//seems to slightly improve speed + accuracy
	if (beta1 < kMIN_BETA1 || beta1 > kMAX_BETA1) {
#else
	if (dest->Sxy <= 0) {
#endif
	#ifdef ABANDON_NEG_SXX_SXY	//either abandon entirely or just return a large distance
		return kHUGE_DIST;
	#else
		// the more negative Sxy, the worse the match; the more positive
		// Sxx and Syy, the farther apart the points are (with the offset
		// normalized)
		return dest->Sxx - dest->Sxy + Syy;
	#endif
	}
	return Syy - dest->Sxy*beta1;
}

// ================================================================
// Lifecycle
// ================================================================

DDWSpotter::DDWSpotter(Template const& ref, SpotterParams const& p):
	PrefixSpotter(ref, p),
	maxWarp(p.maxWarp)
{
	// allocate needed arrays
	validityFlags = new uint8_t[M+1];
	A = new SubseqStats[M];
	D = new data_t[M];
//	yStats = new RefStats[M];

	// initially, no indices valid
	memset(validityFlags, 0, (M+1)*sizeof(uint8_t));

	// bundle up the arrays of y statistics into a single array
	// of structs for better spatial locality
//	for (idx_t i = 0; i < M; i++) {
//		yStats[i].deltaY = deltaY[i];
//		yStats[i].Syy = Syy[i];
//		yStats[i].deltaCoeff = (1.0f - 1.0f/(i+1));
//	}
//	printf("DDWSpotter has maxWarp of %d\n", maxWarp);

	prevHighestValidIdx = 0;
}

DDWSpotter::DDWSpotter(const DDWSpotter& other):
	PrefixSpotter(other),
	prevHighestValidIdx(other.prevHighestValidIdx),
	maxWarp(other.maxWarp)
{
	validityFlags	=(uint8_t*) realloc(validityFlags, (other.M+1)*sizeof(uint8_t));
	A		   = (SubseqStats*) realloc(A,			other.M*sizeof(SubseqStats));
	D				= (data_t*)	realloc(D,			other.M*sizeof(data_t));
//	yStats		  = (RefStats*) realloc(yStats,		other.M*sizeof(RefStats));
	memcpy(validityFlags, other.validityFlags,	   (other.M+1)*sizeof(uint8_t));
	memcpy(A,			other.A,			other.M*sizeof(SubseqStats));
	memcpy(D,			other.D,			other.M*sizeof(data_t));
//	memcpy(yStats,		other.yStats,		other.M*sizeof(RefStats));
}

DDWSpotter::~DDWSpotter() {
	delete [] validityFlags;
	delete [] A;
	delete [] D;
//	delete [] yStats;
}

// ================================================================
// Distance Calculation
// ================================================================

void DDWSpotter::updateMatrix(const tick_t t, const data_t* sample,
							   const data_t rawCutoff) {
	SubseqStats Adiag = A[0], Ahorz;
	idx_t vertWarpStart = 0;
	idx_t n;
	SubseqStats stats;
	data_t d, dBest;
	data_t deltaY, Syy;
	float deltaCoeff;
	idx_t highestValidIdx = 0;
	mTime = t;

#ifdef TRACK_PREFIX_DISTS
	bestPrefixDistance = INFINITY;
#endif

	// -------------------------------
	// compute necessary scalars
	// -------------------------------
	data_t x = *sample;

	//determine normalized cutoff from raw cutoff
	data_t cutoff = denormalize(rawCutoff);

#ifdef ABANDON_PREFIX_DISTS
//	data_t maxDistPerLen = kDIST_PER_LEN_COEFF * cutoff / M;
	data_t maxDistPerLen = kDIST_PER_LEN_COEFF * rawCutoff;
//	data_t totalErrorAllowed = 1 * cutoff * yStats[M-1].Syy;
#endif

	// -------------------------------
	// Initialize final distance
	// -------------------------------
	D[M-1] = INFINITY;

	// -------------------------------
	// Initialize index 0
	// -------------------------------
	A[0] = {};
	A[0].S = t;
	A[0].meanX = x;
	D[0] = 0;
	validityFlags[0] = DIST_VALID;
	validityFlags[1] |= NEW_DIAG_VALID;

#ifndef ABANDON_NEG_SXX_SXY 	//Sxx always 0, so vert dist undefined
	if (maxWarp > 0) {
		validityFlags[1] |= VERT_VALID;
	}
#endif

	// -------------------------------
	// Main loop: compute indices 1 to M-1
	// -------------------------------
	for (idx_t i = 1; i < M; i++) {

		// if can't inherit from any direction, go to next idx;
		// repeat until a possibly valid idx
		if (! (validityFlags[i] & POSSIBLY_VALID_MASK) ) {
			validityFlags[i] = validityFlags[i] >> VALIDITY_SHIFT;
//			global_idx_skips++;

			// if past the last index that could possibly be valid (which
			// is the one diagonally above the last one that was valid at
			// the previous time step), we're done for this time step
			if (i > prevHighestValidIdx+1) {
				break;
			}

			while (i < M-1) {
				if (validityFlags[i+1] & POSSIBLY_VALID_MASK) {
					Adiag = A[i];
					break;
				} else {
//					global_idx_skips++;
					i++;
					validityFlags[i] = validityFlags[i] >> VALIDITY_SHIFT;
				}
			}
			continue;
		}

		// store the stats that were at this index at the previous
		// time step so the next index can have them for diagonal
		// inheritance
		Ahorz = A[i];

		// check if we can inherit horizontally; horizontal inheritance
		// doesn't changes the stats, so we don't need to compute anything
		if (validityFlags[i] & HORZ_VALID) {
			dBest = D[i];
			A[i].H++;
		} else {
			dBest = INFINITY;
		}

		if (validityFlags[i] & (DIAG_VALID | VERT_VALID) ) {
			// get y stats for this index if we can
			// inherit vertically or diagonally
			deltaY = yStats[i].deltaY;
			Syy = yStats[i].Syy;
			n = i+1;
//			deltaCoeff = 1.0f - 1.0f / n;
			deltaCoeff = yStats[i].deltaCoeff;	//actually faster to lookup...

			// check diagonal inheritance
			if (validityFlags[i] & DIAG_VALID) {
				d = inheritAndCalcDist(&Adiag, &stats, x, deltaY, Syy, deltaCoeff, n);
				if (d <= dBest + kFLOATING_PT_ERROR) {
					dBest = d;
					A[i] = stats;
				}
			}

			// check vertical inheritance, making sure to reset the vertical
			// warping constraint unless we actually do inherit vertically
			if (validityFlags[i] & VERT_VALID) {
				d = inheritAndCalcDist(&A[i-1], &stats, x, deltaY, Syy, deltaCoeff, n);
				if (d < dBest) {
					dBest = d;
					A[i] = stats;
				} else {
					vertWarpStart = i;
				}
			} else {
				vertWarpStart = i;
			}
		}

#ifdef ABANDON_PREFIX_DISTS
		data_t prefixDist = dBest / yStats[i].Syy;
		if (dBest < cutoff && prefixDist < maxDistPerLen ) {	// denormalized by Syy
#else
#ifdef TRACK_PREFIX_DISTS
		data_t prefixDist = dBest / yStats[i].Syy;
#endif
		if (dBest < cutoff) {
#endif
			validityFlags[i] |= NEW_DIST_VALID;

			D[i] = dBest;
			if (A[i].H < maxWarp) {
				validityFlags[i]   |= NEW_HORZ_VALID;
			}
			if (i - vertWarpStart < maxWarp) {
				validityFlags[i+1] |= VERT_VALID;
			}
			validityFlags[i+1] |= NEW_DIAG_VALID;

			highestValidIdx = i;

#ifdef TRACK_PREFIX_DISTS
			if (i+1 >= minPrefixLen && prefixDist < bestPrefixDistance) {
				bestPrefix = i;
				bestPrefixDistance = prefixDist;
				bestPrefixStartTime = A[i].S;
			}
#endif
		}

		// store the stats that the next index could inherit diagonally
		Adiag = Ahorz;

		// conceptually, we maintain two arrays that describe which
		// directions it's valid to inherit from at the current time
		// step and next time step; in practice, we just maintain one
		// array and use bits [0:2] to describe the current time step
		// and bits [3:5] to describe the next time step. After we finish
		// computing index i, the next time step becomes the current
		// time step at this index
		validityFlags[i] = validityFlags[i] >> VALIDITY_SHIFT;
	}

	prevHighestValidIdx = highestValidIdx;
}

// ================================================================
// Other Methods
// ================================================================

void DDWSpotter::purgeAllOverlap(tick_t tstart, tick_t tend) {
	purgeInferiorOverlap(tstart, tend, -1);
}

void DDWSpotter::purgeInferiorOverlap(tick_t tstart, tick_t tend, data_t dist) {
	idx_t i;

	// if no overlap at all, don't have to worry about distance
	for (i=0; i < M; i++) {
		if ( idxValidAt(i) && A[i].S <= tend) {
			// once we find the first conflicting idx, we're done,
			// since start times are nonincreasing
			break;
		}
	}

	// once start times overlap, all future start times will also overlap
	if (dist <= 0) {
		// if can't possibly beat dist, just invalidate everything
		memset(&validityFlags[i], 0, (M+1-i)*sizeof(uint8_t));

		// it's possible that the first index we purged could inherit
		// diagonally at the next time step
		if ( i > 0 && idxValidAt(i-1) ) {
			validityFlags[i] = DIAG_VALID;
		}
	} else {
		// otherwse, invalidate everything with a worse distance
		for ( ; i < M; i++) {
			if (idxValidAt(i) && D[i] >= dist) {
				invalidateIdxAt(i);
			}
		}
	}

	//if final index isn't valid after purge, ensure dist of infinity is stored
	if (! idxValidAt(M-1) ) {
		D[M-1] = INFINITY;
	}
}

bool DDWSpotter::sequenceLocallyOptimal(tick_t tend, data_t dist) const {
	// if there's any path that overlaps what we think is the optimal
	// one that still has a distance less than dist, we can't be sure yet
	// that we've found an optimal subsequence
	idx_t i;
	for (i = 0; i < M; i++) {
		if (idxValidAt(i) && A[i].S <= tend ) {
			break;
		}
	}
	// now that we've found the first overlapping index, start checking for
	// subsequences with lower distances
	double distToBeat = dist - kFLOATING_PT_ERROR;
	for ( ; i < M; i++) {
		if ( idxValidAt(i) && D[i] < distToBeat ) {
			return false;
		}
	}
	return true;
}

// getters
data_t DDWSpotter::getCurrentDist() const {
	return normalize(D[M-1]);
}
tick_t DDWSpotter::getCurrentStartTime() const {
	return A[M-1].S;
}

// setters
void DDWSpotter::setMaxWarp(steps_t warp) {
	maxWarp = warp;
}