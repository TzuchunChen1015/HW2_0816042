all: server
	rm *.o
server: server.o userstatus.o
	g++ server.o userstatus.o -o server

server.o: server.cpp
	g++ -c server.cpp

userstatus.o: userstatus.cpp
	g++ -c userstatus.cpp

clean:
	rm server
