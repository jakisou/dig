//
//  test_SwmlideanSearcher.h
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_test_SwmlideanSearcher_h
#define edtw_test_SwmlideanSearcher_h

#ifdef __cplusplus
extern "C" {
#endif

void test_SwmSearcher_all();

void SwmSearcher_Aggressive_ScaleFactor0_SubsequenceAtStart();
void SwmSearcher_Cautious_ScaleFactor0_SubsequenceAtStart();
void SwmSearcher_Aggressive_ScaleFactor0_SubsequenceAfterStart();
void SwmSearcher_Cautious_ScaleFactor0_SubsequenceAfterStart();

void SwmSearcher_Agressive_ScaleFactor0p5_SubsequenceAtStart();
void SwmSearcher_Agressive_ScaleFactor0p5_ShorterSubsequenceAfterStart();
void SwmSearcher_Agressive_ScaleFactor0p67_MaxLenSubsequenceAfterStart();

#ifdef __cplusplus
}
#endif
	
#endif
