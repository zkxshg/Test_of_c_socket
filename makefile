make: server.o client.o
server.o: server.c
	gcc -o server server.c
client.o: client.c
	gcc -o client client.c
.PHONY : initial
initial:
	rm -f *.txt
	cp ./initial/caplist.txt caplist.txt
	cp ./initial/date.txt date.txt
	cp ./initial/filename.txt filename.txt
	cp ./initial/rkey.txt rkey.txt
	cp ./initial/wkey.txt wkey.txt
.PHONY : clear
clear:
	rm -f *.txt
