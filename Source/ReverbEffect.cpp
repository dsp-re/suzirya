/*
  ==============================================================================

    ReverbEffect.cpp
    Created: 21 Oct 2022 9:31:47pm

  ==============================================================================
*/

#include "ReverbEffect.h"
#include <algorithm>

DelayLine::DelayLine()
{
  std::fill(std::begin(buffer), std::end(buffer), 0.0f);
  
  for (int i = 0; i < 256; i++)
  {
    auto x = i / 256.0f;
    lookup_table[i] = std::min(0.99f, (1.0f - x) / (1.0f + x));
  }
}

void DelayLine::fill_in(float block[32])
{
  auto current_index = index & size_mask;
  for (int i = 0; i < 32; i++)
  {
    buffer[current_index] = block[i];
    current_index = (current_index - 1) & size_mask;
  }
}

void DelayLine::process(float block[32], float new_delay)
{
  auto delay_inc = (new_delay - prev_delay) / 32.0f;
  auto delay = prev_delay;
  auto read_index = index;
  auto current_sample = prev_sample;
  for (int i = 0; i < 32; i++)
  {
    delay += delay_inc;
    auto delay_i = static_cast<uint32_t>(delay);
    auto delay_fraction = delay - delay_i;
    auto current_index = (read_index + delay_i) & size_mask;
    read_index = (read_index - 1) & size_mask;
    current_sample = (buffer[current_index] - current_sample) * lookup_table[static_cast<int>(delay_fraction * 256.0f)] + buffer[(current_index + 1) & size_mask];
    block[i] = current_sample;
  }
  prev_delay = delay;
  prev_sample = current_sample;
}

ReverbEffect::ReverbEffect()
{
  std::fill(std::begin(in_out_l), std::end(in_out_l), 0.0f);
  std::fill(std::begin(in_out_r), std::end(in_out_r), 0.0f);

  for (int i = 0; i < 257; i++)
  {
    sin_table_256[i] = sinf((i / 256.0f) * 6.283185f);
  }
  for (int i = 0; i < 16; i++)
  {
    sin_table_16[i] = sinf((i * 3.141593f) / 8.0f);
    cos_table_16[i] = cosf((i * 3.141593f) / 8.0f);
  }
}

inline void cross_fade(float buf_0[32], float buf_1[32], float sin, float neg_sin, float cos)
{
  for (int k = 0; k < 32; k++)
  {
    auto tmp = buf_1[k] * neg_sin + buf_0[k] * cos;
    buf_1[k] = buf_1[k] * cos + buf_0[k] * sin;
    buf_0[k] = tmp;
  }
}


