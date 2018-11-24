all : main.o
	g++ -o main main.o

main.o : main.cpp
	g++ -c main.cpp

.PHONY : clean
clean :
	rm main *.o

