#include <iostream>
#include <nltools/logging/Log.h>
#include "dsp_host.h"
#include "pe_utilities.h"

/* default constructor - initialize (audio) signals */
dsp_host::dsp_host()
{
  /* init main audio output signal */
  m_mainOut_R = 0.f;
  m_mainOut_L = 0.f;
}

/* proper init - initialize engine(s) according to sampleRate and polyphony */
void dsp_host::init(uint32_t _samplerate, uint32_t _polyphony)
{
  /* set up configuration */
  m_samplerate = _samplerate;
  m_voices = _polyphony;
  m_clockDivision[2] = _samplerate / dsp_clock_rates[0];
  m_clockDivision[3] = _samplerate / dsp_clock_rates[1];
  m_upsampleFactor = _samplerate / 48000;
  /* initialize components */
  m_params.init(_samplerate, _polyphony);
  m_decoder.init();
  /* init messages to terminal */

  nltools::Log::info("DSP_HOST::MILESTONE:\t\t", static_cast<float>(test_milestone) * 0.01f);
  nltools::Log::info("DSP_HOST::INIT:\t\t\tsamplerate:", m_samplerate, ", voices:", m_voices);
  nltools::Log::info("DSP_HOST::CLOCK_divisions:\t", m_clockDivision[0], ",", m_clockDivision[1], ",",
                     m_clockDivision[2], ",", m_clockDivision[3]);
  nltools::Log::info("DSP_HOST::UPSAMPLE:\t\t", m_upsampleFactor);
  nltools::Log::info("DSP_HOST::SIZEOF:\t\t", sizeof(dsp_host), "bytes");

  /* Audio Engine */
  initAudioEngine();

  /* Load Initial Preset (TCD zero for every Parameter) */
  loadInitialPreset();

  //testLoadPreset(4);

  /* Examine functionality: TCD MIDI Input Log, observed Parameter, observed Signal */
#if log_examine
  m_tcd_input_log.reset();

  m_param_status.m_selected = static_cast<Parameters>(log_param_id);
  m_param_status.m_id = m_params.getHead(m_param_status.m_selected).m_id;
  m_param_status.m_index = m_params.getHead(m_param_status.m_selected).m_index;
  m_param_status.m_size = m_params.getHead(m_param_status.m_selected).m_size;
  m_param_status.m_clockType = static_cast<uint32_t>(m_params.getHead(m_param_status.m_selected).m_clockType);
  m_param_status.m_polyType = static_cast<uint32_t>(m_params.getHead(m_param_status.m_selected).m_polyType);
  m_param_status.m_scaleId = m_params.getHead(m_param_status.m_selected).m_scaleId;
  m_param_status.m_postId = static_cast<uint32_t>(m_params.getHead(m_param_status.m_selected).m_postId);
  m_param_status.m_normalize = m_params.getHead(m_param_status.m_selected).m_normalize;
  m_param_status.m_scaleArg = m_params.getHead(m_param_status.m_selected).m_scaleArg;

  m_signal_status.m_selected = static_cast<Signals>(log_signal_id);
  m_signal_status.m_size = m_voices;
#endif
}

/* */
void dsp_host::loadInitialPreset()
{
  /* passing TCD zeros to every parameter in every voice */
  evalMidi(0, 127, 127);  // select all voices
  evalMidi(1, 127, 127);  // select all parameters
  evalMidi(2, 0, 0);      // set time to zero
  evalMidi(47, 1, 1);     // enable preload (recall list mode)
  /* traverse parameters, each one receiving zero value */
  for(uint32_t i = 0; i < lst_recall_length; i++)
  {
    if(paramIds_recall[i] == Parameters::P_UN_V)  /// Unison Voices
    {
      evalMidi(5, 0, 1);  // send destination 1
    }
    else  /// anything else
    {
      evalMidi(5, 0, 0);  // send destination 0
    }
  }
  evalMidi(47, 0, 2);  // apply preloaded values
#if test_initialize_fx_sends
  /* temporarily initialize echo and reverb sends to 100% (because compability) */
  evalMidi(1, 334 >> 7, 334 & 127);      // select echo send (ID 334)
  evalMidi(5, 16000 >> 7, 16000 & 127);  // set destination (16000 <-> 100%)
  evalMidi(1, 336 >> 7, 336 & 127);      // select reverb send (ID 336)
  evalMidi(5, 16000 >> 7, 16000 & 127);  // set destination (16000 <-> 100%)
#endif
  /* should time be initialized? */
#if test_initialize_time == 1
  uint32_t time = dsp_initial_time * static_cast<uint32_t>(m_params.m_millisecond);  // no clipping!!!
  evalMidi(2, time >> 7, time & 127);
#endif
  /* clear Selection */
  evalMidi(0, 0, 0);  // select voice 0
  evalMidi(1, 0, 0);  // select parameter 0
}

/* */
void dsp_host::tickMain()
{
  /* first: evaluate slow clock status */
  if(m_clockPosition[3] == 0)
  {
    /* render slow mono parameters and perform mono post processing */
#if PARAM_ITERATOR == 0
    for(const auto &it : m_params.m_parameters.getClockIds(ClockTypes::Slow, PolyTypes::Mono))
    {
      auto i = m_params.getHead(static_cast<Parameters>(it)).m_index;
      m_params.getBody(i).tick();
    }
#else
    auto end = m_params.m_parameters.end(ClockTypes::Slow, PolyTypes::Mono);
    for(param_body* it = m_params.m_parameters.begin(ClockTypes::Slow, PolyTypes::Mono); it != end; it++)
    {
      it->tick();
    }
#endif
    m_params.postProcessMono_slow(m_parameters);
    /* render slow poly parameters and perform poly slow post processing */
    for(uint32_t v = 0; v < m_voices; v++)
    {
#if PARAM_ITERATOR == 0
      for(const auto &it : m_params.m_parameters.getClockIds(ClockTypes::Slow, PolyTypes::Poly))
      {
        auto i = m_params.getHead(static_cast<Parameters>(it)).m_index + v;
        m_params.getBody(i).tick();
      }
#else
      end = m_params.m_parameters.end(ClockTypes::Slow, PolyTypes::Poly, v);
      for(param_body* it = m_params.m_parameters.begin(ClockTypes::Slow, PolyTypes::Poly, v); it != end; it += m_voices)
      {
        it->tick();
      }
#endif
      m_params.postProcessPoly_slow(m_parameters, v);
      /* slow polyphonic Trigger for Filter Coefficients */
      setPolySlowFilterCoeffs(m_parameters, v);
    }
    /* slow monophonic Trigger for Filter Coefficients */
    setMonoSlowFilterCoeffs(m_parameters);
  }
  /* second: evaluate fast clock status */
  if(m_clockPosition[2] == 0)
  {
    /* render fast mono parameters and perform mono post processing */
#if PARAM_ITERATOR == 0
    for(auto &it : m_params.m_parameters.getClockIds(ClockTypes::Fast, PolyTypes::Mono))
    {
      auto i = m_params.getHead(static_cast<Parameters>(it)).m_index;
      m_params.getBody(i).tick();
    }
#else
    auto end = m_params.m_parameters.end(ClockTypes::Fast, PolyTypes::Mono);
    for(param_body* it = m_params.m_parameters.begin(ClockTypes::Fast, PolyTypes::Mono); it != end; it++)
    {
      it->tick();
    }
#endif
    m_params.postProcessMono_fast(m_parameters);
    /* render fast poly parameters and perform poly fast post processing */
    for(uint32_t v = 0; v < m_voices; v++)
    {
#if PARAM_ITERATOR == 0
      for(auto &it : m_params.m_parameters.getClockIds(ClockTypes::Fast, PolyTypes::Poly))
      {
        auto i = m_params.getHead(static_cast<Parameters>(it)).m_index + v;
        m_params.getBody(i).tick();
      }
#else
      end = m_params.m_parameters.end(ClockTypes::Fast, PolyTypes::Poly, v);
      for(param_body* it = m_params.m_parameters.begin(ClockTypes::Fast, PolyTypes::Poly, v); it != end; it += m_voices)
      {
        it->tick();
      }
#endif
      m_params.postProcessPoly_fast(m_parameters, v);
    }
    /* fast monophonic Trigger for Filter Coefficients */
    setMonoFastFilterCoeffs(m_parameters);
  }
  /* third: evaluate audio clock (always) - mono rendering and post processing, poly rendering and post processing */
#if PARAM_ITERATOR == 0
  for(auto &it : m_params.m_parameters.getClockIds(ClockTypes::Audio, PolyTypes::Mono))
  {
    auto i = m_params.getHead(static_cast<Parameters>(it)).m_index;
    m_params.getBody(i).tick();
  }
#else
  auto end = m_params.m_parameters.end(ClockTypes::Audio, PolyTypes::Mono);
  for(param_body* it = m_params.m_parameters.begin(ClockTypes::Audio, PolyTypes::Mono); it != end; it++)
  {
    it->tick();
  }
#endif
  m_params.postProcessMono_audio(m_parameters);
  /* Reset Outputmixer Sum Samples */
  m_outputmixer.m_out_L = 0.f;
  m_outputmixer.m_out_R = 0.f;

  /* this is the MAIN POLYPHONIC LOOP - rendering (and post processing) parameters, envelopes and the AUDIO_ENGINE */
  for(uint32_t v = 0; v < m_voices; v++)
  {
    /* render poly audio parameters */
#if PARAM_ITERATOR == 0
    for(auto &it : m_params.m_parameters.getClockIds(ClockTypes::Audio, PolyTypes::Poly))
    {
      auto i = m_params.getHead(static_cast<Parameters>(it)).m_index + v;
      m_params.getBody(i).tick();
    }
#else
    end = m_params.m_parameters.end(ClockTypes::Audio, PolyTypes::Poly, v);
    for(param_body* it = m_params.m_parameters.begin(ClockTypes::Audio, PolyTypes::Poly, v); it != end; it += m_voices)
    {
      it->tick();
    }
#endif
    /* post processing and envelope rendering */
    m_params.postProcessPoly_audio(m_parameters, v);
  }

  /* AUDIO_ENGINE: poly dsp phase */
  makePolySound(m_parameters);
  /* AUDIO_ENGINE: mono dsp phase */
  makeMonoSound(m_parameters);

  /* Examine Updates (Log) */
#if log_examine
  m_tcd_input_log.tick();
#endif

  /* finally: update (fast and slow) clock positions */
  m_clockPosition[2] = (m_clockPosition[2] + 1) % m_clockDivision[2];
  m_clockPosition[3] = (m_clockPosition[3] + 1) % m_clockDivision[3];
}

