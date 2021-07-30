@echo off

rem Copy asset files.
echo Copying asset files...
xcopy ..\data android\assets\data /E /Y

rem Copy dependencies.
echo Copying dependencies...
xcopy ..\deps\android\arm64-v8a\SDL2\bin android\tmp\lib\arm64-v8a /E /Y
xcopy ..\deps\android\arm64-v8a\SDL2_image\bin android\tmp\lib\arm64-v8a /E /Y
xcopy ..\deps\android\arm64-v8a\SDL2_mixer\bin android\tmp\lib\arm64-v8a /E /Y

rem Build the main library.
echo Building main library...
set NDK_PROJECT_PATH=.
call ndk-build NDK_APPLICATION_MK=./Application64.mk

rem Copy the main library to the apk temp dir.
echo Copying main library to temp dir...
copy libs\arm64-v8a\libmain.so android\tmp\lib\arm64-v8a

rem Run the apk build script
echo Building apk...
cd android
call build-apk
cd ..

rem Move apk to parent dir.
echo Moving apk to output dir...
del android\tmp\tmp.apk
move android\tmp\*.apk ..\bin\android\arm64-v8a
cmd