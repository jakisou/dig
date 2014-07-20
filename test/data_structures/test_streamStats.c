//
//  test_streamStats.c
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "test_streamStats.h"
#include "streamStats.h"
#include "array_utils.h"
#include "type_defs.h"

void test_streamStats_all() {
	printf("================================ Testing StreamStats\n");
	streamStats_stats_withSetLongerThenShorter_correct();
	printf("================================ End StreamStats Test: Success\n");
}

// this is more of an end-to-end test than a unit test, since
// computing the statistics is ugly and it makes a lot of sense
// to validate that the stats are correct at each stage of this
// processs
void streamStats_stats_withSetLongerThenShorter_correct() {
	unsigned int len = 8;
	unsigned int longerLen = 15;
	unsigned int shorterLen = 6;
	data_t data[] = {1,-1, 2,-2, 3, 4,-4, 6};
	
	// build Sum[X]
	data_t sumX_truth[len];
	array_copy_reverse(data, sumX_truth, len);
	array_cum_sum(sumX_truth, len);
	
	// build Sum[X^2]
	data_t sumX2_truth[len];
	array_copy_reverse(data, sumX2_truth, len);
	array_mult(sumX2_truth, sumX2_truth, len);
	array_cum_sum(sumX2_truth, len);
	
	// build (Sum[X])^2
	data_t sumX_2[len];
	array_copy(sumX_truth, sumX_2, len);
	array_mult(sumX_2, sumX_2, len);
	
	// build Sxx
	data_t Sxx_truth[len];
	array_copy(sumX_2, Sxx_truth, len);
	data_t *idxs = array_create_sequence(1, len, 1);
	array_div(idxs, Sxx_truth, len);
	free(idxs);
	array_mult_scalar(Sxx_truth, -1, len);
	array_add(sumX2_truth, Sxx_truth, len);
	
	// create a streamStats and hand it the stream
	streamStats *s = streamStats_new(len);
	for (unsigned int i = 0; i < len; i++) {
		streamStats_update(s, i, data[i]);
	}
	
	// mess with the length to see if this messes it up
	streamStats_setHistoryLength(s, longerLen);

	if (! array_equal(s->sumX, sumX_truth, len) ) {
		printf("TEST FAILED: streamStats SumX correct\n");
		array_print_with_name(s->sumX, len, "sumX_stats");
		array_print_with_name(sumX_truth, len, "sumX_truth");
		assert(0);
	}
	
//	if (! array_equal(s->sumX2, sumX2_truth, len) ) {
//		printf("TEST FAILED: streamStats SumX^2 correct\n");
//		array_print_with_name(s->sumX2, len, "sumX2_stats");
//		array_print_with_name(sumX2_truth, len, "sumX2_truth");
//		assert(0);
//	}
	
	if (! array_equal(s->Sxx, Sxx_truth, len) ) {
		printf("TEST FAILED: streamStats Sxx correct\n");
		array_print_with_name(s->Sxx, len, "Sxx_stats");
		array_print_with_name(Sxx_truth, len, "Sxx_truth");
		assert(0);
	}
	
	// make the history longer to see if this messes it up
	streamStats_setHistoryLength(s, longerLen);
	
	if (! array_equal(s->sumX, sumX_truth, len) ) {
		printf("TEST FAILED: streamStats SumX longerLen correct\n");
		array_print_with_name(s->sumX, len, "sumX_stats");
		array_print_with_name(sumX_truth, len, "sumX_truth");
		assert(0);
	}
	
//	if (! array_equal(s->sumX2, sumX2_truth, len) ) {
//		printf("TEST FAILED: streamStats SumX^2 longerLen correct\n");
//		array_print_with_name(s->sumX2, len, "sumX2_stats");
//		array_print_with_name(sumX2_truth, len, "sumX2_truth");
//		assert(0);
//	}
	
	if (! array_equal(s->Sxx, Sxx_truth, len) ) {
		printf("TEST FAILED: streamStats Sxx longerLen correct\n");
		array_print_with_name(s->Sxx, len, "Sxx_stats");
		array_print_with_name(Sxx_truth, len, "Sxx_truth");
		assert(0);
	}
	
	// make the history shorter to see if this messes it up
	streamStats_setHistoryLength(s, shorterLen);
	
	if (! array_equal(s->sumX, sumX_truth, shorterLen) ) {
		printf("TEST FAILED: streamStats SumX shorterLen correct\n");
		array_print_with_name(s->sumX, shorterLen, "sumX_stats");
		array_print_with_name(sumX_truth, shorterLen, "sumX_truth");
		assert(0);
	}
	
//	if (! array_equal(s->sumX2, sumX2_truth, shorterLen) ) {
//		printf("TEST FAILED: streamStats SumX^2 shorterLen correct\n");
//		array_print_with_name(s->sumX2, shorterLen, "sumX2_stats");
//		array_print_with_name(sumX2_truth, shorterLen, "sumX2_truth");
//		assert(0);
//	}
	
	if (! array_equal(s->Sxx, Sxx_truth, shorterLen) ) {
		printf("TEST FAILED: streamStats Sxx shorterLen correct\n");
		array_print_with_name(s->Sxx, shorterLen, "Sxx_stats");
		array_print_with_name(Sxx_truth, shorterLen, "Sxx_truth");
		assert(0);
	}

	streamStats_free(s);
}
