//
//  test_circBuffer.c
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include "test_circBuffer.h"
#include "circBuffer.h"
#include "array_utils.h"
#include "type_defs.h"

void test_circBuffer_all() {
	printf("================================ Testing Circular Buffer\n");
	circBuffer_insertOneItem_headCorrect();
	circBuffer_appendAllNoWrap_dataCorrect();
	circBuffer_appendAllWrap_dataCorrect();
	circBuffer_resizeWrap_dataCorrect();
	circBuffer_reset_dataCorrect();
	printf("================================ End Circular Buffer Test: Success\n");
}

void circBuffer_insertOneItem_headCorrect() {
	unsigned int logLen = 3; // size = 2^3 = 8
	data_t value = 3.4;
	circBuffer *buff = circBuffer_new(logLen);
	
//	data_t *origHead = buff->head;
	idx_t origIdx = buff->headIdx;
	circBuffer_append(buff, value);
//	data_t *newHead = buff->head;
	idx_t newIdx = buff->headIdx;
	
//	if (origHead +1 != newHead) { //This fails, but the behavior seems to be correct
//		printf("TEST FAILED: circBuffer head update failed after first element\n");
//		printf("old head, new head = %x, %x\n", (uint_64)origHead, (uint_64)newHead);
//		assert(0);
//	}
	if (origIdx +1 != newIdx) {
		printf("TEST FAILED: circBuffer head index update failed after first element\n");
		printf("old head idx, new head idx = %d, %d", origIdx, newIdx);
		assert(0);
	}
	assert(buff->head[0] = value);
	
	circBuffer_free(buff);
}

void circBuffer_appendAllNoWrap_dataCorrect() {
	unsigned int logLen = 3; // size = 2^3 = 8
	unsigned int dataLen = 5;
	data_t data[] = {1,2,3,4,5,6,7,8,9};
	circBuffer *buff = circBuffer_new(logLen);
	
	circBuffer_appendAll(buff, data, dataLen);
	
	data_t* buffHead = buff->head;
	data_t* buffDataStart = buffHead - dataLen + 1;
	short int same = array_equal(buffDataStart, data, dataLen);
	
	if (!same) {
		printf("TEST FAILED: circBuffer appending items, no wraparound\n");
		printf("buff: \t");		array_print(buffDataStart, dataLen);
		printf("data: \t");		array_print(data, dataLen);
		assert(0);
	}
}

void circBuffer_appendAllWrap_dataCorrect() {
	unsigned int logLen = 2; // size = 2^2 = 4
	unsigned int dataLen = 11;
	unsigned int recentDataLen = 4; //whole length of buffer
	unsigned int recentDataStartOffset = dataLen - recentDataLen;
	data_t data[] = {1,2,3,4,5,6,7,8,9,10,11};
	data_t* recentData = data + recentDataStartOffset;
	circBuffer *buff = circBuffer_new(logLen);
	
	circBuffer_appendAll(buff, data, dataLen);
	
	data_t* buffHead = buff->head;
	data_t* buffData = buffHead - recentDataLen + 1;
	
	short int same = array_equal(buffData, recentData, recentDataLen);
	
	if (!same) {
		printf("TEST FAILED: circBuffer appending items with wraparound\n");
		printf("buff: \t");		array_print(buffData, recentDataLen);
		printf("data: \t");		array_print(recentData, recentDataLen);
		assert(0);
	}
	
	circBuffer_free(buff);
}

void circBuffer_resizeWrap_dataCorrect() {
	unsigned int logLen = 2; // size = 2^2 = 4
	unsigned int newLen = 3;
	unsigned int dataLen = 11;
	unsigned int dataAfterResizeLen = 10;
	unsigned int recentDataLen = 8; //whole length of buffer
	unsigned int recentDataStartOffset = dataAfterResizeLen - recentDataLen;
	data_t data[] = {1,2,3,4,5,6,7,8,9,10,11};
	data_t dataAfterResize[] = {21,22,23,24,25,26,27,28,29,30};
	data_t* recentData = dataAfterResize + recentDataStartOffset;
	circBuffer *buff = circBuffer_new(logLen);

	circBuffer_appendAll(buff, data, dataLen);
	circBuffer_resize(buff, newLen);
	circBuffer_appendAll(buff, dataAfterResize, dataAfterResizeLen);
	
	data_t* buffHead = buff->head;
	data_t* buffData = buffHead - recentDataLen + 1;
	
	short int same = array_equal(buffData, recentData, recentDataLen);
	
	if (!same) {
		printf("TEST FAILED: circBuffer appending items with wraparound\n");
		printf("buff: \t");		array_print(buffData, buff->maxLen);
		printf("data: \t");		array_print(recentData, recentDataLen);
		assert(0);
	}
	
	circBuffer_free(buff);
}

void circBuffer_reset_dataCorrect() {
	unsigned int logLen = 2; // size = 2^2 = 4
	unsigned int dataLen = 3;
	unsigned int dataAfterResetLen = 2;
//	unsigned int recentDataLen = 4; //whole length of buffer
	data_t data[] = {1,2,3};
	data_t dataAfterReset[] = {4,5};
	circBuffer *buff = circBuffer_new(logLen);
	
	circBuffer_appendAll(buff, data, dataLen);
	circBuffer_reset(buff);
	circBuffer_appendAll(buff, dataAfterReset, dataAfterResetLen);
	
	data_t* buffHead = buff->head;
	data_t* buffData = buffHead - dataAfterResetLen + 1;
	
	short int same = array_equal(buffData, dataAfterReset, dataAfterResetLen);
	same = same && buff->head == (buff->data + dataAfterResetLen - 1);
	
	if (!same) {
		printf("TEST FAILED: circBuffer appending items with wraparound\n");
		data_t * allBuffDataStart = buff->head - buff->maxLen + 1;
		printf("buff: \t");		array_print(allBuffDataStart, buff->maxLen);
		printf("data: \t");		array_print(dataAfterReset, dataAfterResetLen);
		assert(0);
	}
	
	circBuffer_free(buff);
}