/* */
void dsp_host::evalMidi(uint32_t _status, uint32_t _data0, uint32_t _data1)
{
  /* TCD MIDI evaluation */
  int32_t i;                                   // parsing variable for signed integers
  int32_t v = static_cast<int32_t>(m_voices);  // parsing variable representing number of voices as signed integer
  float f;                                     // parsing variable for floating point values
  _status &= 127;
  /* TCD Log */
#if log_examine
  m_tcd_input_log.add(_status, _data0, _data1);
#endif
  switch(m_decoder.getCommandId(_status))
  {
    case 0:
      /* ignore */
      break;
    case 1:
      /* selectVoice - rigorous safety mechanism */
      i = static_cast<int32_t>(m_decoder.unsigned14(_data0, _data1));
      /* force monophonic mode */
#if log_force_mono == 1
      i = 0;
#endif
      if(((i > -1) && (i < v)) || (i == 16383))
      {
        m_decoder.m_voiceFrom = m_decoder.m_voiceTo = static_cast<uint32_t>(i);
        voiceSelectionUpdate();
      }
      else
      {
        nltools::Log::info("ignored selectVoice(", i, ")");
      }
      break;
    case 2:
      /* selectMultipleVoices - rigorous safety mechanism */
      i = static_cast<int32_t>(m_decoder.unsigned14(_data0, _data1));
      /* force monophonic mode */
#if log_force_mono == 1
      i = 0;
#endif
      if(((i > -1) && (i < v)) || (i == 16383))
      {
        m_decoder.m_voiceTo = static_cast<uint32_t>(i);
        voiceSelectionUpdate();
      }
      else
      {
        nltools::Log::info("ignored selectMultipleVoices(", i, ")");
      }
      break;
    case 3:
      /* selectParam */
      m_decoder.m_paramFrom = m_decoder.m_paramTo = m_decoder.unsigned14(_data0, _data1);
      paramSelectionUpdate();
      break;
    case 4:
      /* selectMultipleParams */
      m_decoder.m_paramTo = m_decoder.unsigned14(_data0, _data1);
      paramSelectionUpdate();
      break;
    case 5:
      /* setTime */
      f = 1.f / static_cast<float>((m_upsampleFactor * m_decoder.unsigned14(_data0, _data1)) + 1);
      timeUpdate(f);
      break;
    case 6:
      /* setTimeUpper */
      m_decoder.unsigned28upper(_data0, _data1);
      break;
    case 7:
      /* setTimeLower */
      f = 1.f
          / static_cast<float>((static_cast<int32_t>(m_upsampleFactor) * m_decoder.apply28lower(_data0, _data1)) + 1);
      timeUpdate(f);
      break;
    case 8:
      /* setDestination */
      f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
      /* distinguish list mode */
      if(m_decoder.m_listId == 0)
      {
        /* list off */
        destinationUpdate(f);
      }
      else
      {
        /* list on */
        listUpdate(f);
      }
      break;
    case 9:
      /* setSignedDestination */
      f = static_cast<float>(m_decoder.signed14(_data0, _data1));
      /* distinguish list mode */
      if(m_decoder.m_listId == 0)
      {
        /* list off */
        destinationUpdate(f);
      }
      else
      {
        /* list on */
        listUpdate(f);
      }
      break;
    case 10:
      /* setDestinationUpper */
      m_decoder.signed28upper(_data0, _data1);
      break;
    case 11:
      /* setDestinationLower */
      f = static_cast<float>(m_decoder.apply28lower(_data0, _data1));
      /* distinguish list mode */
      if(m_decoder.m_listId == 0)
      {
        /* list off */
        destinationUpdate(f);
      }
      else
      {
        /* list on */
        listUpdate(f);
      }
      break;
    case 12:
      /* preload */
      preloadUpdate(_data1, _data0);
      break;
    case 13:
      /* keyUp */
#if test_milestone < 156
      f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
      keyUp(m_decoder.m_voiceFrom, f);
      /* now part of TCD specs: if the key list was specified by preload, voice automatically increases, further shortening TCD seuquences of Unison clusters */
      if(m_decoder.m_listId == 2)
      {
        m_decoder.m_voiceFrom = (m_decoder.m_voiceFrom + 1) % m_voices;
        voiceSelectionUpdate();
      }
#elif test_milestone == 156
      keyUp156(static_cast<float>(m_decoder.unsigned14(_data0, _data1)));
#endif
      break;
    case 14:
      /* keyDown */
#if test_milestone < 156
      f = static_cast<float>(m_decoder.unsigned14(_data0, _data1));
      keyDown(m_decoder.m_voiceFrom, f);
      /* now part of TCD specs: if the key list was specified by preload, voice automatically increases, further shortening TCD seuquences of Unison clusters */
      if(m_decoder.m_listId == 2)
      {
        m_decoder.m_voiceFrom = (m_decoder.m_voiceFrom + 1) % m_voices;
        voiceSelectionUpdate();
      }
#elif test_milestone == 156
      keyDown156(static_cast<float>(m_decoder.unsigned14(_data0, _data1)));
#endif
      break;
    case 15:
      /* reset command (flush / env_stop / dsp_reset) */
      resetUpdate(_data1);
      break;
    case 16:
      /* selectUtility */
      m_decoder.m_utilityId = m_decoder.unsigned14(_data0, _data1);
      break;
    case 17:
      /* setUtility */
      f = static_cast<float>(m_decoder.signed14(_data0, _data1));
      utilityUpdate(f);
      break;
#if test_milestone == 156
    case 18:
      /* 1.56: keyVoice */
      uint32_t state
          = m_decoder.unsigned14(_data0, _data1);  // retrieve state (encoding voice steal and starting voice)
      m_stolen = state & 1;                        // decode voice steal
      m_decoder.m_voiceFrom = state >> 1;          // decode starting voice
      preloadUpdate(1, 2);                         // enable preloaded key event
      break;
#endif
  }
}

/* */
void dsp_host::voiceSelectionUpdate()
{
  /* case ALL voices */
  if(m_decoder.m_voiceTo == 16383)
  {
    m_decoder.m_voiceFrom = 0;
    m_decoder.m_voiceTo = 16382;
  }
  /* reset voice id list */
  m_decoder.m_selectedVoices.reset();
  /* prepare selection event */
  m_decoder.m_event[0] = m_decoder.m_voiceFrom > m_decoder.m_voiceTo ? 1 : 0;
  /* rebuild voice id list by sorting selected and deselected voice ids */
  uint32_t s;
  for(uint32_t v = 0; v < m_voices; v++)
  {
    s = m_decoder.selectionEvent(m_decoder.m_voiceFrom, m_decoder.m_voiceTo, v);
    m_decoder.m_selectedVoices.add(s, v);
  }
}

/* */
void dsp_host::paramSelectionUpdate()
{
  /* case ALL params */
  if(m_decoder.m_paramTo == 16383)
  {
    m_decoder.m_paramFrom = 0;
    m_decoder.m_paramTo = 16382;
  }
  /* reset param id list */
  m_decoder.m_selectedParams.reset();
  /* prepare selection event */
  m_decoder.m_event[0] = m_decoder.m_paramFrom > m_decoder.m_paramTo ? 1 : 0;
  /* rebuild param id list by sorting selected and deselected param ids according to their polyphony */
  uint32_t s;
  for(uint32_t p = 0; p < sig_number_of_params; p++)
  {
    /* only TRUE TCD IDs (< 16383) */
    auto paramID = static_cast<Parameters>(p);

    if(m_params.getHead(paramID).m_id < 16383)
    {
      s = m_decoder.selectionEvent(m_decoder.m_paramFrom, m_decoder.m_paramTo, m_params.getHead(paramID).m_id);
      m_decoder.m_selectedParams.add(m_params.getHead(paramID).m_polyType, s, paramID);
    }
  }
}

