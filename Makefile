IMGUI_BOILERPLATE_ARCHIVE=imgui-boilerplate/imgui-boilerplate.a
DATALOG_REPAIR_ARCHIVE=datalog-repair/datalog-repair.a

OBJS = editor.o dockspace.o main.o ast.o program.o

LIBS = -lm
UNAME_S := $(shell uname -s)
CXXFLAGS = -std=c++17 -fPIC -O2 -g
CXXFLAGS += -Iimgui-boilerplate/imgui

SOUFFLE=souffle
ifdef SOUFFLE_PATH
	SOUFFLE=$(SOUFFLE_PATH:%/=%)/src/souffle
	CXXFLAGS+=-I$(SOUFFLE_PATH:%/=%)/src/include
endif

# LINUX
ifeq ($(UNAME_S), Linux)
	LIBS += -lGL -ldl `sdl2-config --libs`
	CXXFLAGS += `sdl2-config --cflags`
endif

# APPLE
ifeq ($(UNAME_S), Darwin)
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit
	LIBS += -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib -L/opt/local/lib
	CXXFLAGS += `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include
	CXXFLAGS += -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

all: editor libprogram.so

libprogram.so: program.o $(DATALOG_REPAIR_ARCHIVE)
	$(CXX) -o $@ -shared -Wl,--whole-archive $^ -Wl,--no-whole-archive

editor: main.o editor.o dockspace.o ast.o $(IMGUI_BOILERPLATE_ARCHIVE)
	$(CXX) -o $@ $^ $(LIBS)

.PHONY: $(DATALOG_REPAIR_ARCHIVE)

$(IMGUI_BOILERPLATE_ARCHIVE):
	$(MAKE) -C imgui-boilerplate

$(DATALOG_REPAIR_ARCHIVE):
	$(MAKE) -C datalog-repair

clean:
	$(MAKE) -C imgui-boilerplate clean
	$(MAKE) -C datalog-repair clean
	rm -rf $(OBJS)

