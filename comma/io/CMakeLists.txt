SET( PROJECT "io" )
SET( TARGET_NAME comma_${PROJECT} )

FILE( GLOB source   ${SOURCE_CODE_BASE_DIR}/${PROJECT}/*.cpp) 
FILE( GLOB includes ${SOURCE_CODE_BASE_DIR}/${PROJECT}/*.h)
FILE( GLOB impl_source   ${SOURCE_CODE_BASE_DIR}/${PROJECT}/impl/*.cpp)
FILE( GLOB impl_includes ${SOURCE_CODE_BASE_DIR}/${PROJECT}/impl/*.h)
FILE( GLOB zeromq_source   ${SOURCE_CODE_BASE_DIR}/${PROJECT}/zeromq/*.cpp)
FILE( GLOB zeromq_includes ${SOURCE_CODE_BASE_DIR}/${PROJECT}/zeromq/*.h)

SOURCE_GROUP( ${PROJECT} FILES ${source} ${includes} ${impl_source} ${impl_includes} ${zeromq_source} ${zeromq_includes}  )

IF( BUILD_ZEROMQ )
    ADD_LIBRARY( ${TARGET_NAME} ${source} ${includes} ${impl_source} ${impl_includes} ${zeromq_source} ${zeromq_includes} )
ELSE( BUILD_ZEROMQ )
    ADD_LIBRARY( ${TARGET_NAME} ${source} ${includes} ${impl_source} ${impl_includes} )
ENDIF( BUILD_ZEROMQ )

SET_TARGET_PROPERTIES( ${TARGET_NAME} PROPERTIES ${comma_LIBRARY_PROPERTIES} )

TARGET_LINK_LIBRARIES( ${TARGET_NAME} comma_base comma_string ${Boost_LIBRARIES} ${ZeroMQ_LIBRARY} )

INSTALL( FILES ${includes} DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_INCLUDE_DIR}/${PROJECT} )
INSTALL( FILES ${impl_includes} DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_INCLUDE_DIR}/${PROJECT}/impl )
INSTALL( FILES ${zeromq_includes} DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_INCLUDE_DIR}/${PROJECT}/zeromq )
INSTALL(
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR} COMPONENT Runtime     # .exe, .dll
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_LIB_DIR} COMPONENT Runtime     # .so, mod.dll
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_LIB_DIR} COMPONENT Development # .a, .lib
)

IF( comma_BUILD_APPLICATIONS )
    ADD_SUBDIRECTORY( applications )
ENDIF( comma_BUILD_APPLICATIONS )
                                           
IF( comma_BUILD_TESTS )
    ADD_SUBDIRECTORY( test )
ENDIF( comma_BUILD_TESTS )


