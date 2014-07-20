//
//  test_circEnvelope.h
//  edtw
//
//  Created By <Anonymous> on 2/5/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_test_circEnvelope_h
#define edtw_test_circEnvelope_h

#ifdef __cplusplus
extern "C" {
#endif

void test_circEnvelope_all();

void circEnvelope_AddTooFewPoints_EnvelopesEmpty();
void circEnvelope_Warp0_MonotonicallyIncreasingData_EnvelopeCorrect();
void circEnvelope_Warp0_MonotonicallyDecreasingData_EnvelopeCorrect();
void circEnvelope_Warp1_MonotonicallyIncreasingData_EnvelopeCorrect();
void circEnvelope_Warp1_MonotonicallyDecreasingData_EnvelopeCorrect();
void circEnvelope_Warp2_NonMonotonicData_EnvelopeCorrect();

void circEnvelope_Resize_EnvelopeCorrect();
void circEnvelope_Reset_EnvelopeCorrect();
void circEnvelope_SetRAtStart_EnvelopeCorrect();

#ifdef __cplusplus
}
#endif

#endif
