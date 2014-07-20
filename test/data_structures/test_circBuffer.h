//
//  test_circBuffer.h
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_test_circBuffer_h
#define edtw_test_circBuffer_h

#ifdef __cplusplus
extern "C" {
#endif

void test_circBuffer_all();

void circBuffer_insertOneItem_headCorrect();
void circBuffer_appendAllNoWrap_dataCorrect();
void circBuffer_appendAllWrap_dataCorrect();
void circBuffer_resizeWrap_dataCorrect();
void circBuffer_reset_dataCorrect();
	
#ifdef __cplusplus
}
#endif

#endif
