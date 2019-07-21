/* configuration of the test project */

#pragma once

#include <stdint.h>
#include "pe_defines_config.h"
#include "pe_defines_labels.h"

/*
 * Test 3 requires a reMote keyboard with the correct template (64: MSE 2) -> see Linuy Engine - Test 3 for details on handling
 */

/* Preset Definition */

#define testRecallSequences 8
#define testMidiModes 2

const int32_t testPresetData[testRecallSequences][lst_recall_length] = {
  /* preset 0 - default */
  {
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env A
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env B
      0,    9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0,    800,   0,     0,                    // Env C
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc A
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp A
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc B
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp B
      0,    6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,    10560, 10000, 16000, 0,   0,    0,  // Comb
      0,    0,     6000,  16000, 0,     1200,  0,     8000,  0,     0,    0,     0,     0,                    // SVF
      0,    0,     0,     8000,  0,     8000,  0,     0,     6080,                                            // FB Mix
      0,    0,     0,     0,     0,     0,     0,     0,     0,     8000, 0,     6080,  0,                    // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                      // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                               // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,  // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                    // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                           // Reverb
      8000, 0,                                                                           // Master
      1,    0,     0,     0                                                              // Unison
  },
  /* preset 1 - default tone */
  {
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env A
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env B
      0,    9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0,    800,   0,     0,                    // Env C
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc A
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp A
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc B
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp B
      0,    6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,    10560, 10000, 16000, 0,   0,    0,  // Comb
      0,    0,     6000,  16000, 0,     1200,  0,     8000,  0,     0,    0,     0,     0,                    // SVF
      0,    0,     0,     8000,  0,     8000,  0,     0,     6080,                                            // FB Mix
      2000, 0,     0,     0,     0,     0,     0,     0,     0,     8000, 0,     6080,  0,                    // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                      // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                               // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,  // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                    // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                           // Reverb
      8000, 0,                                                                           // Master
      1,    0,     0,     0                                                              // Unison
  },
  /* preset 2 - testing comb filter */
  {
      0,    0,     8000,  8000,  0,     0,     0,     0,     0,     0,    0,     0,     0,     0, 0,    0,  // Env A
      0,    0,     8000,  8000,  0,     0,     0,     0,     0,     0,    0,     0,     0,     0, 0,    0,  // Env B
      0,    0,     0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,                  // Env C
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0, 8000,     // Osc A
      0,    0,     0,     0,     0,     0,     0,     0,                                                    // Shp A
      9200, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0, 8000,     // Osc B
      0,    0,     0,     0,     0,     0,     0,     0,                                                    // Shp B
      0,    6000,  10000, 0,     0,     0,     0,     14000, 16000, 0,    10560, 10000, 16000, 0, 0,    0,  // Comb
      0,    0,     6000,  16000, 0,     1200,  0,     8000,  0,     0,    0,     0,     0,                  // SVF
      0,    0,     0,     8000,  0,     8000,  0,     0,     6080,                                          // FB Mix
      0,    0,     0,     0,     2000,  0,     0,     0,     0,     8000, 0,     6080,  0,                  // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                    // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                               // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,  // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                    // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                           // Reverb
      8000, 0,                                                                           // Master
      1,    0,     0,     0                                                              // Unison
  },
  /* preset 3 - testing state variable filter */
  {
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env A
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env B
      0,    9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0,    800,   0,     0,                    // Env C
      8000, 10000, 0,     0,     0,     8000,  16000, 0,     0,     0,    0,     0,     0,     0,   8000,     // Osc A
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp A
      9200, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc B
      4000, 0,     8000,  0,     8000,  0,     0,     0,                                                      // Shp B
      0,    6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,    10560, 10000, 16000, 0,   0,    0,  // Comb
      0,    0,     6000,  16000, 0,     1200,  0,     8000,  0,     0,    0,     0,     0,                    // SVF
      0,    0,     0,     8000,  0,     8000,  0,     0,     6080,                                            // FB Mix
      0,    0,     0,     0,     0,     0,     2000,  0,     0,     8000, 0,     6080,  0,                    // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                      // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                               // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,  // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                    // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                           // Reverb
      8000, 0,                                                                           // Master
      1,    0,     0,     0                                                              // Unison
  },
  /* preset 4 - testing envelope a/b split */
  {
      0,    9440,  8000,  12640, 0,     8480,  0,     0,     0,     0,    0,     0,     0,     800, 0,    0,  // Env A
      0,    9440,  8000,  12640, 0,     8480,  0,     0,     0,     0,    0,     0,     0,     800, 0,    0,  // Env B
      0,    9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0,    800,   0,     0,                    // Env C
      8000, 10000, 0,     0,     0,     8000,  16000, 0,     0,     0,    0,     0,     0,     0,   8000,     // Osc A
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp A
      9200, 10000, 0,     0,     0,     8000,  16000, 0,     0,     0,    0,     0,     0,     0,   8000,     // Osc B
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp B
      0,    6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,    10560, 10000, 16000, 0,   0,    0,  // Comb
      0,    0,     6000,  16000, 0,     1200,  0,     8000,  0,     0,    0,     0,     0,                    // SVF
      0,    0,     0,     8000,  0,     8000,  0,     0,     6080,                                            // FB Mix
      2000, 0,     2000,  0,     0,     0,     0,     0,     0,     8000, 0,     6080,  0,                    // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                      // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                               // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,  // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                    // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                           // Reverb
      8000, 0,                                                                           // Master
      1,    0,     0,     0                                                              // Unison
  },
  /* preset 5 - testing envelope c retrigger (use one voice polyphony for testing) */
  {
      0,     9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env A
      0,     9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env B
      12000, 9440,  8000,  12640, 4000,  14000, 0,     0,     0,     0,    800,   0,     0,                    // Env C
      8000,  10000, 4000,  0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc A
      2000,  0,     8000,  0,     0,     0,     0,     0,                                                      // Shp A
      8000,  10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc B
      2000,  0,     8000,  0,     0,     0,     0,     0,                                                      // Shp B
      0,     6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,    10560, 10000, 16000, 0,   0,    0,  // Comb
      0,     0,     6000,  16000, 0,     1200,  0,     8000,  0,     0,    0,     0,     0,                    // SVF
      0,     0,     0,     8000,  0,     8000,  0,     0,     6080,                                            // FB Mix
      2000,  0,     0,     0,     0,     0,     0,     0,     0,     8000, 0,     6080,  0,  // Out Mix
      4000,  4000,  4000,  -1600, 10000, 2000,  7200,  0,                                    // Cabinet
      4800,  0,     1200,  8000,  0,     0,                                                  // Gap Filter
      0,     7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,     // Flanger
      6928,  0,     8000,  8000,  12000, 0,     16000,                                       // Echo
      5280,  5280,  8000,  4000,  0,     16000,                                              // Reverb
      8000,  0,                                                                              // Master
      1,     0,     0,     0                                                                 // Unison
  },
  /* preset 6 - SVF NaN issue */
  {
      0,    9440,  8000,  12640, 0,     8480,  0,     0,     0,     0,    0,     0,     0,     800, 0,    0,  // Env A
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,    0,     0,     0,     800, 0,    0,  // Env B
      0,    9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0,    800,   0,     0,                    // Env C
      8000, 10000, 0,     0,     0,     8000,  16000, 0,     0,     0,    0,     0,     0,     0,   8000,     // Osc A
      2000, 0,     8000,  0,     0,     0,     0,     0,                                                      // Shp A
      9200, 10000, 0,     0,     0,     0,     0,     0,     0,     0,    0,     0,     0,     0,   8000,     // Osc B
      4000, 0,     8000,  0,     8000,  0,     0,     0,                                                      // Shp B
      0,    6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,    10560, 10000, 16000, 0,   0,    0,  // Comb
      0,    0,     12000, 16000, 8000,  -6000, 8000,  16000, 0,     0,    0,     0,     0,                    // SVF
      0,    0,     0,     8000,  0,     8000,  0,     0,     6080,                                            // FB Mix
      0,    0,     0,     0,     0,     0,     2000,  0,     0,     8000, 0,     6080,  0,                    // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                      // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                               // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,    0,     14000,  // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                    // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                           // Reverb
      8000, 0,                                                                           // Master
      1,    0,     0,     0                                                              // Unison
  },
#if 0
    /* preset 7 - SVF NAN (handmade copy of "Linux Engine NAN" Bank) */
    {
        0,     9440,  8000,  12640, 0,     8560,  0,     7680,  0,     0,     0,     0,     0,     800,   0,     0,     // Env A
        0,     9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0,     0,     0,     0,     800,   0,     0,     // Env B
        0,     9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0,     800,   0,     0,                          // Env C
        8000,  10000, 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     8000,         // Osc A
        2000,  0,     8000,  0,     8000,  0,     0,     0,                                                             // Shp A
        8000,  10000, 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     8000,         // Osc B
        2000,  0,     8000,  0,     0,     0,     0,     0,                                                             // Shp B
        0,     6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0,     10560, 10000, 16000, 0,     0,     0,     // Comb
        0,     0,     6400,  16000, 0,     4900,  -8000, 12800, 0,     0,     4640,  -2560, 16000,                      // SVF
        0,     0,     0,     8000,  0,     8000,  0,     0,     6080,                                                   // FB Mix
        0,     0,     0,     0,     0,     0,     4320,  0,     0,     8000,  0,     6080,  0,                          // Out Mix
        4000,  4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                             // Cabinet
        4800,  0,     1200,  8000,  0,     0,                                                                           // Gap Filter
        0,     7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0,     0,     14000,                             // Flanger
        6928,  0,     8000,  8000,  12000, 0,     16000,                                                                // Echo
        5280,  5280,  8000,  4000,  0,     16000,                                                                       // Reverb
        8000,  0,                                                                                                       // Master
        1,     0,     0,     0                                                                                          // Unison
    },
#endif
  /* preset 7 - feedback mixer asym issue */
  {
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0, 0,     0,     0,     800, 0,    0,  // Env A
      0,    9440,  8000,  12640, 0,     8480,  0,     7680,  0,     0, 0,     0,     0,     800, 0,    0,  // Env B
      0,    9440,  8000,  12640, 0,     8480,  7680,  0,     0,     0, 800,   0,     0,                    // Env C
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0, 0,     0,     0,     0,   8000,     // Osc A
      0,    0,     0,     0,     0,     0,     0,     0,                                                   // Shp A
      8000, 10000, 0,     0,     0,     0,     0,     0,     0,     0, 0,     0,     0,     0,   8000,     // Osc B
      0,    0,     0,     0,     0,     16000, 0,     0,                                                   // Shp B
      0,    6000,  10000, 0,     0,     5280,  0,     14000, 16000, 0, 10560, 10000, 16000, 0,   0,    0,  // Comb
      0,    0,     6000,  16000, 0,     1200,  0,     8000,  0,     0, 0,     0,     0,                    // SVF
      4000, 0,     0,     8000,  0,     0,     0,     0,     6080,                                         // FB Mix
      0,    0,     4000,  0,     0,     0,     0,     0,     0,     0, 0,     6080,  0,                    // Out Mix
      4000, 4000,  4000,  -1600, 10000, 2000,  7200,  0,                                                   // Cabinet
      4800, 0,     1200,  8000,  0,     0,                                                                 // Gap Filter
      0,    7200,  5072,  3963,  0,     0,     4000,  12000, 0,     0, 0,     14000,                       // Flanger
      6928, 0,     8000,  8000,  12000, 0,     16000,                                                      // Echo
      5280, 5280,  8000,  4000,  0,     16000,                                                             // Reverb
      8000, 0,                                                                                             // Master
      1,    0,     0,     0                                                                                // Unison
  },
};

