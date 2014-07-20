//
//  SpringSpotter.h
//  edtw
//
//  Created By <Anonymous> on 5/20/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__SpringSpotter__
#define __edtw__SpringSpotter__

#include "type_defs.h"
#include "basic_structs.hpp"
#include "PrefixSpotter.hpp"

typedef struct {
	data_t D;
	tick_t S;
} StwmEntry;

class SpringSpotter: public PrefixSpotter {
private:
	SpringSpotter& operator=(const SpringSpotter& other); // no copying
	
	//Subsequence Time Warping Matrix current and previous columns
	StwmEntry *Aprev, *A;
	
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
	explicit SpringSpotter(Template const& ref, SpotterParams const& p);
	explicit SpringSpotter(const SpringSpotter& other);
	~SpringSpotter();
	
	// getters
	data_t getCurrentDist() const;
	tick_t getCurrentStartTime() const;
	
	// setters
	void setMaxWarp(steps_t maxWarp) { }
	
	// other public functions
	void updateMatrix(const tick_t t, const data_t* sample, data_t cutoff);
};


#endif /* defined(__edtw__SpringSpotter__) */
