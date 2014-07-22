//
//  StreamMonitor.cpp
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include "StreamMonitor.hpp"

#include <iostream>
#include <unordered_map>
#include "SignalMonitor.hpp"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "global_test_vals.h"
#include "array_utils.h"

const StreamMonitorMode kDEFAULT_STREAM_MONITOR_MODE = SMMODE_UNIT_TEST;

static inline data_t harmonic_mean(data_t a, data_t b) {
	return a*b / (a+b);
}

StreamMonitor::StreamMonitor(SpotterParams p):
	mSignalMonitors(),
	mTick(-1),
	mNumDims(p.nDims),
	mTotalRefsLen(0),
	mSpotterParams(p),
	mMode(kDEFAULT_STREAM_MONITOR_MODE)
{
	SignalMonitor * mon;
	for (int i = 0; i < mNumDims; i++) {
		mon = new SignalMonitor(p);
		mon->setMode(mMode); //TODO mode as part of spotterParams?
		mSignalMonitors.push_back(mon);
	}
}

StreamMonitor::~StreamMonitor() {
	// free each SignalMonitor to which we have a pointer
	for (int i = 0; i < mSignalMonitors.size(); i++){
		delete mSignalMonitors[i];
	}
}

void StreamMonitor::addTemplate(Template const& seq) {
	unsigned int len = seq.nSamples;
	data_t** signalRefs = array_reshape(&seq.data[0], len, mNumDims);
	data_t* signalRef;
	for (int i = 0; i < mNumDims; i++) {
		signalRef = signalRefs[i];

		Template ref;
		ref.label = seq.label;
		ref.data = std::vector<data_t>(signalRef,signalRef+len);
		ref.nSamples = len;
		ref.nDims = 1;

		mSignalMonitors[i]->addTemplate(ref);
	}
	mTotalRefsLen += len;
}

void StreamMonitor::addTemplates(std::vector<Template> const& seqs) {
	for (int i=0; i < seqs.size(); i++) {
		addTemplate(seqs[i]);
	}
}

void StreamMonitor::updateSignalMonitors(tick_t t, data_t* sample) {
	for (int i = 0; i < mSignalMonitors.size(); i++){
		(mSignalMonitors[i])->processSample(sample+i);
	}
}

bool StreamMonitor::checkSequenceOptimal(Subsequence seq) {

	//TODO this current implementation is sort of nonsensical
	//for multidimensional streams; if stuff isn't reported
	//immediately, needs to determine whether a lower distance
	//is possible for *all* signals at once, not individual ones;
	//needs to just get the first distance below thresh (and
	//associated start time) from each signal (or something like
	//that) and decide in this method
	//
	//To clarify: one stream may be able to find a better subsequence
	//that conflicts with its current best, but this doesn't mean
	//anything unless the other signals' distances are guaranteed
	//to be nonincreasing, which they aren't. We therefore need
	//to evaluate them all simultaneously.

	bool optimal = true;
	for (int i = 0; i < mSignalMonitors.size(); i++) {
		if (optimal) {
			optimal = (mSignalMonitors[i])->checkSequenceOptimal(seq);
		} else {
			break;
		}
	}
	return optimal;
}

//void StreamMonitor::notifySequenceReported(Subsequence seq) {
//	std::cout << "==> RETURNING SEQ: " << seq << std::endl;
//	for (int i = 0; i < mSignalMonitors.size(); i++) {
//		(mSignalMonitors[i])->onSequenceReported(seq);
//	}
//}

std::vector<Subsequence> StreamMonitor::getCurrentBestSubsequencesForSignals() {
	std::vector<Subsequence> seqs;
	for (int i = 0; i < mNumDims; i++) {
		seqs.push_back( (mSignalMonitors[i])->getCurrentBestSeq() );
	}
	return seqs;
}

