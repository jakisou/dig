//
//  Header.h
//  edtw
//
//  Created By <Anonymous> on 1/5/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_run_ucr_funcs_h
#define edtw_run_ucr_funcs_h

#include "ucr_funcs.h"

const int EPOCH = 100000;

//search contents of files (which may be larger than EPOCH)
Index best_dtw_match_in_file(  char queryFile[], char streamFile[],  int m, double r);
Index* all_dtw_matches_in_file(char queryFile[], char streamFile[], int m, double r,
					   double threshold);
Index best_euclidean_match_in_file(  char queryFile[], char streamFile[], int m);
Index* all_euclidean_matches_in_file(char queryFile[], char streamFile[], int m,
							 double threshold);
Index best_us_match_in_file(char queryFile[], char streamFile[], int m,
				    double minScale, double maxScale);
Index* all_us_matches_in_file(char queryFile[], char streamFile[], int m,
					  double minScale, double maxScale, double threshold);

//search arrays
Index best_dtw_match(  data_t* query, data_t* stream, int m, int n, double r);
Index* all_dtw_matches(data_t* query, data_t* stream, int m, int n, double r,
					   double threshold);
Index best_euclidean_match(  data_t* query, data_t* stream, int m, int n);
Index* all_euclidean_matches(data_t* query, data_t* stream, int m, int n,
							 double threshold);
Index best_us_match(data_t* query, data_t* stream, int m,  int n,
					 double minScale, double maxScale);
Index* all_us_matches(data_t* query, data_t* stream, int m,
					  double minScale, double maxScale, double threshold);

#endif
