/*
  ==============================================================================

    ReverbEffect.h
    Created: 21 Oct 2022 9:31:37pm

  ==============================================================================
*/

#pragma once
#include <cstdint>
#include <cmath>

class DelayLine
{
public:
  DelayLine();
  void fill_in(float block[32]);
  void process(float block[32],float delay_samples);
  void advance() { index = (index - 32) & size_mask; }
private:
  float buffer[0x20000];
  float lookup_table[256];
  uint32_t size_mask = 0x1ffff;
  uint32_t index=0;
  float prev_delay = 100.0f;
  float prev_sample = 0.0f;
};


class ReverbEffect
{
public:
  ReverbEffect();
  void process(float** buf_in, float** buff_out, int size);

  void setMix(float value);
  void setWidth(float value);
  void setFeedback(float value);
  void setFeedbackWidth(float value);
  void setDelay(float value);
  void setScale(float value);
  void setMod(float value);
  void setModAmount(float value);
  void setLowCut(float value);
  void setHiCut(float value);
private:
  DelayLine delays[16];
  float delay_bufs[16][32];
  float in_out_l[32];
  float in_out_r[32];
  int current_sample = 0;
  float noise = 1e-15f;

  const float sample_rate = 48000.0f;
  const float param_lp_alpha = 1.0 - exp(-1000.0 / ((sample_rate / 32.0f) * 300.0));
  float wet_mix = 1.0f;
  float dry_mix = 0.0f;

  float width_sin = 1.0f;
  float width_cos = 0.f;

  float feedback = 0.0f;

  float feedback_sin = 0.0f;
  float feedback_neg_sin = 0.0f;
  float feedback_cos = 1.0f;

  float delay_target = 14400.0f;
  float delay_current = 14400.0f;

  float scale_target = 1.0f;
  float scale_current = 1.0f;

  uint32_t mod_phase = 0;
  uint32_t mod_phase_inc = 0;
  float i_component = 0.0f;
  float q_component = 0.0f;
  const float mod_lp_alpha = 1.0 - exp(-1.570796f / (sample_rate / 32.0f));
  float sin_table_256[257];
  float sin_table_16[16];
  float cos_table_16[16];

  float mod_amount_target = 0.0f;
  float mod_amount_current = 0.0f;

  float low_cut_coef = -1.0f;
  float low_cut_prev_sum_l = 0.0f;
  float low_cut_prev_sum_r = 0.0f;

  float hi_cut_coef = 1.0f;
  float hi_cut_prev_sample_l = 0.0f;
  float hi_cut_prev_sample_r = 0.0f;

};