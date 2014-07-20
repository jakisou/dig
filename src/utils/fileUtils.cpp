//
//  streamUtils.cpp
//  StreamMain
//
//  Created by Davis blalock on 7/24/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>	//for mkdir...wat?
#include "fileUtils.hpp"
#include "Subsequence.hpp"

const std::string kSTREAM_DATA_FILE_NAME("data.csv");
const std::string kSTREAM_PATTERNS_FILE_NAME("answers.csv");

std::string gOutputDir(DEFAULT_OUTPUT_DIR);
std::string gTestResultsDir(DEFAULT_RESULTS_DIR);

std::string getGlobalOutputDir() {
	return gOutputDir;
}
void setGlobalOutputDir(std::string dir) {
	gOutputDir = dir;
}
std::string getGlobalTestResultsDir() {
	return gTestResultsDir;
}
void setGlobalTestResultsDir(std::string dir) {
	gTestResultsDir = dir;
}

int ls(std::string dir, std::vector<std::string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL) {
		std::cout << "Error(" << errno << ") opening " << dir << std::endl;
        return errno;
    }
	
    while ((dirp = readdir(dp)) != NULL) {
		std::string fName = std::string(dirp->d_name);
		// if file not hidden and not either "." or "..", add it
		// to the list
		if (! (fName.find(".") == 0) ) {
			files.push_back(fName);
		}
    }
    closedir(dp);
    return 0;
}

void ensureTrailingSlash(std::string* s) {
	if (s->length() > 0) {
		std::string::iterator it = s->end() - 1;
		if (*it != '/') {
			s->append("/");
		}
	}
}

void removeTrailingSlash(std::string* s) {
	if (s->length() > 0) {
		std::string::iterator it = s->end() - 1;
		if (*it == '/' || *it == '\\') {
			s->erase(it, it+1);
		}
	}
}

std::string lastPathComponent(std::string path) {
	std::string component(path);
	removeTrailingSlash(&component);
	
	// Find everything after last slash (with trailing slash removed)
	const size_t last_slash_idx = component.find_last_of("/");
	if (std::string::npos != last_slash_idx) {	// if slash present
		component.erase(0, last_slash_idx + 1);
	}
	return component;
}

//TODO make this cross-platform; mkdir may not work on Windows...
void ensureDirExists(std::string dir) {
	mkdir(dir.c_str(), 0755);
}

std::string replaceSlashes(std::string path) {
	//replace all the / with - so it's one directory, not a path
	std::string fName(path);
	std::replace(fName.begin(), fName.end(), '/', '_');
	return fName;
}

void readDataFile(std::string directory, std::string fileName, Sequence *seq, unsigned int nDims) {
	if (nDims < 1) {
		std::cout << "ERROR: invalid number of dimensions provided: " << nDims
			<< " to read file " << directory << "/" << fileName << "; exiting" << std::endl;
		exit(1);
	}
	
	ensureTrailingSlash(&directory);
	std::string fullName = directory.append(fileName);
	std::ifstream data(fullName.c_str());
	
	//read in each line
    std::string line;
    while(getline(data,line)) {
        std::stringstream lineStream(line);
        std::string cell;
		
		//read in each entry on the line
		unsigned int dim = 0;
        while(getline(lineStream,cell,' ') && dim < nDims) {
			seq->data.push_back( atof(cell.c_str()) );
			dim++;
        }
    }
	
	if (seq->data.size() % nDims != 0) {
		std::cout << "Error in Templateerence file " << fullName << ":" << std::endl;
		std::cout << "inconsistent sample dimensions; all rows should have had " << nDims << std::endl;
		std::cout << "Data length: " << seq->data.size() << std::endl;
	}
	
	seq->nDims = nDims;
	seq->nSamples = (unsigned int)seq->data.size() / nDims;
}

std::vector<std::string> getFileNamesInDir(std::string dir) {
	std::vector<std::string> fNames = std::vector<std::string>();
	ls(dir, fNames);
	return fNames;
}
	
std::vector<std::string> getFilePathsInDir(std::string parentDir) {
	ensureTrailingSlash(&parentDir);
	std::vector<std::string> paths = getFileNamesInDir(parentDir);
	for (int i = 0; i < paths.size(); i++) {
		std::string subdir = parentDir;
		subdir.append(paths[i]);
		paths[i] = subdir;
	}
	return paths;
}

