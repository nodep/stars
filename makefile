CXX      := c++
CXXFLAGS := -DNDEBUG -O2
#CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror
LDFLAGS  := -lstdc++ -lm
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := stars.exe
SRC      := main.cpp calcs.cpp config.cpp constellation_boundary.cpp storage.cpp \
            ngc.cpp ps_canvas.cpp coordinate.cpp process.cpp text.cpp star.cpp custom_objects.cpp

# $(wildcard *.cpp)

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

$(APP_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

clean:
	-@rm -rvf $(OBJ_DIR)/* $(APP_DIR)/*
