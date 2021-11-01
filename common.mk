.PHONY: echoes
.SILENT: echoes

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

CURRENT_DIR := $(shell realpath --relative-to="$(SOURCE_DIR)" .)
BUILD := $(addprefix $(BUILD)/,$(CURRENT_DIR))

all: $(OBJECTS)

$(OBJECTS): %.o: %.c
	gcc $(CFLAGS) -I$(INCLUDE_DIR) $< -o $(BUILD)/$@

echoes:
	echo $(SOURCES)
	echo $(OBJECTS)
