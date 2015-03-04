echo off
echo Removing old release of Fractured Nations
del FracturedNations.zip /Q
rmdir ..\FracturedNations.build /s /q
echo Building release for Fractured Nations
mkdir ..\FracturedNations.build\FracturedNations
(
echo Exclusion_List.txt
echo build_release.bat
echo provinces.txt
echo src\
echo bin\
)>Exclusion_List.txt
xcopy *.* ..\FracturedNations.build\FracturedNations /E /Q /EXCLUDE:Exclusion_List.txt
del Exclusion_List.txt
move ..\FracturedNations.build\FracturedNations\FracturedNations.* ..\FracturedNations.build
"C:\Program Files\7-Zip\7z" a -r FracturedNations.zip ..\FracturedNations.build\*.*
rmdir ..\FracturedNations.build /s /q
echo Done
pause