void ReverbEffect::process(float** buf_in, float** buf_out, int size)
{
  for (int i = 0; i < size; i++)
  {
    {
      auto l = buf_in[0][i];
      auto r = buf_in[1][i];
      buf_out[0][i] = in_out_l[current_sample] * wet_mix + l * dry_mix;
      buf_out[1][i] = in_out_r[current_sample] * wet_mix + r * dry_mix;
      in_out_l[current_sample] = l;
      in_out_r[current_sample] = r;
      current_sample += 1;
      if (current_sample > 31)
      {
        current_sample = 0;

        auto i_phase_index = mod_phase >> 24;
        auto q_phase_index = (i_phase_index + 0x40) & 0xff;

        auto mod_fraction = (mod_phase & 0xffffff) / 16777216.0f;

        mod_phase += mod_phase_inc;

        // lowpass
        i_component = (mod_fraction * sin_table_256[i_phase_index + 1] + (1.0f - mod_fraction) * sin_table_256[i_phase_index] - i_component) * mod_lp_alpha + i_component;
        q_component = (mod_fraction * sin_table_256[q_phase_index + 1] + (1.0f - mod_fraction) * sin_table_256[q_phase_index] - q_component) * mod_lp_alpha + q_component;

        for (int k = 0; k < 32; k++)
        {
          in_out_l[k] += noise;
          in_out_r[k] += noise;
        }

        noise = -noise;

        for (int j = 0; j < 8; j++)
        {
          for (int k = 0; k < 32; k++)
          {
            delay_bufs[j * 2][k] = delay_bufs[j * 2][k] * feedback + in_out_l[k];
            delay_bufs[j * 2 + 1][k] = delay_bufs[j * 2 + 1][k] * feedback + in_out_r[k];
          }
        }

        // lowpass params
        mod_amount_current = (mod_amount_target - mod_amount_current) * param_lp_alpha + mod_amount_current;
        scale_current = (scale_target - scale_current) * param_lp_alpha + scale_current;
        delay_current = (delay_target - delay_current) * param_lp_alpha + delay_current;

        auto delay = delay_current;
        for (int j = 0; j < 16; j++)
        {
          // phase shift each delay;
          auto mod_delay = mod_amount_current * (i_component * cos_table_16[j] + q_component * sin_table_16[j]);

          delay = std::max(delay, mod_amount_current);

          delays[j].fill_in(delay_bufs[j]);
          delays[j].process(delay_bufs[j], delay + mod_delay + 1.0f);
          delays[j].advance();
          delay *= scale_current;
        }

        std::fill(std::begin(in_out_l), std::end(in_out_l), 0.0f);
        std::fill(std::begin(in_out_r), std::end(in_out_r), 0.0f);

        for (int j = 0; j < 8; j++)
        {
          for (int k = 0; k < 32; k++)
          {
            in_out_l[k] += (0.35355338f * delay_bufs[j * 2][k]);
            in_out_r[k] += (0.35355338f * delay_bufs[j * 2 + 1][k]);
          }
        }

        // feedback network cross fade
        for (int j = 0; j < 16; j+=2)
        {
          cross_fade(delay_bufs[j], delay_bufs[j + 1], feedback_sin, feedback_neg_sin, feedback_cos);
        }

        for (int j = 0; j < 16; j += 4)
        {
          cross_fade(delay_bufs[j], delay_bufs[j + 2], feedback_sin, feedback_neg_sin, feedback_cos);
          cross_fade(delay_bufs[j + 1], delay_bufs[j + 3], feedback_sin, feedback_neg_sin, feedback_cos);
        }

        for (int j = 0; j < 16; j += 8)
        {
          cross_fade(delay_bufs[j], delay_bufs[j + 4], feedback_sin, feedback_neg_sin, feedback_cos);
          cross_fade(delay_bufs[j + 1], delay_bufs[j + 5], feedback_sin, feedback_neg_sin, feedback_cos);
          cross_fade(delay_bufs[j + 2], delay_bufs[j + 6], feedback_sin, feedback_neg_sin, feedback_cos);
          cross_fade(delay_bufs[j + 3], delay_bufs[j + 7], feedback_sin, feedback_neg_sin, feedback_cos);
        }

        for (int j = 0; j < 8; j += 1)
        {
          cross_fade(delay_bufs[j], delay_bufs[j + 8], feedback_sin, feedback_neg_sin, feedback_cos);
        }

        auto inv_feedback = sqrtf(1.0f / std::max(1.0f, feedback * 5.0f));
        for (int k = 0; k < 32; k++)
        {
          in_out_l[k] *= inv_feedback;
          in_out_r[k] *= inv_feedback;
        }

        // low cut eq
        {
          auto prev_sum = low_cut_prev_sum_l;
          for (int k = 0; k < 32; k++)
          {
            auto prev_sample = in_out_l[k];
            auto prev_output = prev_sample + prev_sum;
            in_out_l[k] = prev_output;
            prev_sum = -prev_sample - prev_output * low_cut_coef;
          }
          low_cut_prev_sum_l = prev_sum;

          prev_sum = low_cut_prev_sum_r;
          for (int k = 0; k < 32; k++)
          {
            auto prev_sample = in_out_r[k];
            auto prev_output = prev_sample + prev_sum;
            in_out_r[k] = prev_output;
            prev_sum = -prev_sample - prev_output * low_cut_coef;
          }
          low_cut_prev_sum_r = prev_sum;
        }

        // hi cut eq
        {
          auto prev_sample = hi_cut_prev_sample_l;
          for (int k = 0; k < 32; k++)
          {
            prev_sample = (1.0f - hi_cut_coef) * prev_sample + hi_cut_coef * in_out_l[k];
            in_out_l[k] = prev_sample;
          }
          hi_cut_prev_sample_l = prev_sample;

          prev_sample = hi_cut_prev_sample_r;
          for (int k = 0; k < 32; k++)
          {
            prev_sample = (1.0f - hi_cut_coef) * prev_sample + hi_cut_coef * in_out_r[k];
            in_out_r[k] = prev_sample;
          }
          hi_cut_prev_sample_r = prev_sample;
        }

        // crossfade
        for (int k = 0; k < 32; k++)
        {
          auto tmp = in_out_l[k] * width_sin + in_out_r[k] * width_cos;
          in_out_r[k] = in_out_r[k] * width_sin + in_out_l[k] * width_cos;
          in_out_l[k] = tmp;
        }
      }
    }
  }
}

void ReverbEffect::setMix(float value)
{
  wet_mix = sinf(value * 3.141593f * 0.5f);
  dry_mix = cosf(value * 3.141593f * 0.5f);
}

void ReverbEffect::setWidth(float value)
{
  width_sin = sinf(value * 3.141593f * 0.5f);
  width_cos = cosf(value * 3.141593f * 0.5f);
}

void ReverbEffect::setFeedback(float value)
{
  feedback = sqrtf(value) * 0.9999999f + 1e-7f;
}

void ReverbEffect::setFeedbackWidth(float value)
{
  auto tmp = value * 0.5f * 3.141593f * 0.5f;
  feedback_sin = sinf(tmp);
  feedback_neg_sin = -feedback_sin;
  feedback_cos = cosf(tmp);
}

void ReverbEffect::setDelay(float value)
{
  delay_target = value * 48.0f;
}

void ReverbEffect::setScale(float value)
{
  auto tmp = value * 0.7071f;
  scale_target = 1.0f - tmp * tmp * tmp;
}

void ReverbEffect::setMod(float value)
{
  mod_phase_inc = ((value * 256.0f) / (sample_rate * 0.03125f)) * 16777216.0f;
}

void ReverbEffect::setModAmount(float value)
{
  mod_amount_target = (value * value) * 500.0f;
}

void ReverbEffect::setLowCut(float value)
{
  auto tmp = (sample_rate + sample_rate) * (1.0f / (value * 6.283185f));
  low_cut_coef = (1.0f - tmp) / (1.0f + tmp);
}

void ReverbEffect::setHiCut(float value)
{
  hi_cut_coef = 1.0 - exp(value* -6.283185f/sample_rate);
}
