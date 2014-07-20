//
//  test_ucr_funcs.c
//  edtw
//
//  Created By <Anonymous> on 1/5/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ucr_funcs.h"
#include "run_ucr_funcs.h"

/** Enumeration defining possible functions used to search for matches
 * for a given query in a data stream */
typedef enum SearchFunction {
	DTW = 0,
	EUCLIDEAN = 1,
	UNIFORM_SCALING = 2,
} SearchFunction;

/** Structure defining arguments to all possible search functions so that
 *  we can a have a unified means of calling them all */
typedef struct searchArgs {
	int m;
	int n;
	double r;
	double minScale;
	double maxScale;
} SearchArgs;

// ================================================================
// Internal utility functions
// ================================================================

/** Malloc a double array of length len, reporting an error and 
  * exiting if out of memory */
data_t* tryToMalloc(long len) {
	data_t* ar = (data_t *)malloc(sizeof(data_t) * len);
    if( ar == NULL ) {
		printf("Error: out of memory. Exiting\n");
        exit(1);
	}
	return ar;
}

/** Attempt to open the given file, reporting an error and
 * exiting if this fails */
FILE* tryToOpen(const char* fileName, const char* mode) {
	FILE* qp = fopen(fileName, mode);
    if(qp == NULL) {
		printf("Error: couldn't open file: %s\n", fileName);
        exit(1);
	}
	return qp;
}

// ================================================================
// Internal searching functions
// ================================================================

/** search for a given query in a stream using the specified function and arguments */
long search(data_t* query, data_t* stream, SearchFunction func, SearchArgs args,
			double bsf, Index* result) { //TODO option to do all matches
	switch (func) {
		case DTW:
			return dtw_ongoing_search(query, stream, args.m, args.n, args.r, bsf, result);
		case EUCLIDEAN:
			return euc_ongoing_search(query, stream, args.m, args.n, bsf, result);
		case UNIFORM_SCALING:
			return -1; //TODO implement
			break;
		default:
			return -1;
	}
}

Index search_file(char queryFile[], char dataFile[], SearchFunction searchFunc,
				  SearchArgs searchArgs) { //TODO option to do all matches
	int     i;
    double  d;
	double	bsf = INFINITY;
    data_t  *q, *buffer;
    FILE    *fp;
    FILE    *qp;
    Index result;
	
	//unpack search arguments
	int m = searchArgs.m;
	
	//open files for reading
	fp = tryToOpen(dataFile, "r");
	qp = tryToOpen(queryFile, "r");
	
	//allocate space for query and buffer
    q		= tryToMalloc(m);
	buffer	= tryToMalloc(EPOCH);
	
	//read in the query file
    i = 0;
//    j = 0;
    while(fscanf(qp,"%lf",&d) != EOF && i < m) {
        q[i] = d;
        i++;
    }
    fclose(qp);
	
	//read in the data file
    short int done = 0;
    int64_t it = 0;
	int ep = 0, k = 0;
    while (!done) {
		
        /// Read first m-1 points
        ep = 0;
        if (it == 0) {
			//first time filling buffer - use start of file
            for(k = 0; k < m - 1; k++)
                if (fscanf(fp,"%lf",&d) != EOF)
                    buffer[k] = d;
        } else {
			//not first time filling buffer - put end of last buffer
			//at start of new buffer so no subsequences are skipped
            for(k = 0; k < m - 1; k++)
                buffer[k] = buffer[EPOCH - m + 1 + k];
        }
		
        // Read buffer of size EPOCH (or rest of data, if less)
        ep = m - 1;
        while(ep < EPOCH) {
            if (fscanf(fp,"%lf",&d) == EOF) {
				done = 1;
				break;
			}
            buffer[ep] = d;
            ep++;
        }
		
        // data is read into a buffer of size EPOCH; if the entire buffer
		// isn't full, we've hit the end of the data
        if (ep < EPOCH) {
            done = 1;
        } else {
			searchArgs.n = ep;
			long retVal = search(q, buffer, searchFunc, searchArgs, bsf, &result);
			if (retVal != kSUCCESS) {
                printf("Match failed!\n");
                result.index = -1;
				return result;
            }
			
			// store best distance so far
			bsf = result.value; //TODO is this guaranteed to be nonincreasing?
			
            printf("Location : %lli\n", result.index + (it)*(EPOCH-m+1));
            printf("Distance : %2.5f\n", result.value);
        }
        it++;
    }
	free(q);
	free(buffer);
	return result;
}

// ================================================================
// Public File searching functions
// ================================================================

Index best_dtw_match_in_file(char queryFile[], char streamFile[], int m, double r) {
	SearchArgs args;
	args.m = m;
	args.r = r;
	return search_file(queryFile, streamFile, DTW, args);
}

Index best_euclidean_match_in_file(char queryFile[], char streamFile[], int m) {
	SearchArgs args;
	args.m = m;
	return search_file(queryFile, streamFile, EUCLIDEAN, args);
}

Index best_us_match_in_file(char queryFile[], char streamFile[], int m,
							double minScale, double maxScale) {
	SearchArgs args;
	args.m = m;
	args.minScale = minScale;
	args.maxScale = maxScale;
	return search_file(queryFile, streamFile, UNIFORM_SCALING, args);
}

// ================================================================
// Public array searching functions
// ================================================================

Index best_dtw_match(data_t* query, data_t* stream, int m, int n, double r) {
	SearchArgs args;
	args.m = m;
	args.n = n;
	args.r = r;
	Index result;
	search(query, stream, DTW, args, INFINITY, &result);
	return result;
}

Index best_euclidean_match(data_t* query, data_t* stream, int m, int n) {
	SearchArgs args;
	args.m = m;
	args.n = n;
	Index result;
	search(query, stream, DTW, args, INFINITY, &result);
	return result;
}

Index best_us_match(data_t* query, data_t* stream, int m,  int n,
					double minScale, double maxScale) {
	SearchArgs args;
	args.m = m;
	args.n = n;
	args.minScale = minScale;
	args.maxScale = maxScale;
	Index result;
	search(query, stream, DTW, args, INFINITY, &result);
	return result;
}