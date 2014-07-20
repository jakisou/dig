//
//  streamStats.h
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_streamStats_h
#define edtw_streamStats_h

#include "type_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct streamStats {
	idx_t len;
	tick_t t;
	data_t *sumX;
//	data_t *sumX2;
	data_t *meanX;
	data_t *dx;
	data_t *Sxx;
} streamStats;

streamStats* streamStats_new(idx_t len);
void streamStats_reset(streamStats *s);
void streamStats_setHistoryLength(streamStats *s, idx_t len);
short int streamStats_update(streamStats *s, tick_t t, data_t x);
void streamStats_free(streamStats *s);


#ifdef __cplusplus
}
#endif

	
#endif
