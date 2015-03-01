echo off
echo Removing old release of Fractured Nations
del FracturedNations.zip /Q
rmdir ..\FracturedNations.build /s /q
echo Building release for Fractured Nations
mkdir ..\FracturedNations.build\FracturedNations
xcopy *.* ..\FracturedNations.build\FracturedNations /E /Q /EXCLUDE:..\FracturedNations.build\FracturedNations\build_release.bat+..\FracturedNations.build\FracturedNations\provinces.txt+..\FracturedNations.build\FracturedNations\src+..\FracturedNations.build\FracturedNations\bin
move ..\FracturedNations.build\FracturedNations\FracturedNations.* ..\FracturedNations.build
"C:\Program Files\7-Zip\7z" a -r FracturedNations.zip ..\FracturedNations.build\*.*
rmdir ..\FracturedNations.build /s /q
echo Done
pause