const uint32_t testMidiMapping[testMidiModes][128] = {
  /* - - - - - - - Mode 1 - Group Select - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  {
      /*  0      1      2      3      4      5      6      7      8      9     */
      0,  33, 34, 35, 36, 37, 38, 39, 40, 0,  0,  41, 42, 43, 44, 45, 46, 47, 48, 0,  0,  49, 50, 51, 52, 53,
      54, 55, 56, 0,  0,  1,  2,  3,  4,  5,  0,  0,  0,  0,  0,  65, 66, 67, 68, 69, 70, 71, 72, 0,  0,  73,
      74, 75, 76, 77, 78, 79, 80, 0,  0,  81, 82, 83, 84, 85, 86, 87, 88, 0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  },
  /* - - - - - - - Mode 2 - Global Stuff - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  {
      /*  0      1      2      3      4      5      6      7      8      9     */
      0, 6, 18, 19, 20, 0, 0, 0, 0, 0,  0,  7,  16, 17, 0, 0, 0, 0, 0, 0,  0, 8, 9, 10, 11, 12, 13, 14, 15, 0, 0, 1,
      2, 3, 4,  5,  0,  0, 0, 0, 0, 65, 67, 68, 0,  0,  0, 0, 0, 0, 0, 66, 0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0, 0,
      0, 0, 0,  0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0, 0,
      0, 0, 0,  0,  0,  0, 0, 0, 0, 0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0,  0, 0, 0, 0,  0,  0,  0,  0,  0,  0, 0, 0,
  },
};

