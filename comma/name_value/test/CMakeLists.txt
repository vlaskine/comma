SET( KIT name_value )

FILE( GLOB source ${SOURCE_CODE_BASE_DIR}/${KIT}/test/*_test.cpp )

ADD_EXECUTABLE( test_${KIT} ${source} )

TARGET_LINK_LIBRARIES( test_${KIT} comma_xpath comma_string ${GTEST_BOTH_LIBRARIES} pthread )