/* */
void dsp_host::preloadUpdate(uint32_t _mode, uint32_t _listId)
{
  /* provide voice and parameter indices */
  uint32_t v, p;
  /* handle preload according to mode */
  switch(_mode)
  {
    case 0:
      /* disable preload and reset listId */
      m_params.m_preload = 0;
      m_decoder.m_listId = 0;
      break;
    case 1:
      /* enable preload - set preload mode and list id, reset list index */
      m_params.m_preload = 1;
      m_decoder.m_listId = _listId;
      m_decoder.m_listIndex = 0;
      /* reset parameter preload counters */
      for(p = 0; p < sig_number_of_param_items; p++)
      {
        m_params.getBody(p).m_preload = 0;
      }
      /* reset key event preload counters */
      m_params.m_event.m_mono.m_preload = 0;
      for(v = 0; v < m_voices; v++)
      {
        m_params.m_event.m_poly[v].m_preload = 0;
      }
      break;
    case 2:
      /* apply preloaded values - reset preload mode and list id */
      m_params.m_preload = 0;
      m_decoder.m_listId = 0;
      /* apply preloaded values - parameters */
      for(p = 0; p < sig_number_of_params; p++)
      {
        auto paramID = static_cast<Parameters>(p);

        for(v = 0; v < m_params.getHead(paramID).m_size; v++)
        {
          m_params.applyPreloaded(v, paramID);
        }
      }
#if test_preload_update == 0
      /* Trigger Slow Rendering and Post Processing (Key Events should be effective immediately) */
      /* render slow mono parameters and perform mono post processing */
#if PARAM_ITERATOR == 0
      for(const auto &it : m_params.m_parameters.getClockIds(ClockTypes::Slow, PolyTypes::Mono))
      {
        auto i = m_params.getHead(static_cast<Parameters>(it)).m_index;
        m_params.getBody(i).tick();
      }
#else
      auto end = m_params.m_parameters.end(ClockTypes::Slow, PolyTypes::Mono);
      for(param_body *it = m_params.m_parameters.begin(ClockTypes::Slow, PolyTypes::Mono); it != end; it++)
      {
        it->tick();
      }
#endif
      m_params.postProcessMono_slow(m_parameters.bindToVoice(0));
      /* render slow poly parameters and perform poly slow post processing */
      for(v = 0; v < m_voices; v++)
      {
#if PARAM_ITERATOR == 0
        for(const auto &it : m_params.m_parameters.getClockIds(ClockTypes::Slow, PolyTypes::Poly))
        {
          auto i = m_params.getHead(static_cast<Parameters>(it)).m_index + v;
          m_params.getBody(i).tick();
        }
#else

        end = m_params.m_parameters.end(ClockTypes::Slow, PolyTypes::Poly, v);
        for(param_body *it = m_params.m_parameters.begin(ClockTypes::Slow, PolyTypes::Poly, v); it != end;
            it += m_voices)
        {
          it->tick();
        }
#endif
        m_params.postProcessPoly_slow(m_parameters.bindToVoice(v), v);
        /* slow polyphonic Trigger for Filter Coefficients */
        setPolySlowFilterCoeffs(m_parameters.bindToVoice(v), v);
      }

      /* slow monophonic Trigger for Filter Coefficients */
      setMonoSlowFilterCoeffs(m_parameters.bindToVoice(0));
      /* Reset Slow Clock counter (next slow clock tick will occur in 120 samples - @48kHz) */
      m_clockPosition[3] = 1;
      /* apply preloaded values - key events */
      if(m_params.m_event.m_mono.m_preload > 0)
      {
        m_params.m_event.m_mono.m_preload = 0;
        m_params.keyApplyMono();
      }
      for(v = 0; v < m_voices; v++)
      {
        if(m_params.m_event.m_poly[v].m_preload > 0)
        {
          m_params.m_event.m_poly[v].m_preload = 0;
          m_params.keyApply(v);
          keyApply(v);
        }
      }
      /* end of classical apply */
#elif test_preload_update == 1
      /* in theory, only post processing in active voices has to be done -> paramengine::postProcessPoly_key(_s, _v) */
      /* apply preloaded values - key events - mono */
      if(m_params.m_event.m_mono.m_preload > 0)
      {
        m_params.m_event.m_mono.m_preload = 0;  // reset preload counter
        m_params.keyApplyMono();                // trigger env_engine
      }
      /* apply preloaded values - key events - poly */
      for(v = 0; v < m_voices; v++)
      {
        if(m_params.m_event.m_poly[v].m_preload > 0)
        {
          m_params.m_event.m_poly[v].m_preload = 0;  // reset preload counter
          m_params.keyApply(v);                      // trigger env_engine
          keyApply(v);  // update comb filter delay smoother, phase, (determine voice steal)
          m_params.postProcessPoly_key(m_parameters, v);  // update key-related parameters/signals
          setPolySlowFilterCoeffs(m_parameters, v);       // update filter coefficients
        }
      }
#endif
      break;
  }
}

/* */
void dsp_host::destinationUpdate(float _value)
{
  /* provide voice and parameter indices */
  uint32_t v, p;
  /* update (selected) mono parameters */
  for(p = 0; p < m_decoder.m_selectedParams.m_data[0].m_data[1].m_length; p++)
  {
    m_params.setDest(0, m_decoder.m_selectedParams.m_data[0].m_data[1].m_data[p], _value);
  }
  /* update (selected) poly parameters - for selected voices */
  for(v = 0; v < m_decoder.m_selectedVoices.m_data[1].m_length; v++)
  {
    for(p = 0; p < m_decoder.m_selectedParams.m_data[1].m_data[1].m_length; p++)
    {
      m_params.setDest(m_decoder.m_selectedVoices.m_data[1].m_data[v],
                       m_decoder.m_selectedParams.m_data[1].m_data[1].m_data[p], _value);
    }
  }
}

/* */
void dsp_host::timeUpdate(float _value)
{
  /* provide voice and parameter indices */
  uint32_t v, p;
  /* update (selected) mono parameters */
  for(p = 0; p < m_decoder.m_selectedParams.m_data[0].m_data[1].m_length; p++)
  {
    m_params.setDx(0, m_decoder.m_selectedParams.m_data[0].m_data[1].m_data[p], _value);
  }
  /* update (selected) poly parameters - for selected voices */
  for(v = 0; v < m_decoder.m_selectedVoices.m_data[1].m_length; v++)
  {
    for(p = 0; p < m_decoder.m_selectedParams.m_data[1].m_data[1].m_length; p++)
    {
      m_params.setDx(m_decoder.m_selectedVoices.m_data[1].m_data[v],
                     m_decoder.m_selectedParams.m_data[1].m_data[1].m_data[p], _value);
    }
  }
}

/* */
void dsp_host::utilityUpdate(float _value)
{
  /* utility parameters are handled individually and can trigger further events */
  switch(m_decoder.m_utilityId)
  {
    case 0:
      /* ignore - utility parameter 0 serves only as velocity parameter definition */
      break;
    case 1:
      /* set reference tone */
      _value *= m_params.m_utilities[1].m_normalize;
      m_params.m_pitch_reference
          = m_params.scale(m_params.m_utilities[1].m_scaleId, m_params.m_utilities[1].m_scaleArg, _value);
      /* update pitch related values? */
      break;
    case 2:
      /* set test tone frequency */
      _value *= m_params.m_utilities[2].m_normalize;
      m_test_tone.a_frequency = _value;
      m_test_tone.set_frequency(
          m_params.scale(m_params.m_utilities[2].m_scaleId, m_params.m_utilities[2].m_scaleArg, _value));
      break;
    case 3:
      /* set test tone amplitude */
      _value *= m_params.m_utilities[3].m_normalize;
      m_test_tone.a_amplitude = _value;
      m_test_tone.set_amplitude(
          m_params.scale(m_params.m_utilities[3].m_scaleId, m_params.m_utilities[3].m_scaleArg, _value));
      break;
    case 4:
      _value *= m_params.m_utilities[4].m_normalize;
      m_test_tone.set_state(static_cast<uint32_t>(_value));
      break;
    case 5:
      muteOsc(0, static_cast<uint32_t>(_value));
      break;
    case 6:
      muteOsc(1, static_cast<uint32_t>(_value));
      break;
  }
}

/* */
void dsp_host::listUpdate(float _dest)
{
  /* distinguish list ids */
  switch(m_decoder.m_listId)
  {
    case 0:
      /* ignore */
      break;
    case 1:
      /* recall list traversal */
      m_params.setDest(0, m_decoder.traverseRecall(), _dest);
      break;
    case 2:
      /* key event traversal (polyphonic - voice selection up to sender! */
      m_params.setDest(m_decoder.m_voiceFrom, m_decoder.traverseKeyEvent(), _dest);
      break;
  }
}

/* */
void dsp_host::resetUpdate(uint32_t _mode)
{
  switch(_mode)
  {
    case 0:
      // execute flush
      m_flush = true;
      break;
    case 1:
      // execute envelope stop
      resetEnv();
      break;
    case 2:
      // execute dsp reset
      resetDSP();
      break;
  }
}

/* */
void dsp_host::keyUp(uint32_t _voiceId, float _velocity)
{
  /* update values */
  m_params.keyUp(_voiceId, _velocity);
  /* handle preload */
  if(m_params.m_preload == 0)
  {
    /* direct key apply */
    m_params.keyApplyMono();
    m_params.keyApply(_voiceId);
    keyApply(_voiceId);  // not really necessary...
  }
  else
  {
    /* preload key event (increase preload counters) */
    m_params.m_event.m_mono.m_preload++;
    m_params.m_event.m_poly[_voiceId].m_preload++;
  }
}

