.PHONY: all clean run deps

export CFLAGS=-c -O0 -Wall
export BUILD=$(realpath build)
export INCLUDE_DIR=$(realpath include)
export SOURCE_DIR=$(realpath src)

EXEC=unnamed
OBJECTS=$(wildcard build/*.o build/*/*.o build/*/*/*.o)

all: $(EXEC)

$(EXEC): deps
	echo $(OBJECTS)
	gcc $(OBJECTS) -o $(EXEC)

deps:
	$(MAKE) -C src/debug
	$(MAKE) -C src/visitors
	$(MAKE) -C src/vm
	$(MAKE) -C src

run:
	./$(EXEC)

clean:
	rm $(BUILD)/*.o
	rm $(BUILD)/vm/*.o
	rm $(EXEC)
