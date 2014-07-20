//
//  DtwSearcher.h
//  edtw
//
//  Created By <Anonymous> on 2/2/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__DtwSearcher__
#define __edtw__DtwSearcher__

#include "WindowSpotter.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"
#include "circEnvelope.h"

//static const float kDEFAULT_WARPING_R = .05;
static const float kDEFAULT_WARPING_WIDTH = 10;
static const unsigned int kDEFAULT_ENVELOPE_LEN_LOG2 = 5;	//len = 2^5 = 32

/** PatternSpotter using the z-normalized dynamic time warping distance */
class DtwSearcher: public WindowSpotter {
private:
	DtwSearcher& operator=(const DtwSearcher& other); // no copying
	
	static circEnvelope *sStreamEnvelope;
	
	dtw_query *mDtwQuery;
//	data_t *mCumBound1, *mCumBound2;		//arrays in which to write cumulative lower bounds
//	data_t *mNormalizedData;
	
	steps_t mMaxWarp;
	double mMaxWarpR;
	
protected:
	void onDataReceived(tick_t t, data_t x);
	data_t computeDistance(const data_t* buffEnd,
						   tick_t t, tick_t cantStartUntilAfter,
						   data_t meanX, data_t stdX, data_t bsf);
	
public:
	explicit DtwSearcher(Template const& ref, SpotterParams const& p);
	explicit DtwSearcher(const DtwSearcher& other);
	~DtwSearcher();
	
	// -------------------------------
	// Overrides from PatternSpotter
	// -------------------------------
//	tick_t getCurrentStartTime() const;
//	virtual tick_t getCurrentEndTime() const;
	void setMaxWarp(steps_t maxWarp);
};

#endif /* defined(__edtw__DtwSearcher__) */
