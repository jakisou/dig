//
//  PatternSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_PatternSpotter_h
#define edtw_PatternSpotter_h

#include "type_defs.h"
#include "Subsequence.hpp"
#include "basic_structs.hpp"

//This is effectively an abstract class, but we can't put abstract classes
//in STL structures, so we instead have a concrete class whose methods don't
//do anything useful until overridden. A better solution would be to use
//Boost classes that get around this or store pointers in the STL containers,
//but doing this isn't a priority yet.
// NOTE: additional fields MUST NOT be added in subclasses due to slicing
class PatternSpotter {
private:
	void er() const {
		printf("ERROR: PatternSpotter: called abstract virtual function\n");
	};
	PatternSpotter& operator=(const PatternSpotter& other); // no copying

protected:
	
	tick_t mTime; ///< The current time step
	
	// class label
	std::string mLabel;
	

	data_t* refData;	///< The time series itself
	idx_t M;			///< The length of the time series
//	idx_t nDims;		
	
	// sequence attributes: minimum distance, start time, end time
	data_t dMin;		///< The current distance to beat (used as a cutoff)
	tick_t ts;			///< The start time of the subsequence to beat
	tick_t te;			///< The end time of the subsequence to beat
	
	/// how many times this monitor has called the distance function (used)
	/// only for profiling)
	long mDistCalls;
	
	/// whether matches of the same class automatically count as optimal
	bool mSameClassMeansOptimal;
	
	/// enum defining criteria for reporting a match
	ReportCriteria mReportCriteria;
	
	/** Converts distances calculated internally to the normalized distances
	 * used to compare different PatternMonitors' results */
	virtual data_t normalize(data_t dist) const { er(); return kINVALID; };
	
	/** Converts the normalized distances used to compare pattern monitors to
	 * a non-normalized distance that we use internally */
	virtual data_t denormalize(data_t dist) const { er(); return kINVALID; };
	
public:
	explicit PatternSpotter(Template const& ref, SpotterParams const& p);
	explicit PatternSpotter(const PatternSpotter& other);
//	friend void swap(PatternSpotter& first, PatternSpotter& second);
	
	// -------------------------------
	// Virtual functions to be overridden
	// -------------------------------
	virtual ~PatternSpotter(); //virtual destructor because the internet said so
	
	// getters
	virtual unsigned long getDistCalls() const { return mDistCalls;};
	virtual data_t getCurrentDist() const { er(); return kINVALID; };
	virtual tick_t getCurrentStartTime() const { er(); return kINVALID; };
	virtual tick_t getCurrentEndTime() const { return mTime; }
	
	// getters using prefixes
	virtual data_t getBestCurrentDist()		const { return getCurrentDist(); };
	virtual tick_t getBestCurrentStartTime()const { return getCurrentStartTime(); };
	virtual tick_t getBestCurrentEndTime()	const { return getCurrentEndTime(); }
	virtual idx_t  getBestCurrentPrefix() const { return M-1; };
	
	// setters
	virtual void setMaxWarp(steps_t maxWarp) { er(); }
	virtual void setScaleFactor(double r) {}
	virtual void setMinScaling(double r)  {}
	virtual void setMaxScaling(double r)  {}
	virtual void setScalings(double rMin, double rMax) {}
	virtual void setMatchLenRange(length_t minLen, length_t maxLen) {}
	virtual void setQueryLenRange(length_t minLen, length_t maxLen) {}
	virtual void setUseScalingEnvelopes(bool useEnvelopes) {}
	
	// other public functions
	/** Top-level function to be called at each time step that updates the 
	 * current distance (and any other state) when a new sample is received
	 *
	 * @param t The current time
	 * @param sample A pointer to the latest data point in this signal
	 * @param cutoff The distance above which distance calculations should be
	 *		abandoned
	 */
	virtual void updateMatrix(const tick_t t, const data_t* sample, data_t cutoff) {
		er(); };
	/** Returns whether the supplied sequence is guaranteed to be optimal, or 
	 * whether this PatternSpotter might be able to discover a better (conflicting)
	 * one. Note that what's "optimal" depends on this PatternSpotter's reportCriteria
	 * and whether subsequences of the same class are automatically considered
	 * optimal.
	 *
	 * @param seq The subsequence to be evaluated for optimality
	 */
	virtual bool sequenceLocallyOptimal(Subsequence seq) const {
		er(); return false;};
	
	/** Modify this PatternSpotter's state so that it doesn't report any
	 * subsequences that overlap with the sequence provided.
	 *
	 * @param seq The subsequence with which there should be no overlap; note
	 *	that the end time of the subsequence must be less than or equal to the
	 *	current time for this function to work correctly.
	 */
	virtual void purgeAllOverlap(Subsequence seq) {
		er(); };
	
	/** Modify this PatternSpotter's state so that it doesn't report any
	 * subsequences that overlap with the sequence provided and have a higher
	 * distance to this PatternSpotter's time series.
	 *
	 * @param seq The subsequence with which there should be no overlap; note
	 *	that the end time of the subsequence must be less than or equal to the
	 *	current time for this function to work correctly.
	 */
	virtual void purgeInferiorOverlap(Subsequence seq) {
		er(); };
	
	// -------------------------------
	// Non-virtual functions
	// -------------------------------
	
	//getters
	/** Returns the class of the training time series for this PatternSpotter */
	inline std::string getClassLabel() const { return mLabel; }
	
	/** Returns the length of the training time series for this PatternSpotter */
	inline idx_t getLength() const { return M; }
	
	//setters
	/** Set whether matches of the same class can "conflict" for purposes
	 * of determining whether a sequence is locally optimal.
	 *
	 * @param meansOptimal If true, subsequences are optimal if there is
	 *		no better match except in the same class; if false, subsequences are
	 *		optimal if there is no better match in any class
	 */
	inline void setSameClassMeansOptimal(bool meansOptimal) {
		mSameClassMeansOptimal = meansOptimal;
	}
	/** Set the subsequence to beat */
	inline void setBestSubSequence(Subsequence seq) {
		// remove indices that can't yield a better match
		purgeInferiorOverlap(seq);
		
		dMin = denormalize(seq.dist);
		ts = seq.ts;
		te = seq.te;
	}
	
	/** Set the criteria used to determine whether subsequences are optimal 
	 * (and thus should be reported) */
	inline void setReportCriteria(ReportCriteria criteria) {
		mReportCriteria = criteria;
	}
};

PatternSpotter* createPatternSpotter(Template const& seq, SpotterParams const& p);

#endif
