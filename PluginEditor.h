#pragma once
#include "PluginProcessor.h"
#include <vector>
#include <memory>

class VOGEditor final : public juce::AudioProcessorEditor, private juce::Timer {
public:
 explicit VOGEditor(VOGProcessor&);
 ~VOGEditor() override;
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 struct Control {
  juce::Slider slider;
  juce::Label label;
  juce::Label value;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
  juce::String id;
 };
 void timerCallback() override;
 void applyPreset(int);
 VOGProcessor& processor;
 std::vector<std::unique_ptr<Control>> controls;
 juce::ComboBox presetBox;
 juce::Label titleLabel, subtitleLabel, footerLabel;
 juce::TextButton resetButton {"RESET"};
 juce::ComponentBoundsConstrainer resizeConstrainer;
 juce::ResizableCornerComponent corner {this, &resizeConstrainer};
 int effectId = 1;
 JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VOGEditor)
};
