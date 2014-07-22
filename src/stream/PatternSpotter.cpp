//
//  PatternSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdlib.h>
#include "PatternSpotter.hpp"
#include "array_utils.h"
#include "type_defs.h"
#include "basic_structs.hpp"

#include "EuclideanSpotter.hpp"
#include "DDWSpotter.hpp"
#include "EuclideanSearcher.hpp"
#include "DtwSearcher.hpp"
#include "USEuclideanSearcher.hpp"
#include "SwmSearcher.hpp"
#include "SpringSpotter.hpp"

PatternSpotter::PatternSpotter(Template const& ref, SpotterParams const& p):
	mTime(0),
	mLabel(ref.label),
	M(ref.nSamples),
	dMin(INFINITY),
	ts(kINVALID),
	te(kINVALID),
	mDistCalls(0),
	mSameClassMeansOptimal(false),
	mReportCriteria(p.reportCriteria)
{
	refData = new data_t[M];
	array_copy(&ref.data[0], refData, M);
}

PatternSpotter::PatternSpotter(const PatternSpotter& other):
	mTime(other.mTime),
	mLabel(other.mLabel),
	M(other.M),
	dMin(other.dMin),
	ts(other.ts),
	te(other.te),
	mDistCalls(other.mDistCalls),
	mSameClassMeansOptimal(other.mSameClassMeansOptimal),
	mReportCriteria(other.mReportCriteria)
{
	refData = new data_t[M];
	array_copy(other.refData, refData, M);
}

//PatternSpotter& PatternSpotter::operator=(const PatternSpotter& other) {
//	if (this == &other) return *this;
//
//	if (M != other.M) {
//		refData = (data_t *)realloc(refData, other.M*sizeof(data_t));
//	}
//	array_copy(other.refData, refData, other.M);
//
//	mLabel = other.mLabel;
//	M = other.M;
//	nDims = other.nDims;
//	dMin = other.dMin;
//	ts = other.ts;
//	te = other.te;
//	mDistCalls = other.mDistCalls;
//	return *this;
//}

//void swap(PatternSpotter& first, PatternSpotter& second) {
//	using std::swap;
//
//	swap(
//}

PatternSpotter::~PatternSpotter() {
	delete [] refData;
}

PatternSpotter* createPatternSpotter(Template const& seq, SpotterParams const& params) {
	PatternSpotter *ps;

	//TODO have all the constructors take in a SpotterParams so we can avoid
	// 1) constructing and *then* giving them the right values
	// 2) duplicating the reportCriteria code in each case

	SpotterParams p = params;

	switch(p.spotType) {
		case EDW_OPTIMAL:
			p.reportCriteria = CAUTIOUS;	//TODO should be OPTIMAL
			ps = new EuclideanSpotter(seq, p);
			break;
		case EDW_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new EuclideanSpotter(seq, p);
			break;
		case DDW_OPTIMAL:
			p.reportCriteria = CAUTIOUS;	//TODO should be OPTIMAL
			ps = new DDWSpotter(seq, p);
			break;
		case DDW_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new DDWSpotter(seq, p);
			break;
		case ED_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new EuclideanSearcher(seq, p);
			break;
		case ED_CAUTIOUS:
			p.reportCriteria = CAUTIOUS;
			ps = new EuclideanSearcher(seq, p);
			break;
		case DTW_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new DtwSearcher(seq, p);
			break;
		case DTW_CAUTIOUS:
			p.reportCriteria = CAUTIOUS;
			ps = new DtwSearcher(seq, p);
			break;
		case US_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new USEuclideanSearcher(seq, p);
			break;
		case US_CAUTIOUS:
			p.reportCriteria = CAUTIOUS;
			ps = new USEuclideanSearcher(seq, p);
			break;
		case SWM_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new SwmSearcher(seq, p);
			break;
		case SWM_CAUTIOUS:
			p.reportCriteria = CAUTIOUS;
			ps = new SwmSearcher(seq, p);
			break;
		case SPRING_OPTIMAL:
			p.reportCriteria = OPTIMAL;
			ps = new SpringSpotter(seq, p);
			break;
		case SPRING_AGGRESSIVE:
			p.reportCriteria = AGGRESSIVE;
			ps = new SpringSpotter(seq, p);
			break;
		default:
			printf("ERROR: SignalMonitor: invalid SpotType; not adding Template\n");
			return nullptr;
	}

	return ps;
}