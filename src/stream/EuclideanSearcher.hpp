//
//  EuclideanSearcher.h
//  edtw
//
//  Created By <Anonymous> on 2/2/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__EuclideanSearcher__
#define __edtw__EuclideanSearcher__

#include "WindowSpotter.hpp"
#include "type_defs.h"

/** PatternSpotter using the z-normalized Euclidean distance */
class EuclideanSearcher: public WindowSpotter {
private:
	EuclideanSearcher& operator=(const EuclideanSearcher& other); // no copying
	
protected:
	data_t computeDistance(const data_t* buffEnd,
						   tick_t t, tick_t cantStartUntilAfter,
						   data_t meanX, data_t stdX, data_t bsf);
	
public:
	explicit EuclideanSearcher(Template const& ref, SpotterParams const& p);
	explicit EuclideanSearcher(const EuclideanSearcher& other);
	~EuclideanSearcher();
};

#endif /* defined(__edtw__EuclideanSearcher__) */
