
all:
	g++ server.cpp -std=c++11 -lpthread -o chat_server.x
	g++ client.cpp -std=c++11 -lpthread -o chat_client.x
clean: 
	rm * .x *.log