/* */
void dsp_host::keyDown(uint32_t _voiceId, float _velocity)
{
  /* update values */
  m_params.keyDown(_voiceId, _velocity);
  /* handle preload */
  if(m_params.m_preload == 0)
  {
    /* direct key apply */
    m_params.keyApplyMono();
    m_params.keyApply(_voiceId);
    keyApply(_voiceId);
  }
  else
  {
    /* preload key event (increase preload counters) */
    m_params.m_event.m_mono.m_preload++;
    m_params.m_event.m_poly[_voiceId].m_preload++;
  }
}

/* */
void dsp_host::keyApply(uint32_t _voiceId)
{
  /* keyDown events cause triggers to the AUDIO_ENGINE */
  if(m_params.m_event.m_poly[_voiceId].m_type == 1)
  {
    /*Audio DSP trigger */
    m_combfilter.setDelaySmoother(_voiceId);

    /* determine note steal */
#if test_milestone < 156
    if(static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_KEY_VS)) == 1)
    {
      /* AUDIO_ENGINE: trigger voice-steal */
    }
    else
    {
      /* AUDIO_ENGINE: trigger non-voice-steal */
    }
#elif test_milestone == 156
    if(m_stolen == 1)
    {
      /* AUDIO_ENGINE: trigger voice-steal */
    }
    else
    {
      /* AUDIO_ENGINE: trigger non-voice-steal */
    }
#endif
    /* OLD approach of phase reset - over shared array */
    /* update and reset oscillator phases */
    //m_paramsignaldata[_voiceId][OSC_A_PHS] = m_params.m_body[m_params.m_head[Parameters::P_KEY_PA].m_index + _voiceId].m_signal;  // POLY PHASE_A -> OSC_A Phase

    /* AUDIO_ENGINE: reset oscillator phases */
    //resetOscPhase(m_paramsignaldata[_voiceId], _voiceId);
    /* NEW approach of phase reset - no array involved - still only unisono phase */

    /* NEW UNISONO PHASE FUNCTIONALITY, Osc Phases are now offsets */
    //float phaseA = m_params.m_body[m_params.m_head[Parameters::P_KEY_PA].m_index + _voiceId].m_signal;
    //float phaseB = m_params.m_body[m_params.m_head[Parameters::P_KEY_PB].m_index + _voiceId].m_signal;
#if test_milestone == 150
    const float startPhase = m_params.getParameterValue(Parameters::P_KEY_PH, _voiceId);
#elif test_milestone == 155
    const float startPhase = 0.f;
#elif test_milestone == 156
    const float startPhase = 0.f;
#endif
    //m_soundgenerator[_voiceId].resetPhase(phaseA, phaseB);
    m_soundgenerator.resetPhase(startPhase, _voiceId);  // function could be reduced to one argument
  }
}

#if test_milestone == 156

void dsp_host::keyUp156(const float _velocity)
{
#if test_preload_update != 2
  uint32_t voiceId = m_decoder.m_voiceFrom;
  const uint32_t unisonVoices = 1 + static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_UN_V));

  for(uint32_t v = 0; v < unisonVoices; v++)
  {
    keyUp(voiceId, _velocity);
    voiceId++;
  }

  preloadUpdate(2, 0);
#else
  // preparation
  const uint32_t voiceId = m_decoder.m_voiceFrom;
  const uint32_t uVoice = static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_UN_V));
  const uint32_t unisonVoices = 1 + uVoice;
  const uint32_t index = m_params.getHead(Parameters::P_KEY_BP).m_index + voiceId;
  const float velocity = m_params.scale(m_params.m_utilities[0].m_scaleId, m_params.m_utilities[0].m_scaleArg,
                                        _velocity * m_params.m_utilities[0].m_normalize);
  const float uniDetune = m_params.getParameterValue(Parameters::P_UN_DET);
  const float masterTune = m_params.getParameterValue(Parameters::P_MA_T);
  //m_params.m_event.m_mono.m_velocity = velocity;
  //m_params.m_event.m_mono.m_type = 0;
  // disable preload
  m_params.getBody(index).m_preload = 0;
  m_params.m_preload = 0;
  m_decoder.m_listId = 0;
  // unison loop
  /** !! !! !!
     * NOTE that at Milestone 1.56, the LPC still performs the Voice Allocation,
     * and Unison Clusters are GUARANTEED to always be within the allowed number of Voices,
     * never rising above or wrapping around. Example: Unison Voices = 3 --> possible Unison Clusters:
     * [0,1,2], [3,4,5], [6,7,8], [9,10,11], [12,13,14], [15,16,17] < 20
     * !! !! !! **/
  for(uint32_t v = 0; v < unisonVoices; v++)
  {
    const uint32_t voicePos = voiceId + v;
    //m_params.m_event.m_poly[voicePos].m_velocity = velocity;
    //m_params.m_event.m_poly[voicePos].m_type = 0;
    float notePitch = m_params.getBody(index + v).m_value + (uniDetune * m_params.m_unison_detune[uVoice][v])
        + masterTune + m_params.m_note_shift[voicePos];
    m_params.newEnvUpdateStop(voicePos, notePitch, velocity);
  }
#if test_flanger_env_legato == 1
  m_params.m_event.m_active--;
#endif
#endif
}

void dsp_host::keyDown156(const float _velocity)
{
#if test_preload_update != 2
  uint32_t voiceId = m_decoder.m_voiceFrom;
  const uint32_t unisonVoices = 1 + static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_UN_V));
  const uint32_t index = m_params.getHead(Parameters::P_KEY_BP).m_index + voiceId;
  const float pitch = m_params.getBody(index).m_dest;
  //
  for(uint32_t v = 0; v < unisonVoices; v++)
  {
    if(v > 0)
    {
      m_params.getBody(index + v).m_dest = pitch;
      m_params.getBody(index + v).m_preload++;
    }
    m_params.m_unison_index[voiceId] = v;
    keyDown(voiceId, _velocity);
    voiceId++;
  }

  preloadUpdate(2, 0);
#else
  // preparation
  const uint32_t voiceId = m_decoder.m_voiceFrom;
  const uint32_t uVoice = static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_UN_V));
  const uint32_t unisonVoices = 1 + uVoice;
  const uint32_t index = m_params.getHead(Parameters::P_KEY_BP).m_index + voiceId;
  const float pitch = m_params.getBody(index).m_dest;
  const float velocity = m_params.scale(m_params.m_utilities[0].m_scaleId, m_params.m_utilities[0].m_scaleArg,
                                        _velocity * m_params.m_utilities[0].m_normalize);
  const float noteShift = m_params.getParameterValue(Parameters::P_MA_SH);
  const float uniDetune = m_params.getParameterValue(Parameters::P_UN_DET);
  const float masterTune = m_params.getParameterValue(Parameters::P_MA_T);
  //m_params.m_event.m_mono.m_velocity = velocity;
  //m_params.m_event.m_mono.m_type = 1;
  // disable preload
  m_params.getBody(index).m_preload = 0;
  m_params.m_preload = 0;
  m_decoder.m_listId = 0;
  // unison loop
  /** !! !! !!
     * NOTE that at Milestone 1.56, the LPC still performs the Voice Allocation,
     * and Unison Clusters are GUARANTEED to always be within the allowed number of Voices,
     * never rising above or wrapping around. Example: Unison Voices = 3 --> possible Unison Clusters:
     * [0,1,2], [3,4,5], [6,7,8], [9,10,11], [12,13,14], [15,16,17] < 20
     * !! !! !! **/
  for(uint32_t v = 0; v < unisonVoices; v++)
  {
    const uint32_t voicePos = voiceId + v;
    m_params.m_note_shift[voicePos] = noteShift;
    m_params.getBody(index + v).m_value = pitch;
    m_params.m_unison_index[voicePos] = v;
    //m_params.m_event.m_poly[voicePos].m_velocity = velocity;
    //m_params.m_event.m_poly[voicePos].m_type = 1;
    float notePitch
        = pitch + (uniDetune * m_params.m_unison_detune[uVoice][v]) + masterTune + m_params.m_note_shift[voicePos];

    m_params.postProcessPoly_key(m_parameters, voicePos);
    setPolySlowFilterCoeffs(m_parameters, voicePos);
    m_combfilter.setDelaySmoother(voicePos);

    //
#if test_milestone < 156
    if(static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_KEY_VS)) == 1)
    {
      /* AUDIO_ENGINE: trigger voice-steal */
    }
    else
    {
      /* AUDIO_ENGINE: trigger non-voice-steal */
    }
#if test_milestone == 150
    const float startPhase = m_params.getParameterValue(Parameters::P_KEY_PH, voicePos);
#else
    const float startPhase = 0.f;
#endif
#elif test_milestone == 156
    if(m_stolen == 1)
    {
      /* AUDIO_ENGINE: trigger voice-steal */
    }
    else
    {
      /* AUDIO_ENGINE: trigger non-voice-steal */
    }
    const float startPhase = 0.f;
#endif

    m_soundgenerator.resetPhase(startPhase, voicePos);
    m_params.newEnvUpdateStart(voicePos, notePitch, velocity);
  }
  // keyApplyMono
#if test_flanger_env_legato == 0
  m_params.m_new_envelopes.m_env_f.setSegmentDest(0, 1, velocity);
  m_params.m_new_envelopes.m_env_f.start(0);
#elif test_flanger_env_legato == 1
  if(m_params.m_event.m_active == 0)
  {
    m_params.m_new_envelopes.m_env_f.setSegmentDest(0, 1, velocity);
    m_params.m_new_envelopes.m_env_f.start(0);
  }
  m_params.m_event.m_active++;
