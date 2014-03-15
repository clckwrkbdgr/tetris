BIN = tetris
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix tmp/,$(SOURCES:.cpp=.o))
CXXFLAGS = -MD -MP -std=c++0x $(WARNINGS)
LIBS = -lSDL2

all: $(BIN)

run: $(BIN)
	./$(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(LIBS) -o $@ $^

tmp/%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -rf tmp/* $(BIN) 

$(shell mkdir -p tmp)
-include $(OBJ:%.o=%.d)
