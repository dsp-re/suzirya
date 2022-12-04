/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SuziryaAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    SuziryaAudioProcessorEditor (SuziryaAudioProcessor&);
    ~SuziryaAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* sliderThatWasMoved) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SuziryaAudioProcessor& audioProcessor;
    std::unique_ptr<juce::Slider> mix_slider;
    std::unique_ptr<juce::Slider> width_slider;
    std::unique_ptr<juce::Slider> feedback_slider;
    std::unique_ptr<juce::Slider> feedback_width_slider;
    std::unique_ptr<juce::Slider> low_cut_slider;
    std::unique_ptr<juce::Slider> delay_slider;
    std::unique_ptr<juce::Slider> scale_slider;
    std::unique_ptr<juce::Slider> mod_hz_slider;
    std::unique_ptr<juce::Slider> mod_amount_slider;
    std::unique_ptr<juce::Slider> high_cut_slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SuziryaAudioProcessorEditor)
};
