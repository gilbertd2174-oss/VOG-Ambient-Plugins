cmake_minimum_required(VERSION 3.22)
project(VOGAmbientFX VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.10 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)
function(add_vog_effect target title code effect)
  juce_add_plugin(${target}
    COMPANY_NAME "VOG Audio"
    BUNDLE_ID "com.vogaudio.${target}"
    PLUGIN_MANUFACTURER_CODE VogA
    PLUGIN_CODE ${code}
    FORMATS VST3
    PRODUCT_NAME "${title}"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)
  juce_generate_juce_header(${target})
  target_sources(${target} PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
  target_compile_definitions(${target} PRIVATE VOG_EFFECT=${effect} JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_FORCE_USE_LEGACY_PARAM_IDS=0 JUCE_IGNORE_VST3_MISMATCHED_PARAMETER_ID_WARNING=1)
  target_link_libraries(${target} PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_audio_processors juce::juce_gui_extra juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
endfunction()
add_vog_effect(VOGInfiniteReverb "VOG Infinite Reverb" VgR1 1)
add_vog_effect(VOGCosmicDelay "VOG Cosmic Delay" VgD2 2)
add_vog_effect(VOGAtmosphere "VOG Atmosphere" VgA3 3)
add_vog_effect(VOGDreamChorus "VOG Dream Chorus" VgC4 4)
