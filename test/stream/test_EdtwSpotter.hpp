//
//  test_EdtwlideanSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__test_EdtwSpotter__
#define __edtw__test_EdtwSpotter__

void test_EdtwSpotter_all();

void EdtwSpotter_NoWarp_IdenticalSubsequence();
void EdtwSpotter_NoWarp_IdenticalSubsequenceInitialXOfZero();
void EdtwSpotter_NoWarp_IdenticalSubsequenceInitialQOfZero();
void EdtwSpotter_NoWarp_IdenticalSubsequenceInitialQandXOfZero();
void EdtwSpotter_NoWarp_PositiveOffset();
void EdtwSpotter_NoWarp_NegativeOffset();
void EdtwSpotter_NoWarp_AmplitudeScaling();
void EdtwSpotter_NoWarp_PositiveOffsetAndAmplitudeScaling();
void EdtwSpotter_NoWarp_NegativeOffsetAndAmplitudeScaling();
void EdtwSpotter_NoWarp_OffsetAndAmplitudeScalingMultipleMatches();

void EdtwSpotter_Warp1_IdenticalSubsequence();
void EdtwSpotter_Warp1_IdenticalSubsequenceInitialXOfZero();
void EdtwSpotter_Warp1_IdenticalSubsequenceInitialQOfZero();
void EdtwSpotter_Warp1_IdenticalSubsequenceInitialQandXOfZero();
void EdtwSpotter_Warp1_PositiveOffset();
void EdtwSpotter_Warp1_NegativeOffset();
void EdtwSpotter_Warp1_AmplitudeScaling();
void EdtwSpotter_Warp1_PositiveOffsetAndAmplitudeScaling();
void EdtwSpotter_Warp1_NegativeOffsetAndAmplitudeScaling();
void EdtwSpotter_Warp1_OffsetAndAmplitudeScalingMultipleMatches();

void EdtwSpotter_Warp2_IdenticalSubsequence();
void EdtwSpotter_Warp2_IdenticalSubsequenceInitialXOfZero();
void EdtwSpotter_Warp2_IdenticalSubsequenceInitialQOfZero();
void EdtwSpotter_Warp2_IdenticalSubsequenceInitialQandXOfZero();
void EdtwSpotter_Warp2_PositiveOffset();
void EdtwSpotter_Warp2_NegativeOffset();
void EdtwSpotter_Warp2_AmplitudeScaling();
void EdtwSpotter_Warp2_PositiveOffsetAndAmplitudeScaling();
void EdtwSpotter_Warp2_NegativeOffsetAndAmplitudeScaling();
void EdtwSpotter_Warp2_OffsetAndAmplitudeScalingMultipleMatches();


void EdtwSpotter_Warp1_HorzInherit();
void EdtwSpotter_Warp2_HorzInherit();
void EdtwSpotter_Warp3_HorzInherit();
void EdtwSpotter_Warp0_RestrictedHorzInherit();
void EdtwSpotter_Warp1_RestrictedHorzInherit();

void EdtwSpotter_Warp1_VertInherit();

#endif /* defined(__edtw__test_EdtwlideanSpotter__) */
