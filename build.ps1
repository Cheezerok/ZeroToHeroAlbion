# Set Qt paths
$env:QTDIR = "C:\Qt\Qt5.14.0\5.14.0\mingw73_64"

# Find MinGW
$possibleMinGWPaths = @(
    "C:\Qt\Qt5.14.0\Tools\mingw730_64",
    "$env:QTDIR",
    "C:\Qt\Qt5.14.0\5.14.0\mingw73_64"
)

$mingwPath = $null
foreach ($path in $possibleMinGWPaths) {
    Write-Host "Checking for MinGW in: $path"
    if (Test-Path "$path\bin\g++.exe") {
        $mingwPath = $path
        break
    }
}

if ($null -eq $mingwPath) {
    Write-Host "Error: Could not find MinGW installation!"
    Write-Host "Please make sure MinGW is installed in one of these locations:"
    $possibleMinGWPaths | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host "Found MinGW at: $mingwPath"
$env:MINGW = $mingwPath
$env:PATH = "$env:QTDIR\bin;$env:MINGW\bin;$env:PATH"

Write-Host "Current directory: $(Get-Location)"
Write-Host "PATH: $env:PATH"

# Remove old build directory if it exists
if (Test-Path build) {
    Write-Host "Removing old build directory..."
    Remove-Item -Recurse -Force build
}

# Create and enter build directory
Write-Host "Creating new build directory..."
New-Item -ItemType Directory -Force -Path build
Set-Location build

# Create plugins directory and copy Qt plugins
Write-Host "Copying Qt plugins and DLLs..."
New-Item -ItemType Directory -Force -Path platforms
Copy-Item "$env:QTDIR\plugins\platforms\qwindows.dll" -Destination "platforms\" -Force

# Copy Qt DLLs
Copy-Item "$env:QTDIR\bin\Qt5Core.dll" -Destination "." -Force
Copy-Item "$env:QTDIR\bin\Qt5Gui.dll" -Destination "." -Force
Copy-Item "$env:QTDIR\bin\Qt5Widgets.dll" -Destination "." -Force

# Copy MinGW runtime DLLs
Copy-Item "$mingwPath\bin\libgcc_s_seh-1.dll" -Destination "." -Force
Copy-Item "$mingwPath\bin\libstdc++-6.dll" -Destination "." -Force
Copy-Item "$mingwPath\bin\libwinpthread-1.dll" -Destination "." -Force

# Copy images directory
Write-Host "Copying images directory..."
New-Item -ItemType Directory -Force -Path images
Copy-Item "..\images\*" -Destination "images\" -Recurse -Force

# Configure and build
Write-Host "Configuring CMake..."
Write-Host "Using compilers:"
Write-Host "  C: $mingwPath\bin\gcc.exe"
Write-Host "  C++: $mingwPath\bin\g++.exe"

# Remove CMake cache if it exists
if (Test-Path CMakeCache.txt) {
    Write-Host "Removing CMake cache..."
    Remove-Item CMakeCache.txt -Force
}

cmake -G "MinGW Makefiles" `
    -DCMAKE_PREFIX_PATH="$env:QTDIR" `
    -DCMAKE_C_COMPILER="$mingwPath\bin\gcc.exe" `
    -DCMAKE_CXX_COMPILER="$mingwPath\bin\g++.exe" `
    -DCMAKE_MAKE_PROGRAM="$mingwPath\bin\mingw32-make.exe" `
    ..

if ($LASTEXITCODE -eq 0) {
    Write-Host "Building project..."
    cmake --build . --clean-first
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build completed successfully!"
        
        # Create qt.conf to help Qt find the plugins
        @"
[Paths]
Plugins = .
"@ | Out-File -FilePath "qt.conf" -Encoding ASCII
        
        Write-Host "You can now run: .\AlbionZeroToHero.exe"
    } else {
        Write-Host "Build failed!"
    }
} else {
    Write-Host "CMake configuration failed!"
} 