.PHONY: echoes
.SILENT: echoes


CURRENT_DIR := $(shell realpath --relative-to="$(SOURCE_DIR)" .)
BUILD := $(addprefix $(BUILD)/,$(CURRENT_DIR))

SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c, $(BUILD)/%.o, $(SOURCES))

#alls: echoes

all: $(OBJECTS)

$(OBJECTS): $(BUILD)/%.o: %.c
	gcc $(CFLAGS) $(INCLUDE_DIR) $< -o $@

echoes:
	echo $(SOURCES)
	echo $(OBJECTS)
