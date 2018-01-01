BIN_NAME = sdl2_test
CC ?= cc
LIBS = sdl2 gl
COMPILE_FLAGS = -Wpedantic -Wall -Wextra
LINK_FLAGS =
RCOMPILE_FLAGS = 
DCOMPILE_FLAGS = -g -DDEBUG
RLINK_FLAGS =
DLINK_FLAGS =

SOURCES = main.c

ifneq ($(LIBS),)
	CFLAGS += $(shell pkg-config --cflags $(LIBS))
	LDFLAGS += $(shell pkg-config --libs $(LIBS))
endif

release: export CFLAGS := $(CFLAGS) $(COMPILE_FLAGS) $(RCOMPILE_FLAGS)
release: export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) $(RLINK_FLAGS)
debug: export CFLAGS := $(CFLAGS) $(COMPILE_FLAGS) $(DCOMPILE_FLAGS)
debug: export LDFLAGS := $(LDFLAGS) $(LINK_FLAGS) $(DLINK_FLAGS)

.PHONY: all
all: clean debug

.PHONY: debug
debug: executable

.PHONY: release
release: executable

.PHONY: executable
executable: $(BIN_NAME)

.PHONY: clean
clean:
	$(RM) $(BIN_NAME)

$(BIN_NAME):
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) $(LDFLAGS) -o$(BIN_NAME)
