/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.0.2

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
 An auto-generated component, created by the Projucer.

 Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SynthesizerAudioProcessorEditor  : public AudioProcessorEditor,
                                         public SliderListener,
                                         public ButtonListener,
                                         public ComboBoxListener
{
public:
    //==============================================================================
    SynthesizerAudioProcessorEditor (SynthesizerAudioProcessor& p);
    ~SynthesizerAudioProcessorEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    PlayVoices::TypeVoice convertStringToEnum(String value);
    void updateMixOscillators();
    void updateFilters();

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;

    // Binary resources:
    static const char* power_png;
    static const int power_pngSize;
    static const char* _15146901_10210969042120438_1927431087_o_png_jpeg;
    static const int _15146901_10210969042120438_1927431087_o_png_jpegSize;
    static const char* _1481503636357_png;
    static const int _1481503636357_pngSize;
    static const char* _1481503631940_png;
    static const int _1481503631940_pngSize;
    static const char* _1481503633968_png;
    static const int _1481503633968_pngSize;
    static const char* _1481503630030_png;
    static const int _1481503630030_pngSize;
    static const char* _1481503638929_png;
    static const int _1481503638929_pngSize;
    static const char* _1481503638929_png2;
    static const int _1481503638929_png2Size;
    static const char* _1481503636357_png2;
    static const int _1481503636357_png2Size;
    static const char* _1481503631940_png2;
    static const int _1481503631940_png2Size;
    static const char* _1481389904118_png;
    static const int _1481389904118_pngSize;
    static const char* _1481496889823_png;
    static const int _1481496889823_pngSize;
    static const char* background_jpg;
    static const int background_jpgSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SynthesizerAudioProcessor& processor;
    PlayVoices::TypeVoice currentPlayVoiceOsc1;
    PlayVoices::TypeVoice currentPlayVoiceOsc2;
    Oscillator::FilterType currentFilter;
	MidiKeyboardComponent midiKeyboard;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<Slider> selectFilterType;
    ScopedPointer<Slider> qFactor;
    ScopedPointer<Label> label7;
    ScopedPointer<Slider> freqCut;
    ScopedPointer<Label> label6;
    ScopedPointer<ImageButton> turnOnFilterButton;
    ScopedPointer<Slider> attackKnob;
    ScopedPointer<Slider> decayKnob;
    ScopedPointer<Slider> sustainKnob;
    ScopedPointer<Slider> releaseKnob;
    ScopedPointer<ImageButton> sine1Button;
    ScopedPointer<ImageButton> triangle1Button;
    ScopedPointer<ImageButton> saw1Button;
    ScopedPointer<ImageButton> square1Button;
    ScopedPointer<ImageButton> white1Button;
    ScopedPointer<ImageButton> pink1Button;
    ScopedPointer<ImageButton> sine2Button;
    ScopedPointer<ImageButton> triangle2Button;
    ScopedPointer<ImageButton> saw2Button;
    ScopedPointer<ImageButton> white2Button;
    ScopedPointer<ImageButton> pink2Button;
    ScopedPointer<ImageButton> square2Button;
    ScopedPointer<ComboBox> numVoices;
    ScopedPointer<Slider> mixLevel;
    ScopedPointer<Label> oscillator2Label;
    ScopedPointer<Slider> rateTremolo1;
    ScopedPointer<Slider> rateVibrato1;
    ScopedPointer<Slider> depthVibrato1;
    ScopedPointer<Slider> depthTremolo1;
    ScopedPointer<Slider> rateTremolo2;
    ScopedPointer<Slider> rateVibrato2;
    ScopedPointer<Slider> depthVibrato2;
    ScopedPointer<Slider> depthTremolo2;
    ScopedPointer<ComboBox> modulationVibrato1;
    ScopedPointer<ComboBox> modulationTremolo1;
    ScopedPointer<ComboBox> modulationVibrato2;
    ScopedPointer<ComboBox> modulationTremolo2;
    ScopedPointer<ImageButton> turnOnTremolo1;
    ScopedPointer<ImageButton> turnOnVibrato1;
    ScopedPointer<ImageButton> turnOnTremolo2;
    ScopedPointer<ImageButton> turnOnVibrato2;
    ScopedPointer<ImageButton> turnOnAdsr;
    ScopedPointer<Label> oscillators;
    ScopedPointer<Label> numberOfVoicesLabel;
    ScopedPointer<Label> oscillator1Label;
    ScopedPointer<Label> filters;
    ScopedPointer<Label> lowPasslabel;
    ScopedPointer<Label> resonantLabel;
    ScopedPointer<Label> highPassLabel;
    ScopedPointer<Label> rateVibrato1Label;
    ScopedPointer<Label> depthVibrato1Label;
    ScopedPointer<Label> rateTremolo1Label;
    ScopedPointer<Label> depthTremolo1Label;
    ScopedPointer<Label> rateVibrato1Label3;
    ScopedPointer<Label> depthVibrato1Label3;
    ScopedPointer<Label> rateTremolo1Label3;
    ScopedPointer<Label> depthTremolo1Label3;
    ScopedPointer<Label> effectsOsc1;
    ScopedPointer<Label> effectsOsc2;
    ScopedPointer<Label> ampEnvelopeLabel;
    ScopedPointer<Label> attackLabel;
    ScopedPointer<Label> decayLabel;
    ScopedPointer<Label> sustainLabel;
    ScopedPointer<Label> releaseLabel;
    ScopedPointer<TextEditor> textFreqCut;
    ScopedPointer<Label> lfo1Label;
    ScopedPointer<Label> lfo1Label2;
    ScopedPointer<Label> lfo1Label3;
    ScopedPointer<Label> lfo1Label4;
    ScopedPointer<Slider> volumeKnob;
    ScopedPointer<Label> volumeLabel;
    Image cachedImage_background_jpg_1;
    Image cachedImage__1481496889823_png_2;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthesizerAudioProcessorEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
