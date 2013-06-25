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

SKYLINES = \
	skylines_flightreader \
	skylines_flighttimes

skylines-test: $(call name-to-bin,$(SKYLINES))

skylines: $(call name-to-so,py_xcsoar)

SKYLINES_FLIGHTREADER_SOURCES = \
	$(DEBUG_REPLAY_SOURCES) \
	$(SKYLINES_TEST_SRC)/skylines_flightreader.cpp \
	$(SKYLINES_SRC)/FlightReader.cpp
SKYLINES_FLIGHTREADER_LDADD = $(DEBUG_REPLAY_LDADD)
SKYLINES_FLIGHTREADER_DEPENDS = MATH UTIL GEO TIME
SKYLINES_FLIGHTREADER_CPPFLAGS = -I$(TEST_SRC_DIR) -I$(SKYLINES_SRC)
$(eval $(call link-program,skylines_flightreader,SKYLINES_FLIGHTREADER))


SKYLINES_FLIGHTTIMES_SOURCES = \
	$(DEBUG_REPLAY_SOURCES) \
	$(SKYLINES_TEST_SRC)/skylines_flighttimes.cpp \
	$(SKYLINES_SRC)/FlightReader.cpp \
	$(SKYLINES_SRC)/FlightTimes.cpp \
	$(SKYLINES_SRC)/DebugReplayVector.cpp \
	$(TEST_SRC_DIR)/FlightPhaseDetector.cpp \
	$(SRC)/Computer/CirclingComputer.cpp
SKYLINES_FLIGHTTIMES_LDADD = $(DEBUG_REPLAY_LDADD)
SKYLINES_FLIGHTTIMES_DEPENDS = MATH UTIL GEO TIME
SKYLINES_FLIGHTTIMES_CPPFLAGS = -I$(TEST_SRC_DIR) -I$(SKYLINES_SRC)
$(eval $(call link-program,skylines_flighttimes,SKYLINES_FLIGHTTIMES))


SKYLINES_PYTHON_SOURCES = \
        $(DEBUG_REPLAY_SOURCES) \
        $(SKYLINES_SRC)/FlightReader.cpp \
        $(SKYLINES_SRC)/Flight.cpp \
        $(SKYLINES_SRC)/FlightTimes.cpp \
        $(SKYLINES_SRC)/DebugReplayVector.cpp \
	$(SKYLINES_SRC)/AnalyseFlight.cpp \
	$(SKYLINES_SRC)/GoogleEncode.cpp \
	$(SKYLINES_SRC)/DouglasPeuckerMod.cpp \
	$(SKYLINES_SRC)/PythonGlue.cpp \
	$(SKYLINES_SRC)/PythonConverters.cpp \
	$(ENGINE_SRC_DIR)/Trace/Point.cpp \
	$(ENGINE_SRC_DIR)/Trace/Trace.cpp \
	$(SRC)/Computer/CirclingComputer.cpp \
	$(TEST_SRC_DIR)/FlightPhaseDetector.cpp
SKYLINES_PYTHON_LDADD = $(DEBUG_REPLAY_LDADD)
SKYLINES_PYTHON_LDLIBS = -lpython2.7
SKYLINES_PYTHON_DEPENDS = CONTEST UTIL GEO TIME MATH
SKYLINES_PYTHON_CPPFLAGS = -I/usr/include/python2.7 \
	-I$(TEST_SRC_DIR)
$(eval $(call link-python-library,py_xcsoar,SKYLINES_PYTHON))