#endif
#endif
}

#endif

/* End of Main Definition, Test functionality below:
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *
 */

#if test_inputModeFlag == 1

/* midi input for test purposes (reMote) */
void dsp_host::testMidi(uint32_t _status, uint32_t _data0, uint32_t _data1)
{
  /* */
  //uint32_t chan = _status & 15;
  uint32_t type = (_status & 127) >> 4;
  /* */
  switch(type)
  {
    case 0:
      /* NOTE OFF (explicit) */
      if(_data0 > 127)
      {
        printf("ooooooh ... exp %d \n", _data0);
      }
#if test_milestone == 150
      testNoteOff(_data0, _data1);
#elif test_milestone == 155
      testNewNoteOff(_data0, _data1);
#elif test_milestone == 156
      testNoteOff156(_data0, _data1);
#endif
      break;
    case 1:
      /* NOTE ON (if velocity > 0) */
      if(_data1 > 0)
      {
#if test_milestone == 150
        testNoteOn(_data0, _data1);
#elif test_milestone == 155
        testNewNoteOn(_data0, _data1);
#elif test_milestone == 156
        testNoteOn156(_data0, _data1);
#endif
      }
      else
      {
        if(_data0 > 127)
        {
          printf("ooooooh ... exp %d \n", _data0);
        }
#if test_milestone == 150
        testNoteOff(_data0, _data1);
#elif test_milestone == 155
        testNewNoteOff(_data0, _data1);
#endif
      }
      break;
    case 3:
      /* CONTROL CHANGE */
      testRouteControls(testMidiMapping[m_test_midiMode][_data0], _data1);
      break;
  }
}

/* control and mode routing */
void dsp_host::testRouteControls(uint32_t _id, uint32_t _value)
{
  uint32_t type = _id >> 5;
  _id &= 31;
  switch(type)
  {
    case 0:
      /* main triggers */
      switch(_id)
      {
        case 1:
          /* Group Mode */
          nltools::Log::info("enabled GROUP mode");
          m_test_midiMode = 0;
          break;
        case 2:
          nltools::Log::info("enabled GLOBAL mode");
          m_test_midiMode = 1;
          m_test_selectedParam = -1;
          /* Global Mode */
          break;
        case 3:
          if(m_test_midiMode == 0)
          {
            if(m_test_selectedParam > -1)
            {
              /* NULL selected param */
              uint32_t tmp_p = static_cast<uint32_t>(m_test_selectedParam);  // cast as unsigned int
              nltools::Log::info("NULL Param (", tmp_p, ")");                // print to terminal
              /* TCD instructions */
              evalMidi(0, 127, 127);                 // select all voices
              evalMidi(1, tmp_p >> 7, tmp_p & 127);  // select corresponding param
              evalMidi(5, 0, 0);                     // send zero destination
              evalMidi(0, 0, 0);                     // select voice 0
              evalMidi(1, 0, 0);                     // select param 0
            }
          }
          else
          {
            /* Print Head */
            nltools::Log::info("print parameters: HEAD");
            testGetParamHeadData();
          }
          break;
        case 4:
          /* Print Body */
          nltools::Log::info("print parameters: BODY");
          testGetParamRenderData();
          break;
        case 5:
          /* Print Signal */
          nltools::Log::info("print parameters: SIGNAL");
          testGetSignalData();
          break;
        case 6:
          /* Init */
          nltools::Log::info("triggered INIT");
          testInit();
          break;
        case 7:
          /* Flush */
          nltools::Log::info("triggered FLUSH");
          testReset(0);
          break;
        case 8:
          /* Preset 0 */
          nltools::Log::info("load PRESET 0");
          testLoadPreset(0);
          break;
        case 9:
          /* Preset 1 */
          nltools::Log::info("load PRESET 1");
          testLoadPreset(1);
          break;
        case 10:
          /* Preset 2 */
          nltools::Log::info("load PRESET 2");
          testLoadPreset(2);
          break;
        case 11:
          /* Preset 3 */
          nltools::Log::info("load PRESET 3");
          testLoadPreset(3);
          break;
        case 12:
          /* Preset 4 */
          nltools::Log::info("load PRESET 4");
          testLoadPreset(4);
          break;
        case 13:
          /* Preset 5 */
          nltools::Log::info("load PRESET 5");
          testLoadPreset(5);
          break;
        case 14:
          /* Preset 6 */
          nltools::Log::info("load PRESET 6");
          testLoadPreset(6);
          break;
        case 15:
          /* Preset 7 */
          nltools::Log::info("load PRESET 7");
          testLoadPreset(7);
          break;
        case 16:
          // Envelope Stop
          nltools::Log::info("triggered ENV_STOP");
          testReset(1);
          break;
        case 17:
          // DSP Reset
          nltools::Log::info("triggered DSP_RESET");
          testReset(2);
          break;
        case 18:
          // Trigger Test Tone
          m_test_tone_state = 1 - m_test_tone_state;
          nltools::Log::info("triggered TEST_TONE_STATE(", m_test_tone_state, ")");
          evalMidi(8, 0, 4);                   // select utility (test tone state)
          evalMidi(24, 0, m_test_tone_state);  // update utility
          break;
        case 19:
          // Mute OSC A
          m_test_muteA = 1 - m_test_muteA;
          nltools::Log::info("OSC_A_Mute:", m_test_muteA);
          evalMidi(8, 0, 5);
          evalMidi(24, 0, m_test_muteA);
          break;
        case 20:
          // Mute OSC B
          m_test_muteB = 1 - m_test_muteB;
          nltools::Log::info("OSC_B_Mute:", m_test_muteB);
          evalMidi(8, 0, 6);
          evalMidi(24, 0, m_test_muteB);
          break;
      }
      break;
    case 1:
      /* group select */
      m_test_selectedGroup = _id;
      switch(_id)
      {
        case 1:
          /* Env A */
          nltools::Log::info("selected ENVELOPE_A");
          break;
        case 2:
          /* Env B */
          nltools::Log::info("selected ENVELOPE_B");
          break;
        case 3:
          /* Env C */
          nltools::Log::info("selected ENVELOPE_C");
          break;
        case 4:
          /* Osc A */
          nltools::Log::info("selected OSCILLATOR_A");
          break;
        case 5:
          /* Shp A */
          nltools::Log::info("selected SHAPER_A");
          break;
        case 6:
          /* Osc B */
          nltools::Log::info("selected OSCILLATOR_B");
          break;
        case 7:
          /* Shp B */
          nltools::Log::info("selected SHAPER_B");
          break;
        case 8:
          /* Out Mix */
          nltools::Log::info("selected OUTPUT_MIXER");
          break;
        case 9:
          /* Comb Filter */
          nltools::Log::info("selected COMB_FILTER");
          break;
        case 10:
          /* State Variable Filter */
          nltools::Log::info("selected STATE_VARIABLE_FILTER");
          break;
        case 11:
          /* Cabinet & Gap Filter */
          nltools::Log::info("selected CABINET_&_GAP_FILTER");
          break;
        case 12:
          /* Flanger */
          nltools::Log::info("selected FLANGER");
          break;
        case 13:
          /* Echo and Reverb */
          nltools::Log::info("selected ECHO_&_REVERB");
          break;
        case 16:
          /* FB Mix */
          nltools::Log::info("selected FEEDBACK MIXER");
          break;
        case 17:
          /* Master */
          nltools::Log::info("selected MASTER");
          break;
      }
      break;
    case 2:
      /* control edits */
      if(m_test_midiMode == 0)
      {
        auto pId = static_cast<uint32_t>(testParamRouting[m_test_selectedGroup][_id - 1]);
        /* group edits */
        if(pId != static_cast<uint32_t>(Parameters::P_INVALID))
        {
          uint32_t tcdId = param_definition[pId].id;
          uint32_t rng = param_definition[pId].factor;
          uint32_t pol = param_definition[pId].pol;
          float val = _value * m_test_normalizeMidi;
          if(pol > 0)
          {
            val = (2 * val) - 1;
          }
          val *= rng;
          nltools::Log::info("edit PARAM ", tcdId, "(", val, ")");
          /* get unison voices trigger and stop envelopes */
          if(tcdId == 249)
          {
            nltools::Log::info("\tEnvelope Stop");
            resetEnv();
            val += 1;
          }
          evalMidi(1, tcdId >> 7, tcdId & 127);
          testParseDestination(static_cast<int32_t>(val));
          m_test_selectedParam = static_cast<int32_t>(tcdId);
        }
      }
      else
      {
        /* global edits */
        switch(_id)
        {
          case 1:
            /* reference tone */
            testSetReference(80 * _value);
            break;
          case 2:
            /* transition time */
            testSetGlobalTime(_value);
            break;
          case 3:
            // test tone frequency
            testSetToneFreq(_value);
            break;
          case 4:
            // test tone amplitude
            testSetToneAmp(_value);
            break;
        }
      }
      break;
  }
}

