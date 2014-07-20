//
//  type_defs_cpp.h
//  edtw
//
//  Created By <Anonymous> on 1/19/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_type_defs_cpp_h
#define edtw_type_defs_cpp_h

#include <string>
#include <vector>
#include <cmath>
#include "type_defs.h"
#include "Subsequence.hpp"

/** Enumeration defining possible functions used to search for matches
 * for a given query in a data stream */
typedef enum SpotterType {
	DDW_OPTIMAL,
	DDW_AGGRESSIVE,
	EDW_OPTIMAL,
	EDW_AGGRESSIVE,
	ED_AGGRESSIVE,
	ED_CAUTIOUS,
	DTW_AGGRESSIVE,
	DTW_CAUTIOUS,
	US_AGGRESSIVE,
	US_CAUTIOUS,
	SWM_AGGRESSIVE,
	SWM_CAUTIOUS,
	SPRING_OPTIMAL,
	SPRING_AGGRESSIVE,
} SpotterType;

/** Criteria for when a match should be reported */
typedef enum ReportCriteria {
	AGGRESSIVE,		///< Matches should be reported immediately
	CAUTIOUS,		///< Matches should be reported when an overlapping match is impossible
	OPTIMAL,		///< Matches should be reported when a better overlapping match is impossible
} ReportCriteria;

/** Enumeration definining different operating modes for StreamMonitors */
typedef enum StreamMonitorMode {
	SMMODE_UNIT_TEST=0,
	SMMODE_FULL_TEST=1,
} StreamMonitorMode;

// defaults for spotter params (not all the way at the top because
// can't forward declare enums for a lot of compilers)
static const SpotterType	kSPOTPARAMS_DEFAULT_SPOTTER_TYPE		= ED_AGGRESSIVE;
static const ReportCriteria kSPOTPARAMS_DEFAULT_REPORT_CRITERIA		= AGGRESSIVE;
static const data_t			kSPOTPARAMS_DEFAULT_CUTOFF				= INFINITY;
static const data_t			kSPOTPARAMS_DEFAULT_WARP				= 0;
static const float			kSPOTPARAMS_DEFAULT_SCALE_FACTOR		= 0;
static const float			kSPOTPARAMS_DEFAULT_TIME_MATCH_THRESH	= .5;
static const dims_t			kSPOTPARAMS_DEFAULT_DIMENSIONS			= 1;

/** Reference sequence or stream */
typedef struct Sequence {
	bool isValid;			///< Whether the sequence is well-defined and can be used
	length_t nSamples;		///< How many samples are in the sequence
	dims_t nDims;			///< The dimensionality of the sequence
	std::vector<data_t> data;	///< The data in the sequence (of length nSamples * nDims)
} Sequence;

/** Reference sequence we're looking for in the stream */
typedef struct Template: public Sequence {
	std::string label;		///< class label
} Template;

/** Stream of n-dimensional data */
typedef struct Stream: public Sequence {
	std::vector<Subsequence> patterns;	///< The ground truth for what should be found in the stream
	std::string name;					///< A name for the stream
} Stream;

/** Struct to bundle together key parameters for a PatternSpotter */	//TODO make this a class instead
typedef struct SpotterParams {
	SpotterType spotType;			///< The SpotterType to use
	data_t cutoff;					///< The distance cutoff for nearest-neighbor comparisons
	steps_t maxWarp;				///< The maximum amount of time warping
	float scaleFactor;				///< The maximum uniform scaling scale factor
	float timeMatchThresh;			///< How much tolerance there is in subsequences overlapping to still "match"
	dims_t nDims;					///< The number of dimensions in the stream
	ReportCriteria reportCriteria;	///< The criteria used to determine whether to report potential matches
	
	SpotterParams(SpotterType spotType			=kSPOTPARAMS_DEFAULT_SPOTTER_TYPE,
				  ReportCriteria reportCriteria	=kSPOTPARAMS_DEFAULT_REPORT_CRITERIA,
				  data_t cutoff					=kSPOTPARAMS_DEFAULT_CUTOFF,
				  steps_t maxWarp					=kSPOTPARAMS_DEFAULT_WARP,
				  float scaleFactor				=kSPOTPARAMS_DEFAULT_SCALE_FACTOR,
				  float timeMatchThresh			=kSPOTPARAMS_DEFAULT_TIME_MATCH_THRESH,
				  dims_t nDims					=kSPOTPARAMS_DEFAULT_DIMENSIONS);
} SpotterParams;

std::string SpotterType_toString(SpotterType t);

//SpotterParams SpotterParams_construct(	//TODO just make this an actual constructor...
//			SpotterType spotType			=kSPOTPARAMS_DEFAULT_SPOTTER_TYPE,
//			ReportCriteria reportCriteria	=kSPOTPARAMS_DEFAULT_REPORT_CRITERIA,
//			data_t cutoff					=kSPOTPARAMS_DEFAULT_CUTOFF,
//			steps_t maxWarp					=kSPOTPARAMS_DEFAULT_WARP,
//			float scaleFactor				=kSPOTPARAMS_DEFAULT_SCALE_FACTOR,
//			float timeMatchThresh			=kSPOTPARAMS_DEFAULT_TIME_MATCH_THRESH,
//			dims_t nDims					=kSPOTPARAMS_DEFAULT_DIMENSIONS);

std::string SpotterParams_toString(SpotterParams p);
SpotterParams SpotterParams_fromString(std::string str);
std::string SpotterParams_toFileName(SpotterParams p);
std::vector<SpotterParams> readSpotterParams(std::string fileName);
void writeSpotterParams(std::string filename, std::vector<SpotterParams> params);

// convenience functions
bool isSequenceValid(Sequence s);
Sequence Sequence_construct(data_t *data, length_t nSamples, dims_t nDims);
Template Template_construct(data_t *data, length_t nSamples, dims_t nDims,
								  std::string label);
Stream Stream_construct(data_t *data, length_t nSamples, dims_t nDims,
						std::string name, std::vector<Subsequence> answers);

#endif
