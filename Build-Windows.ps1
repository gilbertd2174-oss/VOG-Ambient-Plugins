$ErrorActionPreference = 'Stop'
Set-Location $PSScriptRoot
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) { throw 'CMake is missing from PATH.' }
Write-Host 'VOG Ambient FX 2.0: configuring four VST3 plugins' -ForegroundColor Cyan
cmake -S . -B build-G -G 'Visual Studio 18 2026' -A x64
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
cmake --build build-G --config Release --parallel 4
if ($LASTEXITCODE -ne 0) { throw 'VST3 build failed.' }
Get-ChildItem .\build-G -Filter '*.vst3' -Recurse | Where-Object { $_.FullName -like '*_artefacts\Release\VST3\*' } | Select-Object Name,FullName
Write-Host 'Build complete. Copy the four .vst3 bundles to your VST3 folder.' -ForegroundColor Green
