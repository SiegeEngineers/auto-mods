#! /bin/bash

sed -i 's/#set(Boost_USE_STATIC_LIBS        ON)/set(Boost_USE_STATIC_LIBS        ON)/' 3rd-party/genieutils/CMakeLists.txt
sed -i 's/add_library(${Genieutils_LIBRARY} SHARED/add_library(${Genieutils_LIBRARY} STATIC/' 3rd-party/genieutils/CMakeLists.txt
sed -i 's/target_link_libraries(${Genieutils_LIBRARY} ${ZLIB_LIBRARIES} ${Boost_LIBRARIES} ${ICONV_LIBRARIES})/target_link_libraries(${Genieutils_LIBRARY} ${Boost_LIBRARIES} ${ZLIB_LIBRARIES} ${ICONV_LIBRARIES})/' 3rd-party/genieutils/CMakeLists.txt
