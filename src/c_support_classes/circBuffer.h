//
//  circBuffer.h
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_circBuffer_h
#define edtw_circBuffer_h

#include <stdlib.h>
#include "type_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct circBuffer {
	data_t *data;
	data_t *head;	//we could just use headIdx, but this way there's a
	idx_t headIdx;	//convenient pointer right to the end of the data
	length_t maxLen;
	float fractionPadding;
} circBuffer;

circBuffer* circBuffer_new(unsigned int log2_len);
circBuffer * circBuffer_with_end_padding_new(unsigned int log2_len, float fractionPadding);
void circBuffer_free(circBuffer *buff);
void circBuffer_reset(circBuffer *buff);
void circBuffer_resize(circBuffer* buff, unsigned int log2_len);
void circBuffer_append(circBuffer *buff, data_t val);
void circBuffer_appendAll(circBuffer *buff, data_t* dataVals, length_t len);

#ifdef __cplusplus
}
#endif

#endif
