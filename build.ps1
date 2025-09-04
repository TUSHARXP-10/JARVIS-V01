# Jarvis Build Script for Windows PowerShell

param(
    [string]$BuildType = "Release",
    [string]$Generator = "Ninja",
    [switch]$Clean = $false,
    [switch]$Help = $false
)

function Show-Help {
    Write-Host "Jarvis Build Script"
    Write-Host "Usage: .\build.ps1 [options]"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -BuildType <type>    Build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
    Write-Host "  -Generator <gen>    CMake generator (Ninja, Visual Studio 17 2022, etc.)"
    Write-Host "  -Clean              Clean build directory before building"
    Write-Host "  -Help               Show this help message"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build.ps1 -BuildType Debug"
    Write-Host "  .\build.ps1 -Clean -BuildType Release"
}

if ($Help) {
    Show-Help
    exit 0
}

# Check if CMake is installed
if (!(Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Error "CMake is not installed or not in PATH"
    exit 1
}

# Check if Ninja is installed if using Ninja generator
if ($Generator -eq "Ninja" -and !(Get-Command ninja -ErrorAction SilentlyContinue)) {
    Write-Error "Ninja is not installed or not in PATH"
    Write-Host "Install Ninja with: choco install ninja"
    exit 1
}

# Clean build directory if requested
if ($Clean) {
    Write-Host "Cleaning build directory..."
    if (Test-Path "build") {
        Remove-Item -Recurse -Force "build"
    }
}

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}

Set-Location "build"

Write-Host "Configuring with CMake..."
Write-Host "Build Type: $BuildType"
Write-Host "Generator: $Generator"

# Configure
$cmakeArgs = @(
    "..",
    "-G", $Generator,
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
)

$process = Start-Process cmake -ArgumentList $cmakeArgs -NoNewWindow -Wait -PassThru
if ($process.ExitCode -ne 0) {
    Write-Error "CMake configuration failed"
    Set-Location ..
    exit $process.ExitCode
}

Write-Host "Building..."

# Build
$buildArgs = @(
    "--build", ".",
    "--config", $BuildType,
    "--parallel"
)

$process = Start-Process cmake -ArgumentList $buildArgs -NoNewWindow -Wait -PassThru
if ($process.ExitCode -ne 0) {
    Write-Error "Build failed"
    Set-Location ..
    exit $process.ExitCode
}

Set-Location ..

Write-Host ""
Write-Host "Build completed successfully!"
Write-Host "Executable: build\jarvis.exe"
Write-Host ""
Write-Host "To run Jarvis:"
Write-Host "  .\build\jarvis.exe"