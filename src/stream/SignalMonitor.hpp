//
//  SignalMonitor.hpp
//  edtw
//
//  Created By <Anonymous> on 4/11/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__SignalMonitor__
#define __edtw__SignalMonitor__

#include <vector>
#include <list>
#include "type_defs.h"
#include "Subsequence.hpp"
#include "basic_structs.hpp"
#include "PatternSpotter.hpp"

class SignalMonitor {
private:
	std::list<PatternSpotter*> mSequenceMonitors;	///< Functors to compute distances to each training instance
	tick_t mTick;					///< The current time step
	Subsequence mCurrentBestSeq;	///< The best match found so far and not yet reported
	data_t mCutoff;					///< The cutoff distance for nearest-neighbor distances
	ReportCriteria mReportCriteria;	///< The criteria for when to report a match
	StreamMonitorMode mMode;		///< The mode in which to operate (test vs debug)
	idx_t mTotalRefsLen;			///< The combined length of all training examples
	SpotterParams mSpotterParams;	///< The parameters for how to spot patterns in the stream
	
	/** Update the distances (and any other necessary state) associated with
	 * each traning example.
	 *
	 * @param t The current time
	 * @param sample A pointer to the most recent data point received in this signal
	 */
	void updatePatternSpotters(tick_t t, data_t* sample);
	
	/** Notify each PatternMonitor that a subsequence was reported so that they
	 * can update their state appropriately (likely to avoid computations until
	 * a non-overlapping match is possible)
	 *
	 * @param seq The subsequence that was reported
	 */
	void notifySequenceReported(Subsequence seq);
	
	/** Finds the best subsequence from the set consisting of {those computed
	 * at the current time step by each PatternMonitor, the best from the
	 * previous time step}.
	 *
	 * @param currentBest The best subsequence from the previous time step
	 */
	Subsequence findBestSubsequence(Subsequence currentBest);
	
	/** Sets the best subsequence found so far (and not reported) for this
	 * SignalMonitor.
	 *
	 * @param seq The best subsequence
	 */
	void setBestSubsequence(Subsequence seq);
public:
	SignalMonitor(SpotterParams p);
	~SignalMonitor();
	
	/** Top-level function to be called at each time step
	 * @param sample A pointer to the latest data point in this signal
	 */
	void processSample(data_t* sample);
	
	/** Add a traning example
	 *
	 * @param seq A labeled time series
	 */
	void addTemplate(Template const& seq);
	
	/** Convenience function to call addTemplate() with a number of
	 * sequences
	 *
	 * @param seqs The time series to be added as traning data
	 */
	void addTemplates(std::vector<Template> const& seqs);
	
	/** Determine whether a sequence being considered as a possible event
	 * is the best event that could be found, or whether a better one might
	 * be found in a few time steps (or some heuristic for whether this is
	 * likely / important).
	 *
	 * @param seq The subsequence being considered as a possible event
	 */
	bool checkSequenceOptimal(Subsequence seq);
	
	/** To be called when a subsequence is reported. Notifies each
	 * PatternMonitor that this is the case via notifySequenceReported()
	 * and resets the best sequence found so far to one that doesn't
	 * overlap with that reported */
	void onSequenceReported(Subsequence seq);
	
//	dist_calls_t getDistCallCounts() const;
//	inline tick_t getTime() const { return mTick; }
//	inline data_t getCutoff() { return mCutoff; }
	
	/** Returns the best sequence found so far but not yet reported */
	inline Subsequence getCurrentBestSeq() { return mCurrentBestSeq; }
	
	/** Returns the cutoff distance for nearest-neighbor comparisons */
	inline void setCutoff(data_t newCutoff) { mCutoff = newCutoff; }
	
	/** Sets the SignalMonitor's mode (test vs debug; the former breaks unit tests) */
	void setMode(StreamMonitorMode mode) { mMode = mode; }
	
	/** Sets the maximum amount of time warping to use when computing distances
	 * between time series */
	void setMaxWarp(steps_t maxWarp);
	
//	std::vector<Subsequence> feedStream(data_t*data, tick_t len);
};

#endif
