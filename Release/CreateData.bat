@echo off
setlocal enabledelayedexpansion

set /p url=Enter a playlist URL to download audio from: 
set /p start=Enter the start index (Inclusive, 1-based): 
set /p end=Enter the end index (Inclusive):

cd ..\training_set\audio\raw\

yt-dlp !url! --extract-audio --audio-format wav --postprocessor-args "ExtractAudio:-t 30" -I, --playlist-items !start!:!end!

cd ..

set formattedDirIndex=0

:searchFormattedDir

if not exist formatted\!formattedDirIndex!\ (
    goto :foundFormattedDir
) else (
    set /A formattedDirIndex=formattedDirIndex+1
    goto :searchFormattedDir
)

:foundFormattedDir

mkdir formatted\!formattedDirIndex!\

set count=0
for %%F in (raw\*.*) do (
    ffmpeg -i "%%F" -ar 44100 -acodec pcm_f32le -ac 1 "formatted\!formattedDirIndex!\!count!.wav"
    if %errorlevel%==0 (
        echo !count! - %%F >> Pairings!formattedDirIndex!.txt
        set /A count=count+1
    )
)

echo %count% > formatted\!formattedDirIndex!\count.txt

del /q raw

cd ..

set trainingSetPath=%cd%

mkdir data\!formattedDirIndex!

cd ..\Release

echo Downloading and formatting complete, running analyzer

Analyzer !trainingSetPath!\audio\formatted\!formattedDirIndex!\ !trainingSetPath!\data\!formattedDirIndex!







