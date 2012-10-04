SET(KIT visiting)

FILE( GLOB source ${SOURCE_CODE_BASE_DIR}/${KIT}/test/*test.cpp )

ADD_EXECUTABLE( test_${KIT} ${source} )

TARGET_LINK_LIBRARIES( test_${KIT} comma_base ${GTEST_BOTH_LIBRARIES} pthread )
