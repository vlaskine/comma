SET( dir ${SOURCE_CODE_BASE_DIR}/name_value/applications )
FILE( GLOB source   ${dir}/*.cpp)
FILE( GLOB includes ${dir}/*.h)
SOURCE_GROUP( ${TARGET_NAME} FILES ${source} ${includes} )
ADD_EXECUTABLE( name-value-convert ${dir}/name-value-convert.cpp )
ADD_EXECUTABLE( name-value-get ${dir}/name-value-get.cpp )

TARGET_LINK_LIBRARIES ( name-value-convert ${comma_EXTERNAL_ALL_LIBRARIES} comma_application comma_string comma_xpath )
TARGET_LINK_LIBRARIES ( name-value-get ${comma_EXTERNAL_ALL_LIBRARIES} comma_application comma_string comma_xpath )

INSTALL( TARGETS name-value-convert name-value-get
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR}
         COMPONENT Runtime )
