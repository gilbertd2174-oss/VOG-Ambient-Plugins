VOG AMBIENT FX SUITE 2.0 - CUSTOM INTERFACE SOURCE
==================================================
This is a source-code upgrade, not precompiled VST3 binaries.
Includes: four VST3 effects, shared custom editor, five rotary controls per plugin,
4 simple parameter presets, resizable window, blue/navy VOG design.

WINDOWS BUILD
1. Back up your existing VOG_VST3_Source project and installed .vst3 files.
2. Extract this ZIP. Do not merge individual files into an older Source directory.
3. Open PowerShell in VOG_VST3_Source and run:
   powershell -ExecutionPolicy Bypass -File .\Build-Windows.ps1
4. The project expects Visual Studio 2026 C++ tools, Git, and CMake.
5. If compilation succeeds, find bundles under build-G\<target>_artefacts\Release\VST3.
6. Close Reason 14 before replacing plugin bundles. Copy the new .vst3 bundles
   into the SAME directory Reason currently scans. Reopen Reason and rescan.
7. Keep a backup of your previous plugins until new versions load and sound correct.

NOTES
- This package has not been compiled or host-tested in this environment.
- Presets are applied to plugin parameters and saved in host state; they are not
  independent .fxp preset files.
- Existing DSP engines are retained. No new granular engine or true shimmer
  algorithm is claimed.
- Window has rotary controls and parameter values, not a real-time audio meter.
- The editor is shared, with distinct names and parameters for each effect.
