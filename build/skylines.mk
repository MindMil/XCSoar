name-to-bin = $(patsubst %,$(TARGET_BIN_DIR)/%$(TARGET_EXEEXT),$(1))

INCLUDES += -I$(SKYLINES_SRC) -I$(SKYLINES_TEST_SRC) -I$(TEST_SRC_DIR)

SKYLINES = \
	skylines_flightreader

skylines: $(call name-to-bin,$(SKYLINES))

SKYLINES_FLIGHTREADER_SOURCES = \
	$(DEBUG_REPLAY_SOURCES) \
	$(SKYLINES_TEST_SRC)/skylines_flightreader.cpp \
	$(SKYLINES_SRC)/FlightReader.cpp
SKYLINES_FLIGHTREADER_LDADD = $(DEBUG_REPLAY_LDADD)
SKYLINES_FLIGHTREADER_DEPENDS = MATH UTIL GEO TIME
$(eval $(call link-program,skylines_flightreader,SKYLINES_FLIGHTREADER))

