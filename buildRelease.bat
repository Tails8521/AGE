cd build/
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE:STRING=Release -DwxWidgets_ROOT_DIR:PATH="C:\Cpp\wxWidgets" -DBOOST_LIBRARYDIR:PATH="C:\Cpp\boost_libs" -DBOOST_ROOT:PATH="C:\Cpp\boost_1_47_0" -DZLIB_LIBRARY:FILEPATH="C:\Cpp\zlib\zlib1.dll" -DZLIB_INCLUDE_DIR:PATH="C:\Cpp\zlib\include" ../
mingw32-make 
cd ..
PAUSE