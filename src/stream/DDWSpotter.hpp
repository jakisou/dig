//
//  DDWSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_DDWSpotter_h
#define edtw_DDWSpotter_h

#include "PrefixSpotter.hpp"
#include "streamStats.h"
#include "basic_structs.hpp"
#include "type_defs.h"

static const steps_t kDEFAULT_MAX_WARP = 1;

/** Element of the DDW "stats" matrix, tracking sufficient statistics
 * about possible matching subseuqences in the recent history of the stream */
typedef struct SubseqStats {
	data_t meanX;	///< The mean value of the subsequence associated with this element
	data_t Sxx;		///< The sum of squared errors of the subsequence associated with this element
	data_t Sxy;		///< The Sxy statistic of the subsequence associated with this element
	tick_t S;		///< The start time of the subsequence associated with this element
	steps_t H;		///< How many horizontal steps have led to this element
} SubseqStats;

///** Precomputed sufficient statistics for the DDWSpotter's time series */
//typedef struct RefStats {
//	float deltaCoeff;
//	data_t deltaY;
//	data_t Syy;
//} RefStats;

/** @class PatternSpotter using the Dynamic Data Warping distance */
class DDWSpotter: public PrefixSpotter {

private:
	DDWSpotter& operator=(const DDWSpotter& other); // no copying

	// track the last index that was valid--don't need to calculate
	// indices beyond this
	idx_t prevHighestValidIdx;
	
	// columns of subsequence stats
	SubseqStats *A;
//	RefStats *yStats;
	
	// which indices are below the cutoff
	uint8_t *validityFlags;
	
	// distance values
	data_t *D;
	
	// max number of consecutive steps in horizontal or vertical direction
	steps_t maxWarp;
	
	// remove conflicts in possible subsequences
	void purgeAllOverlap(tick_t tstart, tick_t tend);
	void purgeInferiorOverlap(tick_t tstart, tick_t tend, data_t dist);
	
	// check if a subsequence is optimal
	bool sequenceLocallyOptimal(tick_t te, data_t dist) const;
	
	inline uint8_t idxValidAt(idx_t idx) const;
	inline void invalidateIdxAt(idx_t idx);
public:
	
	// -------------------------------
	// Overrides from PatternSpotter
	// -------------------------------
	
	// lifecycle
	explicit DDWSpotter(Template const& ref, SpotterParams const& p);
	explicit DDWSpotter(const DDWSpotter& other);
	~DDWSpotter();
	
	// getters
	data_t getCurrentDist() const;
	tick_t getCurrentStartTime() const;
	
	// getters using prefixes
	data_t getBestCurrentDist()		const { return bestPrefixDistance; };
	tick_t getBestCurrentStartTime()const { return bestPrefixStartTime; };
	
	// setters
	void setMaxWarp(steps_t maxWarp);
	
	// other public functions
	void updateMatrix(const tick_t t, const data_t* sample, data_t cutoff);
};

#endif
