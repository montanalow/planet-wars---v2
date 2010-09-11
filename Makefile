CC = g++
SRC = ./src/
BUILD = ./build/
O = -02 #-O3 -funroll-loops
PROFILE = #-fprofile-use

all: main package

main: main.o planet_wars.o
	$(CC) $(O) $(PROFILE) -o $(BUILD)planet_wars $(BUILD)main.o $(BUILD)planet_wars.o

main.o:
	$(CC) -c $(O) -o $(BUILD)main.o $(SRC)main.cpp

planet_wars.o:
	$(CC) -c $(O) -o $(BUILD)planet_wars.o $(SRC)planet_wars.cpp

clean:
	rm -rf $(BUILD) planet_wars.zip; mkdir -p $(BUILD)

package: main
	mkdir -p package; cp $(SRC)main.cpp package/MyBot.cc && cp $(SRC)planet_wars.h $(SRC)planet_wars.cpp package && zip -r planet_wars.zip package; rm -rf ./package;


