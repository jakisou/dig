//
//  similaritySearch.h
//  edtw
//
//  Created By <Anonymous> on 5/17/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__similaritySearch__
#define __edtw__similaritySearch__

#include "type_defs.h"
#include "basic_structs.hpp"
#include "Subsequence.hpp"

//Subseq findNearestNeighbors(vector<Sequence> timeSeries, unsigned int numToFind);

typedef std::pair<Subsequence, Subsequence> TwoMotif;

void printMotif(TwoMotif motif);

TwoMotif findMotifOneQueryLen(SpotterParams p, data_t *data, length_t dataLen,
							  length_t queryLen, length_t minLen, length_t maxLen,
							  data_t initialBest=INFINITY);

TwoMotif findMotifAllQueryLens(SpotterParams p, data_t *data, length_t dataLen,
							   length_t minLen, length_t maxLen,
							   data_t initialBest=INFINITY);

/** returns the number of subsequences of the right answers that were found (either 0,1, or 2) */

TwoMotif searchMotifLensUntilFound(SpotterParams p,
									data_t *data, length_t dataLen,
									length_t minLen, length_t maxLen,
									const TwoMotif rightAnswers,
									data_t initialBest=INFINITY);

//have this log runtime and write the data corrsponding to the subseqs
//to some file somewhere
TwoMotif findMotif(SpotterParams p, data_t *data, length_t dataLen,
				   length_t minLen, length_t maxLen, data_t initialBest=INFINITY);

TwoMotif findMotifInStream(SpotterParams p, Stream dataStream,
						   length_t minMotifLen, length_t maxMotifLen);

TwoMotif findMotifInStream(SpotterParams p, std::string streamDir,
						   length_t minMotifLen, length_t maxMotifLen);

#endif /* defined(__edtw__similaritySearch__) */
