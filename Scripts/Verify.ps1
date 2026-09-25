param([string]$Engine = 'C:\Program Files\Epic Games\UE_5.7', [switch]$Editor)
$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$log = Join-Path $root 'Saved\Logs\PupVerification.log'
New-Item -ItemType Directory -Force (Split-Path $log) | Out-Null
if ($Editor) {
    $exe = Join-Path $Engine 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
    $args = '"{0}\PupPop.uproject" -game -PupVerify -RenderOffscreen -windowed -ForceRes -ResX=1600 -ResY=1000 -unattended -nosplash -abslog="{1}"' -f $root,$log
} else {
    $exe = Join-Path $root 'Builds\Windows\PupPop.exe'
    $args = '-PupVerify -RenderOffscreen -windowed -ForceRes -ResX=1600 -ResY=1000 -unattended -nosplash -abslog="{0}"' -f $log
}
$p = Start-Process -FilePath $exe -ArgumentList $args -WorkingDirectory $root -WindowStyle Hidden -PassThru
if (!$p.WaitForExit(180000)) { $p.Kill(); throw 'Pup Pop verification timed out.' }
if ($p.ExitCode -ne 0) { throw "Pup Pop verification failed; see $log" }
$result = Select-String -LiteralPath $log -Pattern 'PUP_VERIFY_OK'
if (!$result) { throw "Missing success marker; see $log" }
Write-Host $result.Line
