//
//  circBuffer.c
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include "reverseCircBuffer.h"

/** For performance, circular buffer must have length that's a power of 2; to
  * ensure that this happens, this function asks for an exponent, not a length */
revCircBuffer * revCircBuffer_new(unsigned int log2_len) {
	revCircBuffer *buff = (revCircBuffer*)malloc(sizeof(revCircBuffer));
	buff->maxLen = 1 << log2_len; // length = 2 * log2_len, done via bit shift
	buff->data = (data_t*)malloc(buff->maxLen * sizeof(data_t) * 2); //2x for contiguous reads
	
	//initially empty, so head is at end+1 (so will be at end after first element added)
	buff->headIdx = buff->maxLen;
	buff->head = buff->data + buff->headIdx;
	
	return buff;
}

void revCircBuffer_free(revCircBuffer *buff) {
	free(buff->data); // we incremented buff->data by maxlen at the start
	free(buff);
}

void revCircBuffer_prepend(revCircBuffer *buff, data_t val) {
	length_t bitmask = buff->maxLen - 1;
	buff->headIdx = (buff->headIdx - 1) & bitmask; //replaces % operator
	buff->head = buff->data + buff->headIdx;
	*buff->head = val;
	*(buff->head + buff->maxLen) = val; // write at idx and idx + length
}

void revCircBuffer_prependAll(revCircBuffer *buff, data_t* dataVals, length_t len) {
	//TODO could check if len > maxLen and only append the last maxLen points,
	//since others will be overwritten; probably don't bother though unless this
	//somehow proves to be a bottleneck
	for (length_t i = 0; i < len; i++) {
		revCircBuffer_prepend(buff, dataVals[i]);
	}
}
