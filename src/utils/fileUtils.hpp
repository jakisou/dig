//
//  streamUtils.h
//  StreamMain
//
//  Created by Davis blalock on 7/22/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#ifndef StreamMain_streamUtils_h
#define StreamMain_streamUtils_h

#include <string>
#include <vector>
#include "basic_structs.hpp"
#include "filePaths.hpp"

extern std::string gOutputDir;
extern std::string gTestResultsDir;

// get and set directories in which to store raw subsequence outputs
// and test results; this is significantly cleaner than passing extra
// args into all of the runTest functions
std::string getGlobalOutputDir();
void setGlobalOutputDir(std::string dir);
std::string getGlobalTestResultsDir();
void setGlobalTestResultsDir(std::string dir);

std::string lastPathComponent(std::string path);

Stream readStreamFromDir(std::string dirName, unsigned int nDims);

/// appends a trailing slash to the given string if one is not already present; NOT safe on Windows
void ensureTrailingSlash(std::string* s);

/// removes a trailing slash in the given string if one is present; NOT safe on Windows
void removeTrailingSlash(std::string* s);

/// returns a std::vector of std::strings corresponding to each item in a directory
std::vector<std::string> getFileNamesInDir(std::string dir);

/// returns a std::vector of std::strings corresponding to the paths to each file in a directory
std::vector<std::string> getFilePathsInDir(std::string dir);

/** Populates a sequence object with the data found in the supplied file
 *
 * @param directory The directory in which the file resides
 * @param fileName The name of the file within this directory (NOT the full path)
 * @param seq The Sequence object to be populated
 * @param nDims The number of dimensions the sequences should have (Default: 1)
 */
void readDataFile(std::string directory, std::string fileName, Sequence *seq,
				  unsigned int nDims=1);

///** Convenience function to call readDataFile() on each file in a directory (and
// * give each resulting sequence the supplied class label, resulting in a Template).
// */
//std::vector<Template> readDataFilesInDir(std::string dir, std::string classLabel,
//									   unsigned int nDims);

/** Constructs streams from each subdirectory in the supplied directory (see
 * project for examples of directory structure and necessary file names)
 *
 * @param streamsDir The directory from which one or more streams should be read
 * @param nDims The number of dimensions in these streams (Default: 1)
 */
std::vector<Stream> readSubdirsAsStreams(std::string streamsDir, unsigned int nDims=1);

/** Constructs templates of the given class and dimensionality using all of
 * the files in the provided directory
 */
std::vector<Template> readTemplateFilesInDir(std::string dir, std::string classLabel,
									  unsigned int nDims);
/** Calls readTemplateFilesInDir() on each subdirectory of the provided directory,
 * using each subdirectory's name as the class label for its constituent sequences.
 */
std::vector<Template> readTemplates(std::string templatesDir, unsigned int nDims);

/** Creates a directory at the given path if it doesn't already exist */
void ensureDirExists(std::string dir);

/** Replaces all slashes in the given std::string with underscores */
std::string replaceSlashes(std::string path);

/** Creates a standardized filename in which to save files (to enforce
 * a consistent and human-comprehensible naming convention).
 * 
 * @param saveDir The directory in which the file should reside
 * @param testName The name of the test (user pTemplateerence--not meaning-bearing)
 * @param p The search parameters associated with the data to be saved in the file
 */
std::string buildFileName(std::string saveDir, std::string testName, SpotterParams p);

#endif
