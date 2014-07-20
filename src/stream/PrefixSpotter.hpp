//
//  PrefixSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__PrefixSpotter__
#define __edtw__PrefixSpotter__

#include <iostream>
#include "PatternSpotter.hpp"
#include "type_defs.h"
#include "basic_structs.hpp"

/** Precomputed sufficient statistics for the PrefixSpotter's time series */
typedef struct RefStats {
	data_t deltaY;
	data_t Syy;
	float deltaCoeff;
} RefStats;

/** Abstract class encapsulating shared functionality for EuclideanSpotter and
  * DTWSpotter */
class PrefixSpotter: public PatternSpotter {

private:
	PrefixSpotter& operator=(const PrefixSpotter& other); // no copying

	data_t normalizationFactor;
	
protected:
	//arrays of precomputed mean(y) and Syy values
	RefStats *yStats;
	
	//best prefix at current time step
	idx_t  bestPrefix;
	tick_t bestPrefixStartTime;
	data_t bestPrefixDistance;
	
	//prefixes to care about (at present, only used by DDW)
	length_t minPrefixLen;
	length_t maxPrefixLen;		//TODO dont need to compute prefixes past this
	
	// remove conflicts in possible subsequences
	virtual void purgeAllOverlap(tick_t tstart, tick_t tend) = 0;
	virtual void purgeInferiorOverlap(tick_t tstart, tick_t tend, data_t dist) = 0;
	
	// check if a subsequence is optimal
	virtual bool sequenceLocallyOptimal(tick_t te, data_t dist) const = 0;
	
	/** convert distances calculated internally to the normalized distances
	 * used to compare different PatternMonitors' results */
	data_t normalize(data_t dist) const { return dist / normalizationFactor; }
	
	/** convert the normalized distances used to compare pattern monitors to
	 * a non-normalized distance that we use internally */
	data_t denormalize(data_t dist) const { return dist * normalizationFactor;}
	
public:
	
	// -------------------------------
	// Overrides from PatternSpotter
	// -------------------------------
	
	// lifecycle
	explicit PrefixSpotter(Template const& ref, SpotterParams const& p);
	explicit PrefixSpotter(const PrefixSpotter& other);
	~PrefixSpotter();
	
	// getters
	virtual data_t getCurrentDist() const = 0;
	virtual tick_t getCurrentStartTime() const = 0;
	idx_t  getBestCurrentPrefix() const { return bestPrefix; };
	
	// setters
	void setSameClassMeansOptimal(bool meansOptimal);
	void setQueryLenRange(length_t minLen, length_t maxLen) {
		minPrefixLen = minLen;
		maxPrefixLen = maxLen;
	}
	
	// other public functions
	virtual void updateMatrix(const tick_t t, const data_t* sample, data_t cutoff) = 0;
	virtual bool sequenceLocallyOptimal(Subsequence seq) const;
	void purgeAllOverlap(Subsequence seq);
	void purgeInferiorOverlap(Subsequence seq);
};

#endif /* defined(__edtw__PrefixSpotter__) */
