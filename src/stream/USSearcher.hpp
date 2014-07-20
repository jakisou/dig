//
//  USSearcher.h
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__USSearcher__
#define __edtw__USSearcher__

#include "WindowSpotter.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"

static const double kDEFAULT_SCALE_FACTOR = 0; // no scaling

/** Abstract class encapsulating shared functoinality for uniform scaling
 * search and scaled warped matching PatternSpotters */
class USSearcher: public WindowSpotter {
private:
	USSearcher& operator=(const USSearcher& other); // no copying

protected:
	length_t mBestLen;
	bool mBestLenValid;
	bool mUseEnvelopes;
	virtual data_t computeDistance(const data_t* buffEnd,
						   tick_t t, tick_t cantStartUntilAfter,
								   data_t meanX, data_t stdX, data_t bsf) = 0;

	virtual void onScalingsChanged() = 0;
public:
	void setUseScalingEnvelopes(bool useEnvelopes);
	
	tick_t getCurrentStartTime() const;
	void setScaleFactor(double r);
	void setMinScaling( double r);
	void setMaxScaling( double r);
	void setScalings(double rMin, double rMax);
	void setMatchLenRange(length_t minLen, length_t maxLen);
	
	explicit USSearcher(Template const& ref, SpotterParams const& p);
	explicit USSearcher(const USSearcher& other);
	virtual ~USSearcher();
};

#endif /* defined(__edtw__USSearcher__) */
