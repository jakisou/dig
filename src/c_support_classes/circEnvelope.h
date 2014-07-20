//
//  circularEnvelope.h
//  edtw
//
//  Created By <Anonymous> on 2/4/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_circEnvelope_h
#define edtw_circEnvelope_h

#include "type_defs.h"
#include "circBuffer.h"
#include "deque.h"

static const float kMAX_WARPING_WIDTH_R = .4;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct circEnvelope {
	circBuffer *recentData;
	circBuffer *u;
	circBuffer *l;
//	deque *du;
//	deque *dl;
	tick_t time;		// note that this is the tick up to which it's been
	length_t maxLen;	// updated, not the length of the envelope
	steps_t r;
} circEnvelope;


circEnvelope *circEnvelope_new(unsigned int log2len, steps_t r);
void circEnvelope_free(circEnvelope *env);
void circEnvelope_reset(circEnvelope *env);
void circEnvelope_resize(circEnvelope *env, unsigned int log2len);
void circEnvelope_setR(circEnvelope *end, steps_t r);
void circEnvelope_append(circEnvelope* env, data_t x);
void circEnvelope_appendAll(circEnvelope* env, const data_t *ar, length_t len);

#ifdef __cplusplus
}
#endif

#endif
