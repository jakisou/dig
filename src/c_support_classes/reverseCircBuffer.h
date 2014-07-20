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

typedef struct revCircBuffer {
	data_t *data;
	data_t *head;	//we could just use headIdx, but this way there's a
	idx_t headIdx;	//convenient pointer right to the end of the data
	length_t maxLen;
} revCircBuffer;

revCircBuffer* revCircBuffer_new(unsigned int log2_len);
void revCircBuffer_free(revCircBuffer *buff);
void revCircBuffer_prepend(revCircBuffer *buff, data_t val);
void revCircBuffer_prependAll(revCircBuffer *buff, data_t* dataVals, length_t len);

#ifdef __cplusplus
}
#endif

#endif
