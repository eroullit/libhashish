SET(LIB_NAME "hashish")
PROJECT(lib${LIB_NAME} C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Activate this when you want to build a shared lib out the the lib/ directory
# Bear in mind that no profiling will be available on 64-bit arch when on
SET(BUILD_SHARED_LIBS ON)

ENABLE_TESTING()

FIND_LIBRARY(M_LIB m REQUIRED)
FIND_PACKAGE(Threads)

IF(CMAKE_HAVE_PTHREAD_CREATE)
	SET(LIBHASHISH_OPTS "${LIBHASHISH_OPTS} --enable-thread-locking")
	MESSAGE("-- Enabled thread locking")
ENDIF(CMAKE_HAVE_PTHREAD_CREATE)

IF(CMAKE_BUILD_TYPE STREQUAL debug)
	SET(LIBHASHISH_OPTS "${LIBHASHISH_OPTS} --enable-memory-check --enable-debug")
	MESSAGE("-- Enabled debug build")
ENDIF(CMAKE_BUILD_TYPE STREQUAL debug)

EXECUTE_PROCESS(COMMAND ${CMAKE_SOURCE_DIR}/configure ${LIBHASHISH_OPTS} --config=${CMAKE_SOURCE_DIR})

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(localhash)
ADD_SUBDIRECTORY(tests)
