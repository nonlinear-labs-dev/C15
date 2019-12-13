#include <stdio.h>
#include <math.h>
#include "cal_to_ref.h"
#include "interpol.h"

namespace Calibrator
{
  static inline int difference(int const a, int const b)
  {
    return abs(a - b);
  }

  CalToRef::CalToRef(Options const option)
      : m_totalSamples(0)
      , m_validSamples(0)
      , m_droppedSamples(0)
      , m_discardedSamples(0)
      , m_lastBinNumber(0)
      , m_lastValue(0)
      , m_mappedTotal(0)
  {
    m_verbose = (option == VerboseMessages);
    for(auto i = 0; i < m_RANGE; i++)
    {
      m_values[i] = 0;
      m_counts[i] = 0;
      m_mappedX[i] = 0;
      m_mappedY[i] = 0;
      m_mappedCount[i] = 0;
    }
  }

  CalToRef::~CalToRef()
  {
  }

  void CalToRef::startAddIn(void) const
  {
    if(m_verbose)
      puts("Reading ref and dut raw values...");
  }

  void CalToRef::endAddIn(void) const
  {
    if(m_verbose)
      printf("Reading done, %u sample pairs total: %u valid, %u dropped, %u discarded\n", m_totalSamples,
             m_validSamples, m_droppedSamples, m_discardedSamples);
  }

  unsigned CalToRef::getValidSamples(void) const
  {
    return m_validSamples;
  }

  // -------------------------------------
  int CalToRef::addInSamplePair(uint16_t const binNumber, uint16_t const value)
  {
    if(m_verbose)
      printf("%5hu %5hu : ", binNumber, value);

    if((binNumber >= m_RANGE) || (value >= m_RANGE))
    {
      printf("FATAL: Value(s) out of range [0,%d]!\n", m_RANGE - 1);
      return 3;  // fatal: values out of range for array index
    }

    ++m_totalSamples;

    if((binNumber < m_THRESHOLD) || (value < m_THRESHOLD))
    {  // drop useless samples
      ++m_droppedSamples;
      if(m_verbose)
        printf("dropped, value(s) lower than worst-case ribbon threshold (%d).\n", m_THRESHOLD);
      return 1;
    }

    int diff;
    if((diff = difference(binNumber, value)) > m_MAX_DELTA)
    {  // discard sample pairs that are not likely to be valid
      ++m_discardedSamples;
      if(m_verbose)
        printf("discarded, value difference (%d) is too large (> %d).\n", diff, m_MAX_DELTA);
      return 2;
    }

    int ret = 0;
    if(m_totalSamples > 1)
    {  // ignore this for first sample pair
      if((diff = difference(int(binNumber), int(m_lastBinNumber))) > m_MAX_DIST)
      {  // discard value if it is too far away from last value and probably invalid
        if(m_verbose)
          printf("discarded, ref point distance (|%u-%u|=%d) is too large (> %d).\n", binNumber, m_lastBinNumber, diff,
                 m_MAX_DIST);
        ret = 2;  // will discard this sample pair later
      }
      if((diff = difference(int(value), int(m_lastValue))) > m_MAX_DIST)
      {  // discard value if it is too far away from last value and probably invalid
        if(m_verbose)
        {
          if(ret)
            printf("              ");
          printf("discarded, dut point distance (|%u-%u|=%d) is too large (> %d).\n", value, m_lastValue, diff,
                 m_MAX_DIST);
        }
        ret = 2;  // will discard this sample pair later
      }
    }
    m_lastBinNumber = binNumber;  // save previous values first so next pair ...
    m_lastValue = value;          // ... has something valid to compare with,
    if(ret)                       // then discard this pair if required
    {
      ++m_discardedSamples;
      return ret;
    }

    // now we finally have a sample pair that is likely to be valid in our context

    if((m_counts[binNumber] >= INT32_MAX) || (m_counts[binNumber] >= (INT32_MAX - value)))
    {  // do not add in value if data would overflow
      ++m_discardedSamples;
      if(m_verbose)
        printf("discarded, arithmetic overflow.\n");
      return 2;
    }

    if(m_verbose)
      printf("Ok\n");
    ++m_validSamples;
    m_values[binNumber] += value;  // accumulate-in value (averaging is done later)
    ++m_counts[binNumber];         // increase denominator for averaging

    return 0;
  }

