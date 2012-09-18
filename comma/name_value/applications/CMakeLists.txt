SET( dir ${SOURCE_CODE_BASE_DIR}/Serialization/Applications )
FILE( GLOB source   ${dir}/*.cpp)
FILE( GLOB includes ${dir}/*.h)
SOURCE_GROUP( ${TARGET_NAME} FILES ${source} ${includes} )
ADD_EXECUTABLE( config-convert ${dir}/config-convert.cpp )
ADD_EXECUTABLE( config-get ${dir}/config-get.cpp )
ADD_EXECUTABLE( config-set ${dir}/config-set.cpp )

TARGET_LINK_LIBRARIES ( config-convert ${Ark_EXTERNAL_ALL_LIBRARIES} ArkApplication ArkString ArkXPath )
TARGET_LINK_LIBRARIES ( config-get ${Ark_EXTERNAL_ALL_LIBRARIES} ArkApplication ArkString ArkXPath )
TARGET_LINK_LIBRARIES ( config-set ${Ark_EXTERNAL_ALL_LIBRARIES} ArkApplication ArkString ArkXPath )

INSTALL( TARGETS config-convert config-get config-set
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${Ark_INSTALL_BIN_DIR}
         COMPONENT Runtime )
