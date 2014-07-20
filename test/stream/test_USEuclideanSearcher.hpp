//
//  test_USEuclideanSearcher.h
//  edtw
//
//  Created By <Anonymous> on 2/8/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_test_USEuclideanSearcher_h
#define edtw_test_USEuclideanSearcher_h

#ifdef __cplusplus
extern "C" {
#endif

void test_USEucSearcher_all();

void USEucSearcher_Aggressive_ScaleFactor0_SubsequenceAtStart();
void USEucSearcher_Cautious_ScaleFactor0_SubsequenceAtStart();
void USEucSearcher_Aggressive_ScaleFactor0_SubsequenceAfterStart();
void USEucSearcher_Cautious_ScaleFactor0_SubsequenceAfterStart();

void USEucSearcher_Agressive_ScaleFactor0p5_SubsequenceAtStart();
void USEucSearcher_Agressive_ScaleFactor0p5_ShorterSubsequenceAfterStart();
void USEucSearcher_Agressive_ScaleFactor0p67_MaxLenSubsequenceAfterStart();

#ifdef __cplusplus
}
#endif
	
#endif
