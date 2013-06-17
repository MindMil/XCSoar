name-to-bin = $(patsubst %,$(TARGET_BIN_DIR)/%$(TARGET_EXEEXT),$(1))

INCLUDES += -I$(SKYLINES_SRC) -I$(SKYLINES_TEST_SRC) -I$(TEST_SRC_DIR)

SKYLINES = \
	skylines_flightreader \
	skylines_flighttimes

skylines: $(call name-to-bin,$(SKYLINES))


SKYLINES_FLIGHTREADER_SOURCES = \
	$(DEBUG_REPLAY_SOURCES) \
	$(SKYLINES_TEST_SRC)/skylines_flightreader.cpp \
	$(SKYLINES_SRC)/FlightReader.cpp
SKYLINES_FLIGHTREADER_LDADD = $(DEBUG_REPLAY_LDADD)
SKYLINES_FLIGHTREADER_DEPENDS = MATH UTIL GEO TIME
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
$(eval $(call link-program,skylines_flighttimes,SKYLINES_FLIGHTTIMES))


SKYLINES_PYTHON_SOURCES = \
        $(DEBUG_REPLAY_SOURCES) \
        $(SKYLINES_SRC)/FlightReader.cpp \
        $(SKYLINES_SRC)/FlightTimes.cpp \
        $(SKYLINES_SRC)/DebugReplayVector.cpp \
	$(SKYLINES_SRC)/AnalyseFlight.cpp \
	$(SKYLINES_SRC)/PythonGlue.cpp \
	$(ENGINE_SRC_DIR)/Trace/Point.cpp \
	$(ENGINE_SRC_DIR)/Trace/Trace.cpp
SKYLINES_PYTHON_LDADD = $(DEBUG_REPLAY_LDADD)
SKYLINES_PYTHON_LDLIBS = -lpython2.7
SKYLINES_PYTHON_DEPENDS = CONTEST UTIL GEO TIME MATH
SKYLINES_PYTHON_CPPFLAGS = -I/usr/include/python2.7
$(eval $(call link-shared-library,XCSoarTools,SKYLINES_PYTHON))

