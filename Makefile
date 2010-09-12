C++ = g++
C++_FLAGS = -02 -c -o $@ $< #-fprofile-use
LINKER = g++
LINKER_FLAGS = -o #-fprofile-use

SRC = src
BUILD = build
TARGET = planet_wars
OBJS = $(BUILD)/main.o\
  $(BUILD)/game_state.o\
  $(BUILD)/fleet.o\
  $(BUILD)/planet.o\
  $(BUILD)/util.o

all: $(TARGET) package

$(TARGET): $(OBJS)
	$(LINKER) $(LINKER_FLAGS) $(BUILD)/$(TARGET) $(OBJS)

$(BUILD)/%.o: $(SRC)/%.cpp $(SRC)/*.h
	$(C++) $(C++_FLAGS)

clean:
	rm -rf $(BUILD) $(TARGET).zip debug.log; mkdir -p $(BUILD)

package:
	mkdir -p ./tmp;\
  cp $(SRC)/main.cpp ./tmp/MyBot.cc;\
  cp $(SRC)/game_state.h $(SRC)/game_state.cpp\
    $(SRC)/planet.h $(SRC)/planet.cpp\
    $(SRC)/fleet.h $(SRC)/fleet.cpp\
    $(SRC)/util.h $(SRC)/util.cpp ./tmp;\
  zip -r $(TARGET).zip ./tmp;\
  rm -rf ./tmp;