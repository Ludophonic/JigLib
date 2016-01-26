# This makefile uses make recursively to allow for separate
# builds: debug, gprof and opt (the latter being the default).
COMPONENTS := water

CC := g++
OPT_FLAGS := -O3 -finline-functions -fno-exceptions -Winline	-Wall -pedantic -DRELEASE
DEBUG_FLAGS := -O0 -g -Wall -pedantic -DDEBUG
GPROF_FLAGS := -pg $(OPT_FLAGS)

INC_FLAGS := $(foreach DIR,$(COMPONENTS),-I$(DIR)) -I../../include
EXTRA_FLAGS := $(INC_FLAGS) -DUSE_FUNCTION

JIGLIB := GetsSetOnRecursion
LDFLAGS := -L../../lib -L/usr/X11R6/lib -L/usr/local/lib -l$(JIGLIB) -lglut -lGLU -lGL -lX11 -lXmu

# gets over-ridden on the real make
# OBJECT_DIR should get over-ridden on recursion
OBJECT_DIR := .error_obj
CFLAGS := .

SOURCES := $(wildcard water/*.cpp)
VPATH := $(COMPONENTS)
# just the object file names
OBJECT_FILES := $(notdir $(SOURCES:.cpp=.o))
# full path to the objects - only valid on recursion
OBJECTS := $(foreach OBJ,$(OBJECT_FILES),$(OBJECT_DIR)/$(OBJ))
DEPS := $(foreach OBJ,$(OBJECT_FILES),$(OBJECT_DIR)/$(OBJ:.o=.d))


#default target
opt: .opt
	@echo "############ Starting Optimised build #########################"
	$(MAKE) ../water CFLAGS="$(OPT_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$< JIGLIB=JigLib 

debug: .debug
	@echo "############ Starting debug build ##############################"
	$(MAKE) ../water_debug CFLAGS="$(DEBUG_FLAGS) $(EXTRA_FLAGS)"  OBJECT_DIR=$< JIGLIB=JigLibDebug

gprof: .gprof
	@echo "############ Starting gprof build ##############################"
	$(MAKE) ../water_gprof CFLAGS="-pg $(OPT_FLAGS) $(EXTRA_FLAGS)"  OBJECT_DIR=$< JIGLIB=JigLibGprof

all: opt debug gprof

../water ../water_debug ../water_gprof: $(OBJECTS) ../../lib/lib$(JIGLIB).a
	@echo objects = $(OBJECTS)
	@echo OBJECT_FILES = $(OBJECT_FILES)
	@echo SOURCES = $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

.debug .opt .gprof:
	mkdir $@

FORCE:

.SECONDARY: $(OBJECTS) $(DEPS)

clean: debug_clean opt_clean gprof_clean emacs_clean
	rm -f ../*.log

emacs_clean:
	rm -f `find . -name "*~"`

# under cygwin gcc adds .exe
opt_clean:
	rm -rf .opt
	rm -f ../water ../water.exe

debug_clean:
	rm -rf .debug
	rm -f ../water_debug ../water_debug.exe

gprof_clean:
	rm -rf .gprof
	rm -f ../water_gprof ../water_gprof.exe

# How we get a .o from a .cpp
$(OBJECT_DIR)/%.o: %.cpp
	@echo
	@echo Compiling $< to $@
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJECT_DIR)/%.d: %.cpp
	@echo Generating dependencies for $*.pp
	$(CC) -MM $(CFLAGS) $< | sed -e "s/\(^[a-zA-Z]\)/$(OBJECT_DIR)\/\1/" -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

ifneq ($(CFLAGS),.)
-include $(DEPS)
endif

