param(
    [string]$RepoPath = (Get-Location).Path
)

$ErrorActionPreference = "Stop"

function Test-Command($name) {
    return [bool](Get-Command $name -ErrorAction SilentlyContinue)
}

function Add-ToUserPath($pathToAdd) {
    $current = [Environment]::GetEnvironmentVariable("Path", "User")
    if ($current -and $current.Split(";") -contains $pathToAdd) {
        return
    }
    $newPath = if ($current) { "$current;$pathToAdd" } else { $pathToAdd }
    [Environment]::SetEnvironmentVariable("Path", $newPath, "User")
    Write-Host "Added to PATH (User): $pathToAdd"
}

if (-not (Test-Path (Join-Path $RepoPath "tools\bifc.py"))) {
    Write-Error "Run this script inside the Bifithon repo root (tools/bifc.py not found)."
}

if (-not (Test-Command "python")) {
    if (Test-Command "winget") {
        Write-Host "Installing Python..."
        winget install -e --id Python.Python.3.11
    } else {
        Write-Error "Python not found and winget is unavailable. Install Python 3.11+ manually."
    }
}

if (-not (Test-Command "g++")) {
    if (Test-Command "winget") {
        Write-Host "Installing MSYS2..."
        winget install -e --id MSYS2.MSYS2
        $msysPath = "C:\msys64\ucrt64\bin"
        $bashPath = "C:\msys64\usr\bin\bash.exe"
        if (Test-Path $bashPath) {
            Write-Host "Installing GCC via pacman..."
            & $bashPath -lc "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc"
            Add-ToUserPath $msysPath
        } else {
            Write-Error "MSYS2 installed but bash.exe not found. Install GCC manually."
        }
    } else {
        Write-Error "g++ not found and winget is unavailable. Install MSYS2/MinGW manually."
    }
}

Write-Host "Building C++ compiler..."
& g++ -std=c++17 -O2 "tools\bifc.cpp" -o "tools\bifc.exe"

Write-Host "Done. You can run: python ide\bifide.py"
