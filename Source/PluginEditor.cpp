#include "PluginProcessor.h"
#include "PluginEditor.h"

SuziryaAudioProcessorEditor::SuziryaAudioProcessorEditor (SuziryaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    mix_slider.reset(new juce::Slider("Mix"));
    addAndMakeVisible(mix_slider.get());
    mix_slider->setRange(0, 1, 0.01);
    mix_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mix_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    mix_slider->addListener(this);
    mix_slider->setValue(1.0);

    width_slider.reset(new juce::Slider("width"));
    addAndMakeVisible(width_slider.get());
    width_slider->setRange(0, 1, 0.01);
    width_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    width_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    width_slider->addListener(this);
    width_slider->setValue(1.0);

    feedback_slider.reset(new juce::Slider("feedback"));
    addAndMakeVisible(feedback_slider.get());
    feedback_slider->setRange(0, 1, 0.01);
    feedback_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedback_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    feedback_slider->addListener(this);
    feedback_slider->setValue(0.0);

    feedback_width_slider.reset(new juce::Slider("feedback_width"));
    addAndMakeVisible(feedback_width_slider.get());
    feedback_width_slider->setRange(0, 1, 0.01);
    feedback_width_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    feedback_width_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    feedback_width_slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff8e989b));
    feedback_width_slider->addListener(this);
    feedback_width_slider->setValue(0.0);

    low_cut_slider.reset(new juce::Slider("low_cut"));
    addAndMakeVisible(low_cut_slider.get());
    low_cut_slider->setRange(10, 2000, 1);
    low_cut_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    low_cut_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    low_cut_slider->addListener(this);
    low_cut_slider->setValue(10.0);

    delay_slider.reset(new juce::Slider("delay"));
    addAndMakeVisible(delay_slider.get());
    delay_slider->setRange(0, 2000, 1);
    delay_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delay_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    delay_slider->addListener(this);
    delay_slider->setValue(14400.0);

    scale_slider.reset(new juce::Slider("scale"));
    addAndMakeVisible(scale_slider.get());
    scale_slider->setRange(0, 1, 0.001);
    scale_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    scale_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    scale_slider->addListener(this);
    scale_slider->setValue(0.0);

    mod_hz_slider.reset(new juce::Slider("mod_hz"));
    addAndMakeVisible(mod_hz_slider.get());
    mod_hz_slider->setRange(0.01, 10, 0.01);
    mod_hz_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mod_hz_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    mod_hz_slider->addListener(this);
    mod_hz_slider->setValue(10.0);

    mod_amount_slider.reset(new juce::Slider("mod_amount"));
    addAndMakeVisible(mod_amount_slider.get());
    mod_amount_slider->setRange(0, 1, 0.01);
    mod_amount_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mod_amount_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    mod_amount_slider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff8e989b));
    mod_amount_slider->addListener(this);
    mod_amount_slider->setValue(0.0);

    high_cut_slider.reset(new juce::Slider("high_cut"));
    addAndMakeVisible(high_cut_slider.get());
    high_cut_slider->setRange(200, 20000, 1);
    high_cut_slider->setSliderStyle(juce::Slider::RotaryVerticalDrag);
    high_cut_slider->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    high_cut_slider->addListener(this);
    high_cut_slider->setValue(20000.0);

    setSize(440, 288);
}

SuziryaAudioProcessorEditor::~SuziryaAudioProcessorEditor()
{
  mix_slider = nullptr;
  width_slider = nullptr;
  feedback_slider = nullptr;
  feedback_width_slider = nullptr;
  low_cut_slider = nullptr;
  delay_slider = nullptr;
  scale_slider = nullptr;
  mod_hz_slider = nullptr;
  mod_amount_slider = nullptr;
  high_cut_slider = nullptr;
}

void SuziryaAudioProcessorEditor::paint (juce::Graphics& g)
{
  g.fillAll(juce::Colour(0xff323e44));
  g.setColour(juce::Colours::white);
	g.setFont(juce::Font(15.00f, juce::Font::plain).withTypefaceStyle("Regular"));
	g.drawText(TRANS("Mix"), 0 * 88, 12, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Width"), 1 * 88, 12, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Feedback"), 2 * 88, 12, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Width"), 3 * 88, 12, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Low cut"), 4 * 88, 12, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Delay"), 0 * 88, 156, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Scale"), 1 * 88, 156, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Mod"), 2 * 88, 156, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("Amount"), 3 * 88, 156, 88, 30, juce::Justification::centred, true);
	g.drawText(TRANS("High cut"), 4 * 88, 156, 88, 30, juce::Justification::centred, true);
}

void SuziryaAudioProcessorEditor::resized()
{
  mix_slider->setBounds(0 * 88, 32, 88, 112);
  width_slider->setBounds(1 * 88, 32, 88, 112);
  feedback_slider->setBounds(2 * 88, 32, 88, 112);
  feedback_width_slider->setBounds(3*88, 32, 88, 112);
  low_cut_slider->setBounds(4 * 88, 32, 88, 112);
  delay_slider->setBounds(0 * 88, 176, 88, 112);
  scale_slider->setBounds(1 * 88, 176, 88, 112);
  mod_hz_slider->setBounds(2 * 88, 176, 88, 112);
  mod_amount_slider->setBounds(3* 88, 176, 88, 112);
  high_cut_slider->setBounds(4* 88, 176, 88, 112);
}

void SuziryaAudioProcessorEditor::sliderValueChanged(juce::Slider* sliderThatWasMoved)
{
  auto index = getIndexOfChildComponent(sliderThatWasMoved);

  void (ReverbEffect:: * table[])(float) = {&ReverbEffect::setMix, &ReverbEffect::setWidth, &ReverbEffect::setFeedback, &ReverbEffect::setFeedbackWidth, &ReverbEffect::setLowCut,
    &ReverbEffect::setDelay, &ReverbEffect::setScale, &ReverbEffect::setMod, &ReverbEffect::setModAmount, &ReverbEffect::setHiCut };

  (audioProcessor.getReverbEffect().*table[index])(sliderThatWasMoved->getValue());
}
