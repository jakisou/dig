//
//  test.cpp
//  StreamMain
//
//  Created by Davis blalock on 7/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <list>
#include <string>
#include "StreamMonitor.hpp"
#include "PatternSpotter.hpp"
#include "Subsequence.hpp"
#include "testUtils.hpp"
#include "fileUtils.hpp"
#include "array_utils.h"
#include "global_test_vals.h"

/* Matching pair of subsequences */
typedef struct SubseqMatch {
	Subsequence output;
	Subsequence truth;
} SubseqMatch;

typedef struct LatencyStats {
	double mean;
	double std;
} LatencyStats;

static const std::string ktemplatesDirName = "refs/";
static const std::string kStreamsDirName = "streams/";


/** utility function to check if a value is roughly 0 (floating point error
 * will sometimes prevent it from being *exactly* zero) */
bool isApproxZero(double x) {
	return fabs(x) < kAPPROXIMATELY_ZERO;
}

inline std::string makeClassName(int num) {
	char label[64];
	sprintf(label, "class%u", num);
	std::string labelStr(label);
	return labelStr;
}

LatencyStats calcLatencyStats(std::vector<SubseqMatch> matches) {
	
	LatencyStats s;
	
	unsigned int numMatches = (unsigned int) matches.size();
	std::vector<data_t> latencies(numMatches);
	
	Subsequence output, truth;
	for (unsigned int i = 0; i < numMatches; i++) {
		output = matches[i].output;
		truth = matches[i].truth;
		latencies[i] = data_t(output.latency) / (truth.te - truth.ts + 1);
//		latencies[i] = data_t(output.latency) / (output.te - output.ts + 1);
	}
	
	s.mean	= array_mean(&latencies[0], numMatches);
	s.std	= sqrt(array_variance(&latencies[0], numMatches));

	return s;
}

std::vector<SubseqMatch> findMatches(std::vector<Subsequence> outputVect,
									 std::vector<Subsequence> answersVect,
									 float timeMatchThreshold) {
	std::vector<SubseqMatch> matches;
	
	unsigned long outputLen = outputVect.size();
	unsigned long answersLen = answersVect.size();
	
	// if no responses or no correct answers possible, set both values to 0
	if (outputLen < 1) {
		printf("No matches: no subsequences found\n");
		return matches;
	}
	if (answersLen < 1) {
		printf("No matches: no answers provided\n");
		return matches;
	}
	
	// otherwise, determine how many matches there are between the output and
	// the right answers
	for (unsigned int i=0; i < outputLen; i++) {
		Subsequence outputSeq = outputVect[i];
		//		std::cout << "checking for matches of subseq " << outputSeq << std::endl;
		for (std::vector<Subsequence>::iterator it=answersVect.begin(); it != answersVect.end(); ++it) {
			if ( outputSeq.matches(*it, timeMatchThreshold, timeMatchThreshold) ) {
				SubseqMatch match;
				match.output = outputSeq;
				match.truth = *it;
				matches.push_back(match);
				answersVect.erase(it);
				break;
			}
		}
	}
	
	if (matches.size() < 1) {
		printf("WARNING: findMatches(); no matches found using timeThresh %f\n",
			   timeMatchThreshold);
	}
	
	return matches;
}

unsigned long calculateMatchCount(std::vector<Subsequence> outputVect,
								 std::vector<Subsequence> answersVect,
								 float timeMatchThreshold) {
	
	std::vector<SubseqMatch> matches = findMatches(outputVect, answersVect,
												   timeMatchThreshold);
	return matches.size();
}

double precision(unsigned long trueMatches, unsigned long reportedMatches) {
	return double(trueMatches) / reportedMatches;
}

double recall(unsigned long trueMatches, unsigned long possibleMatches) {
	return double(trueMatches) / possibleMatches;
}

bool runSimpleTest(data_t *query, data_t *stream,
				   length_t queryLen, length_t streamLen,
				   tick_t trueTs, tick_t trueTe, data_t trueDist,
				   SpotterType spotType, data_t cutoff, steps_t maxWarp) {
	
	SpotterParams p;
	p.spotType = spotType;
	p.cutoff = cutoff;
	p.maxWarp = maxWarp;
	p.timeMatchThresh = 0;
	return runSimpleTest(query, stream, queryLen, streamLen, trueTs, trueTe, trueDist, p);

}

