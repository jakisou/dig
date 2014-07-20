//
//  test_revCircBuffer.h
//  edtw
//
//  Created By <Anonymous> on 1/14/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_test_revCircBuffer_h
#define edtw_test_revCircBuffer_h

#ifdef __cplusplus
extern "C" {
#endif

void test_revCircBuffer_all();

void revCircBuffer_insertOneItem_headCorrect();
void revCircBuffer_prependAllNoWrap_dataCorrect();
void revCircBuffer_prependAllWrap_dataCorrect();

	
#ifdef __cplusplus
}
#endif

#endif
