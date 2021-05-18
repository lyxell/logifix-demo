IMGUI_BOILERPLATE_ARCHIVE=imgui-boilerplate/imgui-boilerplate.a
LOGIFIX_ARCHIVE=logifix/logifix.a

LIBS = -lm -lpthread
UNAME_S := $(shell uname -s)
CXXFLAGS = -std=c++17 -g -fPIC -O2 -Wfatal-errors
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

libprogram.so: program.o $(LOGIFIX_ARCHIVE)
	$(CXX) -o $@ -shared -Wl,--whole-archive $^ -Wl,--no-whole-archive

ImGuiFileDialog.o: ImGuiFileDialog/ImGuiFileDialog.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@

editor: main.o editor.o dockspace.o ast.o logifix/program.o logifix/sjp/parser.o logifix/sjp/lexer.o imedit/imedit.o ImGuiFileDialog.o $(IMGUI_BOILERPLATE_ARCHIVE)
	$(CXX) -o $@ $^ $(LIBS)

logifix/sjp/parser.o: $(LOGIFIX_ARCHIVE)

.PHONY: $(LOGIFIX_ARCHIVE) $(IMGUI_BOILERPLATE_ARCHIVE)

$(IMGUI_BOILERPLATE_ARCHIVE):
	$(MAKE) -C imgui-boilerplate

$(LOGIFIX_ARCHIVE):
	$(MAKE) -C logifix

clean:
	rm -rf *.o *.so editor
	$(MAKE) -C imgui-boilerplate clean
	$(MAKE) -C logifix clean
