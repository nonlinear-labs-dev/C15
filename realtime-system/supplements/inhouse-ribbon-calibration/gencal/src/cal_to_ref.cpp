#include <stdio.h>
#include <math.h>
#include "cal_to_ref.h"
#include "interpol.h"

namespace ctr
{
  static inline int difference(int a, int b)
  {
    return abs(a - b);
  }

  CalToRef::CalToRef(Options option)
      : m_totalSamples(0)
      , m_validSamples(0)
      , m_droppedSamples(0)
      , m_discardedSamples(0)
      , m_lastBinNumber(0)
      , m_lastValue(0)
      , m_mappedCount(0)
  {
    m_verbose = (option == VerboseMessages);
    for(auto i = 0; i < m_RANGE; i++)
    {
      m_data[i].value = 0;
      m_data[i].count = 0;
      m_mappedX[i] = 0;
      m_mappedY[i] = 0;
    }
  }

  CalToRef::~CalToRef()
  {
  }

  void CalToRef::startAddIn(void)
  {
    if(m_verbose)
      puts("Reading ref and dut raw values...");
  }

  void CalToRef::endAddIn(void)
  {
    if(m_verbose)
      printf("Reading done, %u sample pairs total: %u valid, %u dropped, %u discarded\n", m_totalSamples,
             m_validSamples, m_droppedSamples, m_discardedSamples);
  }

  unsigned CalToRef::getValidSamples(void)
  {
    return m_validSamples;
  }

  // -------------------------------------
  int CalToRef::addInSamplePair(uint16_t binNumber, uint16_t value)
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

    if((m_data[binNumber].count >= INT32_MAX) || (m_data[binNumber].count >= (INT32_MAX - value)))
    {  // do not add in value if data would overflow
      ++m_discardedSamples;
      if(m_verbose)
        printf("discarded, arithmetic overflow.\n");
      return 2;
    }

    if(m_verbose)
      printf("Ok\n");
    ++m_validSamples;
    m_data[binNumber].value += value;  // accumulate-in value (averaging is done later)
    ++m_data[binNumber].count;         // increase denominator for averaging

    return 0;
  }

  // -----------------------------
  bool CalToRef::ProcessData(void)
  {
    // average mutliple data values for a bin, if required
    // map raw data into an (potentionally sparse) x->y array
    if(m_verbose)
      puts("Averaging and mapping bins...");
    for(auto i = 0; i < m_RANGE; i++)
    {
      if(m_data[i].count)  // populated bin ?
      {
        m_data[i].value /= m_data[i].count;
        m_mappedX[m_mappedCount] = i;
        m_mappedY[m_mappedCount] = m_data[i].value;
        ++m_mappedCount;
        if(m_verbose)
          printf("%4d, %4d\n", i, m_data[i].value);
      }
    }

    // smooth x and y mapped data
    if(m_mappedCount < 5)
    {
      if(m_verbose)
        printf("Not enough (< 5) valid pairs for mapping/averaging!\n");
      return false;
    }
    if(m_verbose)
      puts("Smoothing map...");
    for(int32_t i = 2; i < m_mappedCount - 2; i++)
    {  // sliding average over 5 values
      m_mappedAvgdX[i - 2]
          = (m_mappedX[i - 2] + m_mappedX[i - 1] + m_mappedX[i] + m_mappedX[i + 1] + m_mappedX[i + 2]) / 5;
      m_mappedAvgdY[i - 2]
          = (m_mappedY[i - 2] + m_mappedY[i - 1] + m_mappedY[i] + m_mappedY[i + 1] + m_mappedY[i + 2]) / 5;
      if(m_verbose)
        printf("%4d, %4d\n", m_mappedAvgdX[i - 2], m_mappedAvgdY[i - 2]);
    }

    // check for continuity/monotonicity of x and y values
    if(m_verbose)
      printf("Checking continuity...");
    for(int32_t i = 1; i < m_mappedCount - 4; i++)
    {
      if(m_mappedAvgdX[i] <= m_mappedAvgdX[i - 1])
      {
        if(m_verbose)
          printf("X data (%d, %d) is not monotonically rising!\n", m_mappedAvgdX[i], m_mappedAvgdX[i - 1]);
        return false;
      }
    }
    for(int32_t i = 1; i < m_mappedCount - 4; i++)
    {
      if(m_mappedAvgdY[i] + 5 < m_mappedAvgdY[i - 1])
      {
        if(m_verbose)
          printf("Y data (%d, %d) is not close enough to monotonically rising!\n", m_mappedAvgdY[i],
                 m_mappedAvgdY[i - 1]);
        return false;
      }
    }
    if(m_verbose)
      printf("Ok\n");

    // set up interpolation table
    m_interpolData.points = m_mappedCount - 4;
    m_interpolData.x_values = m_mappedAvgdX;
    m_interpolData.y_values = m_mappedAvgdY;

    return true;
  }

  bool CalToRef::OutputData(FILE *fp)
  {
    if(fp == nullptr)
      fp = stdout;

    fprintf(fp, "[calibration_y]\n");
    for(auto i = 0; i < 33; i++)
      fprintf(fp, "%d\n", m_RIBBON_REFERENCE_CALIBRATION_TABLE_Y[i]);

    fprintf(fp, "[calibration_x]\n");
    for(auto i = 0; i < 34; i++)
    {
      //int x = m_RIBBON_REFERENCE_CALIBRATION_TABLE_X[i];
      //int y = Interpol::InterpolateValue(&m_interpolData, x);
      //printf("%d %d\n", x, y);
      fprintf(fp, "%d\n", Interpol::InterpolateValue(&m_interpolData, m_RIBBON_REFERENCE_CALIBRATION_TABLE_X[i]));
    }
    return true;
  }

}  //namespace

// EOF
