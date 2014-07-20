//
//  filePaths.hpp
//

#ifndef edtw_filePaths_hpp
#define edtw_filePaths_hpp

#include <string>

#define DDW_DIR "PATH/TO/DDW/PROJECT"			//modify this

// concatenation macros
#define STRCAT(a,b) a b

#define TIMING_DIR "timing/"
#define BEST_PARAMS_DIR "bestParams/"
#define OUTPUT_DIR "output/"
#define RESULTS_DIR "results/"
#define DEFAULT_OUTPUT_DIR STRCAT(DDW_DIR,OUTPUT_DIR)	 //store subseqs found
#define DEFAULT_RESULTS_DIR STRCAT(DDW_DIR,RESULTS_DIR)	 //store results

extern const std::string kUCR_COMBINED_RESULTS_DIR;
extern const std::string kMASKED_SINES_RESULTS_DIR;
extern const std::string kCBF_RESULTS_DIR;

extern const std::string kBEST_PARAMS_DIR;
extern const std::string kTIMING_DIR;
extern const std::string kUCR_TRAIN_DIR;
extern const std::string kUCR_TESTS_DIR;
extern const std::string kMASKED_SINES_DIR;
extern const std::string kMOTIF_DEMO_DIR;
extern const std::string kECG_DIR;
extern const std::string kRANDOM_WALK_DIR;
extern const std::string kCBF_DIR;

#endif
