//
//  evalutate.cpp
//  edtw
//
//  Created By <Anonymous> on 2/11/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "evaluate.hpp"
#include "basic_structs.hpp"
#include "testUtils.hpp"
#include "fileUtils.hpp"
#include "TestResult.hpp"
#include "type_defs.h"
#include "global_test_vals.h"
#include "similaritySearch.hpp"
#include "filePaths.hpp"

static const double kFP_ERROR = .0001; // so our for loops actually work

// ================================================================
// Data structure-related functions
// ================================================================

SweepParams::SweepParams(std::string dir, SpotterType spotterType) {
	dirName = dir;
	spotType = spotterType;
	minCutoff = kDEFAULT_MIN_CUTOFF;
	maxCutoff = kDEFAULT_MAX_CUTOFF;
	cutoffIncrement = kDEFAULT_CUTOFF_INC;
	minWarp = kDEFAULT_MIN_WARP;
	maxWarp = kDEFAULT_MAX_WARP;
	warpIncrement = kDEFAULT_WARP_INC;
	minScaleFactor = kDEFAULT_MIN_SCALE;
	maxScaleFactor = kDEFAULT_MAX_SCALE;
	scaleFactorIncrement = kDEFAULT_SCALE_INC;
}

// ================================================================
// File Utilities (specific to experimental evaluation)
// ================================================================

std::string generateParamsFileName(std::string datasetsDir, SpotterType sType) {
	std::string paramsFile = kBEST_PARAMS_DIR;
	ensureTrailingSlash(&datasetsDir);
	ensureDirExists(datasetsDir);
	paramsFile.append(replaceSlashes(datasetsDir));
	paramsFile.append("/");
	ensureDirExists(paramsFile);
	paramsFile.append(SpotterType_toString(sType));
	paramsFile.append(".csv");	//TODO define file ext somewhere
	return paramsFile;
}

std::string generateUCRParamsFileName(SpotterType sType) {
	return generateParamsFileName(kUCR_TRAIN_DIR, sType);
}
std::string generateWarpedSinesParamsFileName(SpotterType sType) {
	return generateParamsFileName(kMASKED_SINES_DIR, sType);
}

// ================================================================
// Parameter Sweeping Functions
// ================================================================

//================================
// Main functions
//================================

TestResult runSweep(SweepParams s) {
	data_t cutoff;
	steps_t warp;
	float scaleFactor;

	// add a tiny value to the floating point parameters
	// so the loops don't terminate prematurely
	s.maxCutoff += kFP_ERROR;
	s.maxScaleFactor += kFP_ERROR;
	
	TestResult bestSoFar;
	bestSoFar.setPrecision(0);
	bestSoFar.setRecall(0);
	
	for (scaleFactor = s.minScaleFactor; scaleFactor <= s.maxScaleFactor;
		 scaleFactor += s.scaleFactorIncrement) {
		for (warp = s.minWarp; warp <= s.maxWarp; warp += s.warpIncrement) {
			for (cutoff = s.minCutoff; cutoff <= s.maxCutoff; cutoff += s.cutoffIncrement) {
				printf("Running Test at Cutoff=%.3f, Warp=%d, ScaleFactor=%.3f\n",
					   cutoff, warp, scaleFactor);
				
				std::vector<TestResult> results =
					runTestsInDir(s.dirName, s.spotType, cutoff, warp, scaleFactor);
				
				if (results.size() > 0) {
					TestResult res = results.back();	//TODO only supports one test per dir at the moment
					
					// check if a new best test result (as determine by accuracy)
					// is produced by these parameters
					if (res.getF1score() > bestSoFar.getF1score()) {
						bestSoFar = res;
					}
					
					// early abandon sweep when just testing *awful* parameters
					if (cutoff > .4 && bestSoFar.getF1score() > 0 && res.pruningPower > gAbandonAbovePruningPower) {
						break;
					}
					if (cutoff > .4 && res.getPrecision() < .2 && bestSoFar.getPrecision() > .2 &&
						bestSoFar.getF1score() > .2) {
						break;
					}
					if (cutoff > .4 && res.getPrecision() < bestSoFar.getPrecision() - .2 &&
						res.getF1score() < bestSoFar.getF1score() - .2) {
						break;
					}
				} else {
					printf("ERROR: runSweep: no test results returned for dir %s.\n", s.dirName.c_str());
					exit(1);
				}
			}
		}
	}
	return bestSoFar;
}

