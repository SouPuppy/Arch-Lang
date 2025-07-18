# scripts/install/uninstall.ps1
# Uninstall Arch CLI from $env:USERPROFILE\.arch and clean up PATH

param (
    [string]$InstallDir = "$env:USERPROFILE\.arch"
)

$BinDir = Join-Path $InstallDir "bin"

Write-Host "Uninstalling Arch CLI from: $InstallDir" -ForegroundColor Cyan

# Delete the installation directory
if (Test-Path $InstallDir) {
    try {
        Remove-Item -Recurse -Force -Path $InstallDir
        Write-Host "Deleted: $InstallDir" -ForegroundColor Green
    } catch {
        Write-Error "Failed to delete directory: $InstallDir"
        exit 1
    }
} else {
    Write-Warning "Install directory does not exist: $InstallDir"
}

# Remove bin directory from user PATH (if present)
$userPath = [Environment]::GetEnvironmentVariable("Path", "User")
$pathList = $userPath -split ";" | Where-Object { $_.TrimEnd('\') -ne $BinDir }

if ($pathList.Count -ne ($userPath -split ";").Count) {
    try {
        $newPath = ($pathList -join ";").TrimEnd(";")
        [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
        Write-Host "`nRemoved '$BinDir' from user PATH." -ForegroundColor Yellow
    } catch {
        Write-Warning "Failed to update PATH. Please remove '$BinDir' manually."
    }
} else {
    Write-Host "`nPath '$BinDir' not found in user PATH." -ForegroundColor Gray
}

Write-Host "`nUninstallation complete." -ForegroundColor Green