/* test key down */
void dsp_host::testNoteOn(uint32_t _pitch, uint32_t _velocity)
{
  //std::cout << "Unison: " << m_params.m_body[m_params.m_head[183].m_index].m_signal << std::endl;
  /* get current voiceId and trigger list sequence for key event */
  m_test_noteId[_pitch] = m_test_voiceId + 1;  // (plus one in order to distinguish from zero)
  /* prepare pitch and velocity */
  int32_t notePitch = (static_cast<int32_t>(_pitch) - 60) * 1000;
  uint32_t noteVel
      = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0].range);
  /* key event sequence */
  evalMidi(47, 2, 1);              // enable preload (key event list mode)
  evalMidi(0, 0, m_test_voiceId);  // select voice: current
  //testParseDestination(par_key_phaseA);                   // phase a (see pe_defines.config.h)
  //testParseDestination(par_key_phaseB);                   // phase b (see pe_defines.config.h)
  testParseDestination(par_unisono_phase);  // unisono phase
  testParseDestination(notePitch);          // note pitch
  testParseDestination(par_voice_pan);      // voice pan (see pe_defines.config.h)
  //evalMidi(5, 0, 0);                                      // env c rate: 0 (will be removed later - almost certainly)
  evalMidi(5, 0, 0);                          // voice steal: 0
  evalMidi(23, noteVel >> 7, noteVel & 127);  // key down: velocity
#if test_unisonCluster == 0
  evalMidi(47, 0, 2);  // apply preloaded values
  /* take current voiceId and increase it (wrapped around polyphony) - sloppy approach */
  m_test_voiceId = (m_test_voiceId + 1) % m_voices;
#elif test_unisonCluster == 1
  //testParseDestination(par_key_phaseA);                   // phase a (see pe_defines.config.h)
  //testParseDestination(par_key_phaseB);                   // phase b (see pe_defines.config.h)
  testParseDestination(par_unisono_phase);  // unisono phase
  testParseDestination(notePitch + 12000);  // note pitch (one octave apart)
  testParseDestination(par_voice_pan);      // voice pan (see pe_defines.config.h)
  //evalMidi(5, 0, 0);                                      // env c rate: 0 (will be removed later - almost certainly)
  evalMidi(5, 0, 0);                          // voice steal: 0
  evalMidi(23, noteVel >> 7, noteVel & 127);  // key down: velocity
  evalMidi(47, 0, 2);                         // apply preloaded values
  /* take current voiceId and increase it (wrapped around polyphony) - sloppy approach */
  m_test_voiceId = (m_test_voiceId + 2) % m_voices;
#endif
}

/* key down for milestone 1.55 */
void dsp_host::testNewNoteOn(uint32_t _pitch, uint32_t _velocity)
{
  m_test_noteId[_pitch] = m_test_voiceId + 1;  // (plus one in order to distinguish from zero)
  /* prepare pitch, velocity und unison */
  int32_t keyPos = static_cast<int32_t>(_pitch) - 60;
  uint32_t noteVel
      = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0].range);
  m_test_unison_voices = static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_UN_V)) + 1;
  evalMidi(47, 2, 1);              // enable preload (key event list mode)
  evalMidi(0, 0, m_test_voiceId);  // select voice: current
  for(uint32_t uIndex = 0; uIndex < m_test_unison_voices; uIndex++)
  {
    testParseDestination(keyPos * 1000);        // base pitch (factor 1000 because of Scaling)
    evalMidi(5, 0, uIndex);                     // unison index
    evalMidi(5, 0, 0);                          // voice steal (0)
    evalMidi(23, noteVel >> 7, noteVel & 127);  // key down: velocity
  }
  evalMidi(47, 0, 2);  // apply preloaded values
  /* take current voiceId and increase it (wrapped around polyphony) - sloppy approach */
  m_test_voiceId = (m_test_voiceId + m_test_unison_voices) % m_voices;
}

#if test_milestone == 156

void dsp_host::testNoteOn156(uint32_t _pitch, uint32_t _velocity)
{
  m_test_noteId[_pitch] = m_test_voiceId + 1;  // (plus one in order to distinguish from zero)
  /* prepare pitch, velocity und unison */
  int32_t keyPos = static_cast<int32_t>(_pitch) - 60;
  uint32_t noteVel
      = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0].range);
  m_test_unison_voices = static_cast<uint32_t>(m_params.getParameterValue(Parameters::P_UN_V)) + 1;
  /* */
  evalMidi(55, 0, (m_test_voiceId << 1) + 0);  // new keyVoice (current voice, no steal)
  testParseDestination(keyPos * 1000);         // base pitch (factor 1000 because of Scaling)
  evalMidi(23, noteVel >> 7, noteVel & 127);   // key down: velocity
  /* take current voiceId and increase it (wrapped around polyphony) - sloppy approach */
  m_test_voiceId = (m_test_voiceId + m_test_unison_voices) % m_voices;
}

#endif

/* test key up */
void dsp_host::testNoteOff(uint32_t _pitch, uint32_t _velocity)
{
  /* rigorous safety mechanism */
  int32_t checkVoiceId = static_cast<int32_t>(m_test_noteId[_pitch]) - 1;  // (subtract one in order to get real id)
  int32_t v
      = static_cast<int32_t>(m_voices);  // number of voices represented as signed integer (for correct comparisons)
  if((checkVoiceId < 0) || (checkVoiceId >= v))
  {
    nltools::Log::warning("detected Note Off that shouldn't have happened...");
  }
  else
  {
    uint32_t usedVoiceId = static_cast<uint32_t>(checkVoiceId);  // copy valid voiceId
    m_test_noteId[_pitch] = 0;                                   // clear voiceId assignment
    /* prepare velocity */
    uint32_t noteVel
        = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0].range);
    /* key event sequence */
    evalMidi(47, 2, 1);                        // enable preload (key event list mode)
    evalMidi(0, 0, usedVoiceId);               // select voice: used voice (by note number)
    evalMidi(7, noteVel >> 7, noteVel & 127);  // key up: velocity
#if test_unisonCluster == 0
    evalMidi(47, 0, 2);  // apply preloaded values
#elif test_unisonCluster == 1
    evalMidi(7, noteVel >> 7, noteVel & 127);  // key up: velocity
    evalMidi(47, 0, 2);                        // apply preloaded values
#endif
  }
}

/* */
void dsp_host::testNewNoteOff(uint32_t _pitch, uint32_t _velocity)
{
  /* rigorous safety mechanism */
  int32_t checkVoiceId = static_cast<int32_t>(m_test_noteId[_pitch]) - 1;  // (subtract one in order to get real id)
  int32_t v
      = static_cast<int32_t>(m_voices);  // number of voices represented as signed integer (for correct comparisons)
  if((checkVoiceId < 0) || (checkVoiceId >= v))
  {
    nltools::Log::warning("detected Note Off that shouldn't have happened...");
  }
  else
  {
    uint32_t usedVoiceId = static_cast<uint32_t>(checkVoiceId);  // copy valid voiceId
    m_test_noteId[_pitch] = 0;                                   // clear voiceId assignment
    /* prepare velocity */
    uint32_t noteVel
        = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0].range);
    /* key event sequence */
    evalMidi(47, 2, 1);           // enable preload (key event list mode)
    evalMidi(0, 0, usedVoiceId);  // select voice: used voice (by note number)
    for(uint32_t uIndex = 0; uIndex < m_test_unison_voices; uIndex++)
    {
      evalMidi(7, noteVel >> 7, noteVel & 127);  // key up: velocity
    }
    evalMidi(47, 0, 2);  // apply preloaded values
  }
}

#if test_milestone == 156

void dsp_host::testNoteOff156(uint32_t _pitch, uint32_t _velocity)
{
  /* rigorous safety mechanism */
  int32_t checkVoiceId = static_cast<int32_t>(m_test_noteId[_pitch]) - 1;  // (subtract one in order to get real id)
  int32_t v
      = static_cast<int32_t>(m_voices);  // number of voices represented as signed integer (for correct comparisons)
  if((checkVoiceId < 0) || (checkVoiceId >= v))
  {
    nltools::Log::warning("detected Note Off that shouldn't have happened...");
  }
  else
  {
    uint32_t usedVoiceId = static_cast<uint32_t>(checkVoiceId);  // copy valid voiceId
    m_test_noteId[_pitch] = 0;                                   // clear voiceId assignment
    /* prepare velocity */
    uint32_t noteVel
        = static_cast<uint32_t>(static_cast<float>(_velocity) * m_test_normalizeMidi * utility_definition[0].range);
    /* key event sequence */
    evalMidi(55, 0, (usedVoiceId << 1) + 0);   // new keyVoice (current voice, no steal)
    evalMidi(7, noteVel >> 7, noteVel & 127);  // key up: velocity
  }
}

#endif

/* set transition time */
void dsp_host::testSetGlobalTime(uint32_t _value)
{
  nltools::Log::info("\nSET_TIME(", _value, "(ms))");
  /* select all voices, params and update time */
  _value *= static_cast<uint32_t>(m_params.m_millisecond);  // convert time accordingly in samples
  evalMidi(0, 127, 127);                                    // select all voices
  evalMidi(1, 127, 127);                                    // select all params
  if(_value < 16384)
  {
    /* T */
    evalMidi(2, _value >> 7, _value & 127);  // set time
  }
  else
  {
    /* TU + TL */
    uint32_t upper = _value >> 14;
    _value &= 16383;
    evalMidi(34, upper >> 7, upper & 127);    // set time upper
    evalMidi(50, _value >> 7, _value & 127);  // set time lower
  }
  evalMidi(0, 0, 0);  // select voice 0
  evalMidi(1, 0, 0);  // select param 0
}

