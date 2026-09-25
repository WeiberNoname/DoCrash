param([string]$Engine = 'C:\Program Files\Epic Games\UE_5.7')
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$project = Join-Path $root 'PupPop.uproject'
& "$Engine\Engine\Build\BatchFiles\Build.bat" PupPopEditor Win64 Development "-Project=$project" -WaitMutex -NoHotReloadFromIDE
if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
& "$Engine\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" $project -run=pythonscript "-script=$PSScriptRoot\create_content.py" -unattended -nullrhi -AllowCommandletAudio -nosplash
if ($LASTEXITCODE -ne 0) { throw 'Content generation failed.' }
& "$Engine\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun "-project=$project" -noP4 -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$root\Builds" -unattended -utf8output -NoCompileEditor
if ($LASTEXITCODE -ne 0) { throw 'Packaging failed.' }

