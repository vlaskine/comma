SET( dir ${SOURCE_CODE_BASE_DIR}/csv/applications )
FILE( GLOB source   ${dir}/*.cpp)
FILE( GLOB includes ${dir}/*.h)
SOURCE_GROUP( ${TARGET_NAME} FILES ${source} ${includes} )

ADD_EXECUTABLE( csv-size ${dir}/csv-size.cpp )
ADD_EXECUTABLE( csv-bin-cut ${dir}/csv-bin-cut.cpp )
#ADD_EXECUTABLE( csv-join ${dir}/csv-join.cpp )
#ADD_EXECUTABLE( csv-paste ${dir}/csv-paste.cpp )
#ADD_EXECUTABLE( csv-make-blocks ${dir}/csv-make-blocks.cpp )
ADD_EXECUTABLE( csv-split ${dir}/csv-split.cpp ${dir}/split/split.cpp ${dir}/split/split.h )
ADD_EXECUTABLE( csv-time ${dir}/csv-time.cpp )
ADD_EXECUTABLE( csv-time-delay ${dir}/csv-time-delay.cpp )
#ADD_EXECUTABLE( csv-time-join ${dir}/csv-time-join.cpp )
ADD_EXECUTABLE( csv-time-stamp ${dir}/csv-time-stamp.cpp )
ADD_EXECUTABLE( csv-to-bin ${dir}/csv-to-bin.cpp )
ADD_EXECUTABLE( csv-from-bin ${dir}/csv-from-bin.cpp )
ADD_EXECUTABLE( csv-calc ${dir}/csv-calc.cpp )
ADD_EXECUTABLE( csv-crc ${dir}/csv-crc.cpp )
#ADD_EXECUTABLE( csv-play ${dir}/csv-play.cpp ${dir}/play/multiplay.cpp )
#ADD_EXECUTABLE( csv-thin ${dir}/csv-thin.cpp )

TARGET_LINK_LIBRARIES ( csv-size ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_string comma_csv )
TARGET_LINK_LIBRARIES ( csv-bin-cut ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_string comma_csv comma_xpath )
TARGET_LINK_LIBRARIES ( csv-split ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_string comma_csv comma_xpath boost_program_options )
#TARGET_LINK_LIBRARIES ( csv-join ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_csv comma_io comma_xpath comma_string )
#TARGET_LINK_LIBRARIES ( csv-paste ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_string comma_csv comma_io )
#TARGET_LINK_LIBRARIES ( csv-make-blocks ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_csv comma_xpath comma_string )
TARGET_LINK_LIBRARIES ( csv-time ${comma_ALL_EXTERNAL_LIBRARIES} comma_application ArkTiming )
TARGET_LINK_LIBRARIES ( csv-time-delay ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_csv comma_string comma_xpath )
#TARGET_LINK_LIBRARIES ( csv-time-join ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_csv comma_io comma_string comma_xpath )
TARGET_LINK_LIBRARIES ( csv-time-stamp ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_csv comma_string )
TARGET_LINK_LIBRARIES ( csv-to-bin ${comma_ALL_EXTERNAL_LIBRARIES} comma_csv comma_xpath comma_application )
TARGET_LINK_LIBRARIES ( csv-from-bin ${comma_ALL_EXTERNAL_LIBRARIES} comma_csv comma_xpath comma_application )
TARGET_LINK_LIBRARIES ( csv-calc ${comma_ALL_EXTERNAL_LIBRARIES} comma_csv comma_xpath comma_application comma_string )
TARGET_LINK_LIBRARIES ( csv-crc ${comma_ALL_EXTERNAL_LIBRARIES} comma_csv comma_xpath comma_application comma_string )
#TARGET_LINK_LIBRARIES ( csv-play ${comma_ALL_EXTERNAL_LIBRARIES} comma_csv comma_xpath comma_application comma_io )
#TARGET_LINK_LIBRARIES ( csv-thin ${comma_ALL_EXTERNAL_LIBRARIES} comma_application comma_io )
                  
INSTALL( TARGETS csv-bin-cut 
                 #csv-join
                 #csv-paste
                 #csv-make-blocks
                 csv-split
                 csv-time
                 csv-time-delay
                 #csv-time-join
                 csv-time-stamp
                 csv-to-bin
                 csv-from-bin
                 csv-size
                 csv-calc
                 #csv-play
                 csv-crc
                 #csv-thin
         RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/${comma_INSTALL_BIN_DIR}
         COMPONENT Runtime )
