//
//  test.h
//  StreamMain
//
//  Created by Davis blalock on 7/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef StreamMain_test_h
#define StreamMain_test_h

#include <limits.h>
#include <math.h>
#include <string>
#include "TestResult.hpp"
#include "basic_structs.hpp"

static const double kAPPROXIMATELY_ZERO = .0001;	///< Constant for equality comparisons ignoring floating-point error
static const float kSUBSEQUENCE_OVERLAP_THRESH = .5; ///< start and end times can be off by this much

/** Utility function to check if a value is roughly 0 (floating point error
 * will sometimes prevent it from being *exactly* zero) */
bool isApproxZero(double x);

/** Computes the precision given the number of true positives reported (matches)
 * and total reported positives.
 *
 * @param trueMatches The number of matches reported that were correct
 * @param reportedMatches The total number of matches that were reported
 */
double precision(unsigned long trueMatches, unsigned long reportedMatches);

/** Computes the recall given the number of true positives reported (matches)
 * and total positives that could have been reported.
 *
 * @param trueMatches The number of matches reported that were correct
 * @param possibleMatches The total number of matches that were actually present
 */
double recall(unsigned long trueMatches, unsigned long possibleMatches);

std::string generateTestResultsFileName(std::string testName, SpotterType spotType);

unsigned long calculateMatchCount(std::vector<Subsequence> outputVect,
								  std::vector<Subsequence> answersVect,
								  float timeMatchThreshold=.5);

/** Writes the test results to a file in the specified directory, with a name 
 * determined by the SpotterType used to produce the results.
 *
 * @param testName A name for the subdirectory of the global gTestResultsDir 
 *	in which to write the file
 * @param spotType The SpotterType on which the test was run
 * @param results The collection of TestResults to store
 */
void storeTestResults(std::string testName, SpotterType spotType, std::vector<TestResult> results);

/** Convenience function for unit tests that searches for a 1D query in a 1D stream.
 *
 * @param query An array of data used as the query
 * @param stream An array of data used as the stream
 * @param queryLen The length of the query array
 * @param streamLen The length of the stream array
 * @param trueTs The start time that should be reported
 * @param trueTe The end time that should be reported
 * @param trueDist The distance that should be reported
 * @param spotType The spotType to be used in searching the stream
 * @param cutoff The distance cutoff to be used in searching the stream (Default value: INFINITY)
 * @param maxWarp The maximum amount of time warping in searching the stream
 */
bool runSimpleTest(data_t *query, data_t *stream,
				  length_t queryLen, length_t streamLen,
				  tick_t trueTs, tick_t trueTe, data_t trueDist,
				  SpotterType spotType,
				  data_t cutoff=INFINITY, steps_t maxWarp=0);

/** Convenience function for unit tests that searches for a 1D query in a 1D stream.
 *
 * @param query An array of data used as the query
 * @param stream An array of data used as the stream
 * @param queryLen The length of the query array
 * @param streamLen The length of the stream array
 * @param trueTs The start time that should be reported
 * @param trueTe The end time that should be reported
 * @param trueDist The distance that should be reported
 * @param p The parameters that should be used to search the stream
 */
bool runSimpleTest(data_t *query, data_t *stream,
				   length_t queryLen, length_t streamLen,
				   tick_t trueTs, tick_t trueTe, data_t trueDist,
				   SpotterParams p);

/** Main test function. Searches for the supplied training data in the stream
 * using the given search parameters.
 * 
 * @param templates The training data--a collection of labeled time series
 * @param stream The Stream to be searchd for matches for the training data
 * @param p The parameters used to search the stream
 */
TestResult runTest(std::vector<Template> const& templates, Stream const& stream,
				   SpotterParams p);

/** Convenience function to call runTest() using the training data and streams
 * found in the supplied directories (see example directory structures in project) 
 */
std::vector<TestResult> runTests(std::string templatesDir, std::string streamsDir,
								 SpotterParams p);

/** Convenience function to call runTests() using appropriately named
 * subdirectories of the directory supplied (see example directory structures 
 * in project)
 */
std::vector<TestResult> runTestsInDir(std::string dir, SpotterParams p);

/** Convenience function to call runTests() using appropriately named
 * subdirectories of the directory supplied (see example directory structures
 * in project)
 */
std::vector<TestResult> runTestsInDir(std::string dir, SpotterType spotType,
									  data_t cutoff, steps_t maxWarp=0,
									  float scaleFactor=0);

/** Convenience function to call runTestsInDir() in each subdirectory of the
 * supplied directory. Uses successive SpotterParams for tests in successive
 * subdirectories. Errors out if the number of parameters is not equal to
 * the number of subdirectories.
 */
std::vector<TestResult> runTestsInSubDirs(std::string parentDir, std::vector<SpotterParams> params);

/** Convenience function to call runTestsInDir() in each subdirectory of the
 * supplied directory. Uses the same SpotterParams for all tests.
 */
std::vector<TestResult> runTestsInSubDirs(std::string parentDir, SpotterParams p);

#endif