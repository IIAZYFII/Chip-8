build:
	gcc -Wfatal-errors \
	-std=c++11 \
	./*.cpp \
	-I"C:\libsdl\include" \
	-L"C:\libsdl\lib" \
	-lmingw32 \
	-lSDL2main \
	-lSDL2 \
	-o example.exe