//
//  EuclideanSearcher.cpp
//  edtw
//
//  Created By <Anonymous> on 2/2/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "EuclideanSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"

// ================================================================
// Lifecycle
// ================================================================

EuclideanSearcher::EuclideanSearcher(Template const& ref, SpotterParams const& p):
	WindowSpotter(ref, p) {}

EuclideanSearcher::EuclideanSearcher(const EuclideanSearcher& other):
WindowSpotter(other) {}

EuclideanSearcher::~EuclideanSearcher() {}

// ================================================================
// Methods
// ================================================================

data_t EuclideanSearcher::computeDistance(const data_t* buffEnd,
										  tick_t t, tick_t cantStartUntilAfter,
										  data_t meanX, data_t stdX,
										  data_t bsf) {
	tick_t startIdx = t - M + 1;
	if ( startIdx <= cantStartUntilAfter ) return INFINITY;
	
	const data_t *buffStart = buffEnd - M + 1;

	data_t dist = euclidean_dist_sq(mSortedY, buffStart, M, meanX, stdX, mOrder, bsf);
	return dist;
}
