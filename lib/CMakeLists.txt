FILE(GLOB LIBHASHISH_HDRS ${CMAKE_SOURCE_DIR}/include/*.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

ADD_LIBRARY(${LIB_NAME} cmp_funcs.c
			hash_func.c
			hash_func_cube.c
			hash_func_goulburn.c
			hash_func_hsieh.c
			hash_func_jenkins2.c
			hash_func_jenkins3.c
			hash_func_murmur.c
			hash_func_sha1.c
			hash_func_skein256.c
			hi_array.c
			hi_bloom_filter.c
			hi_helper.c
			hi_iterator.c
			hi_key_int16_t.c
			hi_key_int32_t.c
			hi_key_str.c
			hi_key_uint16_t.c
			hi_key_uint32_t.c
			hi_list.c
			hi_operations.c
			hi_rbtree.c
			hi_set.c
			lib_fini.c
			lib_init.c
			xutils.c)

TARGET_LINK_LIBRARIES(${LIB_NAME} ${LIBS})

INSTALL(TARGETS ${LIB_NAME} LIBRARY DESTINATION /usr/lib)
INSTALL(FILES ${LIBHASHISH_HDRS} DESTINATION /usr/local/include/${PROJECT_NAME})