std::vector<TestResult> runSweepsInSubDirs(SweepParams s) {
	std::string parentDir = s.dirName;
	ensureTrailingSlash(&parentDir);
	std::vector<std::string> subdirs = getFileNamesInDir(parentDir);
	
	//run all the tests in each subdirectory
	std::vector<TestResult> allResults = std::vector<TestResult>();
	for (int i = 0; i < subdirs.size(); i++) {
		std::string subdir = parentDir;
		subdir.append(subdirs[i]);
		s.dirName = subdir;
		allResults.push_back(runSweep(s));
	}
	
	// store best parameters
	std::string paramsFile = generateParamsFileName(parentDir, s.spotType);
	std::vector<SpotterParams> bestParams = std::vector<SpotterParams>();
	for (int i = 0; i < allResults.size(); i++) {
		bestParams.push_back(allResults[i].params);
	}
	writeSpotterParams(paramsFile, bestParams);
	
	return allResults;
}

// -------------------------------
// Convenience functions
// -------------------------------

TestResult runSweep(std::string testDir, SpotterType spotType,
			  data_t minCutoff, data_t maxCutoff,
			  steps_t minWarp, steps_t maxWarp,
			  float minScaleFactor, float maxScaleFactor, float scaleFactorIncrement) {
	SweepParams s(testDir, spotType);
	s.minCutoff = minCutoff;
	s.maxCutoff = maxCutoff;
	s.minWarp = minWarp;
	s.maxWarp = maxWarp;
	s.minScaleFactor = minScaleFactor;
	s.maxScaleFactor = maxScaleFactor;
	s.scaleFactorIncrement = scaleFactorIncrement;
	return runSweep(s);
}

std::vector<TestResult> runSweepsInSubDirs(std::string testDir, SpotterType spotType,
						data_t minCutoff, data_t maxCutoff,
						steps_t minWarp, steps_t maxWarp,
						float minScaleFactor, float maxScaleFactor, float scaleFactorIncrement) {
	SweepParams s(testDir, spotType);
	s.minCutoff = minCutoff;
	s.maxCutoff = maxCutoff;
	s.minWarp = minWarp;
	s.maxWarp = maxWarp;
	s.minScaleFactor = minScaleFactor;
	s.maxScaleFactor = maxScaleFactor;
	s.scaleFactorIncrement = scaleFactorIncrement;
	return runSweepsInSubDirs(s);
}

// ================================================================
// Wrappers to run experiments
// ================================================================

//================================
// UCR Streams
//================================

// -------------------------------
// Parameter Learning
// -------------------------------

// ------------------------------- Dynamic Data Warping
void learnParametersDDW(std::string dir, SpotterType sType=DDW_AGGRESSIVE) {
	SweepParams s(dir, sType);
	s.minWarp = 1;
	s.maxWarp = 1;
	s.minCutoff = .05;
	s.maxCutoff = .75;
	runSweepsInSubDirs(s);
}
void learnUCRStreamsParametersDDW() {
	learnParametersDDW(kUCR_TRAIN_DIR);
}
void learnUCRStreamsParametersDDW_Opt() {
	learnParametersDDW(kUCR_TRAIN_DIR, DDW_OPTIMAL);
}

// ------------------------------- Euclidean Distance
void learnParametersED(std::string dir) {
	SweepParams s(dir, ED_AGGRESSIVE);
	s.minWarp = 0;
	s.maxWarp = 0;
	s.minCutoff = .1;
	s.maxCutoff = 1.0;
	runSweepsInSubDirs(s);
}
void learnUCRStreamsParametersED() {
	learnParametersED(kUCR_TRAIN_DIR);
}

