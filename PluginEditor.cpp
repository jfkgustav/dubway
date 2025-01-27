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

    delayVolume.setSliderStyle(Slider::LinearBarVertical);
    delayVolume.setRange(0.0, 127.0, 1.0);
    delayVolume.setTextBoxStyle(Slider::NoTextBox, false, 90, 0);
    delayVolume.setPopupDisplayEnabled(true, false, this);
    delayVolume.setTextValueSuffix(" Delay Volume");
    delayVolume.setValue(1.0);

    addAndMakeVisible(&delayVolume);
    delayVolume.addListener(this);
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
    delayVolume.setBounds(40, 30, 20, getHeight() - 60);
}

void DubwayAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    audioProcessor.noteOnVel = delayVolume.getValue();
}

