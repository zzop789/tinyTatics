<#
.SYNOPSIS
Creates a paired C++ header/source file for this project.

.DESCRIPTION
Flow: developer command -> NewTinyClass.ps1 -> ClassName.h/ClassName.cpp.
Use this when adding a new engine or sandbox class and you want the basic
namespace, include path, constructor/destructor, and navigation comments filled in.

The script will not overwrite existing files unless -Force is passed.
The generated .cpp include path is based on the project src/ root when possible,
for example:
    #include "Engine/Renderer/Camera2D.h"

.EXAMPLE
powershell -ExecutionPolicy Bypass -File scripts\NewTinyClass.ps1 `
  -ClassName Camera2D `
  -Directory src\Engine\Renderer

Creates:
    src\Engine\Renderer\Camera2D.h
    src\Engine\Renderer\Camera2D.cpp

Uses the default namespace:
    TinyTactics

.EXAMPLE
powershell -ExecutionPolicy Bypass -File scripts\NewTinyClass.ps1 `
  -ClassName BaseLayer `
  -Directory src\Sandbox\Layers `
  -Namespace TinyTacticsSandbox

Creates a class under a custom namespace.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File scripts\NewTinyClass.ps1 `
  -ClassName Camera2D `
  -Directory src\Engine\Renderer `
  -Force

Overwrites existing files. Use carefully.
#>

param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^[A-Za-z_][A-Za-z0-9_]*$')]
    [string]$ClassName,

    [Parameter(Mandatory = $true)]
    [string]$Directory,

    [string]$Namespace = "TinyTactics",

    [switch]$Force
)

$ErrorActionPreference = "Stop"

$workspaceRoot = Split-Path -Parent $PSScriptRoot

function Get-RelativePathCompat {
    param(
        [Parameter(Mandatory = $true)]
        [string]$BasePath,

        [Parameter(Mandatory = $true)]
        [string]$TargetPath
    )

    $baseUri = New-Object System.Uri(($BasePath.TrimEnd('\', '/') + [System.IO.Path]::DirectorySeparatorChar))
    $targetUri = New-Object System.Uri($TargetPath)
    return [System.Uri]::UnescapeDataString($baseUri.MakeRelativeUri($targetUri).ToString()).Replace('/', '\')
}

$targetDirectory = if ([System.IO.Path]::IsPathRooted($Directory)) {
    $Directory
} else {
    Join-Path $workspaceRoot $Directory
}

$targetDirectory = [System.IO.Path]::GetFullPath($targetDirectory)
$headerPath = Join-Path $targetDirectory "$ClassName.h"
$sourcePath = Join-Path $targetDirectory "$ClassName.cpp"

if ((Test-Path $headerPath) -and -not $Force) {
    throw "Header already exists: $headerPath. Use -Force to overwrite."
}

if ((Test-Path $sourcePath) -and -not $Force) {
    throw "Source already exists: $sourcePath. Use -Force to overwrite."
}

New-Item -ItemType Directory -Force -Path $targetDirectory | Out-Null

$srcRoot = [System.IO.Path]::GetFullPath((Join-Path $workspaceRoot "src"))
$targetHeaderFullPath = [System.IO.Path]::GetFullPath($headerPath)

if ($targetHeaderFullPath.StartsWith($srcRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
    $includePath = $targetHeaderFullPath.Substring($srcRoot.Length).TrimStart('\', '/')
} else {
    $includePath = Get-RelativePathCompat -BasePath $workspaceRoot -TargetPath $targetHeaderFullPath
}

$includePath = $includePath.Replace('\', '/')

if ([string]::IsNullOrWhiteSpace($Namespace)) {
    $headerContent = @"
#pragma once

// Owns the public-facing behavior for $ClassName.
// Flow: caller -> $ClassName -> next subsystem.
// Jump: add implementation details in $ClassName.cpp.
class $ClassName
{
public:
    $ClassName();
    ~$ClassName();
};
"@

    $sourceContent = @"
#include "$includePath"

$ClassName::$ClassName() = default;

$ClassName::~$ClassName() = default;
"@
} else {
    $headerContent = @"
#pragma once

namespace $Namespace
{
    // Owns the public-facing behavior for $ClassName.
    // Flow: caller -> $ClassName -> next subsystem.
    // Jump: add implementation details in $ClassName.cpp.
    class $ClassName
    {
    public:
        $ClassName();
        ~$ClassName();
    };
}
"@

    $sourceContent = @"
#include "$includePath"

namespace $Namespace
{
    $ClassName::$ClassName() = default;

    $ClassName::~$ClassName() = default;
}
"@
}

$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText($headerPath, $headerContent, $utf8NoBom)
[System.IO.File]::WriteAllText($sourcePath, $sourceContent, $utf8NoBom)

Write-Host "Created: $headerPath"
Write-Host "Created: $sourcePath"
