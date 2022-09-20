CC := gcc
INCLUDES := -Isrc

LIBNAME  := neural_engine
LIB		 := lib$(LIBNAME).so
LINKS    := -lm -l$(LIBNAME)

### OBJECTS ###
SOURCES := $(wildcard src/*.c)
TEST_SOURCES := $(wildcard tests/*.c)

OBJECTS := $(SOURCES:src/%.c=%.o)
TEST_OBJECTS := $(TEST_SOURCES:tests/%.c=%.o)
###############

### EXCLUDE ###
EXCLUDE_OBJECTS := gui.o
EXCLUDE_TESTS	:=

OBJECTS := $(filter-out $(EXCLUDE_OBJECTS), $(OBJECTS))
TEST_OBJECTS := $(filter-out $(EXCLUDE_TESTS), $(TEST_OBJECTS))
###############

### TARGETS ###
CFLAGS := -Wall -Wextra -pedantic-errors -std=c11

TARGET ?= RELEASE
ifeq ($(TARGET), RELEASE)
	CFLAGS += -DNDEBUG -O3
	BUILD_PREFIX := targets/release
else ifeq ($(TARGET), DEBUG)
	CFLAGS += -O0 -g 
	BUILD_PREFIX := targets/debug
endif
###############

LIB_DIR := $(BUILD_PREFIX)/lib
TEST_DIR := $(BUILD_PREFIX)/tests
OBJECTS := $(OBJECTS:%=$(LIB_DIR)/obj/%)

# specify were to search for sources
VPATH := src:include:tests

.PHONY: build clean test gui

TEST_OUT := $(TEST_OBJECTS:%.o=$(TEST_DIR)/%.out)

build: $(LIB_DIR)/$(LIB) $(TEST_OUT)

GUI_FLAGS := $(shell pkg-config --cflags gtk+-3.0)
GUI_LIBS  := $(shell pkg-config --libs gtk+-3.0)

gui: build
	$(CC) $(GUI_FLAGS) -o $(BUILD_PREFIX)/$@ src/$@.c $(GUI_LIBS)

test: build
	$(foreach test, $(TEST_OUT), ./$(test) &&)\
	echo "TOTAL SUCCESS!" || echo "EPIC FAIL!"

clean:
	rm -rf src/*.d tests/*.d targets
 
print:
	@echo lib:     $(LIBNAME) \($(LIB)\)
	@echo target:  $(TARGET)
	@echo libdir:  $(LIB_DIR)
	@echo testdir: $(TEST_DIR)
	@echo tests:
		$(foreach test, $(TEST_OUT), echo $(test))

### DEPENDS ###
# creating dependency files from sources
%.d: %.c
	$(CC) -MM $(INCLUDES) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

# include dependency files
include $(SOURCES:.c=.d)
include $(TEST_SOURCES:.c=.d)
###############

$(LIB_DIR)/obj/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -fPIC -c -o $(@:src/%=%) $(<:.d=.c)

$(LIB_DIR)/$(LIB): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) -shared $^ -lm -o $@

$(TEST_DIR)/obj/%.o: tests/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDES) -fPIC -c -o $(@:src/%=%) $(<:.d=.c)

$(TEST_DIR)/%.out: $(TEST_DIR)/obj/%.o $(LIB_DIR)/$(LIB)
	$(CC) $< $(INCLUDES) $(LINKS) -Wl,-rpath=$(PWD)/$(LIB_DIR) -L$(LIB_DIR) -o $@


