//
//  evalutate.hpp
//  edtw
//
//  Created By <Anonymous> on 2/11/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__evaluate__
#define __edtw__evaluate__

#include <string>
#include "type_defs.h"
#include "basic_structs.hpp"
#include "TestResult.hpp"
#include "similaritySearch.hpp"

static const data_t kDEFAULT_MIN_CUTOFF = .05;
static const data_t kDEFAULT_MAX_CUTOFF = .75;
static const data_t kDEFAULT_CUTOFF_INC = .05;
static const steps_t kDEFAULT_MIN_WARP = 0;
static const steps_t kDEFAULT_MAX_WARP = 0;
static const steps_t kDEFAULT_WARP_INC = 5;
static const float  kDEFAULT_MIN_SCALE = 0;
static const float  kDEFAULT_MAX_SCALE = 0;
static const float  kDEFAULT_SCALE_INC = .05;

//static const steps_t kDEFAULT_MIN_WARP_DDW = 1;
//static const steps_t kDEFAULT_MAX_WARP_DDW = 1;
//static const steps_t kDEFAULT_MIN_WARP_DTW = 5;
//static const steps_t kDEFAULT_MAX_WARP_DTW = 10;
//static const float  kDEFAULT_MIN_SCALE_US = .1;
//static const float  kDEFAULT_MAX_SCALE_US = .3;

/** A struct encapsulating all parameters regarding how to sweep
 * variables for a given test
 */
typedef struct SweepParams {
	std::string dirName;	///< The directory in which the test files reside
	SpotterType spotType;	///< The SpotterType to use
	data_t minCutoff;		///< The minimum cutoff distance in the sweep
	data_t maxCutoff;		///< The maximum cutoff distance in the sweep
	data_t cutoffIncrement;	///< The increment to use when sweeping the cutoff distance
	steps_t minWarp;		///< The minimum amount of time warping in the sweep
	steps_t maxWarp;		///< The maximum amount of time warping in the sweep
	steps_t warpIncrement;	///< The increment to use when sweeping the amount of warping
	float minScaleFactor;	///< The minimum amount of uniform scaling in the sweep
	float maxScaleFactor;	///< The maximum amount of uniform scaling in the sweep
	float scaleFactorIncrement; ///< The increment to use when sweeping the amount of uniform scaling
	
	//constructor so it'll use default values
	SweepParams(std::string dir, SpotterType spotterType);
	
} SweepParams;

/** Calls runTest() with each combination of variables defined by the
 * sweep parameters provided. Output is logged to a file defined in testUtils.cpp.
 *
 * @param s The SweepParams object defining what values should be swept through
 * for each variable. The cartesian product of all values of each variable is
 * tested.
 *
 * @return The best TestResult for any combination of parameters, as defined by
 * F1 score. Note that the parameters used are included as a field in the 
 * TestResult.
 */
TestResult runSweep(SweepParams s);

/** Convenience function that calls runSweep(SweepParams).
 *
 * @see runSweep()
 * @see SweepParams
 */
TestResult runSweep(std::string testDir, SpotterType spotType,
			  data_t minCutoff = kDEFAULT_MIN_CUTOFF,
			  data_t maxCutoff = kDEFAULT_MAX_CUTOFF,
			  steps_t minWarp = kDEFAULT_MIN_WARP,
			  steps_t maxWarp = kDEFAULT_MAX_WARP,
			  float minScaleFactor = kDEFAULT_MIN_SCALE,
			  float maxScaleFactor = kDEFAULT_MAX_SCALE,
			  float scaleFactorIncrement = kDEFAULT_SCALE_INC);

/** Calls runSweep() in each subdirectory of testDir
 *
 * @see SweepParams
 */
std::vector<TestResult> runSweepsInSubDirs(std::string testDir, SpotterType spotType,
						data_t minCutoff = kDEFAULT_MIN_CUTOFF,
						data_t maxCutoff = kDEFAULT_MAX_CUTOFF,
						steps_t minWarp = kDEFAULT_MIN_WARP,
						steps_t maxWarp = kDEFAULT_MAX_WARP,
						float minScaleFactor = kDEFAULT_MIN_SCALE,
						float maxScaleFactor = kDEFAULT_MAX_SCALE,
						float scaleFactorIncrement = kDEFAULT_SCALE_INC);

/** Calls runSweep() in each subdirectory of testDir
 *
 * \see SweepParams
 */
std::vector<TestResult> runSweepsInSubDirs(std::string parentDir, SweepParams s);

//================================
// UCR Streams
//================================
void learnUCRStreamsParametersDDW();
void learnUCRStreamsParametersED();
void learnUCRStreamsParametersDTW();
void learnUCRStreamsParametersUS();
void learnUCRStreamsParametersSWM();
void learnUCRStreamsParameters();

//unused
void learnUCRStreamsParametersDDW_Opt();

void evaluateUCRForSpotterType(SpotterType sType);
void evaluateUCRUsingTrainedParams();

//================================
// Masked Sines
//================================
void learnWarpedSinesParamsDDW();
void learnWarpedSinesParamsED();
void learnWarpedSinesParamsDTW();
void learnWarpedSinesParamsUS();
void learnWarpedSinesParamsSWM();
void learnWarpedSinesParams();

//unused
void learnWarpedSinesParamsDDW_Opt();

void evaluateWarpedSinesForSpotterType(SpotterType sType);
void evaluateWarpedSinesUsingTrainedParams();

//================================
// Segmented Cylinder-Bell-Funnel (additional experiment, not in paper)
//================================
void evaluateCBFForSpotterType(SpotterType sType);

void learnCBFParametersDDW();
void learnCBFParametersED();
void learnCBFParametersDTW();
void learnCBFParametersUS();
void learnCBFParametersSWM();

//================================
// Motifs
//================================
void findMotifInStreamAndStoreResult(SpotterType sType, std::string streamDir, int minMotifLen, int maxMotifLen);
void searchForMotifUntilFound(SpotterType sType, std::string streamDir, int minMotifLen, int maxMotifLen);

void evaluateMotifDemo(SpotterType sType);
void evaluateECG(SpotterType sType);
void evaluateRandomWalk(SpotterType sType);

#endif /* defined(__edtw__evalutate__) */
