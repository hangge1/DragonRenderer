param(
    [string]$Root = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
)

$ErrorActionPreference = "Stop"
$utf8 = [System.Text.UTF8Encoding]::new($false, $true)

$textExtensions = @(
    ".cc", ".h", ".hpp", ".c", ".cpp", ".cppm", ".inl", ".cmake", ".txt", ".md",
    ".json", ".ps1", ".gitignore", ".gitattributes", ".editorconfig",
    ".yml", ".yaml"
)
$textNames = @("CMakeLists.txt", "LICENSE")
$excludedSegments = @(
    [IO.Path]::DirectorySeparatorChar + ".git" + [IO.Path]::DirectorySeparatorChar,
    [IO.Path]::DirectorySeparatorChar + "build" + [IO.Path]::DirectorySeparatorChar,
    [IO.Path]::DirectorySeparatorChar + "assets" + [IO.Path]::DirectorySeparatorChar,
    [IO.Path]::DirectorySeparatorChar + "README.assets" + [IO.Path]::DirectorySeparatorChar,
    [IO.Path]::DirectorySeparatorChar + "thirdparts" + [IO.Path]::DirectorySeparatorChar
)

$rootPath = (Resolve-Path $Root).Path
$invalidFiles = New-Object System.Collections.Generic.List[string]

Get-ChildItem -LiteralPath $rootPath -Recurse -File | ForEach-Object {
    $path = $_.FullName
    foreach($segment in $excludedSegments) {
        if($path.Contains($segment)) {
            return
        }
    }

    if(($textExtensions -notcontains $_.Extension) -and ($textNames -notcontains $_.Name)) {
        return
    }

    try {
        $bytes = [System.IO.File]::ReadAllBytes($path)
        [void]$utf8.GetString($bytes)
    }
    catch {
        $invalidFiles.Add($path)
    }
}

if($invalidFiles.Count -gt 0) {
    Write-Error ("Non-UTF-8 text files found:`n" + ($invalidFiles -join "`n"))
    exit 1
}

Write-Host "UTF-8 encoding check passed."
