//
//  WindowSpotter.h
//  edtw
//
//  Created By <Anonymous> on 2/1/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__WindowSpotter__
#define __edtw__WindowSpotter__

#include <iostream>
#include "PatternSpotter.hpp"
#include "Subsequence.hpp"
#include "circBuffer.h"
#include "type_defs.h"
#include "basic_structs.hpp"

static const unsigned int kDEFAULT_LOG_BUFFLEN = 6;
static const ReportCriteria kDEFAULT_REPORT_CRITERIA = AGGRESSIVE;
static const tick_t kRESET_SUMS_AFTER = 100*1000;


/** @class Abstract superclass encapsulating shared functionality for
 * Euclidean distance, dynamic time warping distance, uniform scaling
 * distance, and scaled warped matching distance PatternSpotters (all
 * of which are based on sliding windows).
 */
class WindowSpotter: public PatternSpotter {
private:
	WindowSpotter& operator=(const WindowSpotter& other); // no copying

	static circBuffer *sBuff;
	static tick_t sBufferUpdatedTime;

	inline void ensureBufferLongEnough(idx_t len);
	inline void resetBufferIfNeeeded(tick_t currentTime);
	inline void updateBuffer(tick_t time, data_t x);

	data_t mCurrentDist;
	tick_t mEpochCounter;
	data_t mSumX;
	data_t mSumX2;
	data_t mMeanX;
	data_t mStdX;

protected:
	idx_t mMaxWindowLen; // will be > M under uniform scaling
	idx_t mMinWindowLen; // will be < M under uniform scaling

	tick_t mLastMatchEndTime;

	// order for index comparisons for euclidean / LB_Keogh distances (we
	// want to compare more extreme indices first to accumulate distance
	// as fast as possible)
	idx_t *mOrder;

	// normalized and sorted version of the sequence we're searching for
	data_t *mSortedY;

	// callback for receiving new data; always called before computeDistance()
	virtual void onDataReceived(tick_t t, data_t x) {}

	// distance function
	virtual data_t computeDistance(const data_t* buffEnd,
								   tick_t t, tick_t cantStartUntilAfter,
								   data_t meanX, data_t stdX, data_t bsf)=0;

	/** convert distances calculated internally to the normalized distances
	 * used to compare different PatternMonitors' results */
	virtual data_t normalize(data_t dist) const { return dist / M; };

	/** convert the normalized distances used to compare pattern monitors to
	 * a non-normalized distance that we use internally */
	virtual data_t denormalize(data_t dist) const { return dist * M; };
public:

	// -------------------------------
	// Overrides from PatternSpotter
	// -------------------------------

	// lifecycle
	explicit WindowSpotter(Template const& ref, SpotterParams const& p);
	explicit WindowSpotter(const WindowSpotter& other);
	virtual ~WindowSpotter();

	// getters
	virtual data_t getCurrentDist() const {
		return mCurrentDist < INFINITY ? normalize(mCurrentDist) : INFINITY;
	}
	virtual tick_t getCurrentStartTime() const { return mTime - M + 1; }

	// setters
	virtual void setMaxWarp(steps_t maxWarp) { }
	virtual void setUseScalingEnvelopes(bool useEnvelopes) {}

	// other public functions
	virtual void updateMatrix(const tick_t t, const data_t* sample, data_t cutoff);
	virtual bool sequenceLocallyOptimal(Subsequence seq) const;
	void purgeAllOverlap(Subsequence seq);
	void purgeInferiorOverlap(Subsequence seq) {};
};

#endif /* defined(__edtw__WindowSpotter__) */
