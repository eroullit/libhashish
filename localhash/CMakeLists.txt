INCLUDE_DIRECTORIES (${CMAKE_SOURCE_DIR}/${LIBHASHISH}/include)
ADD_LIBRARY(localhash mt.c random.c datagen.c)
