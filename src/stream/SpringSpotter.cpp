//
//  SpringSpotter.cpp
//  edtw
//
//  Created By <Anonymous> on 5/20/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include "SpringSpotter.hpp"
#include "global_test_vals.h"

SpringSpotter::SpringSpotter(Template const& ref, SpotterParams const& p):
	PrefixSpotter(ref, p)
{
	Aprev = new StwmEntry[M];
	A = new StwmEntry[M];
	
	// at t = 0, no valid distances from previous time step
	for (idx_t i = 0; i < M; i++) {
		A[i].D = INFINITY;
	}
}

SpringSpotter::SpringSpotter(const SpringSpotter& other):
	PrefixSpotter(other)
{
	Aprev	= (StwmEntry*)realloc(Aprev,	other.M*sizeof(StwmEntry));
	A		= (StwmEntry*)realloc(A,		other.M*sizeof(StwmEntry));
	memcpy(Aprev,	other.Aprev,	other.M*sizeof(StwmEntry));
	memcpy(A,		other.A,		other.M*sizeof(StwmEntry));
}

SpringSpotter::~SpringSpotter() {
	delete [] Aprev;
	delete [] A;
}

static inline data_t dist(data_t x, data_t y) {
	global_dist_calls++;
	data_t diff = x - y;
	return diff*diff;
}

void SpringSpotter::updateMatrix(const tick_t t, const data_t* sample, data_t rawCutoff) {
	
	// -------------------------------
	// compute necessary scalars
	// -------------------------------
	data_t x = *sample;
	
	//determine normalized cutoff from raw cutoff
//	data_t cutoff = denormalize(rawCutoff);		//SPRING doesn't use a cutoff...
	
	// -------------------------------
	// Set previous column as current column
	// -------------------------------
	StwmEntry* tmp = Aprev;
	Aprev = A;
	A = tmp;
	
	// -------------------------------
	// Initialize index 0
	// -------------------------------
	A[0].D = dist(x, refData[0]);
	A[0].S = t;
	
	// -------------------------------
	// Main loop: compute indices 1 to M-1
	// -------------------------------
	for (idx_t idx = 1; idx < M; idx++) {
		idx_t oldIdx = idx-1;
		data_t prevDist = A[oldIdx].D;
		A[idx].S = A[oldIdx].S;
		
		// if the distance at the same index in the previous time step
		// is smaller, use that instead
		if (Aprev[idx].D < prevDist) {
			prevDist = Aprev[idx].D;
			A[idx].S = Aprev[idx].S;
		}
		
		// if the distance of the current index - 1 is at least as small
		// as the best of the other two, use that instead. This one gets
		// the ties because we prefer diagonal moves, since it means one
		// value in the reference is corresponding to one value in the
		// stream
		if (Aprev[oldIdx].D <= prevDist) {
			prevDist = Aprev[oldIdx].D;
			A[idx].S = Aprev[oldIdx].S;
		}
		
		// compute distance at this index using best previous distance
		A[idx].D = prevDist + dist(refData[idx], x);
	}
}

void SpringSpotter::purgeAllOverlap(tick_t tstart, tick_t tend) {
	purgeInferiorOverlap(tstart, tend, -1);
}

void SpringSpotter::purgeInferiorOverlap(tick_t tstart, tick_t tend, data_t dist) {
	idx_t i;
	for (i = 0; i < M; i++) {
		if (A[i].S <= tend) {
			break;
		}
	}
	// now that we've found the first overlapping index, start checking for
	// subsequences with lower distances
	double distToBeat = dist - kFLOATING_PT_ERROR;
	for ( ; i < M; i++) {
		if ( A[i].D >= distToBeat ) {
			A[i].D = INFINITY;
		}
	}
}

// check if a subsequence is optimal
bool SpringSpotter::sequenceLocallyOptimal(tick_t tend, data_t dist) const {
	idx_t i;
	for (i = 0; i < M; i++) {
		if (A[i].S <= tend) {
			break;
		}
	}
	// now that we've found the first overlapping index, start checking for
	// subsequences with lower distances
	double distToBeat = dist - kFLOATING_PT_ERROR;
	for ( ; i < M; i++) {
		if ( A[i].D < distToBeat ) {
			return false;
		}
	}
	return true;
}

// getters
data_t SpringSpotter::getCurrentDist() const {
	return normalize(A[M-1].D);
}
tick_t SpringSpotter::getCurrentStartTime() const {
	return A[M-1].S;
}

//// just swap pointers so we can avoid copying data
//
//// pattern cells (distances, etc.)
//SpringPatCell* tmp = Aprev;
//Aprev = A;
//A = tmp;
