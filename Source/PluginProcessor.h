#pragma once
#include <JuceHeader.h>
class VOGProcessor final : public juce::AudioProcessor {
public:
 VOGProcessor();
 void prepareToPlay(double,int) override;
 void releaseResources() override {}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
 bool isBusesLayoutSupported(const BusesLayout&) const override;
 juce::AudioProcessorEditor* createEditor() override;
 bool hasEditor() const override {return true;}
 const juce::String getName() const override;
 bool acceptsMidi() const override {return false;}
 bool producesMidi() const override {return false;}
 bool isMidiEffect() const override {return false;}
 double getTailLengthSeconds() const override {return 12.0;}
 int getNumPrograms() override {return 1;}
 int getCurrentProgram() override {return 0;}
 void setCurrentProgram(int) override {}
 const juce::String getProgramName(int) override {return {};}
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock&) override;
 void setStateInformation(const void*,int) override;
 juce::AudioProcessorValueTreeState parameters;
 static juce::AudioProcessorValueTreeState::ParameterLayout makeLayout();
private:
 juce::Reverb reverb;
 juce::dsp::Chorus<float> chorus;
 juce::AudioBuffer<float> delayBuffer;
 int writePosition=0;
 double sampleRateHz=44100.0;
 float lfoPhase=0.0f;
 float lowpass[2]{};
 float readDelay(int channel,float delaySamples) const;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VOGProcessor)
};
