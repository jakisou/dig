//
//  global_dist_calls.h
//  edtw
//
//  Created By <Anonymous> on 2/11/14.
//  Copyright (c) 2014 University of <Anonymous>. All rights reserved.
//

#ifndef edtw_global_dist_calls_h
#define edtw_global_dist_calls_h

#include "type_defs.h"

extern dist_calls_t global_dist_calls;	///< The number of times that the innermost distance subroutine has been called

extern double gAbandonAbovePruningPower;	///< The pruning power above which inefficient searches can be abandoned

//extern long optimal_dist_time;
//extern long inherit_time;
//extern long shift_time;
//extern long invalid_time;

#endif
