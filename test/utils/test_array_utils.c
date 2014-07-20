//
//  test_array_utils.c
//  edtw
//
//  Created By <Anonymous> on 1/15/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include "test_array_utils.h"
#include "array_utils.h"

void test_array_utils_all() {
	printf("================================ Testing Array Utils\n");
	resample_sameSampRate_correct();
	resample_upsampleByInteger_correct();
	resample_upsampleByFraction_correct();
	resample_downsampleByInteger_correct();
	resample_downsampleByFraction_correct();
	copyReverse_evenLen_correct();
	copyReverse_oddLen_correct();
	reshape_dimsNotFactorOfLen_returnsNull();
	printf("================================ End Array Utils Test: Success\n");
}

void resample_sameSampRate_correct() {
	unsigned int srcLen = 5;
	unsigned int destLen = 5;
	data_t src[] = {1,2,3,4,5};
	data_t destTruth[] = {1,2,3,4,5};
	data_t dest[destLen];
	
	array_resample(src,dest,srcLen,destLen);
	
	if (! array_equal(dest,destTruth,destLen) ) {
		printf("TEST FAILED: resample to same length\n");
		array_print_with_name(src,srcLen,"src");
		array_print_with_name(dest,destLen,"dest");
		assert(0);
	}
}

void resample_upsampleByInteger_correct() {
	unsigned int srcLen = 5;
	unsigned int destLen = 15;
	data_t src[] = {1,2,3,4,5};
	data_t destTruth[] = {1,1,1, 2,2,2, 3,3,3, 4,4,4, 5,5,5};
	data_t dest[destLen];
	
	array_resample(src,dest,srcLen,destLen);
	
	if (! array_equal(dest,destTruth,destLen) ) {
		printf("TEST FAILED: upsample by integer factor\n");
		array_print_with_name(src,srcLen,"src");
		array_print_with_name(dest,destLen,"dest");
		assert(0);
	}
}

void resample_upsampleByFraction_correct() {
	unsigned int srcLen = 5;
	unsigned int destLen = 7;
	data_t src[] = {1,2,3,4,5};
	data_t destTruth[] = {1,1,2,3,3,4,5,};
	data_t dest[destLen];
	
	array_resample(src,dest,srcLen,destLen);
	
	if (! array_equal(dest,destTruth,destLen) ) {
		printf("TEST FAILED: upsample by integer factor\n");
		array_print_with_name(src,srcLen,"src");
		array_print_with_name(dest,destLen,"dest");
		assert(0);
	}
}

void resample_downsampleByInteger_correct() {
	unsigned int srcLen = 6;
	unsigned int destLen = 3;
	data_t src[] = {1,2,3,4,5,6};
	data_t destTruth[] = {1,3,5};
	data_t dest[destLen];
	
	array_resample(src,dest,srcLen,destLen);
	
	if (! array_equal(dest,destTruth,destLen) ) {
		printf("TEST FAILED: upsample by integer factor\n");
		array_print_with_name(src,srcLen,"src");
		array_print_with_name(dest,destLen,"dest");
		assert(0);
	}
}

void resample_downsampleByFraction_correct() {
	unsigned int srcLen = 7;
	unsigned int destLen = 3;
	data_t src[] = {1,2,3,4,5,6,7};
	data_t destTruth[] = {1,3,5};
	data_t dest[destLen];
	
	array_resample(src,dest,srcLen,destLen);
	
	if (! array_equal(dest,destTruth,destLen) ) {
		printf("TEST FAILED: upsample by integer factor\n");
		array_print_with_name(src,srcLen,"src");
		array_print_with_name(dest,destLen,"dest");
		assert(0);
	}
}

