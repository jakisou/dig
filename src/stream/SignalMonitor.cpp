//
//  SignalMonitor.cpp
//  edtw
//
//  Created By <Anonymous> on 4/11/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include "SignalMonitor.hpp"

#include <iostream>
#include "SignalMonitor.hpp"
#include "PatternSpotter.hpp"
#include "EuclideanSpotter.hpp"
#include "DDWSpotter.hpp"
#include "EuclideanSearcher.hpp"
#include "DtwSearcher.hpp"
#include "USEuclideanSearcher.hpp"
#include "SwmSearcher.hpp"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "global_test_vals.h"
#include "StreamMonitor.hpp" //for kDEFAULT_STREAM_MONITOR_MODE only...

#include "array_utils.h"	//TODO remove after test

static const double kFLOATING_PT_ERR_MARGIN = .0001;

SignalMonitor::SignalMonitor(SpotterParams p):
	mCutoff(p.cutoff),
	mSequenceMonitors(),
	mTick(-1),
	mMode(kDEFAULT_STREAM_MONITOR_MODE),
	mTotalRefsLen(0),
	mSpotterParams(p)
{
	mCurrentBestSeq.dist = INFINITY;
}

void SignalMonitor::addTemplate(Template const& seq) {
	PatternSpotter *pm = createPatternSpotter(seq, mSpotterParams);
	
	if (pm == nullptr) return;
	
	mTotalRefsLen += seq.nSamples;
	
	if (mMode == SMMODE_FULL_TEST) {
		// don't prevent intra-class checks for optimality unless
		// we're running a test on a real data set; this will shatter
		// all of our unit tests if set because they almost all use a
		// single pattern monitor (which will always match its own class)
		pm->setSameClassMeansOptimal(true);
		
		// only use uniform scaling envelopes for real tests; they don't use
		// exact z-normalizations of the data, and so cause uniform scaling
		// based spotters to fail all their unit tests
		pm->setUseScalingEnvelopes(true);
	}
	
	mSequenceMonitors.push_back(pm);
}

void SignalMonitor::addTemplates(std::vector<Template> const& seqs) {
	for (int i=0; i < seqs.size(); i++) {
		addTemplate(seqs[i]);
	}
}

//TODO find the matching PatternSpotter and free it; just removing
//the pointer is a memory leak
//void SignalMonitor::removeTemplate(Template seq) {
//	PatternSpotter pm(seq);
//	mSequenceMonitors.remove(seq);
//}

void SignalMonitor::setMaxWarp(steps_t maxWarp) {
	mSpotterParams.maxWarp = maxWarp;
	// set the warping constraints for each PatternMonitor
	for (std::list<PatternSpotter*>::iterator it = mSequenceMonitors.begin();
		 it != mSequenceMonitors.end(); it++) {
		(**it).setMaxWarp(maxWarp);
	}
}

void SignalMonitor::updatePatternSpotters(tick_t t, data_t* sample) {
	
//	if ((t & 1023) == 0) {
//		printf("--------, t=%ld, x=%.2f, distCalls=%lld\n", t, *sample, global_dist_calls); //TODO remove
//	}
	
	data_t bsf = mCutoff;
	data_t dist;
	// have each PatternSpotter update based on the sample we just received
	for (std::list<PatternSpotter*>::iterator it = mSequenceMonitors.begin();
		 it != mSequenceMonitors.end(); it++) {
		(**it).updateMatrix(mTick, sample, bsf);
		
		// check if there's a new best-so-far distance;
		// empirically, this makes virtually no difference since
		// we report things as soon as they're below the cutoff, so
		// it would *maybe* be faster for one time step
		dist = (**it).getCurrentDist();
//		printf("got dist %.3f\n", dist);	TODO remove
		if (dist < bsf) {
			bsf = dist;
		}
	}
}

