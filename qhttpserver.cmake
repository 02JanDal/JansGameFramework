find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(QHTTPSERVER_SOURCES
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/http-parser/http_parser.h
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/http-parser/http_parser.c
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpconnection.h
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpconnection.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpserver.h
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpserver.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttprequest.h
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttprequest.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpresponse.h
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpresponse.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpserverapi.h
 ${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/src/qhttpserverfwd.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qhttpserver/http-parser)

add_library(QHttpServer STATIC ${QHTTPSERVER_SOURCES})
qt5_use_modules(QHttpServer Core Network)
