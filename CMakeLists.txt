PROJECT(libhashish C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

EXECUTE_PROCESS(COMMAND ${CMAKE_SOURCE_DIR}/${LIBHASHISH}/configure --enable-memory-check --enable-thread-locking --enable-debug --config=${CMAKE_SOURCE_DIR}/${LIBHASHISH})

ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(localhash)
ADD_SUBDIRECTORY(tests)