// ------------------------------- Dynamic Time Warping
void learnParametersDTW(std::string dir) {
	SweepParams s(dir, DTW_AGGRESSIVE);
	s.minWarp = 5;
	s.maxWarp = 15;
	s.minCutoff = .05;
	s.maxCutoff = .75;
	runSweepsInSubDirs(s);
}
void learnUCRStreamsParametersDTW() {
	learnParametersDTW(kUCR_TRAIN_DIR);
}

// ------------------------------- Uniform Scaling
void learnParametersUS(std::string dir) {
	SweepParams s(dir, US_AGGRESSIVE);
	s.minWarp = 0;
	s.maxWarp = 0;
	s.minCutoff = .05;
	s.maxCutoff = .8;
	s.minScaleFactor = .05;
	s.maxScaleFactor = .25;
	runSweepsInSubDirs(s);
}
void learnUCRStreamsParametersUS() {
	learnParametersUS(kUCR_TRAIN_DIR);
}

// ------------------------------- Scaled Warped Matching
void learnParametersSWM(std::string dir) {
	SweepParams s(dir, SWM_AGGRESSIVE);
	s.minWarp = 5;
	s.maxWarp = 10;
	s.minCutoff = .05;
	s.maxCutoff = .75;
	s.minScaleFactor = .05;
	s.maxScaleFactor = .25;
	runSweepsInSubDirs(s);
}
void learnUCRStreamsParametersSWM() {
	learnParametersUS(kUCR_TRAIN_DIR);
}

// ------------------------------- All
void learnUCRStreamsParameters() {
	// learn the best parameters for each distance measure
	learnUCRStreamsParametersDDW();
	learnUCRStreamsParametersED();
	learnUCRStreamsParametersDTW();
	learnUCRStreamsParametersUS();
	learnUCRStreamsParametersSWM();
}

// -------------------------------
// Evaluation via best parameters
// -------------------------------

void evaluateUCRForSpotterType(SpotterType sType) {
	
	// get the datasets for each trial
	std::vector<std::string> ucrTestDirs = getFilePathsInDir(kUCR_TESTS_DIR);
	long numTrials = ucrTestDirs.size();
	if (numTrials < 1) {
		printf("ERROR: evaluateUCRUsingTrainedParams: no test sets in dir %s\n", kUCR_TESTS_DIR.c_str());
		exit(1);
	}
	
	std::string paramsFile = generateUCRParamsFileName(sType);
	printf("Reading params from file: %s\n", paramsFile.c_str());
	std::vector<SpotterParams> params = readSpotterParams(paramsFile);
	std::vector<std::vector<TestResult>> allTrialResults(numTrials);
	for (int trial = 0; trial < numTrials; trial++) {
		allTrialResults[trial] = runTestsInSubDirs(ucrTestDirs[trial], params);
	}
	
	// average results from all trials
	long numDatasets = allTrialResults[0].size();
	std::vector<TestResult> combinedResults = std::vector<TestResult>();
	for (int dataset = 0; dataset < numDatasets; dataset++) {
		TestResult res = allTrialResults[0][dataset];
		for (int trial = 1; trial < numTrials; trial++) {
			res += allTrialResults[trial][dataset];
		}
		res *= (1.0/numTrials);
		combinedResults.push_back(res);
	}
	
	// store results from all trials
	storeTestResults(kUCR_COMBINED_RESULTS_DIR, sType, combinedResults);
}

void evaluateUCRUsingTrainedParams() {
	// compute results for each algorithm
	evaluateUCRForSpotterType(DDW_AGGRESSIVE);
	evaluateUCRForSpotterType(ED_AGGRESSIVE);
	evaluateUCRForSpotterType(DTW_AGGRESSIVE);
	evaluateUCRForSpotterType(US_AGGRESSIVE);
	evaluateUCRForSpotterType(SWM_AGGRESSIVE);
}

//================================
// Warped Sines
//================================

// -------------------------------
// Parameter Learning
// -------------------------------

