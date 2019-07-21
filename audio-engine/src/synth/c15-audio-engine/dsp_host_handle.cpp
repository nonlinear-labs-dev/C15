#include "paramengine.h"
#include "dsp_host.h"

#if 0

namespace Nl
{
  namespace DSP_HOST_HANDLE
  {

    dsp_host m_host;  // renamed member dsp_host to m_host (Matthias)

    /** @brief    Callback function for Sine Generator and Audio Input - testing with ReMote 61
            @param    Input Buffer
            @param    Output Buffer

            @param    buffer size
            @param    Sample Specs
    */
    void dspHostCallback(uint8_t *out, const SampleSpecs &sampleSpecs, std::any ptr)
    {

      if(sw == nullptr)
      {
        sw.reset(new StopWatch("AudioCallback", 100, StopWatch::SUMMARY, sampleSpecs.latency));
      }
      //                                              (could take DETAILED for SUMMARY)
      //TODO: Fixme. This RAII variant seems not to work
      //StopBlockTime(sw, "dsp_host");
      sw->start("callback");

      // std::this_thread::sleep_for(std::chrono::milliseconds(100));

      JobHandle jh = std::any_cast<JobHandle>(ptr);

      Nl::CommandBuffer::Command c;
      float tmp = 0.f;
      while((c = jh.cmdBuffer->get()) != Nl::CommandBuffer::CMD_NO_CMD)
      {
        switch(c)
        {
          case Nl::CommandBuffer::CMD_GET_PARAM:
            m_host.examineParameter();
            jh.cmdBufferResponse->insert(pack<::examine_param>(m_host.m_param_status));
            break;
          case Nl::CommandBuffer::CMD_GET_SIGNAL:
            m_host.examineSignal();
            jh.cmdBufferResponse->insert(pack<::examine_signal>(m_host.m_signal_status));
            break;
          case Nl::CommandBuffer::CMD_GET_TCD_INPUT:
            jh.cmdBufferResponse->insert(pack<::examine_tcd_input_log>(m_host.m_tcd_input_log));
            m_host.m_tcd_input_log.reset();
            break;
          case Nl::CommandBuffer::CMD_GET_CPU_LOAD:
            jh.cmdBufferResponse->insert(pack<SharedStopWatchHandle>(sw));
            break;
          case Nl::CommandBuffer::CMD_RESET:
            m_host.resetDSP();
            jh.cmdBufferResponse->insert(pack<std::string>("Reset executed"));
            break;
          case Nl::CommandBuffer::CMD_TOGGLE_TEST_TONE:
            tmp = static_cast<float>(1 - m_host.m_test_tone.m_state);
            m_host.m_decoder.m_utilityId = 4;
            m_host.utilityUpdate(tmp);
            std::cout << "Test Tone toggled(" << m_host.m_test_tone.m_state << ")" << std::endl;
            break;
          case Nl::CommandBuffer::CMD_FOCUS_TEST_TONE_FREQ:
            m_host.m_test_tone.m_focus = 0;
            std::cout << "Test Tone: Frequency:\t" << m_host.m_test_tone.a_frequency << " Hz" << std::endl;
            break;
          case Nl::CommandBuffer::CMD_FOCUS_TEST_TONE_AMP:
            m_host.m_test_tone.m_focus = 1;
            std::cout << "Test Tone: Amplitude:\t" << m_host.m_test_tone.a_amplitude << " dB" << std::endl;
            break;
          case Nl::CommandBuffer::CMD_EDIT_TEST_TONE_PLUS:
            switch(m_host.m_test_tone.m_focus)
            {
              case 0:
                m_host.m_decoder.m_utilityId = 2;
                tmp = std::clamp(m_host.m_test_tone.a_frequency + 10.f, 0.f, 1000.f);
                m_host.utilityUpdate(tmp);
                std::cout << "Test Tone: Frequency:\t" << tmp << " Hz" << std::endl;
                break;
              case 1:
                m_host.m_decoder.m_utilityId = 3;
                tmp = std::clamp(m_host.m_test_tone.a_amplitude + 1.f, -60.f, 0.f);
                m_host.utilityUpdate(tmp);
                std::cout << "Test Tone: Amplitude:\t" << tmp << " dB" << std::endl;
                break;
            }
            break;
          case Nl::CommandBuffer::CMD_EDIT_TEST_TONE_MINUS:
            switch(m_host.m_test_tone.m_focus)
            {
              case 0:
                m_host.m_decoder.m_utilityId = 2;
                tmp = std::clamp(m_host.m_test_tone.a_frequency - 10.f, 0.f, 1000.f);
                m_host.utilityUpdate(tmp);
                std::cout << "Test Tone: Frequency:\t" << tmp << " Hz" << std::endl;
                break;
              case 1:
                m_host.m_decoder.m_utilityId = 3;
                tmp = std::clamp(m_host.m_test_tone.a_amplitude - 1.f, -60.f, 0.f);
                m_host.utilityUpdate(tmp);
                std::cout << "Test Tone: Amplitude:\t" << tmp << " dB" << std::endl;
                break;
            }
            break;
          case Nl::CommandBuffer::CMD_NO_CMD:
            // Just to suppress the warning (NEVER use this element)
            break;
        }
      }

      auto midiBuffer = getBufferForName("MidiBuffer");

      //---------------- Retrieve Midi Information if midi values have changed
      if(midiBuffer)
      {
        unsigned char midiByteBuffer[3];  // MidiByteBuffer Structure: [0] - , [1] - Played Note Address, [2] - Velocity

        while(midiBuffer->availableToRead() >= 3)
        {
          midiBuffer->get(midiByteBuffer, 3);

          // printf("%02X %02X %02X\n", midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);      // MIDI Value Control Output

/* run the program either in pure TCD mode (0) or test functionality (1) - defined by test_inputModeFlag in pe_defines_config.h */
#if test_inputModeFlag
          m_host.testMidi(midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
#else
          m_host.evalMidi(midiByteBuffer[0], midiByteBuffer[1], midiByteBuffer[2]);
#endif
        }
      }

      processAudioFrames(out, sampleSpecs);
      sw->stop();
    }

    void processAudioFrames(uint8_t *out, const SampleSpecs &sampleSpecs)
    {
      for(unsigned int frameIndex = 0; frameIndex < sampleSpecs.buffersizeInFramesPerPeriode; ++frameIndex)
      {
        m_host.tickMain();

        float outputSample;

        for(unsigned int channelIndex = 0; channelIndex < sampleSpecs.channels; ++channelIndex)
        {
          if(channelIndex)
          {
            outputSample = m_host.m_mainOut_R;
          }
          else if(!channelIndex)
          {
            outputSample = m_host.m_mainOut_L;
          }

          if(outputSample > 1.f || outputSample < -1.f)  // Clipping
          {
            printf("WARNING!!! C15 CLIPPING!!!\n");
          }

          setSample(out, outputSample, frameIndex, channelIndex, sampleSpecs);
        }
      }
    }

    // Matthias: added polyphony as argument
    JobHandle dspHostTCDControl(const AlsaAudioCardIdentifier &audioOutCard, const AlsaMidiCardIdentifier &midiInCard,
                                unsigned int buffersize, unsigned int samplerate, unsigned int polyphony)
    {
      m_host.init(samplerate, polyphony);
      JobHandle ret;

      ret.cmdBufferResponse = createSharedCommandBufferResponse();
      ret.cmdBuffer = createSharedCommandBuffer();

      // No input here
      ret.inBuffer = nullptr;
      ret.audioInput = nullptr;

      ret.outBuffer = createBuffer("OutputBuffer");
      ret.audioOutput = createAlsaOutputDevice(audioOutCard, ret.outBuffer, buffersize);
      ret.audioOutput->setSamplerate(samplerate);

      ret.inMidiBuffer = createBuffer("MidiBuffer");
      ret.midiInput = createRawMidiDevice(midiInCard, ret.inMidiBuffer);

      ret.audioOutput->start();
      ret.midiInput->start();

      ret.workingThreadHandle = registerOutputCallbackOnBuffer(ret.outBuffer, dspHostCallback, ret);

      return ret;
    }

    dsp_host &getDspHost()
    {
      return m_host;
    }

  }  // namespace DSP_HOST
}  // namespace Nl

#endif
