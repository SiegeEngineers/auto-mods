#! /bin/bash

sed -i 's/#set(Boost_USE_STATIC_LIBS        ON)/set(Boost_USE_STATIC_LIBS        ON)/' genieutils/CMakeLists.txt
sed -i 's/find_library(iconv REQUIRED)/find_package(Iconv REQUIRED)/' genieutils/CMakeLists.txt
sed -i 's/add_library(${Genieutils_LIBRARY} SHARED/add_library(${Genieutils_LIBRARY} STATIC/' genieutils/CMakeLists.txt
sed -i 's/target_link_libraries(${Genieutils_LIBRARY} ${ZLIB_LIBRARIES} ${Boost_LIBRARIES} ${ICONV_LIBRARIES})/target_link_libraries(${Genieutils_LIBRARY} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${ICONV_LIBRARIES})/' genieutils/CMakeLists.txt
