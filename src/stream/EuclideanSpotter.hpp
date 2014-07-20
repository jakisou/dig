//
//  EuclideanSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_EuclideanSpotter_h
#define edtw_EuclideanSpotter_h

#include "PrefixSpotter.hpp"
#include "streamStats.h"
#include "basic_structs.hpp"

static const idx_t kSTREAM_STATS_DEFAULT_LEN = 10;

/** PatternSpotter using the optimally normalized Euclidean distance */
class EuclideanSpotter: public PrefixSpotter {

private:
	EuclideanSpotter& operator=(const EuclideanSpotter& other); // no copying
	
	//store stats about the stream for all instances
	static streamStats *sStats;

	//distance values and remaining sufficient statistics
	data_t *D;
	data_t *Sxy;
	
	// indices of valid (not pruned) subsequences at each time step
	idx_t *validIdxs;
	
	// number of valid indices at the previous time step
	idx_t numParentIdxs;

	// remove conflicts in possible subsequences
	void purgeAllOverlap(tick_t tstart, tick_t tend);
	void purgeInferiorOverlap(tick_t tstart, tick_t tend, data_t dist);
	
	// check if a subsequence is optimal
	bool sequenceLocallyOptimal(tick_t te, data_t dist) const;
	
public:
	
	// -------------------------------
	// Overrides from PatternSpotter
	// -------------------------------
	
	// lifecycle
	explicit EuclideanSpotter(Template const& ref, SpotterParams const& p);
	explicit EuclideanSpotter(const EuclideanSpotter& other);
	~EuclideanSpotter();

	// getters
	data_t getCurrentDist() const;
	tick_t getCurrentStartTime() const;
	
	// setters
	void setMaxWarp(steps_t maxWarp) { }
	
	// other public functions
	void updateMatrix(const tick_t t, const data_t* sample, data_t cutoff);
};

#endif
