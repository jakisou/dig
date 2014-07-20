//
//  SwmSearcher.hpp
//  edtw
//
//  Created By <Anonymous> on 2/9/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__USDtwSearcher__
#define __edtw__USDtwSearcher__

#include "USSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"
#include "circEnvelope.h"

/** PatternSpotter using the z-normalized scaled warped matching distance */
class SwmSearcher: public USSearcher {
private:
	SwmSearcher& operator=(const SwmSearcher& other); // no copying
	
	static circEnvelope *sStreamEnvelope;
	
	swm_query* mQuery;
	double mMaxWarpR;
	steps_t mMaxWarp;
	
protected:
	void onDataReceived(tick_t t, data_t x);
	data_t computeDistance(const data_t* buffEnd,
						   tick_t t, tick_t cantStartUntilAfter,
						   data_t meanX, data_t stdX, data_t bsf);
	void onScalingsChanged();
	
public:
	void setMaxWarp(steps_t maxWarp);
	explicit SwmSearcher(Template const& ref, SpotterParams const& p);
	explicit SwmSearcher(const SwmSearcher& other);
	~SwmSearcher();
};

#endif /* defined(__edtw__USDtwSearcher__) */
