#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>
#ifndef VOG_EFFECT
#define VOG_EFFECT 1
#endif
static constexpr int effect=VOG_EFFECT;
static juce::String effectName(){
 if constexpr(effect==1) return "VOG Infinite Reverb";
 if constexpr(effect==2) return "VOG Cosmic Delay";
 if constexpr(effect==3) return "VOG Atmosphere";
 return "VOG Dream Chorus";
}
static void add(juce::AudioProcessorValueTreeState::ParameterLayout& layout,const char* id,const char* name,float min,float max,float def){
 layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id,1},name,juce::NormalisableRange<float>(min,max),def));
}
juce::AudioProcessorValueTreeState::ParameterLayout VOGProcessor::makeLayout(){
 juce::AudioProcessorValueTreeState::ParameterLayout p;
 add(p,"mix","Wet Mix",0,1,0.4f);
 if constexpr(effect==1){add(p,"size","Room Size",0,1,0.85f);add(p,"damp","Damping",0,1,0.25f);add(p,"width","Stereo Width",0,1,1);add(p,"freeze","Freeze",0,1,0);}
 if constexpr(effect==2){add(p,"time","Delay Time ms",50,1500,420);add(p,"feedback","Feedback",0,0.94f,0.55f);add(p,"pingpong","Ping Pong",0,1,1);add(p,"tone","Darkness",0,1,0.3f);}
 if constexpr(effect==3){add(p,"time","Texture Length ms",80,1300,480);add(p,"feedback","Texture Feedback",0,0.92f,0.67f);add(p,"motion","Motion",0,1,0.5f);add(p,"tone","Warmth",0,1,0.5f);}
 if constexpr(effect==4){add(p,"rate","Rate Hz",0.05f,8,0.4f);add(p,"depth","Depth",0,1,0.7f);add(p,"centre","Centre Delay ms",1,25,9);add(p,"feedback","Feedback",-0.85f,0.85f,0.12f);}
 return p;
}
VOGProcessor::VOGProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),parameters(*this,nullptr,"VOGParameters",makeLayout()){}
const juce::String VOGProcessor::getName() const{return effectName();}
bool VOGProcessor::isBusesLayoutSupported(const BusesLayout& l) const{return l.getMainInputChannelSet()==l.getMainOutputChannelSet() && (l.getMainOutputChannelSet()==juce::AudioChannelSet::mono() || l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo());}
void VOGProcessor::prepareToPlay(double sr,int block){
 sampleRateHz=sr;writePosition=0;lfoPhase=0;lowpass[0]=lowpass[1]=0;
 delayBuffer.setSize(2,juce::jmax(2,(int)(sr*3)));delayBuffer.clear();
 reverb.setSampleRate(sr);reverb.reset();
 chorus.prepare({sr,(juce::uint32)juce::jmax(1,block),2});chorus.reset();
}
float VOGProcessor::readDelay(int channel,float delaySamples) const{
 const int n=delayBuffer.getNumSamples();
 float pos=(float)writePosition-juce::jlimit(1.0f,(float)n-2.0f,delaySamples);
 while(pos<0)pos+=n;
 const int a=(int)pos;const int b=(a+1)%n;const float f=pos-a;
 return juce::jmap(f,delayBuffer.getSample(channel,a),delayBuffer.getSample(channel,b));
}
void VOGProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&){
 juce::ScopedNoDenormals noDenormals;
 const int channels=juce::jmin(2,buffer.getNumChannels());
 for(int ch=channels;ch<buffer.getNumChannels();++ch)buffer.clear(ch,0,buffer.getNumSamples());
 const float mix=*parameters.getRawParameterValue("mix");
 if constexpr(effect==1){
  juce::Reverb::Parameters p;
  p.roomSize=*parameters.getRawParameterValue("size");p.damping=*parameters.getRawParameterValue("damp");
  p.width=*parameters.getRawParameterValue("width");p.freezeMode=*parameters.getRawParameterValue("freeze")>=0.5f?1.0f:0.0f;
  p.wetLevel=mix;p.dryLevel=1.0f-mix;reverb.setParameters(p);
  if(channels==2)reverb.processStereo(buffer.getWritePointer(0),buffer.getWritePointer(1),buffer.getNumSamples());
  else if(channels==1)reverb.processMono(buffer.getWritePointer(0),buffer.getNumSamples());
 }else if constexpr(effect==4){
  chorus.setRate(*parameters.getRawParameterValue("rate"));chorus.setDepth(*parameters.getRawParameterValue("depth"));
  chorus.setCentreDelay(*parameters.getRawParameterValue("centre"));chorus.setFeedback(*parameters.getRawParameterValue("feedback"));chorus.setMix(mix);
  juce::dsp::AudioBlock<float> audioBlock(buffer);juce::dsp::ProcessContextReplacing<float> context(audioBlock);chorus.process(context);
 }else{
  const float time=*parameters.getRawParameterValue("time");
  const float feedback=*parameters.getRawParameterValue("feedback");
  const float tone=*parameters.getRawParameterValue("tone");
  const float motion=effect==3?parameters.getRawParameterValue("motion")->load():0.0f;
  const float ping=effect==2?parameters.getRawParameterValue("pingpong")->load():0.0f;
  const float cutoff=juce::jmap(tone,0.5f,0.015f);
  const int size=delayBuffer.getNumSamples();
  for(int i=0;i<buffer.getNumSamples();++i){
   const float modulation=effect==3?std::sin(lfoPhase)*motion*90.0f:0;
   const float samples=(time+modulation)*0.001f*(float)sampleRateHz;
   float wet[2]={readDelay(0,samples),readDelay(1,samples)};
   float in[2]={buffer.getSample(0,i),channels>1?buffer.getSample(1,i):buffer.getSample(0,i)};
   for(int c=0;c<2;++c){
    const float feedbackSignal=juce::jmap(ping,wet[c],wet[1-c]);
    lowpass[c]+=cutoff*(feedbackSignal-lowpass[c]);
    delayBuffer.setSample(c,writePosition,std::tanh(in[c]+feedback*lowpass[c]));
    if(c<channels)buffer.setSample(c,i,in[c]*(1-mix)+wet[c]*mix);
   }
   writePosition=(writePosition+1)%size;
   lfoPhase+=juce::MathConstants<float>::twoPi*0.13f/(float)sampleRateHz;
   if(lfoPhase>juce::MathConstants<float>::twoPi)lfoPhase-=juce::MathConstants<float>::twoPi;
  }
 }
}
void VOGProcessor::getStateInformation(juce::MemoryBlock& out){auto xml=parameters.copyState().createXml();copyXmlToBinary(*xml,out);}
void VOGProcessor::setStateInformation(const void* data,int size){auto xml=getXmlFromBinary(data,size);if(xml && xml->hasTagName(parameters.state.getType()))parameters.replaceState(juce::ValueTree::fromXml(*xml));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new VOGProcessor();}