std::vector<Template> readTemplateFilesInDir(std::string dir, std::string classLabel, unsigned int nDims) {
	std::vector<Template> templates = std::vector<Template>();
	
	// get the names of all the files in this directory
	std::vector<std::string> fNames = getFileNamesInDir(dir);
	
	// extract the Templateerence sequences from each file
	for (unsigned int i=0; i < fNames.size(); i++) {
		std::string fName = fNames[i];
		Template Template;
		readDataFile(dir, fName, &Template, nDims);
		
		Template.label = classLabel;
		
		//if no actual samples in a file, throw it out	//TODO better error check
		if (Template.nSamples ) {
			templates.push_back(Template);
		}
	}
	
	return templates;
}


Stream readStreamFromDir(std::string dirName, unsigned int nDims) {
	ensureTrailingSlash(&dirName);
	
	Stream stream;
	
	// return an invalid stream if this directory doesn't contain an
	// appropriately-named data file
	std::string dataFileName = std::string(dirName).append(kSTREAM_DATA_FILE_NAME);
	std::ifstream dataFile(dataFileName.c_str());
	if (! dataFile) {
		printf("No valid stream data file: %s\n", dataFileName.c_str());
		exit(1);
//		stream.isValid = false;
//		return stream;
	}
	dataFile.close();
	
	// if the data file existed, read in its values
	readDataFile(dirName, kSTREAM_DATA_FILE_NAME, &stream, nDims);
	
	// if the file of correct answers exists, read that in and add the
	// answers to our stream object
	std::string patternsFileName = std::string(dirName).append(kSTREAM_PATTERNS_FILE_NAME);
	std::ifstream patternsFile(patternsFileName.c_str());
	if (patternsFile) {
		patternsFile.close();
		stream.patterns = readSubsequences(patternsFileName);
		printf("gave stream %ld answers\n", stream.patterns.size());
	} else {
		printf("No valid stream answers file: %s\n", patternsFileName.c_str());
		exit(1);
	}
	
	stream.name = lastPathComponent(dirName);
	stream.isValid = true;
	return stream;
}

std::vector<Stream> readSubdirsAsStreams(std::string streamsDir, unsigned int nDims) {
	std::vector<Stream> streams = std::vector<Stream>();
	
	//read in all the subdirectories that contain
	//the instances of the various classes
	std::vector<std::string> subdirs = getFileNamesInDir(streamsDir);
	
	for (int i=0; i < subdirs.size(); i++) {
		std::string fullDirName = std::string(streamsDir).append(subdirs[i]);
		printf("searching for streamDir: %s\n", fullDirName.c_str());
		Stream s = readStreamFromDir(fullDirName, nDims);
		if (s.isValid) {
			streams.push_back(s);
		}
	}
	
	return streams;
}

std::vector<Template> readTemplates(std::string templatesDir, unsigned int nDims) {
	
	//read in all the subdirectories that contain
	//the instances of the various classes
	std::vector<std::string> subdirs = getFileNamesInDir(templatesDir);
	
//	printf("Templates subdirs: \n");
//	for (unsigned int j=0; j < subdirs.size(); j++) {
//		printf("-- %s\n", subdirs[j].c_str());
//	}
//	printf("\n");
	
	unsigned int numClasses = (unsigned int)subdirs.size();
	
	//create a std::vector of Templateerence data matrices
	std::vector<Template> templates = std::vector<Template>();
	
	for (unsigned int i = 0; i < numClasses; i++) {
		std::string dir = std::string(templatesDir).append(subdirs[i]);
		std::vector<Template> templatesInDir = readTemplateFilesInDir(dir, subdirs[i], nDims);
		templates.insert(templates.end(), templatesInDir.begin(), templatesInDir.end());
	}

	return templates;
}

std::string buildFileName(std::string saveDir, std::string testName, SpotterParams p) {
	std::string subDirName = replaceSlashes(testName);
	
	// append this filename to the defined output directory
	std::string outFileName(saveDir);
	outFileName.append(subDirName);
	outFileName.append("/");
	ensureDirExists(outFileName);
	
	// different subdirectories for different distance measures
	outFileName.append(SpotterType_toString(p.spotType));
	outFileName.append("/");
	ensureDirExists(outFileName);
	
	//create the actual filename based on the parameters of the test
	outFileName.append(SpotterParams_toFileName(p));
	
	return outFileName;
}
