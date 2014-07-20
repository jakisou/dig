//
//  test_revCircBuffer.c
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include "test_revCircBuffer.h"
#include "reverseCircBuffer.h"
#include "array_utils.h"
#include "type_defs.h"

void test_revCircBuffer_all() {
	printf("================================ Testing Reverse Circular Buffer\n");
	revCircBuffer_insertOneItem_headCorrect();
	revCircBuffer_prependAllNoWrap_dataCorrect();
	revCircBuffer_prependAllWrap_dataCorrect();
	printf("================================ End Reverse Circular Buffer Test: Success\n");
}

void revCircBuffer_insertOneItem_headCorrect() {
	unsigned int logLen = 3; // size = 2^3 = 8
	data_t value = 3.4;
	revCircBuffer *buff = revCircBuffer_new(logLen);
	
//	data_t *origHead = buff->head;
	idx_t origIdx = buff->headIdx;
	revCircBuffer_prepend(buff, value);
//	data_t *newHead = buff->head;
	idx_t newIdx = buff->headIdx;

	if (origIdx -1 != newIdx) {
		printf("TEST FAILED: revCircBuffer head index update failed after first element\n");
		printf("old head idx, new head idx = %d, %d", origIdx, newIdx);
		assert(0);
	}
	assert(buff->head[0] = value);
	
	revCircBuffer_free(buff);
}

void revCircBuffer_prependAllNoWrap_dataCorrect() {
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 5;
	data_t data[] = {1,2,3,4,5,6,7,8,9};
	data_t truth[dataLen];
	array_copy_reverse(data, truth, dataLen);
	revCircBuffer *buff = revCircBuffer_new(logLen);
	
	revCircBuffer_prependAll(buff, data, dataLen);
	
	data_t* buffHead = buff->head;
	data_t* buffDataStart = buffHead;
	short int same = array_equal(buffDataStart, truth, dataLen);

	if (!same) {
		printf("TEST FAILED: revCircBuffer prepending items, no wraparound\n");
		printf("buff: \t");		array_print(buffDataStart, dataLen);
		printf("data: \t");		array_print(data, dataLen);
		assert(0);
	}
	
	revCircBuffer_free(buff);
}

void revCircBuffer_prependAllWrap_dataCorrect() {
	unsigned int logLen = 2; // size = 2^2 = 4
	unsigned int dataLen = 11;
	unsigned int recentDataLen = 4; //whole length of buffer
	data_t data[] = {1,2,3,4,5,6,7,8,9,10,11};
	data_t truth[dataLen];
	array_copy_reverse(data, truth, dataLen);
	revCircBuffer *buff = revCircBuffer_new(logLen);
	
	revCircBuffer_prependAll(buff, data, dataLen);
	
	data_t* buffData = buff->head;
	short int same = array_equal(buffData, truth, recentDataLen);
	
	if (!same) {
		printf("TEST FAILED: revCircBuffer prepending items with wraparound\n");
		printf("buff: \t");		array_print(buffData, recentDataLen);
		printf("data: \t");		array_print(truth, recentDataLen);
		assert(0);
	}
	
	revCircBuffer_free(buff);
}