/* set reference frequency */
void dsp_host::testSetReference(uint32_t _value)
{
  /* prepare value */
  uint32_t val = static_cast<uint32_t>(static_cast<float>(_value) * m_test_normalizeMidi * utility_definition[1].range);
  nltools::Log::info("\nSET_REFERENCE(", val, ")");
  /* select and update reference utility */
  evalMidi(8, 0, 1);                  // select utility (reference tone)
  evalMidi(24, val >> 7, val & 127);  // update utility
}

/* set test tone frequency  */
void dsp_host::testSetToneFreq(uint32_t _value)
{
  _value += 400;
  nltools::Log::info("Set_TestTone_Frequency:", _value, "[Hz]");
  evalMidi(8, 0, 2);                        // select utility (test tone freq)
  evalMidi(24, _value >> 7, _value & 127);  // update utility
}

/* set test tone amplitude */
void dsp_host::testSetToneAmp(uint32_t _value)
{
  int32_t val = static_cast<int32_t>(_value) - 127;
  nltools::Log::info("Set_TestTone_Amplitude:", val, "[dB]");
  if(val < 0)
  {
    _value = 8192 + static_cast<uint32_t>(val * -1);
  }
  else
  {
    _value = static_cast<uint32_t>(val);
  }
  evalMidi(8, 0, 3);                        // select utility (test tone amp)
  evalMidi(24, _value >> 7, _value & 127);  // update utility
}

/* preset recall approach */
void dsp_host::testLoadPreset(uint32_t _presetId)
{
  nltools::Log::info("RECALL:", _presetId);
  /* run a recall sequence based on the given preset id (predefined presets in pe_defines_testconfig.h) */
  /* recall sequence - no flush */
  evalMidi(47, 1, 1);  // enable preload (recall list mode)
  for(uint32_t p = 0; p < lst_recall_length; p++)
  {
    /* traverse normalized recall array */
    testParseDestination(testPresetData[_presetId][p]);
  }
  evalMidi(47, 0, 2);  // apply preloaded values
}

/* trigger flush */
void dsp_host::testReset(uint32_t _mode)
{
  nltools::Log::info("RESET:", _mode);
  /* pass the trigger TCD message */
  evalMidi(39, 0, _mode);  // flush/env_stop/dsp_reset
}

/* glance at current signals */
void dsp_host::testGetSignalData()
{
  /* print out the signal array to the terminal */
  nltools::Log::info("PARAM_SIGNAL:");
  for(uint32_t p = 0; p < sig_number_of_signal_items; p++)
  {
    nltools::Log::info<nltools::Log::LogMode::InsertSpaces>(p, "-");
    for(uint32_t v = 0; v < m_voices; v++)
    {
      nltools::Log::info<nltools::Log::LogMode::Plain>(m_parameters.getOld(v, static_cast<Signals>(p)), ", ");
    }
    nltools::Log::info();
  }
  /* print out the left and right output signal to the terminal */
  nltools::Log::info("OUTPUT_SIGNAL:", m_mainOut_L, ",", m_mainOut_R);
}

