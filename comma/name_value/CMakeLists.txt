SET( PROJECT "name_value" )
SET( TARGET_NAME comma_${PROJECT} )

FILE( GLOB source   ${SOURCE_CODE_BASE_DIR}/${PROJECT}/*.cpp)
FILE( GLOB includes ${SOURCE_CODE_BASE_DIR}/${PROJECT}/*.h )
FILE( GLOB impl_includes ${SOURCE_CODE_BASE_DIR}/${PROJECT}/impl/*.h )
SOURCE_GROUP( ${PROJECT} FILES ${source} ${includes} ${impl_includes} )
ADD_CUSTOM_TARGET( ${TARGET_NAME} ${source} ${includes} ${impl_includes} )

INSTALL( FILES ${includes} DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_INCLUDE_DIR}/${PROJECT} )
INSTALL( FILES ${impl_includes} DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_INCLUDE_DIR}/${PROJECT}/impl )

ADD_SUBDIRECTORY( applications )

IF( comma_BUILD_TESTS )
    ADD_SUBDIRECTORY( test )
ENDIF( comma_BUILD_TESTS )