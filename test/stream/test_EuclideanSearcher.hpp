//
//  test_EuclideanSearcher.h
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__test_EuclideanSearcher__
#define __edtw__test_EuclideanSearcher__

void test_EucSearcher_all();

void EucSearcher_Aggressive_SubsequenceAtStart();
void EucSearcher_Cautious_SubsequenceAtStart();
void EucSearcher_Aggressive_SubsequenceAfterStart();
void EucSearcher_Cautious_SubsequenceAfterStart();
void EucSearcher_Aggressive_MatchesGreedily();
void EucSearcher_Cautious_MatchesOptimally();
void EucSearcher_Aggressive_MultipleConsecutiveMatches();
void EucSearcher_Cautious_MultipleConsecutiveMatches();
void EucSearcher_Aggressive_MultipleMatches();
void EucSearcher_Cautious_MultipleMatches();

//void EucSearcher_Subsequence();
//void EucSearcher_SubsequenceInitialXOfZero();
//void EucSearcher_SubsequenceInitialQOfZero();
//void EucSearcher_SubsequenceInitialQandXOfZero();
//void EucSearcher_PositiveOffset();
//void EucSearcher_NegativeOffset();
//void EucSearcher_AmplitudeScaling();
//void EucSearcher_PositiveOffsetAndAmplitudeScaling();
//void EucSearcher_NegativeOffsetAndAmplitudeScaling();
//void EucSearcher_OffsetAndAmplitudeScalingMultipleMatches();

#endif /* defined(__edtw__test_EuclideanSearcher__) */