void learnWarpedSinesParamsDDW() {
	learnParametersDDW(kMASKED_SINES_DIR);
}
void learnWarpedSinesParamsDDW_Opt() {
	learnParametersDDW(kMASKED_SINES_DIR, DDW_OPTIMAL);
}
void learnWarpedSinesParamsED() {
	learnParametersED(kMASKED_SINES_DIR);
}
void learnWarpedSinesParamsDTW() {
	learnParametersDTW(kMASKED_SINES_DIR);
}
void learnWarpedSinesParamsUS() {
	double origAbandonThreshold = gAbandonAbovePruningPower;
	gAbandonAbovePruningPower = 20.0;
	SweepParams s(kMASKED_SINES_DIR, US_AGGRESSIVE);
	s.minWarp = 0;
	s.maxWarp = 0;
	s.minCutoff = .05;
	s.maxCutoff = .8;
	s.minScaleFactor = .05;
	s.maxScaleFactor = 1.2;	//cant call learnParams since this needs to be huge
	runSweepsInSubDirs(s);
	gAbandonAbovePruningPower = origAbandonThreshold;
}
void learnWarpedSinesParamsSWM() {
	double origAbandonThreshold = gAbandonAbovePruningPower;
	gAbandonAbovePruningPower = 20.0;
	SweepParams s(kMASKED_SINES_DIR, SWM_AGGRESSIVE);
	s.minWarp = 5;
	s.maxWarp = 15;
	s.minCutoff = .05;
	s.maxCutoff = .75;
	s.minScaleFactor = .05;
	s.maxScaleFactor = 1.2; //cant call learnParams since this needs to be huge
	runSweepsInSubDirs(s);
	gAbandonAbovePruningPower = origAbandonThreshold;
}

void learnWarpedSinesParams() {
	// learn best parameters when using each distance measure
	learnWarpedSinesParamsDDW();
	learnWarpedSinesParamsED();
	learnWarpedSinesParamsDTW();
	learnWarpedSinesParamsUS();
	learnWarpedSinesParamsSWM();
}

// -------------------------------
// Evaluation via best parameters
// -------------------------------

void evaluateWarpedSinesForSpotterType(SpotterType sType) {
	
	// update max pruning power before abandoning for US and SWM
	// so that they can actually get results
	bool changeAbandonThreshold = sType == US_AGGRESSIVE ||
		sType == US_CAUTIOUS ||
		sType == SWM_AGGRESSIVE ||
		sType == SWM_CAUTIOUS;
	double origAbandonThreshold;
	if (changeAbandonThreshold) {
		origAbandonThreshold = gAbandonAbovePruningPower;
		gAbandonAbovePruningPower = 20.0;
	}
	
	// read in best parameters
	std::string paramsFile = generateWarpedSinesParamsFileName(sType);
	printf("Reading params from file: %s\n", paramsFile.c_str());
	std::vector<SpotterParams> params = readSpotterParams(paramsFile);
	
	// search datasets using best parameters
	std::vector<TestResult> results = runTestsInSubDirs(kMASKED_SINES_DIR, params);
	
	// store results
	storeTestResults(kMASKED_SINES_RESULTS_DIR, sType, results);
	
	//restore original max pruning power if necessary
	if (changeAbandonThreshold) {
		gAbandonAbovePruningPower = origAbandonThreshold;
	}
}

void evaluateWarpedSinesUsingTrainedParams() {
	evaluateWarpedSinesForSpotterType(DDW_AGGRESSIVE);
	evaluateWarpedSinesForSpotterType(ED_AGGRESSIVE);
	evaluateWarpedSinesForSpotterType(DTW_AGGRESSIVE);
	evaluateWarpedSinesForSpotterType(US_AGGRESSIVE);
	evaluateWarpedSinesForSpotterType(SWM_AGGRESSIVE);
}

//================================
// CBF
//================================

void evaluateDatasetsForSpotterType(SpotterType sType, std::string testDir, std::string resultsDir) {
	
	// read in best parameters
	std::string paramsFile = generateParamsFileName(testDir, sType);
	printf("Reading params from file: %s\n", paramsFile.c_str());
	std::vector<SpotterParams> params = readSpotterParams(paramsFile);
	
	// search dataset using best parameters
	std::vector<TestResult> results = runTestsInSubDirs(testDir, params);
	
	// store results
	storeTestResults(resultsDir, sType, results);
}

