//
//  test_EdtwlideanSpotter.h
//  edtw
//
//  Created By <Anonymous> on 1/23/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef __edtw__test_DtwSearcher__
#define __edtw__test_DtwSearcher__

void test_DtwSearcher_all();

void DtwSearcher_Aggressive_SubsequenceAtStart();
void DtwSearcher_Cautious_SubsequenceAtStart();
void DtwSearcher_Aggressive_SubsequenceAfterStart();
void DtwSearcher_Cautious_SubsequenceAfterStart();
void DtwSearcher_Aggressive_MatchesGreedily();
void DtwSearcher_Cautious_MatchesOptimally();
void DtwSearcher_Aggressive_MultipleConsecutiveMatches();
void DtwSearcher_Cautious_MultipleConsecutiveMatches();
void DtwSearcher_Aggressive_MultipleMatches();
void DtwSearcher_Cautious_MultipleMatches();

void DtwSearcher_Warp3();

#endif /* defined(__edtw__test_DtwSearcher__) */
