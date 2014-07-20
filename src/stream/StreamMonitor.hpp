//
//  StreamMonitor.h
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_StreamMonitor_h
#define edtw_StreamMonitor_h

#include <vector>
#include "type_defs.h"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "SignalMonitor.hpp"

//static const data_t kDEFAULT_CUTOFF = INFINITY;
//static const data_t kDEFAULT_WARP	= 0;
//static const SpotterType kDEFAULT_SPOTTER_TYPE = EUCLIDEAN;
//static const dims_t kDEFAULT_DIMENSIONS = 1;

// by default, assume we're running a unit test, since these are what
// create the vast majority of StreamMonitor instances
extern const StreamMonitorMode kDEFAULT_STREAM_MONITOR_MODE;

class StreamMonitor {
private:
//	std::list<PatternSpotter*> mSequenceMonitors;
	std::vector<SignalMonitor*> mSignalMonitors;	///< Objects to monitor each dimension of the stream
//	std::vector<Subsequence> mCurrentBestSequences;
	tick_t mTick;					///< The current time
	dims_t mNumDims;				///< The number of dimensions in the stream
	ReportCriteria mReportCriteria;	///< An object describing when matches should be reported
	idx_t mTotalRefsLen;			///< The combined length in samples of all templates (training examples)
	SpotterParams mSpotterParams;	///< Parameters regarding how to spot patterns in the stream
	StreamMonitorMode mMode;		///< An enum describing the "mode" (test vs debug) in which to run

	// core functions executed for each sample
	/** Forwards the received sample to each SignalMonitor so that they
	 * compute distances for this time step or carry out other updates.
	 *
	 * @param t The current time step
	 * @param sample A pointer to an mNumDims-length array of data values
	 *		representing the most recently received sample
	 */
	void updateSignalMonitors(tick_t t, data_t* sample);
	
	/** Fetches the best match that each SignalMonitor has at the
	 *  the current time step.
	 */
	std::vector<Subsequence> getCurrentBestSubsequencesForSignals();
	
	/** Given the collection of subsequences that are the best matches
	 * for each individual signal, computes an estimate of what event
	 * may have just taken place.
	 *
	 * @param seqs The subsequences that are the current best matches
	 *		for each individual signal
	 */
	Subsequence computeEvent(std::vector<Subsequence> seqs);
	
	/** Given a best estimate for what event may have taken place at
	 * a given time step, decide whether or not this really should be
	 * considered an event.
	 *
	 * @param seq The Subsequence to consider
	 */
	bool decideIfEvent(Subsequence seq);
	
	/** Output an event that the StreamMonitor believes it saw and notify
	 * each SignalMonitor that this event has been reported so that it can
	 * avoid finding the same event at subsequent time steps.
	 * 
	 * @param seq The Subsequence representing the event to be reported
	 */
	void reportEvent(Subsequence seq);
	
	/** Determine whether a sequence being considered as a possible event 
	 * is the best event that could be found, or whether a better one might
	 * be found in a few time steps (or some heuristic for whether this is 
	 * likely / important).
	 *
	 * @param seq The subsequence being considered as a possible event
	 */
	bool checkSequenceOptimal(Subsequence seq);

public:
	StreamMonitor(SpotterParams p);
	~StreamMonitor();
	
	/** Top-level function to be called at each time step
	 *
	 * @param sample An array of length k corresponding to the latest
	 *		sample in the k-dimensional stream
	 */
	Subsequence processSample(data_t* sample);
	
	/** Add a traning example
	 * 
	 * @param seq A labeled time series (of the appropriate dimensionality)
	 */
	void addTemplate(Template const& seq);
	
	/** Convenience function to call addTemplate() with a number of
	 * sequences
	 *
	 * @param seqs The time series to be added as traning data
	 */
	void addTemplates(std::vector<Template> const& seqs);

	/** Return the number of calls so far to the core distance-computing
	 * subroutine */
	dist_calls_t getDistCallCounts() const;
	
	/** Returns the current time step */
	inline tick_t getTime() const { return mTick; }
	
	/** Returns the cutoff distance for what counts as a match in the
	 * nearest-neighbor searches StreamMonitors use */
	inline data_t getCutoff() { return mSpotterParams.cutoff; };
	
	/** Sets the cutoff distance for nearest-neighbor searches */
	void setCutoff(data_t newCutoff);
	
	/** Sets the StreamMonitor's mode (test vs debug; the former breaks unit tests) */
	void setMode(StreamMonitorMode mode);
	
	/** Sets the maximum amount of time warping to use when computing distances
	 * between time series */
	void setMaxWarp(steps_t maxWarp);
	
	/** Returns the events found in the supplied stream of data 
	 *
	 * @param data The array of data describing the stream
	 * @param len The number of samples in the stream; note that for a stream
	 *		of k dimensions, this means that the array supplied must be of 
	 *		length k*len
	 * @return The collection of non-overlapping matching subsequences ("events"), 
	 *		in chronological order, discovered in the stream.
	 */
	std::vector<Subsequence> feedStream(data_t*data, tick_t len);
};

#endif