void evaluateCBFForSpotterType(SpotterType sType) {
	evaluateDatasetsForSpotterType(sType, kCBF_DIR, kCBF_RESULTS_DIR);
}

void learnCBFParametersDDW() {
	learnParametersDDW(kCBF_DIR);
}
void learnCBFParametersED() {
	learnParametersED(kCBF_DIR);
}
void learnCBFParametersDTW() {
	learnParametersDTW(kCBF_DIR);
}
void learnCBFParametersUS() {
	learnParametersUS(kCBF_DIR);
}
void learnCBFParametersSWM() {
	learnParametersSWM(kCBF_DIR);
}

//================================
// Motif discovery
//================================

// -------------------------------
// Utility functions
// -------------------------------

//highly unsafe convenience function to concatenate
template<typename T, typename R>
std::string concat(T x, R y, std::string sep="") {
	std::ostringstream oss;
	oss << x << sep << y;
	return oss.str();			//timingDir/streamName/SpotType_minLen_maxLen.csv
}

/** returns <kTIMING_DIR>/streamName/SpotType_minLen_maxLen.csv */
std::string generateMotifTimingFileName(SpotterParams p, std::string streamName, int minLen, int maxLen) {
	std::string fileName(kTIMING_DIR);
	ensureTrailingSlash(&fileName);
	ensureDirExists(fileName);			//timingDir/
	
	fileName.append(streamName);
	ensureTrailingSlash(&fileName);
	ensureDirExists(fileName);			//timingDir/streamName
	
	fileName.append(concat(SpotterType_toString(p.spotType), ".csv"));
	
//	std::ostringstream oss;
//	oss << SpotterType_toString(p.spotType) << "_" << minLen << "_" << maxLen << ".csv";
//	fileName.append(oss.str());			//timingDir/streamName/SpotType_minLen_maxLen.csv
	return fileName;
}

std::vector<Subsequence> twoMotifToSubseqsVect(TwoMotif motif) {
	// bundle up the motif in a vector so we can check the answers
	std::vector<Subsequence> subseqs;
	subseqs.push_back(motif.first);
	subseqs.push_back(motif.second);
	return subseqs;
}

TestResult buildMotifTestResult(TwoMotif motif, long cpuTime_ms,
								SpotterParams p, Stream dataStream,
								length_t minMotifLen, length_t maxMotifLen) {
	// determine accuracy
	double precision = 0;
	double recall = 0;
	std::vector<Subsequence> reportedSubseqs = twoMotifToSubseqsVect(motif);
	size_t numSubseqs = reportedSubseqs.size();
	size_t numAnswers = dataStream.patterns.size();
	if (numAnswers) {
//		long numMatches = calculateMatchCount(dataStream.patterns,reportedSubseqs, p.timeMatchThresh);
		long numMatches = calculateMatchCount(reportedSubseqs,dataStream.patterns, p.timeMatchThresh);
		precision = numMatches / numSubseqs;
		recall = numMatches / numAnswers;
	}
	
	// create a results object
	TestResult res;
	res.name = concat(minMotifLen, maxMotifLen, ", ");	//hack to store motif min + max len
	res.params = p;
	res.distCalls = global_dist_calls;
	res.cpuTime = cpuTime_ms;
	res.setPrecision(precision);
	res.setRecall(recall);

	return res;
}

void storeMotifSearchResult(TwoMotif motif, long cpuTime_ms,
							SpotterParams p, Stream dataStream,
							length_t minMotifLen, length_t maxMotifLen) {
	// create a TestResult object
	TestResult res = buildMotifTestResult(motif, cpuTime_ms, p, dataStream, minMotifLen, maxMotifLen);
	
	// bundle up the object in a vector to store
	std::vector<TestResult> results;
	results.push_back(res);
	
	// store the results
	std::string resultsFile = generateMotifTimingFileName(p.spotType, dataStream.name, minMotifLen, maxMotifLen);
	writeTestResults(resultsFile, results);
}

