SET( dir ${SOURCE_CODE_BASE_DIR}/io/applications )
FILE( GLOB source   ${dir}/*.cpp)
FILE( GLOB includes ${dir}/*.h)
SOURCE_GROUP( ${TARGET_NAME} FILES ${source} ${includes} )

ADD_EXECUTABLE( io-publish ${dir}/io-publish.cpp ${dir}/publish.h ${dir}/publish.cpp )
TARGET_LINK_LIBRARIES ( io-publish ${comma_EXTERNAL_ALL_LIBRARIES} comma_base comma_io comma_application ) # profiler )

IF( NOT WIN32 )
ADD_EXECUTABLE( udp-client ${dir}/udp-client.cpp )
TARGET_LINK_LIBRARIES ( udp-client comma_application comma_io comma_base ${comma_EXTERNAL_ALL_LIBRARIES} ) # profiler )
ENDIF( NOT WIN32 )

IF( BUILD_ZEROMQ )
ADD_EXECUTABLE( zero-cat ${dir}/zero-cat.cpp )
TARGET_LINK_LIBRARIES ( zero-cat ${ZeroMQ_LIBRARY} comma_io comma_application boost_program_options )
INSTALL( TARGETS zero-cat
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR}
         COMPONENT Runtime )
ENDIF( BUILD_ZEROMQ )

IF( NOT WIN32 )
INSTALL( TARGETS udp-client
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR}
         COMPONENT Runtime )
ENDIF( NOT WIN32 )

IF(NOT WIN32) # io-publish uses select on stdin and would throw on windows
INSTALL( TARGETS io-publish
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR}
         COMPONENT Runtime )
ENDIF(NOT WIN32)
