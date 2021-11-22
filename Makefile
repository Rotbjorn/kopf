.PHONY: all clean run deps
.IGNORE: clean

export CFLAGS=-c -O0 -Wall -g
export BUILD=$(realpath build)
export INCLUDE_DIR=-I$(realpath include)
export SOURCE_DIR=$(realpath src)

DOTS=$(realpath dots)

EXEC=unnamed
OBJECTS=$(wildcard build/*.o build/*/*.o build/*/*/*.o)

all: $(EXEC)

$(EXEC): deps
	mkdir -p build
	echo $(OBJECTS)
	gcc $(OBJECTS) \
		-o $(EXEC) \
		-L /usr/local/lib/rotbjorn \
		-lskrivarn
deps:
	mkdir -p build/{vm,visitors,debug}
	$(MAKE) -C src/debug
	$(MAKE) -C src/visitors
	$(MAKE) -C src/vm
	$(MAKE) -C src

run: $(EXEC)
	./$(EXEC)

dot: $(DOTS) ast.dot
	dot -Tsvg ast.dot -o $(DOTS)/ast.svg 
	dot -Tpng -Gdpi=300 ast.dot -o $(DOTS)/ast.png 

ast.dot: run

$(DOTS):
	mkdir -p dots

clean:
	rm -r $(BUILD)/*
	rm $(EXEC)
