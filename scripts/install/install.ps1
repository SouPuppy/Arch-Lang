# scripts/install/install.ps1
# Install `arch` CLI into $env:USERPROFILE\.arch\bin

param (
    [string]$InstallDir = "$env:USERPROFILE\.arch"
)

# Locate the top-level CMakeLists.txt as project root
function Find-ProjectRoot {
    $current = Get-Location
    while ($current -ne $null) {
        if (Test-Path (Join-Path $current "CMakeLists.txt")) {
            return $current
        }
        $parent = Split-Path $current -Parent
        if ($parent -eq $current) { break }
        $current = $parent
    }
    throw "CMakeLists.txt not found. Please run this script from within the project directory."
}

# Find root directory
try {
    $ProjectRoot = Find-ProjectRoot
    Write-Host "Project root found at: $ProjectRoot" -ForegroundColor Cyan
} catch {
    Write-Error $_.Exception.Message
    exit 1
}

# CMake build and install
$BuildDir = Join-Path $ProjectRoot "build"
$Config = "Release"

Write-Host "Installing Arch CLI to: $InstallDir" -ForegroundColor Cyan

# Configure
cmake -S $ProjectRoot -B $BuildDir -DCMAKE_BUILD_TYPE=$Config -DCMAKE_INSTALL_PREFIX="$InstallDir"
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configure failed."
    exit 1
}

# Build
cmake --build $BuildDir --config $Config
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed."
    exit 1
}

# Install
cmake --install $BuildDir
if ($LASTEXITCODE -ne 0) {
    Write-Error "Install failed."
    exit 1
}

# Add bin directory to user PATH permanently if not present
$BinDir = Join-Path $InstallDir "bin"
$userPath = [Environment]::GetEnvironmentVariable("Path", "User")

if (-not ($userPath -split ";" | Where-Object { $_.TrimEnd('\') -eq $BinDir })) {
    try {
        $newPath = "$userPath;$BinDir"
        [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
        Write-Host "`n'$BinDir' has been added to your user PATH." -ForegroundColor Green
        Write-Host "You may need to restart your terminal or re-login to apply changes." -ForegroundColor Yellow
    } catch {
        Write-Warning "Failed to update PATH. Please add '$BinDir' manually."
    }
} else {
    Write-Host "`nInstalled successfully and already in user PATH." -ForegroundColor Green
}
