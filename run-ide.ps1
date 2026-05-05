<#
.SYNOPSIS
  One-command Tau IDE launcher (Windows PowerShell)

.DESCRIPTION
  Builds the tau binary (if needed), sets up a Python virtual environment,
  installs dependencies, and starts the IDE server.

.PARAMETER Port
  Server port (default: 8080)

.PARAMETER SkipBuild
  Skip C++ build. The IDE still works for editing, help, grammar,
  visualization, and AI — only Run/Eval/REPL need the binary.

.EXAMPLE
  .\run-ide.ps1
  .\run-ide.ps1 -Port 9090
  .\run-ide.ps1 -SkipBuild
#>
param(
    [int]$Port = 8080,
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
Set-Location $ScriptDir

function Info  ($msg) { Write-Host "[IDE] $msg" -ForegroundColor Cyan }
function Ok    ($msg) { Write-Host "[IDE] $msg" -ForegroundColor Green }
function Warn  ($msg) { Write-Host "[IDE] $msg" -ForegroundColor Yellow }
function Fail  ($msg) { Write-Host "[IDE] $msg" -ForegroundColor Red; exit 1 }

# ---- Locate Python ---------------------------------------------------------
$Python = $null
foreach ($cmd in @("python3", "python")) {
    try {
        $ver = & $cmd -c "import sys; print(sys.version_info[:2] >= (3,10))" 2>$null
        if ($ver -eq "True") {
            $Python = $cmd
            break
        }
    } catch {}
}
if (-not $Python) { Fail "Python 3.10+ not found. Install it and try again." }
$pyVer = & $Python --version 2>&1
Info "Using Python: $Python ($pyVer)"

# ---- Fix unordered_dense if needed -----------------------------------------
function Fix-UnorderedDense {
    $parserDir = Join-Path $ScriptDir "external\parser"
    $vendorDir = Join-Path $parserDir "external\ankerl"
    $header    = Join-Path $vendorDir "unordered_dense.h"
    $cmakePath = Join-Path $parserDir "CMakeLists.txt"

    if ((Test-Path $cmakePath) -and (Select-String -Path $cmakePath -Pattern "unordered_dense" -Quiet)) {
        if (-not (Test-Path $header)) {
            Info "Vendoring ankerl/unordered_dense header..."
            New-Item -ItemType Directory -Path $vendorDir -Force | Out-Null
            $url = "https://raw.githubusercontent.com/martinus/unordered_dense/v4.4.0/include/ankerl/unordered_dense.h"
            try {
                Invoke-WebRequest -Uri $url -OutFile $header -UseBasicParsing
                Ok "Vendored unordered_dense.h"
            } catch {
                Warn "Failed to download header: $_"
            }
        }
    }
}

# ---- Find tau binary -------------------------------------------------------
function Find-TauBinary {
    foreach ($d in @("build-Release", "build-Debug", "build-RelWithDebInfo", "build")) {
        $p = Join-Path $ScriptDir "$d\tau.exe"
        if (Test-Path $p) { return $p }
        $p2 = Join-Path $ScriptDir "$d\tau"
        if (Test-Path $p2) { return $p2 }
    }
    $inPath = Get-Command tau -ErrorAction SilentlyContinue
    if ($inPath) { return $inPath.Source }
    return $null
}

# ---- Build tau binary ------------------------------------------------------
if (-not $SkipBuild) {
    $tauBin = Find-TauBinary
    if ($tauBin) {
        Ok "Found tau binary: $tauBin"
    } else {
        Info "No tau binary found - building from source..."

        if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
            Fail "cmake not found. Install CMake 3.22+ and try again."
        }

        # Initialize submodules
        if (Test-Path ".gitmodules") {
            Info "Initializing git submodules..."
            git submodule update --init --recursive 2>$null
        }

        Fix-UnorderedDense

        Info "Building tau (release mode)..."
        if (Test-Path "release.sh") {
            # On Windows with Git Bash available
            $gitBash = Get-Command bash -ErrorAction SilentlyContinue
            if ($gitBash) {
                & bash release.sh
            } else {
                New-Item -ItemType Directory -Path "build-Release" -Force | Out-Null
                Set-Location "build-Release"
                cmake .. -DCMAKE_BUILD_TYPE=Release
                cmake --build . --config Release
                Set-Location $ScriptDir
            }
        } else {
            New-Item -ItemType Directory -Path "build-Release" -Force | Out-Null
            Set-Location "build-Release"
            cmake .. -DCMAKE_BUILD_TYPE=Release
            cmake --build . --config Release
            Set-Location $ScriptDir
        }

        $tauBin = Find-TauBinary
        if ($tauBin) {
            Ok "Build successful: $tauBin"
        } else {
            Warn "Build completed but tau binary not found."
            Warn "The IDE will still work for editing, help, grammar, visualization, and AI."
        }
    }
} else {
    Info "Skipping C++ build (-SkipBuild)"
    $tauBin = Find-TauBinary
    if ($tauBin) { Ok "Found existing tau binary: $tauBin" }
    else { Warn "No tau binary found. Run/Eval/REPL will be unavailable." }
}

# ---- Set up Python venv ----------------------------------------------------
$VenvDir = Join-Path $ScriptDir ".venv"
if (-not (Test-Path $VenvDir)) {
    Info "Creating Python virtual environment..."
    & $Python -m venv $VenvDir
    if ($LASTEXITCODE -ne 0) { Fail "Failed to create venv." }
}

# Activate
$activateScript = Join-Path $VenvDir "Scripts\Activate.ps1"
if (-not (Test-Path $activateScript)) {
    # Linux-style venv on WSL
    $activateScript = Join-Path $VenvDir "bin\Activate.ps1"
}
& $activateScript

Info "Activated venv: $VenvDir"

# ---- Install Python dependencies -------------------------------------------
Info "Installing Python dependencies..."
pip install --quiet --upgrade pip 2>$null
pip install --quiet -r ide\requirements.txt
if ($LASTEXITCODE -ne 0) { Fail "pip install failed" }
Ok "Python dependencies installed"

# ---- Launch IDE server ------------------------------------------------------
Write-Host ""
Ok "============================================"
Ok "  Tau IDE starting on http://localhost:$Port"
Ok "============================================"
Write-Host ""
Info "Press Ctrl+C to stop the server."
Write-Host ""

& $Python -m uvicorn ide.server:app --host 0.0.0.0 --port $Port
