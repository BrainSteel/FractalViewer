# Project name, C flags, and compiler
PROJECT_NAME = frac
CFLAGS_GENERAL = -std=c99 -Wall

LINK_DEBUG = -O0
LINK_RELEASE = -Os -flto -fwhole-program
COMPILE_DEBUG = $(LINK_DEBUG) -D DEBUG -g
COMPILE_RELEASE = $(LINK_RElEASE) -D RELEASE

CC = gcc

# Object file names
OBJECTS = main.o selfsquared.o Font.o
BMPS = 540x20Font.bmp

# SDL2 paths
SDL2_LDIR = SDL/lib
SDL2_LIB = SDL2
SDL2_IDIR = SDL/include

# MINGW specifics for cross-compilation
MINGW_CC = i586-mingw32-gcc
BDIR_WIN32 = win32
MINGW_SDL2_LDIR = SDL/win32lib
MINGW_SDL2_LIB = -lmingw32 -static-libgcc -lSDL2main -lSDL2 -lSDL2.dll

# Our project paths
ODIR_GENERAL = obj
BDIR_GENERAL = bin
SDIR = src
IDIR = inc
RDIR = rsc
DEBUG_DIR = debug
RELEASE_DIR = release

# Extend object names into full object paths
# OBJECTS = $(OBJECT_NAMES:%.o=$(ODIR)/%.o)
# BMPS = $(BMP_NAMES:%.bmp=$(RDIR)/%.bmp)

REF_FRAMEWORKS =
UNAME = $(shell uname)
# If on Mac OS X, reference required frameworks
ifeq ($(UNAME),Darwin)
	REQ_FRAMEWORKS = AudioToolbox AudioUnit Carbon Cocoa CoreAudio CoreFoundation ForceFeedback IOKit OpenGL
	REF_FRAMEWORKS = $(REQ_FRAMEWORKS:%=-framework %)
endif

# Make the project in the bin directory
debug: BDIR = $(BDIR_GENERAL)/$(DEBUG_DIR)
debug: ODIR = $(ODIR_GENERAL)/$(DEBUG_DIR)
debug: CFLAGS = $(COMPILE_DEBUG) $(CFLAGS_GENERAL)
debug: $(OBJECTS) bin-dir $(BMPS)
	$(CC) -L$(SDL2_LDIR) -l$(SDL2_LIB) $(REF_FRAMEWORKS) $(LINK_DEBUG) -o $(BDIR)/$(PROJECT_NAME) $(OBJECTS:%.o=$(ODIR)/%.o)

release: BDIR = $(BDIR_GENERAL)/$(RELEASE_DIR)
release: ODIR = $(ODIR_GENERAL)/$(RELEASE_DIR)
release: CFLAGS = $(COMPILE_RELEASE) $(CFLAGS_GENERAL)
release: $(OBJECTS) bin-dir $(BMPS)
	$(CC) -L$(SDL2_LDIR) -l$(SDL2_LIB) $(REF_FRAMEWORKS) $(LINK_RELEASE) -o $(BDIR)/$(PROJECT_NAME) $(OBJECTS:%.o=$(ODIR)/%.o)

mingw-debug: BDIR = $(BDIR_WIN32)/$(DEBUG_DIR)
mingw-debug: CFLAGS = $(COMPILE_DEBUG) $(CFLAGS_GENERAL)
mingw-debug: bin-dir $(BMPS) mingw-copy
	$(MINGW_CC) $(OBJECTS:%.o=$(SDIR)/%.c) -I$(IDIR) -I$(SDL2_IDIR) -L$(MINGW_SDL2_LDIR) $(MINGW_SDL2_LIB) $(CFLAGS) -o $(BDIR)/$(PROJECT_NAME).exe

mingw-release: BDIR = $(BDIR_WIN32)/$(RELEASE_DIR)
mingw-release: CFLAGS = $(COMPILE_RELEASE) $(CFLAGS_GENERAL)
mingw-release: bin-dir $(BMPS) mingw-copy
	$(MINGW_CC) $(OBJECTS:%.o=$(SDIR)/%.c) -I$(IDIR) -I$(SDL2_IDIR) -L$(MINGW_SDL2_LDIR) $(MINGW_SDL2_LIB) $(CFLAGS) -o $(BDIR)/$(PROJECT_NAME).exe

mingw-copy:
	cp $(MINGW_SDL2_LDIR)/SDL2.dll $(BDIR)/SDL2.dll

# Make the object directory
obj-dir :
	mkdir -p $(ODIR)

# Make the bin directory
bin-dir :
	mkdir -p $(BDIR)

# Make the rsc directory
rsc-dir : bin-dir
	mkdir -p $(BDIR)/$(RDIR)

# Make the object files
$(OBJECTS): %.o : $(SDIR)/%.c obj-dir
	$(CC) -c -I$(IDIR) -I$(SDL2_IDIR) $(CFLAGS) -o $(ODIR)/$@ $<

# Copy resources
%.bmp : rsc-dir FORCE
	cp $(RDIR)/$@ $(BDIR)/$(RDIR)/$@
FORCE:

# Clean the project
.PHONY : clean clean-debug clean-release clean-mingw-debug clean-mingw-release
clean :
	${MAKE} clean-debug
	${MAKE} clean-release
	${MAKE} clean-mingw-debug
	${MAKE} clean-mingw-release
	rm -f -r $(ODIR_GENERAL)
	rm -f -r $(BDIR_GENERAL)
	rm -f -r $(BDIR_WIN32)

clean-debug :
	rm -f -r $(ODIR_GENERAL)/$(DEBUG_DIR) $(BDIR_GENERAL)/$(DEBUG_DIR)

clean-release :
	rm -f -r $(ODIR_GENERAL)/$(RELEASE_DIR) $(BDIR_GENERAL)/$(RELEASE_DIR)

clean-mingw-debug :
	rm -f -r $(BDIR_WIN32)/$(DEBUG_DIR)

clean-mingw-release :
	rm -f -r $(BDIR_WIN32)/$(RELEASE_DIR)

#Rebuild the project
rebuild :
	${MAKE} clean
	${MAKE} debug
	${MAKE} release
	${MAKE} mingw-debug
	${MAKE} mingw-release

rebuild-debug:
	${MAKE} clean-debug
	${MAKE} debug

rebuild-release:
	${MAKE} clean-release
	${MAKE} release

rebuild-mingw-debug:
	${MAKE} clean-mingw-debug
	${MAKE} mingw-debug

rebuild-mingw-release:
	${MAKE} clean-mingw-release
	${MAKE} mingw-release
