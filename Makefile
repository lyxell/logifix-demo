IMGUI_BOILERPLATE_DIR = imgui-boilerplate
SJP_DIR = sjp
LINUX_GL_LIBS = -lGL
LIBS =
UNAME_S := $(shell uname -s)
CXXFLAGS = -std=c++17 -O2

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

.NOTPARALLEL: all

all: update_dependencies editor

editor: editor.o dockspace.o main.o
	@mkdir -p build
	$(CXX) \
		-o $@ \
		$(IMGUI_BOILERPLATE_DIR)/imgui.o \
		$(IMGUI_BOILERPLATE_DIR)/imgui_demo.o \
		$(IMGUI_BOILERPLATE_DIR)/imgui_draw.o \
		$(IMGUI_BOILERPLATE_DIR)/imgui_impl_opengl3.o \
		$(IMGUI_BOILERPLATE_DIR)/imgui_impl_sdl.o \
		$(IMGUI_BOILERPLATE_DIR)/imgui_widgets.o \
		$(IMGUI_BOILERPLATE_DIR)/imgui_tables.o \
		$(IMGUI_BOILERPLATE_DIR)/gl3w.o \
		$(IMGUI_BOILERPLATE_DIR)/window.o \
		$(SJP_DIR)/sjp.o \
		$(SJP_DIR)/parser.o \
		editor.o \
		dockspace.o \
		main.o \
		$(CXXFLAGS) $(LIBS)

.PHONY: update_dependencies clean

update_dependencies:
	$(MAKE) -C $(IMGUI_BOILERPLATE_DIR)
	$(MAKE) -C $(SJP_DIR)

clean:
	$(MAKE) -C $(IMGUI_BOILERPLATE_DIR) clean
	$(MAKE) -C $(SJP_DIR) clean
	rm -rf build
