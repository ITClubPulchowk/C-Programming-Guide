@echo off

setlocal

pushd ..

if not exist "Libraries" mkdir Libraries
if exist "Libraries/SDL2/SDL2-2.0.14" goto SkipDownloadSDL
echo Downloading SDL

pushd Libraries
mkdir SDL2
curl "https://www.libsdl.org/release/SDL2-devel-2.0.14-VC.zip" --output SDL2.zip
tar -zxvf SDL2.zip -C SDL2
del SDL2.zip
ren "SDL2\SDL2-2.0.14\include" "SDL2"
popd

:SkipDownloadSDL
if exist "Libraries/SDL2MinGw/SDL2-2.0.14/" goto SkipDownloadSDLMinGw
echo Downloading SDLMinGw

pushd Libraries
mkdir SDL2MinGw
curl "https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz" --output SDL2MinGw.tar.gz
tar -xf SDL2MinGw.tar.gz -C SDL2MinGw
del SDL2MinGw.tar.gz
popd

:SkipDownloadSDLMinGw
echo -------------------

popd
