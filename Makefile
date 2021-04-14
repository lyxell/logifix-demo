#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2
# MSYS2:
#   pacman -S mingw-w64-i686-SDL2
#

EXE = build/app
IMGUI_DIR = imgui-boilerplate/imgui
SOURCES = main.cpp dockspace.cpp editor.cpp imgui-boilerplate/window.cpp build/parser.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp
SOURCES += $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp
SOURCES += $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp
OBJS = $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -std=c++17 -O2 -Wall -Wformat -Wfatal-errors
LIBS =

##---------------------------------------------------------------------
## OPENGL LOADER / OPENGL ES
##---------------------------------------------------------------------

## See below for OpenGL ES option (no loader required) - comment out
## the following if you want to use OpenGL ES instead of Desktop GL.

## Using OpenGL loader: gl3w [default]
SOURCES += $(IMGUI_DIR)/examples/libs/gl3w/GL/gl3w.c
CXXFLAGS += -I$(IMGUI_DIR)/examples/libs/gl3w -DIMGUI_IMPL_OPENGL_LOADER_GL3W

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) -ldl `sdl2-config --libs`

	CXXFLAGS += `sdl2-config --cflags`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib -L/opt/local/lib

	CXXFLAGS += `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
    ECHO_MESSAGE = "MinGW"
    LIBS += -lgdi32 -lopengl32 -limm32 `pkg-config --static --libs sdl2`

    CXXFLAGS += `pkg-config --cflags sdl2`
    CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

build/sjp.hpp: sjp/sjp_re2c.hpp
	re2c -W --input-encoding utf8 -i $< -o $@

build/parser.cpp: sjp/parser.dl
	souffle --no-warn \
			--generate=$@ \
			--fact-dir=build \
			--output-dir=build \
			$^

build/main.o: build/sjp.hpp

build/parser.o: build/parser.cpp
	@mkdir -p build
	$(CXX) -O2 -std=c++17 -D__EMBEDDED_SOUFFLE__ -c -o $@ $<

build/%.o: %.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: imgui-boilerplate/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI_DIR)/backends/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o: $(IMGUI_DIR)/examples/libs/gl3w/GL/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf build