const Parameters testParamRouting[25][24] = {
  /* IGNORE group */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* ENV A */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_EA_ATT,  Parameters::P_EA_DEC1, Parameters::P_EA_BP,   Parameters::P_EA_DEC2,
      Parameters::P_EA_SUS,  Parameters::P_EA_REL,  Parameters::P_EA_GAIN, Parameters::P_EA_LV,
      Parameters::P_EA_AV,   Parameters::P_EA_AC,   Parameters::P_EA_D1V,  Parameters::P_EA_D2V,
      Parameters::P_EA_SPL,  Parameters::P_EA_RV,   Parameters::P_EA_LKT,  Parameters::P_EA_TKT,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* ENV B */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_EB_ATT,  Parameters::P_EB_DEC1, Parameters::P_EB_BP,   Parameters::P_EB_DEC2,
      Parameters::P_EB_SUS,  Parameters::P_EB_REL,  Parameters::P_EB_GAIN, Parameters::P_EB_LV,
      Parameters::P_EB_AV,   Parameters::P_EB_AC,   Parameters::P_EB_D1V,  Parameters::P_EB_D2V,
      Parameters::P_EB_SPL,  Parameters::P_EB_RV,   Parameters::P_EB_LKT,  Parameters::P_EB_TKT,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* ENV C */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_EC_ATT,  Parameters::P_EC_DEC1, Parameters::P_EC_BP,   Parameters::P_EC_DEC2,
      Parameters::P_EC_SUS,  Parameters::P_EC_REL,  Parameters::P_INVALID, Parameters::P_EC_LV,
      Parameters::P_EC_AV,   Parameters::P_EC_AC,   Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_EC_RH,   Parameters::P_EC_RV,   Parameters::P_EC_LKT,  Parameters::P_EC_TKT,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* OSC A */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_OA_P,     Parameters::P_OA_PKT,  Parameters::P_OA_PEC,   Parameters::P_OA_F,
      Parameters::P_OA_FEC,   Parameters::P_OA_PMS,  Parameters::P_OA_PMSEA, Parameters::P_OA_PMSSH,
      Parameters::P_INVALID,  Parameters::P_OA_PHS,  Parameters::P_OA_CHI,   Parameters::P_OA_PMF,
      Parameters::P_OA_PMFEC, Parameters::P_OA_PMB,  Parameters::P_OA_PMBEB, Parameters::P_OA_PMBSH,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID,
  },
  /* SHP A */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_SA_DRV,  Parameters::P_SA_FLD,  Parameters::P_SA_ASM,  Parameters::P_SA_MIX,
      Parameters::P_SA_FBM,  Parameters::P_SA_RM,   Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_SA_DEA,  Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_SA_FBEC, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* OSC B */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_OB_P,     Parameters::P_OB_PKT,  Parameters::P_OB_PEC,   Parameters::P_OB_F,
      Parameters::P_OB_FEC,   Parameters::P_OB_PMS,  Parameters::P_OB_PMSEB, Parameters::P_OB_PMSSH,
      Parameters::P_INVALID,  Parameters::P_OB_PHS,  Parameters::P_OB_CHI,   Parameters::P_OB_PMF,
      Parameters::P_OB_PMFEC, Parameters::P_OB_PMA,  Parameters::P_OB_PMAEA, Parameters::P_OB_PMASH,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID,
  },
  /* SHP B */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_SB_DRV,  Parameters::P_SB_FLD,  Parameters::P_SB_ASM,  Parameters::P_SB_MIX,
      Parameters::P_SB_FBM,  Parameters::P_SB_RM,   Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_SB_DEB,  Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_SB_FBEC, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* OUT MIX */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_OM_AL,   Parameters::P_OM_BL,   Parameters::P_OM_CL,   Parameters::P_OM_SL,
      Parameters::P_OM_LVL,  Parameters::P_OM_DRV,  Parameters::P_OM_FLD,  Parameters::P_OM_ASM,
      Parameters::P_OM_AP,   Parameters::P_OM_BP,   Parameters::P_OM_CP,   Parameters::P_OM_SP,
      Parameters::P_OM_KP,   Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* COMB FILTER */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_CMB_AB,  Parameters::P_CMB_P,    Parameters::P_CMB_PKT,  Parameters::P_CMB_PEC,
      Parameters::P_CMB_D,   Parameters::P_CMB_DKT,  Parameters::P_CMB_DG,   Parameters::P_CMB_PM,
      Parameters::P_CMB_APT, Parameters::P_CMB_APKT, Parameters::P_CMB_APEC, Parameters::P_CMB_APR,
      Parameters::P_CMB_LP,  Parameters::P_CMB_LPKT, Parameters::P_CMB_LPEC, Parameters::P_CMB_PMAB,
      Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID,  Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID,  Parameters::P_INVALID,
  },
  /* STATE VARIABLE FILTER */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_SVF_AB,  Parameters::P_SVF_CUT, Parameters::P_SVF_CKT, Parameters::P_SVF_CEC,
      Parameters::P_INVALID, Parameters::P_SVF_SPR, Parameters::P_SVF_LBH, Parameters::P_SVF_FM,
      Parameters::P_SVF_CMB, Parameters::P_SVF_RES, Parameters::P_SVF_RKT, Parameters::P_SVF_REC,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_SVF_PAR, Parameters::P_SVF_FMAB,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* CABINET,     GAP FILTER */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_CAB_DRV, Parameters::P_CAB_FLD, Parameters::P_CAB_ASM, Parameters::P_CAB_TILT,
      Parameters::P_CAB_LPF, Parameters::P_CAB_HPF, Parameters::P_CAB_LVL, Parameters::P_CAB_MIX,
      Parameters::P_GAP_CNT, Parameters::P_GAP_STE, Parameters::P_GAP_GAP, Parameters::P_GAP_RES,
      Parameters::P_GAP_BAL, Parameters::P_GAP_MIX, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* FLANGER */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_FLA_TMOD, Parameters::P_FLA_PHS, Parameters::P_FLA_RTE, Parameters::P_FLA_TIME,
      Parameters::P_FLA_STE,  Parameters::P_FLA_FB,  Parameters::P_FLA_CFB, Parameters::P_FLA_MIX,
      Parameters::P_FLA_ENV,  Parameters::P_FLA_APM, Parameters::P_FLA_APT, Parameters::P_FLA_LPF,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* ECHO,     REVERB */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_DLY_SND, Parameters::P_DLY_TIME, Parameters::P_DLY_STE, Parameters::P_DLY_FB,
      Parameters::P_DLY_CFB, Parameters::P_DLY_LPF,  Parameters::P_DLY_MIX, Parameters::P_INVALID,
      Parameters::P_REV_SND, Parameters::P_REV_SIZE, Parameters::P_REV_PRE, Parameters::P_REV_COL,
      Parameters::P_REV_CHO, Parameters::P_REV_MIX,  Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID,  Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* FEEDBACK MIX */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_FBM_CMB, Parameters::P_FBM_SVF, Parameters::P_FBM_FX,  Parameters::P_FBM_REV,
      Parameters::P_FBM_DRV, Parameters::P_FBM_FLD, Parameters::P_FBM_ASM, Parameters::P_FBM_LVL,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_FBM_LKT,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* MASTER,     UNISON */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_MA_V,    Parameters::P_MA_T,    Parameters::P_MA_SH,   Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_UN_V,    Parameters::P_UN_DET,  Parameters::P_UN_PHS,  Parameters::P_UN_PAN,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
  /* (none) */
  {
      /*  0             1             2             3             4             5             6             7            */
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
      Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID, Parameters::P_INVALID,
  },
};
