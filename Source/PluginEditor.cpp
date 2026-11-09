#include "PluginEditor.h"
#include <cmath>

namespace {
const juce::Colour bg {0xff0b1424}, panel {0xff17273d}, cyan {0xff4edbff}, ink {0xffe5f2ff};
struct VOGLookAndFeel : juce::LookAndFeel_V4 {
 VOGLookAndFeel() { setColour(juce::Slider::thumbColourId, cyan); setColour(juce::ComboBox::backgroundColourId, panel); setColour(juce::ComboBox::textColourId, ink); setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff42617f)); }
 void drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float pos,float start,float end,juce::Slider&) override {
  auto b=juce::Rectangle<float>((float)x,(float)y,(float)width,(float)height).reduced(13.0f);
  auto diameter=juce::jmin(b.getWidth(),b.getHeight()); auto c=b.getCentre(); auto r=diameter*0.45f;
  g.setColour(juce::Colour(0xff08111e)); g.fillEllipse(c.x-r,c.y-r,2*r,2*r);
  g.setColour(juce::Colour(0xff39506b)); g.drawEllipse(c.x-r,c.y-r,2*r,2*r,5.0f);
  juce::Path track; track.addCentredArc(c.x,c.y,r,r,0.0f,start,end,true);
  g.setColour(juce::Colour(0xff2b4158)); g.strokePath(track,juce::PathStrokeType(5.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
  juce::Path active; active.addCentredArc(c.x,c.y,r,r,0.0f,start,start+pos*(end-start),true);
  g.setColour(cyan); g.strokePath(active,juce::PathStrokeType(5.0f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
  auto a=start+pos*(end-start); auto tip=c+juce::Point<float>(std::sin(a),-std::cos(a))*(r-14.0f);
  g.setColour(ink); g.drawLine(c.x,c.y,tip.x,tip.y,3.0f);
  g.setColour(juce::Colour(0xff203a53)); g.fillEllipse(c.x-6,c.y-6,12,12);
 }
};
VOGLookAndFeel& vogLook() { static VOGLookAndFeel l; return l; }
}

VOGEditor::VOGEditor(VOGProcessor& p):juce::AudioProcessorEditor(&p),processor(p) {
#if VOG_EFFECT == 1
 effectId=1;
#elif VOG_EFFECT == 2
 effectId=2;
#elif VOG_EFFECT == 3
 effectId=3;
#else
 effectId=4;
#endif
 setLookAndFeel(&vogLook());
 setOpaque(true);
 const juce::String names[] = {"", "INFINITE REVERB", "COSMIC DELAY", "ATMOSPHERE", "DREAM CHORUS"};
 titleLabel.setText("VOG  /  " + names[effectId],juce::dontSendNotification);
 titleLabel.setFont(juce::Font(juce::FontOptions(27.0f,juce::Font::bold))); titleLabel.setColour(juce::Label::textColourId,ink); addAndMakeVisible(titleLabel);
 subtitleLabel.setText("AMBIENT FX SUITE     •     VST3     •     V2.0",juce::dontSendNotification);
 subtitleLabel.setColour(juce::Label::textColourId,cyan); subtitleLabel.setFont(juce::Font(juce::FontOptions(12.0f))); addAndMakeVisible(subtitleLabel);
 footerLabel.setText("VOG AUDIO     /     SOUND DESIGN TOOLS",juce::dontSendNotification);
 footerLabel.setColour(juce::Label::textColourId,juce::Colour(0xff8faac2)); footerLabel.setFont(juce::Font(juce::FontOptions(11.0f))); addAndMakeVisible(footerLabel);
 presetBox.addItem("INIT / DEFAULT",1); presetBox.addItem("WIDE & SOFT",2); presetBox.addItem("DEEP SPACE",3); presetBox.addItem("SUBTLE MOTION",4);
 presetBox.setSelectedId(1,juce::dontSendNotification); presetBox.onChange=[this]{applyPreset(presetBox.getSelectedId());}; addAndMakeVisible(presetBox);
 resetButton.onClick=[this]{presetBox.setSelectedId(1,juce::sendNotification);}; resetButton.setColour(juce::TextButton::buttonColourId,juce::Colour(0xff284461)); addAndMakeVisible(resetButton);
 auto& params=processor.parameters;
 const juce::String ids1[]={"mix","size","damp","width","freeze"};
 const juce::String ids2[]={"mix","time","feedback","pingpong","tone"};
 const juce::String ids3[]={"mix","time","feedback","motion","tone"};
 const juce::String ids4[]={"mix","rate","depth","centre","feedback"};
 const juce::String labels1[]={"WET MIX","ROOM SIZE","DAMPING","STEREO WIDTH","FREEZE"};
 const juce::String labels2[]={"WET MIX","DELAY TIME","FEEDBACK","PING PONG","DARKNESS"};
 const juce::String labels3[]={"WET MIX","TEXTURE LENGTH","FEEDBACK","MOTION","WARMTH"};
 const juce::String labels4[]={"WET MIX","RATE","DEPTH","CENTRE DELAY","FEEDBACK"};
 const juce::String* ids=effectId==1?ids1:effectId==2?ids2:effectId==3?ids3:ids4;
 const juce::String* labels=effectId==1?labels1:effectId==2?labels2:effectId==3?labels3:labels4;
 for(int i=0;i<5;++i){
  auto c=std::make_unique<Control>(); c->id=ids[i];
  c->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); c->slider.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
  c->slider.setRotaryParameters(juce::MathConstants<float>::pi*1.25f,juce::MathConstants<float>::pi*2.75f,true);
  c->label.setText(labels[i],juce::dontSendNotification); c->label.setJustificationType(juce::Justification::centred);
  c->label.setColour(juce::Label::textColourId,ink); c->label.setFont(juce::Font(juce::FontOptions(12.0f,juce::Font::bold)));
  c->value.setJustificationType(juce::Justification::centred); c->value.setColour(juce::Label::textColourId,cyan);
  c->value.setFont(juce::Font(juce::FontOptions(14.0f,juce::Font::bold)));
  addAndMakeVisible(c->slider); addAndMakeVisible(c->label); addAndMakeVisible(c->value);
  c->attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(params,ids[i],c->slider);
  controls.push_back(std::move(c));
 }
 resizeConstrainer.setSizeLimits(650,370,1400,820); addAndMakeVisible(corner); setResizable(true,false); setSize(920,510); startTimerHz(20);
}
VOGEditor::~VOGEditor(){stopTimer();setLookAndFeel(nullptr);}
void VOGEditor::timerCallback(){for(auto& c:controls){auto v=c->slider.getValue();juce::String s;if(c->id=="time"||c->id=="centre")s=juce::String(v,0)+" ms";else if(c->id=="rate")s=juce::String(v,2)+" Hz";else if(c->id=="freeze"||c->id=="pingpong")s=v>=0.5?"ON":"OFF";else s=juce::String(v,2);c->value.setText(s,juce::dontSendNotification);}}
void VOGEditor::applyPreset(int index){
 if(index<1||index>4)return;
 for(auto& c:controls){auto* raw=processor.parameters.getRawParameterValue(c->id);if(!raw)continue;
  float v=raw->load();
  if(index==1){if(c->id=="mix")v=0.4f;else if(c->id=="size")v=0.85f;else if(c->id=="damp")v=0.25f;else if(c->id=="width")v=1.f;else if(c->id=="freeze")v=0.f;else if(c->id=="time")v=effectId==2?420.f:480.f;else if(c->id=="feedback")v=effectId==2?0.55f:effectId==3?0.67f:0.12f;else if(c->id=="pingpong")v=1.f;else if(c->id=="tone")v=0.3f;else if(c->id=="motion")v=0.5f;else if(c->id=="rate")v=0.4f;else if(c->id=="depth")v=0.7f;else if(c->id=="centre")v=9.f;}
  else if(index==2){if(c->id=="mix")v=0.3f;else if(c->id=="feedback")v=0.28f;else if(c->id=="width")v=1.f;else if(c->id=="depth")v=0.5f;else if(c->id=="motion")v=0.2f;}
  else if(index==3){if(c->id=="mix")v=0.7f;else if(c->id=="feedback")v=0.75f;else if(c->id=="size")v=0.95f;else if(c->id=="time")v=effectId==2?850.f:1050.f;else if(c->id=="motion")v=0.8f;}
  else {if(c->id=="mix")v=0.2f;else if(c->id=="feedback")v=0.15f;else if(c->id=="motion")v=0.25f;else if(c->id=="depth")v=0.25f;}
  auto* param=processor.parameters.getParameter(c->id);if(param){auto norm=param->convertTo0to1(v);param->beginChangeGesture();param->setValueNotifyingHost(norm);param->endChangeGesture();}
 }
}
void VOGEditor::paint(juce::Graphics& g){
 g.fillAll(bg); auto b=getLocalBounds();
 g.setGradientFill(juce::ColourGradient(juce::Colour(0xff16304a),0,0,bg,(float)getWidth(),(float)getHeight(),false));g.fillRect(0,0,getWidth(),118);
 g.setColour(cyan);g.fillRect(0,0,getWidth(),5);
 auto area=juce::Rectangle<float>(22.f,139.f,(float)getWidth()-44.f,(float)getHeight()-213.f);
 g.setColour(panel);g.fillRoundedRectangle(area,18.f);g.setColour(juce::Colour(0xff345575));g.drawRoundedRectangle(area,18.f,1.2f);
 g.setColour(juce::Colour(0xff24445e));for(int i=1;i<5;++i){auto x=area.getX()+area.getWidth()*i/5.f;g.drawLine(x,area.getY()+16,x,area.getBottom()-16,1.f);}
 g.setColour(juce::Colour(0xff26425c));g.drawLine(24.f,(float)getHeight()-53.f,(float)getWidth()-24.f,(float)getHeight()-53.f,1.f);
}
void VOGEditor::resized(){
 auto w=getWidth(),h=getHeight(); titleLabel.setBounds(30,18,w-350,43);subtitleLabel.setBounds(32,66,w-350,26);
 presetBox.setBounds(w-286,32,176,32);resetButton.setBounds(w-99,32,72,32);
 int left=28,usable=w-56;int top=juce::jmax(160,h/3);int knob=juce::jmin(145,juce::jmax(84,usable/6));
 for(int i=0;i<(int)controls.size();++i){auto& c=*controls[(size_t)i];int cellLeft=left+i*usable/5;int cellW=usable/5;int center=cellLeft+cellW/2;
  c.label.setBounds(cellLeft,top-4,cellW,26);c.slider.setBounds(center-knob/2,top+23,knob,knob);c.value.setBounds(cellLeft,top+knob+27,cellW,30);
 }
 footerLabel.setBounds(30,h-47,w-90,28);corner.setBounds(w-18,h-18,16,16);
}
juce::AudioProcessorEditor* VOGProcessor::createEditor(){return new VOGEditor(*this);}