bool runSimpleTest(data_t *query, data_t *stream,
				   length_t queryLen, length_t streamLen,
				   tick_t trueTs, tick_t trueTe, data_t trueDist,
				   SpotterParams p) {
	std::string label = "q";

	dims_t oneDim = 1;
	
	// create the appropriate structs for the query and correct answer
	Template q = Template_construct(query, queryLen, oneDim, label);
	Subsequence answer(trueTs, trueTe, trueDist, label);
	
	// bundle up the answers in std::vectors to build a stream
	std::vector<Subsequence> answers;
	answers.push_back(answer);
	Stream s = Stream_construct(stream, streamLen, oneDim, "stream", answers);
	
	StreamMonitor monitor(p);
	monitor.setMode(SMMODE_UNIT_TEST);
	monitor.addTemplate(q);
	std::vector<Subsequence> output = monitor.feedStream(stream, streamLen);
	
	std::cout << "comparing to answer: " << answer << std::endl;
	
	if (output.empty()) {
		printf("No subsequences found");
		return false;
	}
	
	Subsequence response = output.back(); // if multiple subsequences, check last one
	return	response.ts == trueTs &&
			response.te == trueTe &&
			isApproxZero(response.dist - trueDist);
}

void storeOutput(std::string name, SpotterParams p, std::vector<Subsequence> output) {
	std::string outFileName = buildFileName(getGlobalOutputDir(), name, p);
	writeSubsequences(outFileName, output);
}

std::string generateTestResultsFileName(std::string testName, SpotterType spotType) {
	std::string path(getGlobalTestResultsDir());
	std::string subDir = replaceSlashes(testName);
	path.append(subDir);
	path.append("/");
	ensureDirExists(path);
	std::string fName = SpotterType_toString(spotType);
	path.append(fName);
	path.append(".csv");	//TODO define standard file ext somewhere
	return path;
}

void storeTestResults(std::string testName, SpotterType spotType, std::vector<TestResult> results) {
	//write to RESULTS_DIR/<dirName>/<spotType>.csv
	std::string path = generateTestResultsFileName(testName, spotType);
	writeTestResults(path, results);
}

//TODO this function really needs to be split up into like 5 functions...
TestResult runTest(std::vector<Template> const& templates, Stream const& stream,
				   SpotterParams p) {
	
	//reset the total number of calls to the dist() function
	global_dist_calls = 0;
	
	float timeMatchThresh = p.timeMatchThresh;
	
	size_t numtemplates = templates.size();
	std::vector<data_t> streamData = stream.data;
	std::vector<Subsequence> output = std::vector<Subsequence>();
	
	// create a StreamMonitor and have it look for each reference sequence
	StreamMonitor monitor(p);
	monitor.setMode(SMMODE_FULL_TEST);
	monitor.addTemplates(templates);
	
	// search the data stream (and time it)
	clock_t start = clock(), diff;
	output = monitor.feedStream(&streamData[0], streamData.size());
	diff = clock() - start;
	long time_msec = diff * 1000 / CLOCKS_PER_SEC;
	
	// store the output
	storeOutput(stream.name, p, output);
	
	// assess the StreamMonitor's pruning power
	dist_calls_t distCalls = monitor.getDistCallCounts();
	dist_calls_t possibleDistCalls = 0;
	for (int i=0; i < numtemplates; i++) {
		possibleDistCalls += templates[i].nSamples;
	}
	possibleDistCalls *= monitor.getTime();
	double pruningPower = double(distCalls) / possibleDistCalls;
	
	std::vector<SubseqMatch> trueMatches = findMatches(output, stream.patterns,
												  timeMatchThresh);
	
	unsigned long trueMatchCount = trueMatches.size();
	unsigned long reportedMatches = output.size();
	unsigned long possibleMatches = stream.patterns.size();
	
	LatencyStats latencyStats = calcLatencyStats(trueMatches);
	
	// bundle together and return the results of this test
	TestResult res;
	res.params		= p;
	res.name		= stream.name;
	res.meanLatency = latencyStats.mean;
	res.stdLatency  = latencyStats.std;
	res.cpuTime		= time_msec;
	res.distCalls	= distCalls;
	res.pruningPower= pruningPower;
	res.setPrecision( precision(trueMatchCount, reportedMatches));
	res.setRecall(    recall(trueMatchCount, possibleMatches));
	
	return res;
}

