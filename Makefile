VERSION=$(shell git tag | sed 's/.*\([0-9]\+\.[0-9]\+\.[0-9]\+\)/\1/' | sort -nt . | tail -1)

BIN = tetris
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix tmp/,$(SOURCES:.cpp=.o))
CXXFLAGS += -MD -MP -std=c++0x $(WARNINGS)
LIBS += -lSDL2 -lchthon2

all: $(BIN)

run: $(BIN)
	./$(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

deb: $(BIN)
	@debpackage.py \
		catris \
		-v $(VERSION) \
		--maintainer 'umi041 <umi0451@gmail.com>' \
		--bin $(BIN) \
		--build-dir tmp \
		--dest-dir . \
		--description 'Catris.'

tmp/%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $@

tmp/tetris.o: neko.xpm

.PHONY: clean

clean:
	$(RM) -rf tmp/* $(BIN) 

$(shell mkdir -p tmp)
-include $(OBJ:%.o=%.d)