void copyReverse_evenLen_correct() {
	unsigned int len = 6;
	data_t src[] = {1,2,3,4,5,6};
	data_t dest[len];
	data_t destTruth[] = {6,5,4,3,2,1};

	array_copy_reverse(src, dest, len);
	
	if (! array_equal(dest,destTruth,len) ) {
		printf("TEST FAILED: copy reverse, even length\n");
		array_print_with_name(src,len,"src");
		array_print_with_name(dest,len,"dest");
		assert(0);
	}
}
void copyReverse_oddLen_correct() {
	unsigned int len = 7;
	data_t src[] = {1,2,3,4,5,6,7};
	data_t dest[len];
	data_t destTruth[] = {7,6,5,4,3,2,1};
	
	array_copy_reverse(src, dest, len);
	
	if (! array_equal(dest,destTruth,len) ) {
		printf("TEST FAILED: copy reverse, even length\n");
		array_print_with_name(src,len,"src");
		array_print_with_name(dest,len,"dest");
		assert(0);
	}
}

void reshape_dimsNotFactorOfLen_returnsNull() {
	unsigned int len = 7;
	data_t x[] = {1,2,3,4,5,6,7};
	unsigned int newNumDims = 2;
	
	data_t** newArrays = array_reshape(x, len, newNumDims);

	assert((newArrays == NULL && "TEST FAILED: reshape_dimsNotFactorOfLen_returnsNull()\n"));
}

void reshape_stillOneDim_unchanged() {
	unsigned int len = 7;
	data_t x[] = {1,2,3,4,5,6,7};
	unsigned int newNumDims = 1;
	
	data_t** newArrays = array_reshape(x, len, newNumDims);
	
	if (! array_equal(x,newArrays[0],len) ) {
		printf("TEST FAILED: reshape_stillOneDim_unchanged\n");
		array_print_with_name(x,len,"x");
		array_print_with_name(newArrays[0],len,"reshaped");
		assert(0);
	}
}

void reshape_twoDims_correct() {
	unsigned int len = 8;
	data_t x[] = {1,2,3,4,5,6,7,8};
	unsigned int newNumDims = 2;
	
	//answers
	unsigned int newLen = 4;
	data_t col1_truth[] = {1,3,5,7};
	data_t col2_truth[] = {2,4,6,8};
	
	//check if it worked
	data_t** newArrays = array_reshape(x, len, newNumDims);
	data_t* col1 = newArrays[0];
	data_t* col2 = newArrays[1];
	short int equal = array_equal(col1, col1_truth, newLen);
	equal = equal &&  array_equal(col2, col2_truth, newLen);
	
	if (! equal ) {
		printf("TEST FAILED: reshape_stillOneDim_unchanged\n");
		array_print_with_name(x,len,"orig");
		array_print_with_name(col1_truth,	newLen,"col1_truth");
		array_print_with_name(col1,			newLen,"col1");
		array_print_with_name(col2_truth,	newLen,"col2_truth");
		array_print_with_name(col2,			newLen,"col2");
		assert(0);
	}
}

void reshape_threeDims_correct() {
	unsigned int len = 3;
	data_t x[] = {1,2,3};
	unsigned int newNumDims = 3;
	
	//answers
	unsigned int newLen = 1;
	data_t col1_truth[] = {1};
	data_t col2_truth[] = {2};
	data_t col3_truth[] = {1};
	
	//check if it worked
	data_t** newArrays = array_reshape(x, len, newNumDims);
	data_t* col1 = newArrays[0];
	data_t* col2 = newArrays[1];
	data_t* col3 = newArrays[2];
	short int equal = array_equal(col1, col1_truth, newLen);
	equal = equal &&  array_equal(col2, col2_truth, newLen);
	equal = equal &&  array_equal(col3, col3_truth, newLen);
	
	if (! equal ) {
		printf("TEST FAILED: reshape_stillOneDim_unchanged\n");
		array_print_with_name(x,len,"orig");
		array_print_with_name(col1_truth,	newLen,"col1_truth");
		array_print_with_name(col1,			newLen,"col1");
		array_print_with_name(col2_truth,	newLen,"col2_truth");
		array_print_with_name(col2,			newLen,"col2");
		array_print_with_name(col3_truth,	newLen,"col3_truth");
		array_print_with_name(col3,			newLen,"col3");
		assert(0);
	}
}
