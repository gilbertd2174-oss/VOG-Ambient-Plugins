# VOG Ambient FX Suite — Windows VST3 source

Four separate audio-effect plugins built with JUCE 8 and CMake:

- **VOG Infinite Reverb**: room size, damping, stereo width, freeze threshold, wet/dry mix.
- **VOG Cosmic Delay**: time, feedback, ping-pong, tone, wet/dry mix.
- **VOG Atmosphere**: modulated feedback delay texture, warmth, motion, wet/dry mix.
- **VOG Dream Chorus**: chorus rate, depth, centre delay, feedback, wet/dry mix.

These are **real VST3 audio processors**, not browser effects. They use JUCE's standard generic parameter editor. Parameters can be automated by the DAW, and state can be saved with the session.

## Build on Windows

1. Install **Visual Studio 2022** with the **Desktop development with C++** workload.
2. Install **CMake 3.22+** and **Git** (make both available on PATH).
3. Extract this ZIP, open PowerShell in the `VOG_VST3_Source` folder, and run `powershell -ExecutionPolicy Bypass -File .\Build-Windows.ps1`.
4. CMake downloads JUCE 8.0.10 from GitHub at configuration time; internet access is required for the initial build.
5. Locate the generated `.vst3` bundles in the build directory, then copy them to `C:\Program Files\Common Files\VST3` and rescan plugins in your DAW.

## Notes

- Windows 64-bit VST3 only. These plugins accept mono/stereo audio, and are audio effects, not instruments.
- VOG Atmosphere is a modulated feedback texture effect, **not a true granular or pitch-shifting engine**.
- VOG Infinite Reverb uses JUCE's reverb with freeze; **pitch-shifted shimmer is not implemented**.
- Cosmic Delay time is in milliseconds, not host-tempo synced.
- Source is provided because this Linux build environment cannot generate or validate Windows VST3 bundles. The Windows compilation has **not been tested**.
- JUCE has its own licensing terms; review https://juce.com/legal/ before distribution.
