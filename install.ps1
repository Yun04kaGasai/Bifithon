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

if (-not (Test-Path (Join-Path $RepoPath "tools\bifc.cpp"))) {
    Write-Error "Run this script inside the Bifithon repo root (tools/bifc.cpp not found)."
}

function To-MsysPath($path) {
    $p = $path -replace "\\", "/"
    if ($p -match "^([A-Za-z]):") {
        $drive = $matches[1].ToLower()
        $p = "/$drive" + $p.Substring(2)
    }
    return $p
}

$msysPath = "C:\msys64\ucrt64\bin"
$bashPath = "C:\msys64\usr\bin\bash.exe"

function Invoke-Msys($command) {
    & $bashPath -lc "export MSYSTEM=UCRT64; export PATH=/ucrt64/bin:/usr/bin; $command"
}
if (Test-Command "winget" -and -not (Test-Path $bashPath)) {
    Write-Host "Installing MSYS2..."
    winget install -e --id MSYS2.MSYS2
}

if (Test-Path $bashPath) {
    Write-Host "Updating pacman and installing GCC, wxWidgets, CMake..."
    Invoke-Msys "pacman -Syu --noconfirm"
    Invoke-Msys "pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-wxWidgets mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make"
    Add-ToUserPath $msysPath
} else {
    if (-not (Test-Command "g++")) {
        Write-Error "g++ not found and MSYS2 is unavailable. Install MSYS2/MinGW manually."
    }
    if (-not (Test-Command "cmake")) {
        Write-Error "cmake not found. Install CMake manually."
    }
    Write-Error "wxWidgets not found. Install wxWidgets and try again."
}

$bashPath = "C:\msys64\usr\bin\bash.exe"
if (Test-Path $bashPath) {
    $msysRepo = To-MsysPath $RepoPath
    Write-Host "Building C++ compiler..."
    Invoke-Msys "cd $msysRepo && g++ -std=c++17 -O2 tools/bifc.cpp -o tools/bifc.exe"

    Write-Host "Building C++ IDE..."
    Invoke-Msys "cd $msysRepo/ide && cmake -S . -B build && cmake --build build"

    Write-Host "Done. Run: ide\\build\\bifide.exe"
} else {
    Write-Error "bash.exe not found. Install MSYS2 and try again."
}
