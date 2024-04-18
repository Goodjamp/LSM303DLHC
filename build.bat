rmdir /S /Q build
set buildType=RELEASE
set /A varNuber = 0

:: Count the number of input arguments
for %%x in (%*) do Set /A  varNuber+=1

if %varNuber%==1 set buildType=%1

cmake -DCMAKE_C_COMPILER:FILEPATH=arm-none-eabi-gcc.exe -DCMAKE_CXX_COMPILER:FILEPATH="arm-none-eabi-g++.exe" -DBUILD_MODE:STRING=%buildType% -B build  -G "MinGW Makefiles"
cd build
make -j16
cd ..
