COMPONENTS := collision geometry maths physics utils vehicles

CC := g++
OPT_FLAGS := -O3 -finline-functions -fno-exceptions -Winline -Wall -pedantic -DRELEASE
OPTSYM_FLAGS := -g -O2 -finline-functions -fno-exceptions -Winline	-Wall -pedantic -DRELEASE
DEBUG_FLAGS := -O0 -g -Wall -pedantic -DDEBUG
GPROF_FLAGS := -pg $(OPT_FLAGS)

INC_FLAGS := -Iinclude $(foreach DIR,$(COMPONENTS),-I$(DIR)/include)
EXTRA_FLAGS := $(INC_FLAGS) -DUSE_FUNCTION
LDFLAGS := 

OUTPUT_DIR = lib
# OBJECT_DIR should get over-ridden on recursion
OBJECT_DIR := .error_obj
CFLAGS := .

# we build everything that is in the source directories of the above
# components

SOURCES := $(wildcard $(foreach DIR,$(COMPONENTS),$(DIR)/src/*.cpp))
VPATH := $(wildcard $(foreach DIR,$(COMPONENTS),$(DIR)/src))
# just the object file names
OBJECT_FILES := $(notdir $(SOURCES:.cpp=.o))
# full path to the objects - only valid on recursion
OBJECTS := $(foreach OBJ,$(OBJECT_FILES),$(OBJECT_DIR)/$(OBJ))
DEPS := $(foreach OBJ,$(OBJECT_FILES),$(OBJECT_DIR)/$(OBJ:.o=.d))

#default target
opt: .opt
	@echo "########### Starting optimised build ###########"
	$(MAKE) real_opt CFLAGS="$(OPT_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$<
real_opt: $(OUTPUT_DIR) $(OUTPUT_DIR)/libJigLib.a

optsym: .optsym
	@echo "##### Starting optimised with symbols build #####"
	$(MAKE) real_optsym CFLAGS="$(OPTSYM_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$<
real_optsym: $(OUTPUT_DIR) $(OUTPUT_DIR)/libJigLibOptSym.a

debug: .debug
	@echo "########### Starting debug build ###########"
	$(MAKE) real_debug CFLAGS="$(DEBUG_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$<
real_debug: $(OUTPUT_DIR) $(OUTPUT_DIR)/libJigLibDebug.a

gprof: .gprof
	@echo "########### Starting gprof build ###########"
	$(MAKE) real_gprof CFLAGS="$(GPROF_FLAGS) $(EXTRA_FLAGS)" OBJECT_DIR=$<
real_gprof: $(OUTPUT_DIR) $(OUTPUT_DIR)/libJigLibGprof.a

all: opt optsym debug gprof

jigtest_all: all
	$(MAKE) jigtest jigtest_optsym jigtest_debug jigtest_gprof

jigtest: opt
	cd jigtest/src && $(MAKE) $<

jigtest_optsym: optsym
	cd jigtest/src && $(MAKE) $<

jigtest_debug: debug
	cd jigtest/src && $(MAKE) $<

jigtest_gprof: gprof
	cd jigtest/src && $(MAKE) $<

water_all: all
	$(MAKE) water water_debug water_gprof

water: opt
	cd water/src && $(MAKE) $<

water_debug: debug
	cd water/src && $(MAKE) $<

water_gprof: gprof
	cd water/src && $(MAKE) $<

opt_clean: 
	rm -f $(OUTPUT_DIR)/libJigLib.a
	rm -rf .opt

optsym_clean: 
	rm -f $(OUTPUT_DIR)/libJigLibOptSym.a
	rm -rf .optsym

debug_clean: 
	rm -f $(OUTPUT_DIR)/libJigLibDebug.a
	rm -rf .debug

gprof_clean: 
	rm -f $(OUTPUT_DIR)/libJigLibGprof.a
	rm -rf .gprof

emacs_clean:
	rm -f `find . -name "*~"`

jigtest_clean:
	cd jigtest/src && $(MAKE) clean

water_clean:
	cd water/src && $(MAKE) clean

clean: opt_clean optsym_clean debug_clean gprof_clean emacs_clean
	rm -rf $(OUTPUT_DIR)

.debug:
	mkdir .debug

.opt:
	mkdir .opt

.optsym:
	mkdir .optsym

.gprof:
	mkdir .gprof

$(OUTPUT_DIR):
	mkdir $@

%.a: $(OBJECTS) $(DEPS)
	ar -rs $@ $(OBJECTS)

.SECONDARY: $(OBJECTS) $(DEPS)

# How we get a .o from a .cpp
$(OBJECT_DIR)/%.o: %.cpp
	@echo
	@echo Compiling $< to $@
	$(CC) -c $(CFLAGS) -o $@ $<

# How we generate the dependancies
$(OBJECT_DIR)/%.d: %.cpp
	@echo Generating dependencies for $*.pp
	$(CC) -MM $(CFLAGS) $< | sed -e "s/\(^[a-zA-Z]\)/$(OBJECT_DIR)\/\1/" -e 's@^\(.*\)\.o:@\1.d \1.o:@' > $@

# can only include the dependencies on recursion, since only then do
# we know (a) what the flags are and (b) where they are.
ifneq ($(CFLAGS),.)
-include $(DEPS)
endif


