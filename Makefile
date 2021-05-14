IMGUI_BOILERPLATE_ARCHIVE=imgui-boilerplate/imgui-boilerplate.a
SQUARELOG_ARCHIVE=squarelog/logifix.a

LIBS = -lm -lpthread
UNAME_S := $(shell uname -s)
CXXFLAGS = -std=c++17 -fPIC -O2
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

libprogram.so: program.o $(SQUARELOG_ARCHIVE)
	$(CXX) -o $@ -shared -Wl,--whole-archive $^ -Wl,--no-whole-archive

ImGuiFileDialog.o: ImGuiFileDialog/ImGuiFileDialog.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

editor: main.o editor.o dockspace.o ast.o imedit/imedit.o ImGuiFileDialog.o $(IMGUI_BOILERPLATE_ARCHIVE)
	$(CXX) -o $@ $^ $(LIBS)

.PHONY: $(SQUARELOG_ARCHIVE) $(IMGUI_BOILERPLATE_ARCHIVE)

$(IMGUI_BOILERPLATE_ARCHIVE):
	$(MAKE) -C imgui-boilerplate

$(SQUARELOG_ARCHIVE):
	$(MAKE) -C squarelog

clean:
	$(MAKE) -C imgui-boilerplate clean
	$(MAKE) -C squarelog clean