Subsequence StreamMonitor::computeEvent(std::vector<Subsequence> seqs) {

	size_t numSeqs = seqs.size();
//	printf("computeEvent: got %ld seqs\n", numSeqs);	//TODO remove
	std::unordered_map<std::string, data_t> classDists(numSeqs);

	// create combined estimates for the likelihood of each class based
	// upon the classifications returned for each signal. This "likelihood"
	// is reflected by a distance value that decreases when more signals
	// report the same class
	data_t dist;
	std::string bestClass;
	Subsequence seq, retVal;
	retVal.dist = kINVALID;
//	printf("computeEvent: examing seqs:\n");	//TODO remove
	for (int i = 0; i < numSeqs; i++) {
		seq = seqs[i];

//		std::cout << seq;

		if (seq.isValid()) {
//			printf(", which is valid");

			// another signal already thinks it's found an instance of
			// this class, so create a combined distance value
			if (classDists.count(seq.label)) {
				dist = harmonic_mean(classDists[seq.label],seq.dist);
			// this is the first signal to think it's found an instance
			// of this class
			} else {
				dist = seq.dist;
			}
			classDists[seq.label] = dist;

			// check if this distance beats the current best; if so,
			// set the most likely event to this subsequence (but with
			// the combined distance calculated above)
			//
			// TODO better scheme for picking start and end times
			if (dist < retVal.dist || retVal.dist == kINVALID) {
				retVal = seq;
				retVal.dist = dist;
			}
		}
//		else {
//			printf(", which is invalid");
//		}

//		std::cout << std::endl;
	}

	// return the most likely event
//	std::cout << "computeEvent: event = " << retVal << std::endl;
	return retVal;
}

bool StreamMonitor::decideIfEvent(Subsequence seq) {

	// if just plain an invalid subsequence (infinite dist, meaning
	// nothing below cutoff found), clearly not an event
	if (! seq.isValid()) return false;

	// not an event if any SignalMonitor doesn't think it's optimal
	// (and cares about optimality)
	if (! checkSequenceOptimal(seq)) return false;

	// custom logic to decide whether it's an event
	return true;	//just assume it is for now iff it's optimal
}

void StreamMonitor::reportEvent(Subsequence seq) {
	for (int i = 0; i < mNumDims; i++){
		(mSignalMonitors[i])->onSequenceReported(seq);
	}
}

Subsequence StreamMonitor::processSample(data_t* sample) {
	mTick++;

	updateSignalMonitors(mTick, sample);
	std::vector<Subsequence> seqs = getCurrentBestSubsequencesForSignals();

	Subsequence retVal;
	Subsequence seq = computeEvent(seqs);
	if (decideIfEvent(seq)) {
		reportEvent(seq);
		seq.latency = mTick - seq.te;
		retVal = seq;
//		std::cout << "==> REPORTING SEQ: " << seq << std::endl;
	}

//	std::cout << "returning seq: " << retVal << std::endl;
	return retVal;
}

dist_calls_t StreamMonitor::getDistCallCounts() const {
	return global_dist_calls;
}

std::vector<Subsequence> StreamMonitor::feedStream(data_t *data, tick_t len) {
	std::vector<Subsequence> output = std::vector<Subsequence>();

	// pass the StreamMonitor each sample in the data stream and record
	// the matches it thinks it finds
	for (tick_t t=0; t < len; t++) {
		Subsequence seq = processSample(&data[t*mNumDims]);
		if (seq.isValid()) {
//			printf("...found valid seq!\n");
			output.push_back(seq);
		}

		//abandon early if this is matching horribly and taking forever
		if ((t & 4095) == 0 && t > 4096) {
			dist_calls_t possible_dist_calls = mTotalRefsLen * t;
			double pruningPower = double(global_dist_calls) / possible_dist_calls;
			if (pruningPower > gAbandonAbovePruningPower) {
				output.clear();
				break;
			}
		}
	}

	return output;
}

void StreamMonitor::setCutoff(data_t newCutoff) {
	mSpotterParams.cutoff = newCutoff;
	// set the warping constraints for each SignalMonitor
	for (int i = 0; i < mSignalMonitors.size(); i++){
		(mSignalMonitors[i])->setMaxWarp(newCutoff);
	}
}

void StreamMonitor::setMaxWarp(steps_t maxWarp) {
	mSpotterParams.maxWarp = maxWarp;
	// set the warping constraints for each SignalMonitor
	for (int i = 0; i < mSignalMonitors.size(); i++){
		(mSignalMonitors[i])->setMaxWarp(maxWarp);
	}
}

void StreamMonitor::setMode(StreamMonitorMode mode) {
	// set the warping constraints for each SignalMonitor
	for (int i = 0; i < mNumDims; i++){
//		printf("setMode: accessing sigmonitor with addr %p\n", mSignalMonitors[i]);
		(mSignalMonitors[i])->setMode(mode);
	}
}
