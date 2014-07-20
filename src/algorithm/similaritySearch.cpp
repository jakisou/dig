//
//  similaritySearch.cpp
//  edtw
//
//  Created By <Anonymous> on 5/17/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include "similaritySearch.hpp"

#include <algorithm>
#include "PatternSpotter.hpp"
#include "PrefixSpotter.hpp"
#include "WindowSpotter.hpp"
#include "fileUtils.hpp"			//for convenience function to motif search files

#define min(x,y) ((x)<(y)?(x):(y)) //leave as macro to handle different types;

static const std::string kDEFAULT_CLASS_LBL = "1";

void printMotif(TwoMotif motif) {
	std::cout << "Motif: " << std::endl;
	std::cout << motif.first << std::endl;
	std::cout << motif.second << std::endl;
}

inline Template initQuerySeed() {
	Template querySeed;
	querySeed.nDims = 1;
	querySeed.isValid = true;
	return querySeed;
}

inline TwoMotif initBestMotif() {
	TwoMotif bestMotif;
	bestMotif.first.label = kDEFAULT_CLASS_LBL;
	bestMotif.first.latency = 0;
	bestMotif.second.latency = 0;
	bestMotif.second.label = kDEFAULT_CLASS_LBL;
	return bestMotif;
}

inline bool isPrefixSpotter(SpotterType sType) {
	return (sType == DDW_OPTIMAL		||
			sType == DDW_AGGRESSIVE		||
			sType == EDW_OPTIMAL		||
			sType == EDW_AGGRESSIVE		||
			sType == SPRING_OPTIMAL		||
			sType == SPRING_AGGRESSIVE);
}

// same as windowSpotterFindMotif(), but no innermost loop
TwoMotif findMotifOneQueryLen(SpotterParams p, data_t *data, length_t dataLen,
							  length_t queryLen, length_t minLen, length_t maxLen,
							  data_t initialBest) {
	printf("Searching for motif using query len %d\n", queryLen);
	Template querySeed = initQuerySeed();
	TwoMotif bestMotif = initBestMotif();
	
	data_t bestDist = initialBest;
	data_t dist;
	idx_t finalStartTime = dataLen - minLen + 1;
	for (idx_t startTime = 0; startTime < finalStartTime; startTime++) {
		
		// construct a PatternSpotter to compute distances to the
		// subsequence at this index and of this length
		length_t endTime = min(startTime + queryLen -1, dataLen);
		querySeed.data.assign(&data[startTime], &data[endTime+1]);
		querySeed.nSamples = querySeed.data.size();
		PatternSpotter* spotter = createPatternSpotter(querySeed, p);
		spotter->setQueryLenRange(minLen, maxLen);	//only works for PrefixSpotters--still uses one length
		
		idx_t firstIdxToSearch = endTime + 1;
		for (idx_t i = firstIdxToSearch; i < dataLen; i++) {	//dont check overlaping subseqs
			spotter->updateMatrix(i - firstIdxToSearch, &data[i], bestDist);	// times have to start from 0
			dist = spotter->getBestCurrentDist();

			//check if we've found a new best motif
			if (dist < bestDist && dist != kINVALID ) {
				bestDist = dist;
				bestMotif.first.ts = startTime;
				bestMotif.first.te = startTime + spotter->getBestCurrentPrefix();
//				bestMotif.first.te = endTime;
				bestMotif.first.dist = dist;
				
				bestMotif.second.ts = spotter->getBestCurrentStartTime() + firstIdxToSearch;
				bestMotif.second.te = spotter->getBestCurrentEndTime() + firstIdxToSearch;
				bestMotif.second.te = i;
				bestMotif.second.dist = dist;

				printf("found new best motif at t=%d\n", i);
			}
		}
		delete spotter;
	}
	
	return bestMotif;
}

TwoMotif searchMotifLensUntilFound(SpotterParams p, data_t *data, length_t dataLen,
							  length_t minLen, length_t maxLen,
							  const TwoMotif rightAnswers, data_t initialBest) {
	TwoMotif bestMotif = initBestMotif();
	double bestSoFarDist = initialBest;
	double dist;

	TwoMotif motif;
	bool firstMatches, secondMatches;
	
	// for prefixSpotters, only need to search one len
	length_t firstLen = minLen;
	length_t lastLen = maxLen;
	if (isPrefixSpotter(p.spotType)) {
		firstLen = lastLen = maxLen;
	}
	
	for (length_t len = firstLen; len <= lastLen; len++) {
		motif = findMotifOneQueryLen(p, data, dataLen, len, minLen, maxLen, bestSoFarDist);
		
		// check if this is a new best motif
		dist = motif.first.dist;
		if (dist < bestSoFarDist &&
			motif.first.isValid() &&
			motif.second.isValid() )
		{
			bestMotif = motif;
			bestSoFarDist = dist;
			
			// if correct answers were supplied, stop when we get them
			firstMatches = bestMotif.first.matches(rightAnswers.first, p.timeMatchThresh);
			secondMatches = bestMotif.second.matches(rightAnswers.second, p.timeMatchThresh);
			if (firstMatches && secondMatches) {
				return bestMotif;
			}
		}
	}
	return bestMotif;
}

TwoMotif findMotifAllQueryLens(SpotterParams p, data_t *data, length_t dataLen,
							   length_t minLen, length_t maxLen, data_t initialBest) {
	TwoMotif bestMotif = initBestMotif();
	TwoMotif motif;
	double bestSoFarDist = initialBest;
	double dist;
	
	for (length_t len = minLen; len <= maxLen; len++) {
		motif = findMotifOneQueryLen(p, data, dataLen, len, minLen, maxLen, bestSoFarDist);
		
		// check if this is a new best motif
		dist = motif.first.dist;
		if (dist < bestSoFarDist &&
			motif.first.isValid() &&
			motif.second.isValid() )
		{
			bestMotif = motif;
			bestSoFarDist = dist;
		}
	}
	
	return bestMotif;
}

TwoMotif findMotif(SpotterParams p, data_t *data, length_t dataLen,
				   length_t minLen, length_t maxLen, data_t initialBest) {
	if (isPrefixSpotter(p.spotType)) {
		int querySeedLen = maxLen;
		return findMotifOneQueryLen(p, data, dataLen, querySeedLen, minLen, maxLen, initialBest);
	} else {
		return findMotifAllQueryLens(p, data, dataLen, minLen, maxLen, initialBest);
	}
}

TwoMotif findMotifInStream(SpotterParams p, Stream dataStream,
						   length_t minMotifLen, length_t maxMotifLen) {
	return findMotif(p, &dataStream.data[0], dataStream.data.size(),
					 minMotifLen, maxMotifLen);
}

TwoMotif findMotifInStream(SpotterParams p, std::string streamDir,
						   length_t minMotifLen, length_t maxMotifLen) {
	int numDims = 1;
	Stream dataStream = readStreamFromDir(streamDir, numDims);
	return findMotif(p, &dataStream.data[0], dataStream.data.size(),
					 minMotifLen, maxMotifLen);
}
