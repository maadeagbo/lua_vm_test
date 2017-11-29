all:
	g++ -Wall -ggdb -std=c++11 -o lua_vm *.cpp \
	-I/usr/include/lua5.3 -I./ -llua5.3 -lstdc++fs
	rm -rf *.o

clean:
	rm -rf *.o lua_vm