bool SignalMonitor::checkSequenceOptimal(Subsequence seq) {
	//	printf("SignalMonitor: checking if subsequence optimal: ");
	//	std::cout << seq << std::endl;
	//	return true;	//uncomment to search
	
	// sequence with distance above cutoff can't be optimal
	if (seq.dist > mCutoff) return false;
	if (! seq.isValid() ) return false;
	
	// a sequence is optimal if there's no pattern monitor that might be
	// able to find a pattern that conflicts with it
	bool optimal = true;
	for (std::list<PatternSpotter*>::iterator it = mSequenceMonitors.begin();
		 it != mSequenceMonitors.end(); it++) {
		if (optimal) {
			optimal = (**it).sequenceLocallyOptimal(seq);
		} else {
			break;
		}
	}
	return optimal;
}

void SignalMonitor::notifySequenceReported(Subsequence seq) {
	for (std::list<PatternSpotter*>::iterator it = mSequenceMonitors.begin();
		 it != mSequenceMonitors.end(); it++) {
		(**it).purgeAllOverlap(seq); //TODO onSequenceReported() instead
	}
}

Subsequence SignalMonitor::findBestSubsequence(Subsequence currentBest) {
	// see if we've found a better bestSeq
	//	printf("SignalMonitor: searching for subsequence better than:");
	//	std::cout << currentBest << std::endl;
	for (std::list<PatternSpotter*>::iterator it = mSequenceMonitors.begin();
		 it != mSequenceMonitors.end(); it++) {
		data_t dist = (**it).getCurrentDist();
		if ( dist < currentBest.dist) {
			currentBest.dist = dist;
			currentBest.label = (**it).getClassLabel();
			currentBest.ts = (**it).getCurrentStartTime();
			currentBest.te = mTick;
//			std::cout << "signalMonitor found better subseq: " << currentBest << std::endl;
		}
	}
	return currentBest;
}

void SignalMonitor::setBestSubsequence(Subsequence newBest) {
	mCurrentBestSeq = newBest;
	
	// tell all of the PatternSpotters what the best sequence is, so
	// they know what they have to beat
	if (newBest.dist <= mCutoff) {
		for (std::list<PatternSpotter*>::iterator it = mSequenceMonitors.begin();
			 it != mSequenceMonitors.end(); it++) {
			(**it).setBestSubSequence(newBest);
		}
	}
}

void SignalMonitor::onSequenceReported(Subsequence seq) {
	// tell the PatternMonitors that a sequence has been reported
	notifySequenceReported(seq);
	
	// reset current best sequence to the best that didn't
	// conflict with the one we just reported
	mCurrentBestSeq.dist = INFINITY;
	mCurrentBestSeq = findBestSubsequence(mCurrentBestSeq);
}

void SignalMonitor::processSample(data_t* sample) {
	//increment time tick
	mTick++;
//	printf("signal monitor time tick just became: %ld\n", mTick);
	
	updatePatternSpotters(mTick, sample);

	// get the best sequence since the last report
	Subsequence newBest = findBestSubsequence(mCurrentBestSeq);
	if (newBest.dist < mCurrentBestSeq.dist - kFLOATING_PT_ERR_MARGIN) {
		setBestSubsequence(newBest);
	}
	
	// if the current best sequence is optimal, have all of our pattern
	// monitors remove any overlap with it from their distance matrices
//	bool optimal = checkSequenceOptimal(mCurrentBestSeq, mCutoff);
//	if (optimal) {
//		retVal = mCurrentBestSeq;
//		retVal.latency = mTick - retVal.te;
//		//		std::cout << retVal << std::endl;	//uncomment for search
//		
//		onSequenceReported(retVal);
//		
////		// tell the PatternMonitors that a sequence has been reported
////		notifySequenceReported(retVal);		//comment out to search, not spot
////		
////		// reset current best sequence to the best that didn't
////		// conflict with the one we just reported
////		mCurrentBestSeq.dist = INFINITY;
////		mCurrentBestSeq = findBestSubsequence(mCurrentBestSeq); //comment for search
//	}
}

SignalMonitor::~SignalMonitor() {
	// free the each PatternSpotter to which we have a pointer
	for (std::list<PatternSpotter*>::const_iterator it = mSequenceMonitors.begin();
		 it != mSequenceMonitors.end(); it++) {
		delete (*it);
	}
}
