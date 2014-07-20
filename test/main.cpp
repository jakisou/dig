//
//  main.cpp
//

//#define MATCH_IGNORE_TIMING	//ignore start/end times for matching
//#define DEBUG_OUTPUT

#include <iostream>
#include <time.h>
#include <string>
#include <vector>
#include "global_test_vals.h"
#include "testUtils.hpp"
#include "test_ucr_funcs.h"
#include "test_circBuffer.h"
#include "test_revCircBuffer.h"
#include "test_array_utils.h"
#include "test_streamStats.h"
#include "test_EuclideanSpotter.hpp"
#include "test_EdtwSpotter.hpp"
#include "test_EuclideanSearcher.hpp"
#include "test_USEuclideanSearcher.hpp"
#include "test_SwmSearcher.hpp"
#include "test_circEnvelope.h"
#include "test_DtwSearcher.hpp"
#include "evaluate.hpp"

#include "PatternSpotter.hpp"

//#include "ucr_funcs.h"
#include <stdint.h>
#include "array_utils.h"
#include "fileUtils.hpp"


void printTime(long clocks, const char* name) {
	printf("%s: %ld\n", name, clocks * 1000 / CLOCKS_PER_SEC);
}

int main(int argc, const char * argv[]) {
	
	// Unit tests - if any of these fail, we broke something
	test_array_utils_all();
	test_circBuffer_all();
	test_revCircBuffer_all();
	test_ucr_funcs_all();
	test_streamStats_all();
	test_EucSpotter_all();
	test_EdtwSpotter_all();
	test_EucSearcher_all();
	test_circEnvelope_all();
	test_USEucSearcher_all();
	test_DtwSearcher_all();
	test_SwmSearcher_all();
	
	// run range queries on the UCR archive data
//	learnUCRStreamsParametersDDW();
//	evaluateUCRForSpotterType(DDW_AGGRESSIVE);
//	learnUCRStreamsParametersED();
//	evaluateUCRForSpotterType(ED_AGGRESSIVE);
//	learnUCRStreamsParametersDTW();
//	evaluateUCRForSpotterType(DTW_AGGRESSIVE);
//	learnUCRStreamsParametersUS();
//	evaluateUCRForSpotterType(US_AGGRESSIVE);
//	learnUCRStreamsParametersSWM();
//	evaluateUCRForSpotterType(SWM_AGGRESSIVE);
	
	// run range queries on the Warped Sines data
//	learnWarpedSinesParamsDDW();
//	evaluateWarpedSinesForSpotterType(DDW_AGGRESSIVE);
//	learnWarpedSinesParamsED();
//	evaluateWarpedSinesForSpotterType(ED_AGGRESSIVE);
//	learnWarpedSinesParamsDTW();
//	evaluateWarpedSinesForSpotterType(DTW_AGGRESSIVE);
//	learnWarpedSinesParamsUS();
//	evaluateWarpedSinesForSpotterType(US_AGGRESSIVE);
//	learnWarpedSinesParamsSWM();
//	evaluateWarpedSinesForSpotterType(SWM_AGGRESSIVE);
	
	// search for motifs in the motifDemo data (not in the paper)
//	evaluateMotifDemo(DDW_AGGRESSIVE);
//	evaluateMotifDemo(ED_AGGRESSIVE);
//	evaluateMotifDemo(DTW_AGGRESSIVE);
//	evaluateMotifDemo(US_AGGRESSIVE);
//	evaluateMotifDemo(SWM_AGGRESSIVE);
	
	// search for motifs in the ECG data
//	evaluateECG(DDW_AGGRESSIVE);
//	evaluateECG(ED_AGGRESSIVE);
//	evaluateECG(DTW_AGGRESSIVE);
//	evaluateECG(US_AGGRESSIVE);
//	evaluateECG(SWM_AGGRESSIVE);
	
	// search for motifs in the Random Walk data
//	evaluateRandomWalk(DDW_AGGRESSIVE);
//	evaluateRandomWalk(ED_AGGRESSIVE);
//	evaluateRandomWalk(DTW_AGGRESSIVE);
//	evaluateRandomWalk(US_AGGRESSIVE);
//	evaluateRandomWalk(SWM_AGGRESSIVE);
	
	// segmented Cylinder-Bell-Funnel dataset
//	learnCBFParametersDDW();
//	evaluateCBFForSpotterType(DDW_AGGRESSIVE);
//	learnCBFParametersED();
//	evaluateCBFForSpotterType(ED_AGGRESSIVE);
//	learnCBFParametersDTW();
//	evaluateCBFForSpotterType(DTW_AGGRESSIVE);
//	learnCBFParametersUS();
//	evaluateCBFForSpotterType(US_AGGRESSIVE);
//	learnCBFParametersSWM();
//	evaluateCBFForSpotterType(SWM_AGGRESSIVE);
	
	printf("Done.");
	
    return 0;
}
