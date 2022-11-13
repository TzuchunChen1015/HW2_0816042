all: server
	rm *.o
server: server.o userstatus.o room.o
	g++ server.o userstatus.o room.o -o server

server.o: server.cpp
	g++ -c server.cpp

userstatus.o: userstatus.cpp
	g++ -c userstatus.cpp

room.o: room.cpp
	g++ -c room.cpp

clean:
	rm server
