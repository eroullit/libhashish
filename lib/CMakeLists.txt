INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${LIBHASHISH}/include)
ADD_LIBRARY(hashish cmp_funcs.c hash_func.c hash_func_cube.c hash_func_goulburn.c hash_func_hsieh.c hash_func_jenkins2.c hash_func_jenkins3.c hash_func_murmur.c hash_func_sha1.c hash_func_skein256.c hi_array.c hi_bloom_filter.c hi_helper.c hi_iterator.c hi_key_int16_t.c hi_key_int32_t.c hi_key_str.c hi_key_uint16_t.c hi_key_uint32_t.c hi_list.c hi_operations.c hi_rbtree.c hi_set.c lib_fini.c lib_init.c xutils.c)
TARGET_LINK_LIBRARIES(hashish ${LIBS})