std::vector<TestResult> runTests(std::string templatesDir, std::string streamsDir,
								 SpotterParams p) {
	dims_t nDims = p.nDims;
	
	// initialize test results container
	std::vector<TestResult> results = std::vector<TestResult>();
	
	// read in our training and test data
	std::vector<Template> templates = readTemplates(templatesDir, nDims);
	std::vector<Stream> streams = readSubdirsAsStreams(streamsDir, nDims);
	
	// run each test
	for (int i=0; i < streams.size(); i++) {
		results.push_back(runTest(templates, streams[i], p));
	}
	
	// print out which streams were tested
	printf("Tested streams:\n");
	for (int i = 0; i < streams.size(); i++) {
		std::cout << streams[i].name << std::endl;
	}
	
	// print out a summary of all the test results in a format we can paste
	// into a spreadsheet, MATLAB, etc
	printf("Type Cutoff Warp Scale ");
//	printf("Cpu Dists PP Prec Rec Lmean Lstd\n");
	printf("Cpu Dists PP Prec Rec F1\n");
	for (int i = 0; i < results.size(); i++) {
		std::cout << results[i] << "\n";
	}
	
	return results;
}

std::vector<TestResult> runTestsInDir(std::string dir, SpotterParams p) {
	ensureTrailingSlash(&dir);
	
	if (p.timeMatchThresh == 0) {
		p.timeMatchThresh = kSUBSEQUENCE_OVERLAP_THRESH;
	}
	
	std::string templatesDir = dir;
	std::string streamsDir = dir;
	templatesDir.append(ktemplatesDirName);
	streamsDir.append(kStreamsDirName);
	
	std::vector<TestResult> results = runTests(templatesDir, streamsDir, p);
	
	// store the results
	storeTestResults(dir, p.spotType, results);
	
	return results;
}

std::vector<TestResult> runTestsInDir(std::string dir, SpotterType spotType,
									  data_t cutoff, steps_t maxWarp, float scaleFactor) {
	SpotterParams p;
	p.spotType = spotType;
	p.cutoff = cutoff;
	p.maxWarp = maxWarp;
	p.scaleFactor = scaleFactor;
	return runTestsInDir(dir, p);
}

//NOTE that this doesn't actually handle multiple streams in any given subdirectory
std::vector<TestResult> runTestsInSubDirs(std::string parentDir, std::vector<SpotterParams> params) {
	ensureTrailingSlash(&parentDir);
	std::vector<std::string> subdirs = getFilePathsInDir(parentDir);
	
	// validate input	//TODO validate more and better
	if (subdirs.size() != params.size() && params.size() != 1) {
		printf("ERROR: runTestsInSubDirs: number of subdirectories %ld does not match number of parameters %ld",
			   subdirs.size(), params.size());
		exit(1);
	}
	
	//run all the tests in each subdirectory
	std::vector<TestResult> allResults = std::vector<TestResult>();
	for (int i = 0; i < subdirs.size(); i++) {
		std::vector<TestResult> subdirResults;
		if (params.size() == 1) {
			subdirResults = runTestsInDir(subdirs[i], params[0]);
		} else {
			subdirResults = runTestsInDir(subdirs[i], params[i]);
		}
//		allResults.insert(allResults.end(), subdirResults.begin(), subdirResults.end());
		allResults.push_back(subdirResults[0]);
	}
	
	// output results
	//	size_t numResults = allResults.size();
	//	double sumFstat = 0;
	//	printf("Name\tCpuTime\tDistCalls\tPruningPower\tPrecision\tRecall\n");
	//	for (int i=0; i < numResults; i++) {
	//		TestResult r = allResults[i];
	//		sumFstat += r.getF1score();
	//		printf("%s\t%ld\t%lld\t%f\t%f\t%f\t%f\n", r.name.c_str(), r.cpuTime, r.distCalls,
	//			   r.pruningPower, r.getPrecision(), r.getRecall(), r.getF1score());
	//	}
	//	printf("mean fStat = %f\n",sumFstat / numResults);
	
	return allResults;
}

//Note that this enforces a fixed number of dimensions, cutoff, and maxWarp for
//all tests in all subdirectories
std::vector<TestResult> runTestsInSubDirs(std::string parentDir, SpotterParams p) {
	std::vector<SpotterParams> params = std::vector<SpotterParams>();
	params.push_back(p);
	return runTestsInSubDirs(parentDir, params);
}