SpotterParams defaultMotifSearchParams(SpotterType sType) {
	SpotterParams p;
	p.spotType = sType;
	p.maxWarp = 10;
	p.scaleFactor = .25;
	p.timeMatchThresh = .5;

	if (sType == DDW_OPTIMAL || sType == DDW_AGGRESSIVE) {
		p.maxWarp = 1;
	}
	return p;
}

inline long msSince(clock_t start) {
	long msec = (clock() - start) * 1000 / CLOCKS_PER_SEC;
	printf("\nelapsed time: %ldms\n\n", msec);
	return msec;
}

// -------------------------------
// Search function wrappers for evaluation
// -------------------------------

void findMotifInStreamAndStoreResult(SpotterParams p, std::string streamDir, int minMotifLen, int maxMotifLen) {
	int numDims = 1;
	Stream dataStream = readStreamFromDir(streamDir, numDims);
	
	// initialize timing
	global_dist_calls = 0;	//reset calls to dist function
	clock_t start = clock();
	
	// run the motif search
	TwoMotif motif = findMotifInStream(p, dataStream, minMotifLen, maxMotifLen);
	
	// stop timing
	long time_ms = msSince(start);
	
	// store the result
	storeMotifSearchResult(motif, time_ms, p, dataStream, minMotifLen, maxMotifLen);
}

void findMotifInStreamAndStoreResult(SpotterType sType, std::string streamDir, int minMotifLen, int maxMotifLen) {
	SpotterParams p = defaultMotifSearchParams(sType);
	findMotifInStreamAndStoreResult(p, streamDir, minMotifLen, maxMotifLen);
}


void searchForMotifUntilFound(SpotterParams p, std::string streamDir, int minMotifLen, int maxMotifLen) {
	int numDims = 1;
	Stream dataStream = readStreamFromDir(streamDir, numDims);
	
	// basic input validation
	long numAnswers = dataStream.patterns.size();
	if (numAnswers != 2) {
		printf("ERROR: searchForMotifUntilFound: number of answers %ld != 2\n", numAnswers);
		exit(1);
	}
	
	// create correct motif
	TwoMotif answers;
	answers.first = dataStream.patterns[0];
	answers.second = dataStream.patterns[1];
	
	// initialize timing
	global_dist_calls = 0;	//reset calls to dist function
	clock_t start = clock();
	
	// carry out the search
	TwoMotif motif = searchMotifLensUntilFound(p, &dataStream.data[0],
											   dataStream.data.size(),
											   minMotifLen, maxMotifLen,
											   answers);
	printMotif(motif);
	
	// stop timing
	long time_ms = msSince(start);
	
	// store the result
	storeMotifSearchResult(motif, time_ms, p, dataStream, minMotifLen, maxMotifLen);
}

void searchForMotifUntilFound(SpotterType sType, std::string streamDir, int minMotifLen, int maxMotifLen) {
	SpotterParams p = defaultMotifSearchParams(sType);
	searchForMotifUntilFound(p, streamDir, minMotifLen, maxMotifLen);
}

// -------------------------------
// MotifDemo
// -------------------------------

void evaluateMotifDemo(SpotterType sType) {
//	searchForMotifUntilFound(sType, kMOTIF_DEMO_DIR, 50, 90);
	findMotifInStreamAndStoreResult(sType, kMOTIF_DEMO_DIR, 50, 100);
}

// -------------------------------
// ECG
// -------------------------------

void evaluateECG(SpotterType sType) {
	int minLen = 50;
	int maxLenToTest = 250;
	int maxLenInc = 25;
	for (int maxLen = 75; maxLen <= maxLenToTest; maxLen += maxLenInc) {
		findMotifInStreamAndStoreResult(sType, kECG_DIR, minLen, maxLen);
	}
}

// -------------------------------
// Random Walk
// -------------------------------

void evaluateRandomWalk(SpotterType sType) {
	int minLen = 50;
	int maxLenToTest = 250;
	int maxLenInc = 25;
	for (int maxLen = 75; maxLen <= maxLenToTest; maxLen += maxLenInc) {
		findMotifInStreamAndStoreResult(sType, kRANDOM_WALK_DIR, minLen, maxLen);
	}
}

