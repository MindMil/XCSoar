name-to-bin = $(patsubst %,$(TARGET_BIN_DIR)/%$(TARGET_EXEEXT),$(1))
name-to-so = $(patsubst %,$(TARGET_BIN_DIR)/%.so,$(1))

# Generates a shared library linking rule for python.
#
# Example: $(eval $(call link-shared-library,Foo,FOO))
#
# Arguments: NAME, PREFIX
#
# NAME is the name of the library binary, without the path, without
# the prefix (lib) and without the suffix (.exe).
#
# PREFIX is a prefix for variables that will hold detailed information
# about what is linked, and now.  These must be set before this
# generator function is called.  The following variables will be used:
#
#  _SOURCES: a list of source files
#
#  _CPPFLAGS: preprocessor flags for the compiler
#
#  _LDADD: a list of static libraries that will be linked into the binary
#
#  _LDFLAGS: linker flags
#
#  _DEPENDS: a list of library names this executable depends on; it
#  will use its CPPFLAGS, LDADD and LDFLAGS
#
#  _STRIP: if "y", then the library will be stripped
#
define link-python-library

$(2)_BIN = $$(TARGET_BIN_DIR)/$(1).so

ifeq ($$($(2)_STRIP),y)
$(2)_NOSTRIP = $$(TARGET_BIN_DIR)/$(1)-ns.so
else
$(2)_NOSTRIP = $$($(2)_BIN)
endif

$(2)_LDADD += $(patsubst %,$$(%_LDADD),$($(2)_DEPENDS))
$(2)_LDLIBS += $(patsubst %,$$(%_LDLIBS),$($(2)_DEPENDS))
$(2)_CPPFLAGS += -fPIC -Wno-write-strings
$(2)_FILTER_FLAGS += -Wwrite-strings

# Compile
$(2)_OBJS = $$(call SRC_TO_OBJ,$$($(2)_SOURCES))
$$($(2)_OBJS): CPPFLAGS += $$($(2)_CPPFLAGS)
	FILTER_FLAGS = $$($(2)_FILTER_FLAGS)
$$($(2)_OBJS): CPPFLAGS += $(patsubst %,$$(%_CPPFLAGS),$($(2)_DEPENDS))

# Link the unstripped binary
$$($(2)_NOSTRIP): LDFLAGS += -Wl,-Bsymbolic -shared
ifeq ($$(TARGET),ANDROID)
$$($(2)_NOSTRIP): LDFLAGS += -nostdlib
endif

$$($(2)_LDADD): CPPFLAGS += -fPIC

$$($(2)_NOSTRIP): $$($(2)_OBJS) $$($(2)_LDADD) $$(TARGET_LDADD) | $$(TARGET_BIN_DIR)/dirstamp
	@$$(NQ)echo "  LINK    $$@"
	$$(Q)$$(LINK) $$(ld-flags) -o $$@ $$^ $$(ld-libs) $$($(2)_LDLIBS)

# Strip the binary (optional)
ifeq ($$($(2)_STRIP),y)
$$($(2)_BIN): $$($(2)_NOSTRIP)
	@$$(NQ)echo "  STRIP   $$@"
	$$(Q)$$(STRIP) $$< -o $$@
endif

endef

PYTHON_TEST = \
	skylines_flightreader \
	skylines_flighttimes

python-test: $(call name-to-bin,$(PYTHON_TEST))

python: $(call name-to-so,py_xcsoar)

PYTHON_FLIGHTREADER_SOURCES = \
	$(DEBUG_REPLAY_SOURCES) \
	$(PYTHON_TEST_SRC)/skylines_flightreader.cpp \
	$(PYTHON_SRC)/FlightReader.cpp
PYTHON_FLIGHTREADER_LDADD = $(DEBUG_REPLAY_LDADD)
PYTHON_FLIGHTREADER_DEPENDS = MATH UTIL GEO TIME
PYTHON_FLIGHTREADER_CPPFLAGS = -I$(TEST_SRC_DIR) -I$(PYTHON_SRC)
$(eval $(call link-program,skylines_flightreader,PYTHON_FLIGHTREADER))


PYTHON_FLIGHTTIMES_SOURCES = \
	$(DEBUG_REPLAY_SOURCES) \
	$(PYTHON_TEST_SRC)/skylines_flighttimes.cpp \
	$(PYTHON_SRC)/FlightReader.cpp \
	$(PYTHON_SRC)/FlightTimes.cpp \
	$(PYTHON_SRC)/DebugReplayVector.cpp \
	$(TEST_SRC_DIR)/FlightPhaseDetector.cpp \
	$(SRC)/Computer/CirclingComputer.cpp
PYTHON_FLIGHTTIMES_LDADD = $(DEBUG_REPLAY_LDADD)
PYTHON_FLIGHTTIMES_DEPENDS = MATH UTIL GEO TIME
PYTHON_FLIGHTTIMES_CPPFLAGS = -I$(TEST_SRC_DIR) -I$(PYTHON_SRC)
$(eval $(call link-program,skylines_flighttimes,PYTHON_FLIGHTTIMES))


PYTHON_PYTHON_SOURCES = \
        $(DEBUG_REPLAY_SOURCES) \
        $(PYTHON_SRC)/FlightReader.cpp \
        $(PYTHON_SRC)/Flight.cpp \
        $(PYTHON_SRC)/FlightTimes.cpp \
        $(PYTHON_SRC)/DebugReplayVector.cpp \
	$(PYTHON_SRC)/AnalyseFlight.cpp \
	$(PYTHON_SRC)/GoogleEncode.cpp \
	$(PYTHON_SRC)/DouglasPeuckerMod.cpp \
	$(PYTHON_SRC)/PythonGlue.cpp \
	$(PYTHON_SRC)/PythonConverters.cpp \
	$(ENGINE_SRC_DIR)/Trace/Point.cpp \
	$(ENGINE_SRC_DIR)/Trace/Trace.cpp \
	$(SRC)/Computer/CirclingComputer.cpp \
	$(TEST_SRC_DIR)/FlightPhaseDetector.cpp
PYTHON_PYTHON_LDADD = $(DEBUG_REPLAY_LDADD)
PYTHON_PYTHON_LDLIBS = -lpython2.7
PYTHON_PYTHON_DEPENDS = CONTEST UTIL GEO TIME MATH
PYTHON_PYTHON_CPPFLAGS = -I/usr/include/python2.7 \
	-I$(TEST_SRC_DIR)
$(eval $(call link-python-library,py_xcsoar,PYTHON_PYTHON))
