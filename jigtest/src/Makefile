# This makefile uses make recursively to allow for separate
# builds: debug, gprof and opt (the latter being the default).
COMPONENTS := framework application

CC := g++
OPT_FLAGS := -O3 -finline-functions -fno-exceptions -Winline	-Wall -pedantic -DRELEASE
OPTSYM_FLAGS := -g -O2 -finline-functions -fno-exceptions -Winline	-Wall -pedantic -DRELEASE
DEBUG_FLAGS := -O0 -g -Wall -pedantic -D_DEBUG
GPROF_FLAGS := -pg $(OPT_FLAGS)

INC_FLAGS := $(foreach DIR,$(COMPONENTS),-I$(DIR)) -I../../include
EXTRA_FLAGS := $(INC_FLAGS) -DUSE_FUNCTION -DNO_XML

JIGLIB := GetsSetOnRecursion
LDFLAGS := -L../../lib -L/usr/X11R6/lib -L/usr/local/lib -l$(JIGLIB) -lpng -lSDL -lGLU -lGL -lX11

# gets over-ridden on the real make
# OBJECT_DIR should get over-ridden on recursion
OBJECT_DIR := .error_obj
CFLAGS := .

SOURCES := $(wildcard framework/*.cpp) $(wildcard application/*.cpp)
VPATH := $(COMPONENTS)
# just the object file names
OBJECT_FILES := $(notdir $(SOURCES:.cpp=.o))
# full path to the objects - only valid on recursion
OBJECTS := $(foreach OBJ,$(OBJECT_FILES),$(OBJECT_DIR)/$(OBJ))
DEPS := $(foreach OBJ,$(OBJECT_FILES),$(OBJECT_DIR)/$(OBJ:.o=.d))


#default target
opt: .opt
	@echo "############# Starting Optimised build #########################"
	$(MAKE) ../jigtest CFLAGS="$(OPT_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$< JIGLIB=JigLib 

optsym: .optsym
	@echo "######### Starting Optimised (with symbols) build ##############"
	$(MAKE) ../jigtest_optsym CFLAGS="$(OPTSYM_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$< JIGLIB=JigLibOptSym 

debug: .debug
	@echo "############ Starting debug build ##############################"
	$(MAKE) ../jigtest_debug CFLAGS="$(DEBUG_FLAGS) $(EXTRA_FLAGS)"  OBJECT_DIR=$< JIGLIB=JigLibDebug

gprof: .gprof
	@echo "############ Starting gprof build ##############################"
	$(MAKE) ../jigtest_gprof CFLAGS="-pg $(OPT_FLAGS) $(EXTRA_FLAGS)"  OBJECT_DIR=$< JIGLIB=JigLibGprof

all: opt optsym debug gprof

../jigtest ../jigtest_optsym ../jigtest_debug ../jigtest_gprof: $(OBJECTS) ../../lib/lib$(JIGLIB).a
	@echo objects = $(OBJECTS)
	@echo OBJECT_FILES = $(OBJECT_FILES)
	@echo SOURCES = $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS)

.debug .opt .optsym .gprof:
	mkdir $@

FORCE:

.SECONDARY: $(OBJECTS) $(DEPS)

clean: debug_clean opt_clean optsym_clean gprof_clean emacs_clean
	rm -f ../*.log

emacs_clean:
	rm -f `find . -name "*~"`

# under cygwin gcc adds .exe
opt_clean:
	rm -rf .opt
	rm -f ../jigtest ../jigtest.exe

optsym_clean:
	rm -rf .optsym
	rm -f ../jigtest_optsym ../jigtest_optsym.exe

debug_clean:
	rm -rf .debug
	rm -f ../jigtest_debug ../jigtest_debug.exe

gprof_clean:
	rm -rf .gprof
	rm -f ../jigtest_gprof ../jigtest_gprof.exe

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