  // -----------------------------
  bool CalToRef::ProcessData(void)
  {
    // average mutliple data values for a bin
    if(m_verbose)
      puts("Calculating bin values...");
    for(auto i = 0; i < m_RANGE; i++)
    {
      if(m_counts[i])  // populated bin ?
        m_values[i] /= m_counts[i];
    }

    // map raw data into an (potentionally sparse) x->y array
    if(m_verbose)
      puts("Remove zero bins...");
    for(auto i = 0; i < m_RANGE; i++)
    {
      if(m_counts[i])  // populated bin ?
      {
        m_mappedX[m_mappedTotal] = i;
        m_mappedY[m_mappedTotal] = m_values[i];
        m_mappedCount[m_mappedTotal] = m_counts[i];
        ++m_mappedTotal;
        if(m_verbose)
          printf("%4d, %7.2f\n", i, double(m_values[i]));
      }
    }

    if(m_verbose)
      puts("Finding peak bins, discarding others...");

    int index = 0;
    int map_index = 0;

    // populate zero point where interpolation must meet
    m_mappedPeaksX[map_index] = 0;
    m_mappedPeaksY[map_index] = 0;
    ++map_index;

    do
    {
      // find next minimum
      int search_index = index;
      int min_index = m_RANGE;
      int min = m_RANGE;  // set min to a larger value than will be ever there
      do
      {
        if(m_mappedCount[search_index] > min)
        {
          min_index = search_index - 1;  // this value is greater now, ...
          break;                         // ... so min is found at previous index
        }
        min = m_mappedCount[search_index];
      } while(++search_index < m_mappedTotal);  // step through as long as still lower values have been found
      if(min_index == m_RANGE)                  // no min found ?
        min_index = index;                      // use start value as min
      index = min_index;                        // advance position to min

      // find next maximum
      search_index = index;
      int max_index = m_RANGE;
      int max = 0;  // set min to a lesser value than will be ever there
      do
      {
        if(m_mappedCount[search_index] < max)
        {
          max_index = search_index - 1;  // this value is smaller now, ...
          break;                         // ... so max is found at previous index
        }
        max = m_mappedCount[search_index];
      } while(++search_index < m_mappedTotal);  // step through as long as still lower values have been found
      if(max_index == m_RANGE)                  // no min found ?
        max_index = index;                      // use start value as min
      index = max_index;                        // advance position to min

      // set interpolation point
      if(m_mappedCount[index] > 10)  // but only when a reasonable max is found  TODO : check optimum value
      {
        m_mappedPeaksX[map_index] = m_mappedX[index];
        m_mappedPeaksY[map_index] = m_mappedY[index];
        ++map_index;
      }
    } while(++index < m_RANGE);
    // populate max point where interpolation must meet
    m_mappedPeaksX[map_index] = 4095;
    m_mappedPeaksY[map_index] = 4095;
    ++map_index;

    if(m_verbose)
      printf("%d peak bins found\n", map_index - 2);

    if(map_index < 33 + 2)  // 33 points are required, one press at each dot position
    {
      if(m_verbose)
        printf("Not enough (< 32) peak bins found\n");
      return false;
    }

#if 0
    // smooth x and y mapped data
    if(m_mappedTotal < m_AVG_SIZE)
    {
      if(m_verbose)
        printf("Not enough (< %d) valid pairs for mapping/averaging!\n", m_AVG_SIZE);
      return false;
    }
    if(m_verbose)
      puts("Smoothing map...");
    for(int32_t i = m_AVG_CENTER; i < m_mappedTotal - m_AVG_CENTER; i++)
    {  // sliding average over N values
      m_mappedPeaksX[i - m_AVG_CENTER] = 0;
      for(auto k = -m_AVG_CENTER; k <= +m_AVG_CENTER; k++)
        m_mappedPeaksX[i - m_AVG_CENTER] += m_mappedX[i + k];
      m_mappedPeaksX[i - m_AVG_CENTER] /= m_AVG_SIZE;

      m_mappedPeaksY[i - m_AVG_CENTER] = 0;
      for(auto k = -m_AVG_CENTER; k <= +m_AVG_CENTER; k++)
        m_mappedPeaksY[i - m_AVG_CENTER] += m_mappedY[i + k];
      m_mappedPeaksY[i - m_AVG_CENTER] /= m_AVG_SIZE;

      if(m_verbose)
        printf("%7.2f, %7.2f\n", double(m_mappedPeaksX[i - m_AVG_CENTER]), double(m_mappedPeaksY[i - m_AVG_CENTER]));
    }

    // check for continuity/monotonicity of x and y values
    m_mappedTotal -= m_AVG_SIZE - 1;
    if(m_verbose)
      printf("Checking continuity...");
    for(int32_t i = 1; i < m_mappedTotal; i++)
    {
      if(m_mappedPeaksX[i] <= m_mappedPeaksX[i - 1])
      {
        if(m_verbose)
          printf("X data (%7.2f, %7.2f) is not monotonically rising!\n", double(m_mappedPeaksX[i]),
                 double(m_mappedPeaksX[i - 1]));
        return false;
      }
    }
    for(int32_t i = 1; i < m_mappedTotal; i++)
    {
      if(m_mappedPeaksY[i] + m_MONOTONICITY_DELTA < m_mappedPeaksY[i - 1])
      {
        if(m_verbose)
          printf("Y data (%7.2f, %7.2f) is not close enough to monotonically rising!\n", double(m_mappedPeaksY[i - 1]),
                 double(m_mappedPeaksY[i]));
        return false;
      }
    }
#endif

    if(m_verbose)
      printf("Ok\n");

    // set up interpolation table
    m_interpolData.points = map_index;
    m_interpolData.x_values = m_mappedPeaksX;
    m_interpolData.y_values = m_mappedPeaksY;

    return true;
  }

  bool CalToRef::OutputData(FILE *outfile) const
  {
    if(outfile == nullptr)
      outfile = stdout;

    // X (input) data points
    fprintf(outfile, "[Calibration-X]\n");
    for(auto i = 0; i < 34; i++)
    {
      if(i == 0)
        fprintf(outfile, "%d",
                int(round(Interpol::InterpolateValue(&m_interpolData, m_RIBBON_REFERENCE_CALIBRATION_TABLE_X[i]))));
      else
        fprintf(outfile, ", %d",
                int(round(Interpol::InterpolateValue(&m_interpolData, m_RIBBON_REFERENCE_CALIBRATION_TABLE_X[i]))));
    }
    fprintf(outfile, "\n");

    // Y (output) data points
    fprintf(outfile, "[Calibration-Y]\n");
    for(auto i = 0; i < 33; i++)
    {
      if(i == 0)
        fprintf(outfile, "%d", int(round(m_RIBBON_REFERENCE_CALIBRATION_TABLE_Y[i])));
      else
        fprintf(outfile, ", %d", int(round(m_RIBBON_REFERENCE_CALIBRATION_TABLE_Y[i])));
    }
    fprintf(outfile, "\n");

    return true;
  }

}  //namespace

// EOF
