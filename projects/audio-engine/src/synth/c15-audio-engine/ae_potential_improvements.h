#pragma once

/******************************************************************************/
/** @file       ae_potential_improvements.h
    @date       2020-04-22
    @version    1.7B-4
    @author     M. Seeber

    @brief      contains define flags enabling potential dsp improvements

                - primarily attempting to lower permanent cpu usage
                - if possible, maybe reduce some event-related cpu usage as well

                (this file is intended to only be temporary and should be deleted after everything is implemented, tested, decided and cleaned up)

    @todo
*******************************************************************************/

// two basic states (disabled, enabled) are provided (disabled refers to the current implementation)

#define __POTENTIAL_IMPROVEMENT_DISABLED__ 0
#define __POTENTIAL_IMPROVEMENT_ENABLED__ 1

// proposal declaration

#define __POTENTIAL_IMPROVEMENT_PROPOSAL__ __POTENTIAL_IMPROVEMENT_ENABLED__
#define __POTENTIAL_IMPROVEMENT_NUMERIC_TESTS__ __POTENTIAL_IMPROVEMENT_DISABLED__
// - enable or disable:
//   1) implementations of potential improvements
//   2) numeric tests of chosen implementations

// individual improvements

// COMB IMPROVEMENTS

#define POTENTIAL_IMPROVEMENT_COMB_REDUCE_VOICE_LOOP_1 __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_COMB_REDUCE_VOICE_LOOP_2 __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_COMB_REDUCE_VOICE_LOOP_3 __POTENTIAL_IMPROVEMENT_PROPOSAL__
// - three options, potentially reducing voice loops within the Comb Filter to zero

// DNC IMPROVEMENTS

#define POTENTIAL_IMPROVEMENT_DNC_CONST_OF_ZERO __POTENTIAL_IMPROVEMENT_DISABLED__
// - first option just as a proof of concept
#define POTENTIAL_IMPROVEMENT_DNC_OMIT_POLYPHONIC __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_DNC_OMIT_MONOPHONIC __POTENTIAL_IMPROVEMENT_PROPOSAL__
// - two further options potentially saving many additions

// PARALLEL DATA SPECIFIC IMPROVEMENTS

#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_ABS __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_MINMAX_FLOAT __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_STD_ROUND __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_KEEP_FRACTIONAL __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_SINP3_WRAP __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_SINP3_NOWRAP __POTENTIAL_IMPROVEMENT_PROPOSAL__
#define POTENTIAL_IMPROVEMENT_PARALLEL_DATA_THREE_RANGES __POTENTIAL_IMPROVEMENT_PROPOSAL__
// - at least in theory, all provided methods of ParallelData.h should perform SIMD compliant
