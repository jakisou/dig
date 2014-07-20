//
//  USEuclideanSearcher.h
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__USEuclideanSearcher__
#define __edtw__USEuclideanSearcher__

#include "USSearcher.hpp"
#include "basic_structs.hpp"
#include "type_defs.h"
#include "ucr_funcs.h"

/** PatternSpotter using the z-normalized uniform scaling distance */
class USEuclideanSearcher: public USSearcher {
private:
	USEuclideanSearcher& operator=(const USEuclideanSearcher& other); // no copying
	us_query* mQuery;

protected:
	data_t computeDistance(const data_t* buffEnd,
						   tick_t t, tick_t cantStartUntilAfter,
						   data_t meanX, data_t stdX, data_t bsf);
	void onScalingsChanged();
	
public:
//	tick_t getCurrentStartTime() const;
//	void setScaleFactor(double r);
//	void setMinScaling( double r);
//	void setMaxScaling( double r);
//	void setScalings(double rMin, double rMax);
	explicit USEuclideanSearcher(Template const& ref, SpotterParams const& p);
	explicit USEuclideanSearcher(const USEuclideanSearcher& other);
	~USEuclideanSearcher();
};

#endif /* defined(__edtw__USEuclideanSearcher__) */
