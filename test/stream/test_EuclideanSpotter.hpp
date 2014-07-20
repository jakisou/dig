//
//  test_EuclideanSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__test_EuclideanSpotter__
#define __edtw__test_EuclideanSpotter__

void test_EucSpotter_all();

void EucSpotter_IdenticalSubsequence();
void EucSpotter_IdenticalSubsequenceInitialXOfZero();
void EucSpotter_IdenticalSubsequenceInitialQOfZero();
void EucSpotter_IdenticalSubsequenceInitialQandXOfZero();
void EucSpotter_PositiveOffset();
void EucSpotter_NegativeOffset();
void EucSpotter_AmplitudeScaling();
void EucSpotter_PositiveOffsetAndAmplitudeScaling();
void EucSpotter_NegativeOffsetAndAmplitudeScaling();
void EucSpotter_OffsetAndAmplitudeScalingMultipleMatches();

#endif /* defined(__edtw__test_EuclideanSpotter__) */
