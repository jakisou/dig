//
//  circBuffer.c
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "circBuffer.h"

/** For performance, circular buffer must have length that's a power of 2; to
  * ensure that this happens, this function asks for an exponent, not a length */
circBuffer * circBuffer_new(unsigned int log2_len) {
	circBuffer *buff = (circBuffer*)malloc(sizeof(circBuffer));
	buff->maxLen = 1 << log2_len; // length = 2 * log2_len, done via bit shift
	buff->data = (data_t*)malloc(buff->maxLen * sizeof(data_t) * 2); //2x for contiguous reads
	
	//data "starts" at halfway point so there are always maxLen points before it
	buff->data += buff->maxLen;
	
	buff->fractionPadding = 0;
	
	//initially empty, so head is at -1 (so will be at 0 after first element added)
	circBuffer_reset(buff);
	
	return buff;
}

//fractionPadding is the fraction of the normal length to ensure always exists and
//is writable; the point for our purposes is letting us append the final values to
//a warping envelope with making the buffer do anything
circBuffer * circBuffer_with_end_padding_new(unsigned int log2_len, float fractionPadding) {
	circBuffer *buff = (circBuffer*)malloc(sizeof(circBuffer));
	buff->maxLen = 1 << log2_len; // length = 2 * log2_len, done via bit shift
	buff->data = (data_t*)malloc(buff->maxLen * sizeof(data_t) * (2+fractionPadding)); //2x for contiguous reads
	
	//data "starts" at halfway point so there are always maxLen points before it
	buff->data += buff->maxLen;
	
	buff->fractionPadding = fractionPadding;
	
	//initially empty, so head is at -1 (so will be at 0 after first element added)
	circBuffer_reset(buff);
	
	return buff;
}

void circBuffer_free(circBuffer *buff) {
	free(buff->data - buff->maxLen); // we incremented buff->data by maxlen at the start
	free(buff);
}

void circBuffer_reset(circBuffer *buff) {
	buff->headIdx = -1;
	buff->head = buff->data + buff->headIdx;
}

/** Resize this circular buffer's capacity; note that this function produces
  * undefined behavior when reducing the capacity below the current number
  * of entries */
void circBuffer_resize(circBuffer* buff, unsigned int log2_len) {
	unsigned int len = 1 << log2_len;
	data_t *dataStart = buff->data - buff->maxLen;
	
	if (buff->headIdx >= 0) {
		unsigned int oldFilledLen = buff->headIdx + 1;
		data_t * tmp = (data_t*)malloc(oldFilledLen*sizeof(buff->data[0]));
		memcpy(tmp, buff->data, oldFilledLen*sizeof(buff->data[0]) );
		
		buff->maxLen = len;
		buff->data = realloc(dataStart, len * sizeof(buff->data[0]) * (2 + buff->fractionPadding));
		buff->data += buff->maxLen;		// start at halfway point
		circBuffer_appendAll(buff, tmp, oldFilledLen);
		free(tmp);
	} else {
		buff->maxLen = len;
		buff->data = realloc(dataStart, len * sizeof(buff->data[0]) * (2 + buff->fractionPadding));
		buff->data += buff->maxLen;		// start at halfway point
		buff->head = buff->data + buff->headIdx;
	}
}

void circBuffer_append(circBuffer *buff, data_t val) {
	length_t bitmask = buff->maxLen - 1;
	buff->headIdx = (buff->headIdx + 1) & bitmask; //replaces % operator
	buff->head = buff->data + buff->headIdx;
	*buff->head = val;
	*(buff->head - buff->maxLen) = val; // write at idx and idx - length
}

void circBuffer_appendAll(circBuffer *buff, data_t* dataVals, length_t len) {
	//TODO could check if len > maxLen and only append the last maxLen points,
	//since others will be overwritten; probably don't bother though unless this
	//somehow proves to be a bottleneck
	for (length_t i = 0; i < len; i++) {
		circBuffer_append(buff, dataVals[i]);
	}
}
