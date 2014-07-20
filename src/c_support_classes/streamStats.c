//
//  streamStats.c
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>

#include "streamStats.h"

streamStats* streamStats_new(idx_t len) {
	streamStats *s	= (streamStats*) malloc(sizeof(streamStats));
	s->sumX			= (data_t*) malloc(len*sizeof(data_t));
//	s->sumX2		= (data_t*) malloc(len*sizeof(data_t));
	s->meanX		= (data_t*) malloc(len*sizeof(data_t));
	s->dx			= (data_t*) malloc(len*sizeof(data_t));
	s->Sxx			= (data_t*) malloc(len*sizeof(data_t));
	s->len = len;
	s->t = -1;
	
	//TODO make sure mallocs succeeded

	return s;
}

/** Set how long in the past we should record stats for */
void streamStats_setHistoryLength(streamStats *s, idx_t len) {
	s->len = len;
	s->sumX		= realloc(s->sumX, len*sizeof(s->sumX[0]));
//	s->sumX2	= realloc(s->sumX2,len*sizeof(s->sumX2[0]));
	s->meanX	= realloc(s->meanX, len*sizeof(s->meanX[0]));
	s->dx		= realloc(s->dx,   len*sizeof(s->dx[0]));
	s->Sxx		= realloc(s->Sxx,  len*sizeof(s->Sxx[0]));
}


/** Reset this streamStats object, as if it had just been created */
void streamStats_reset(streamStats *s) {
	s->t = -1;
}

/* Actually compute the updated Sum[X], Sum[X^2], and Sxx with the given x */
//static inline void updateStreamStats(data_t* sumX, data_t* sumX2, data_t* Sxx, data_t x, idx_t len) {
static inline void updateStreamStats(data_t* sumX, data_t* meanX, data_t* dx, data_t* Sxx, data_t x, idx_t len) {
//	data_t x_sq = x*x;
//	data_t sum_x, sum_x2;
//	data_t sum_x, dx, mean_x;
	for (idx_t i = len - 1; i > 0 ; i--) {
		
//		printf("i,sumx = %d,%.1f\n",i,sum_x);
		sumX[i] = sumX[i-1] + x;
//		sum_x2 = sumX2[i-1] + x_sq;
//		sumX2[i] = sum_x2;
//		Sxx[i] = sum_x2 - sum_x*sum_x / (i+1);
		
		dx[i] = x - meanX[i-1];
		meanX[i] = meanX[i-1] + (dx[i] / (i+1));
		Sxx[i] = Sxx[i-1] + dx[i]*(x-meanX[i]);
	}
	sumX[0]	= x;
	meanX[0] = x;
	dx[0] = x;	//TODO is this right? meanX[-1] isn't really defined...
//	sumX2[0]= x_sq;	//although shouldn't matter cuz Sxy[0] doesn't need this cuz just 0
	Sxx[0]	= 0;
}

/** Update Sum[X], Sum[X^2], and Sxx with the given x value if the time
  * supplied is later than the the StreamStats's last update time */
short int streamStats_update(streamStats *s, tick_t t, data_t x) {
	// don't update stats if already up-to-date for this time
	if (t <= s->t) {
		return kFAILURE;
	}
	
	// update time, but print a warning if we seem to have skipped time...
	if (t != s->t +1) {
		printf("WARNING: streamStats jumping from time %ld to %ld\n", s->t, t);
	}
	s->t = t;
	
//	updateStreamStats(s->sumX, s->sumX2, s->Sxx, x, s->len);
	updateStreamStats(s->sumX, s->meanX, s->dx, s->Sxx, x, s->len);
	return kSUCCESS;
}

void streamStats_free(streamStats *s) {
	free(s->sumX);
//	free(s->sumX2);
	free(s->meanX);
	free(s->dx);
	free(s->Sxx);
}