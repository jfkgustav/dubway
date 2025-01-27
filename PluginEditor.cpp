/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DubwayAudioProcessorEditor::DubwayAudioProcessorEditor (DubwayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 400);
    gainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    feedbackSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    mixSlider.setSliderStyle(Slider::SliderStyle::Rotary);

    for (auto* slider : { &gainSlider, &feedbackSlider, &mixSlider }) {
        slider->setTextBoxStyle(Slider::TextBoxBelow, true, 200, 30);
        addAndMakeVisible(slider);
    }


}

DubwayAudioProcessorEditor::~DubwayAudioProcessorEditor()
{
}

//==============================================================================
void DubwayAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (FontOptions (15.0f));
    g.drawFittedText ("dub volume", getLocalBounds(), Justification::centred, 1);
}

void DubwayAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    Rectangle<int> bounds = getLocalBounds();
    int margin = 20;

    Rectangle<int> gainBounds = bounds.removeFromRight (getWidth() / 3);
    gainSlider.setBounds (gainBounds);

    Rectangle<int> knobsBounds = bounds.removeFromTop(getHeight() / 2);
    Rectangle<int> feedbackBounds = knobsBounds.removeFromLeft(knobsBounds.getWidth() / 2);
    feedbackSlider.setBounds(feedbackBounds.reduced(margin));
    mixSlider.setBounds(knobsBounds.reduced(margin));
}

void DubwayAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
}