/* glance at parameter definition */
void dsp_host::testGetParamHeadData()
{
  /* print out the parameter definitions to the terminal */
  nltools::Log::info("PARAM_HEAD:");
  for(uint32_t p = 0; p < sig_number_of_params; p++)
  {
    auto parameterId = static_cast<Parameters>(p);
    param_head *obj = &m_params.getHead(parameterId);
    nltools::Log::info<nltools::Log::LogMode::Plain>("id: ", obj->m_id, ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("index: ", obj->m_index, ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("size: ", obj->m_size, ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("clock: ", static_cast<uint32_t>(obj->m_clockType), ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("poly: ", static_cast<uint32_t>(obj->m_polyType), ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("scaleId: ", static_cast<uint32_t>(obj->m_scaleId), ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("postId: ", static_cast<uint32_t>(obj->m_postId), ", ");
    nltools::Log::info<nltools::Log::LogMode::Plain>("norm: ", obj->m_normalize, ", ");
    nltools::Log::info<nltools::Log::LogMode::AppendNewLine>("scaleArg: ", obj->m_scaleArg);
  }
}

/* glance at parameter rendering status */
void dsp_host::testGetParamRenderData()
{
  /* print out the parameter rendering status to the terminal */
  nltools::Log::info("PARAM_BODY:");
  for(uint32_t p = 0; p < sig_number_of_params; p++)
  {
    auto parameterId = static_cast<Parameters>(p);
    param_head *obj = &m_params.getHead(parameterId);
    uint32_t index = obj->m_index;
    for(uint32_t i = 0; i < obj->m_size; i++)
    {
      param_body *item = &m_params.getBody(index);
      nltools::Log::info<nltools::Log::LogMode::Plain>("P(", obj->m_id, ", ", i, "):\t");
      nltools::Log::info<nltools::Log::LogMode::Plain>("state: ", item->m_state, ",\tpreload: ", item->m_preload);
      nltools::Log::info<nltools::Log::LogMode::Plain>(",\tsignal: ", item->m_value, ",\tdx:[", item->m_dx[0], ", ",
                                                       item->m_dx[1], "]");
      nltools::Log::info<nltools::Log::LogMode::Plain>(",\tx: ", item->m_x, ",\tstart: ", item->m_start);
      nltools::Log::info<nltools::Log::LogMode::AppendNewLine>(",\tdiff: ", item->m_diff, ",\tdest: ", item->m_dest);
      index++;
    }
  }
}

/* prepare destinations */
void dsp_host::testParseDestination(int32_t _value)
{
  /* prepare value */
  uint32_t val = static_cast<uint32_t>(std::abs(_value));
  uint32_t upper = val >> 14;
  /* determine fitting destina)tion format */
  if(_value < -8191)
  {
    /* DU + DL (negative) */
    evalMidi(37, (upper >> 7) + 64, upper & 127);
    evalMidi(53, (val & 16383) >> 7, val & 127);
  }
  else if(_value < 0)
  {
    /* DS (negative) */
    evalMidi(21, (val >> 7) + 64, val & 127);
  }
  else if(_value < 16384)
  {
    /* D */
    evalMidi(5, val >> 7, val & 127);
  }
  else
  {
    /* DU + DL (positive) */
    evalMidi(37, upper >> 7, upper & 127);
    evalMidi(53, (val & 16383) >> 7, val & 127);
  }
}

/* init procedure */
void dsp_host::testInit()
{
  /* */
  nltools::Log::info("INIT SEQUENCE");
  evalMidi(0, 127, 127);  // select all voices
  evalMidi(1, 127, 127);  // select all parameters
  testSetGlobalTime(dsp_initial_time);
  testLoadPreset(1);  // load default preset
}

#endif

/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::initAudioEngine()
{
  //******************************** Flushing ******************************//
  float fade_time = 0.003f;
  float sample_interval = 1.f / m_samplerate;

  m_table_indx = 0;
  m_raised_cos_table.resize(static_cast<uint32_t>(fade_time * static_cast<float>(m_samplerate)) * 2 + 2);
  m_flush_indx = static_cast<uint32_t>((m_raised_cos_table.size() - 1) / 2);

  for(uint32_t ind = 0; ind < m_raised_cos_table.size(); ind++)
  {
    if(ind == m_flush_indx + 1)
    {
      m_raised_cos_table[ind] = m_raised_cos_table[ind - 1];
    }
    else if(ind < m_flush_indx)
    {
      float x = 1.5708f * sample_interval * ind / fade_time;
      m_raised_cos_table[ind] = std::pow(std::cos(x), 2.f);
    }
    else if(ind > m_flush_indx)
    {
      float x = 1.5708f * sample_interval * (ind - 1) / fade_time;
      m_raised_cos_table[ind] = std::abs(std::pow(std::cos(x), 2.f));
    }
  }

  m_flush = false;

  //****************************** DSP Modules *****************************//
  m_soundgenerator.init(static_cast<float>(m_samplerate));
  m_combfilter.init(static_cast<float>(m_samplerate), m_upsampleFactor);
  m_svfilter.init(static_cast<float>(m_samplerate));
  m_feedbackmixer.init(static_cast<float>(m_samplerate));

  m_outputmixer.init(static_cast<float>(m_samplerate), m_voices);
  m_cabinet.init(static_cast<float>(m_samplerate));
  m_gapfilter.init(static_cast<float>(m_samplerate));
  m_echo.init(static_cast<float>(m_samplerate), m_upsampleFactor);
  m_flanger.init(static_cast<float>(m_samplerate), m_upsampleFactor);
  m_reverb.init(static_cast<float>(m_samplerate), m_upsampleFactor);
  m_test_tone.init(static_cast<float>(m_samplerate));
}

/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::makePolySound(SignalStorage &signals)
{
  m_soundgenerator.generate(m_feedbackmixer.m_out, signals);
  m_combfilter.apply(m_soundgenerator.m_out_A, m_soundgenerator.m_out_B, signals);
  m_svfilter.apply(m_soundgenerator.m_out_A, m_soundgenerator.m_out_B, m_combfilter.m_out, signals);
  m_feedbackmixer.apply(m_combfilter.m_out, m_svfilter.m_out, m_reverb.m_out_FX, signals);
  m_soundgenerator.m_feedback_phase = m_feedbackmixer.m_out;
  m_outputmixer.combine(m_soundgenerator.m_out_A, m_soundgenerator.m_out_B, m_combfilter.m_out, m_svfilter.m_out,
                        signals);
}

/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::makeMonoSound(SignalStorage &signals)
{
  float mst_gain = signals.get<Signals::MST_VOL>() * m_raised_cos_table[m_table_indx];

  //****************************** Mono Modules ****************************//
  m_outputmixer.filter_level(signals);
  m_flanger.apply(m_outputmixer.m_out_L, m_outputmixer.m_out_R, signals);
  m_cabinet.apply(m_flanger.m_out_L, m_flanger.m_out_R, signals);
  m_gapfilter.apply(m_cabinet.m_out_L, m_cabinet.m_out_R, signals);
  m_echo.apply(m_gapfilter.m_out_L, m_gapfilter.m_out_R, signals);
  m_reverb.apply(m_echo.m_out_L, m_echo.m_out_R, signals);

  //******************************* Soft Clip ******************************//
  //****************************** Left Channel ****************************//
  m_mainOut_L = m_reverb.m_out_L * mst_gain;

  m_mainOut_L *= 0.1588f;
  m_mainOut_L = std::clamp(m_mainOut_L, -0.25f, 0.25f);

  m_mainOut_L += -0.25f;
  m_mainOut_L += m_mainOut_L;

  m_mainOut_L = 0.5f - std::abs(m_mainOut_L);

  float sample_square = m_mainOut_L * m_mainOut_L;
  m_mainOut_L = m_mainOut_L * ((2.26548f * sample_square - 5.13274f) * sample_square + 3.14159f);

  //****************************** Right Channel ****************************//
  m_mainOut_R = m_reverb.m_out_R * mst_gain;

  m_mainOut_R *= 0.1588f;
  m_mainOut_R = std::clamp(m_mainOut_R, -0.25f, 0.25f);

  m_mainOut_R += -0.25f;
  m_mainOut_R += m_mainOut_R;

  m_mainOut_R = 0.5f - std::abs(m_mainOut_R);

  sample_square = m_mainOut_R * m_mainOut_R;
  m_mainOut_R = m_mainOut_R * ((2.26548f * sample_square - 5.13274f) * sample_square + 3.14159f);

  //****************************** Fade n Flush ****************************//
  if(m_flush)
  {
    if(m_table_indx == m_flush_indx)
    {
      flushDSP();
    }

    m_table_indx++;

    if(m_table_indx == m_raised_cos_table.size())
    {
      m_table_indx = 0;
      m_flush = false;
    }
  }
  //****************************** Test Tone ****************************//
  if(m_test_tone.m_state)
  {
    m_test_tone.tick();
    m_mainOut_L = std::clamp(m_mainOut_L + m_test_tone.m_signal, -1.f, 1.f);
    m_mainOut_R = std::clamp(m_mainOut_R + m_test_tone.m_signal, -1.f, 1.f);
  }
}

/******************************************************************************/
/**
*******************************************************************************/

inline void dsp_host::setPolySlowFilterCoeffs(SignalStorage &signals, uint32_t _voiceID)
{
  m_soundgenerator.set(signals, _voiceID);
  m_combfilter.set(signals, static_cast<float>(m_samplerate), _voiceID);
  m_feedbackmixer.set(signals, _voiceID);
}

/******************************************************************************/
/**
*******************************************************************************/

inline void dsp_host::setMonoSlowFilterCoeffs(SignalStorage &signals)
{
  m_flanger.set_slow(signals);
  m_cabinet.set(signals);
  m_gapfilter.set(signals);
  m_echo.set(signals);
  /* reverb setter (if reverb params render with slow clock) - see pe_defines.config.h */
#if test_reverbParams == 1
  m_reverb.set(signals);
#endif
}

/******************************************************************************/
/**
*******************************************************************************/

inline void dsp_host::setMonoFastFilterCoeffs(SignalStorage &signals)
{
  m_flanger.set_fast(signals);
  /* reverb setter (if reverb params render with fast clock) - see pe_defines.config.h */
#if test_reverbParams == 0
  m_reverb.set(signals);
#endif
}

/******************************************************************************/
/**
*******************************************************************************/

void dsp_host::examineParameter()
{
  uint32_t v;
  for(v = 0; v < m_param_status.m_size; v++)
  {
    param_body *par = &m_params.getBody(m_param_status.m_index);
    m_param_status.m_state[v] = par->m_state;
    m_param_status.m_preload[v] = par->m_preload;
    m_param_status.mparams[v] = par->m_value;
    m_param_status.m_dx[v][0] = par->m_dx[0];
    m_param_status.m_dx[v][1] = par->m_dx[1];
    m_param_status.m_x[v] = par->m_x;
    m_param_status.m_start[v] = par->m_start;
    m_param_status.m_diff[v] = par->m_diff;
    m_param_status.m_dest[v] = par->m_dest;
  }
}

void dsp_host::examineSignal()
{
  uint32_t v;
  m_signal_status.m_left = m_mainOut_L;
  m_signal_status.m_right = m_mainOut_R;

  for(v = 0; v < m_signal_status.m_size; v++)
  {
    m_signal_status.m_value[v] = m_parameters.getOld(v, m_signal_status.m_selected);
  }
}

/******************************************************************************/
/**
*******************************************************************************/

/* Audio Engine Reset */
void dsp_host::resetDSP()
{
  // reset: envlopes
  resetEnv();
  // reset: audio engine poly section (full flush included)
  m_soundgenerator.resetDSP();
  m_combfilter.resetDSP();
  m_svfilter.resetDSP();
  m_feedbackmixer.resetDSP();

  // audio engine mono section (full flush included)
  m_outputmixer.resetDSP();
  m_flanger.resetDSP();
  m_cabinet.resetDSP();
  m_gapfilter.resetDSP();
  m_echo.resetDSP();
  m_reverb.resetDSP();

  // null host output signal
  m_mainOut_L = 0.f;
  m_mainOut_R = 0.f;
}

/******************************************************************************/
/**
*******************************************************************************/
void dsp_host::flushDSP()
{
  // reset: audio engine poly section (full flush included)
  m_soundgenerator.resetDSP();
  m_combfilter.resetDSP();
  m_svfilter.resetDSP();
  m_feedbackmixer.resetDSP();

  // audio engine mono section (full flush included)
  m_outputmixer.resetDSP();
  m_flanger.resetDSP();
  m_cabinet.resetDSP();
  m_gapfilter.resetDSP();
  m_echo.resetDSP();
  m_reverb.resetDSP();
}

/******************************************************************************/
/**
*******************************************************************************/

/* */
void dsp_host::resetEnv()
{
  // reset new envelopes
  // iterate voices
  uint32_t v;

  // reset Enevlopes
  for(v = 0; v < m_voices; v++)
  {
    // reset new envelopes (Env A)
    m_params.m_new_envelopes.m_env_a.m_body[v].m_index = 0;
    m_params.m_new_envelopes.m_env_a.m_body[v].m_x = 0.f;
    m_params.m_new_envelopes.m_env_a.m_body[v].m_signal_timbre = 0.f;
    m_params.m_new_envelopes.m_env_a.m_body[v].m_start_timbre = 0.f;
    m_params.m_new_envelopes.m_env_a.m_body[v].m_signal_magnitude = 0.f;
    m_params.m_new_envelopes.m_env_a.m_body[v].m_start_magnitude = 0.f;
    // reset new envelopes (Env B)
    m_params.m_new_envelopes.m_env_b.m_body[v].m_index = 0;
    m_params.m_new_envelopes.m_env_b.m_body[v].m_x = 0.f;
    m_params.m_new_envelopes.m_env_b.m_body[v].m_signal_timbre = 0.f;
    m_params.m_new_envelopes.m_env_b.m_body[v].m_start_timbre = 0.f;
    m_params.m_new_envelopes.m_env_b.m_body[v].m_signal_magnitude = 0.f;
    m_params.m_new_envelopes.m_env_b.m_body[v].m_start_magnitude = 0.f;
    // reset new envelopes (Env C)
    m_params.m_new_envelopes.m_env_c.m_body[v].m_index = 0;
    m_params.m_new_envelopes.m_env_c.m_body[v].m_x = 0.f;
    m_params.m_new_envelopes.m_env_c.m_body[v].m_signal_magnitude = 0.f;
    m_params.m_new_envelopes.m_env_c.m_body[v].m_start_magnitude = 0.f;
    // reset new envelopes (Env G)
    m_params.m_new_envelopes.m_env_g.m_body[v].m_index = 0;
    m_params.m_new_envelopes.m_env_g.m_body[v].m_x = 0.f;
    m_params.m_new_envelopes.m_env_g.m_body[v].m_signal_magnitude = 0.f;
    m_params.m_new_envelopes.m_env_g.m_body[v].m_start_magnitude = 0.f;
  }
  // mono flanger envelope (new envelopes only)
  m_params.m_new_envelopes.m_env_f.m_body[0].m_index = 0;
  m_params.m_new_envelopes.m_env_f.m_body[0].m_x = 0.f;
  m_params.m_new_envelopes.m_env_f.m_body[0].m_signal_magnitude = 0.f;
  m_params.m_new_envelopes.m_env_f.m_body[0].m_start_magnitude = 0.f;
}

void dsp_host::muteOsc(uint32_t _oscId, uint32_t _state)
{
  switch(_oscId)
  {
    case 0:
      m_soundgenerator.m_mute_state_A = _state ? 0 : 1;
      break;
    case 1:
      m_soundgenerator.m_mute_state_B = _state ? 0 : 1;
      break;
  }
}
