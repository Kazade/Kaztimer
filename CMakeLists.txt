CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(kaztimer)

SET(KAZTIMER_SOURCES
    ${CMAKE_SOURCE_DIR}/kaztimer/kaztimer.cpp
)

SET(KAZTIMER_HEADERS
    ${CMAKE_SOURCE_DIR}/kaztimer/kaztimer.h
)

SET(CMAKE_C_FLAGS "-std=c99")
LINK_LIBRARIES(
    -lrt
)

ADD_LIBRARY(kaztimer SHARED ${KAZTIMER_SOURCES})
SET_TARGET_PROPERTIES(kaztimer
                      PROPERTIES
                      VERSION 0.0.1
                      SOVERSION 1)
INSTALL(TARGETS kaztimer DESTINATION lib)

INSTALL(FILES ${KAZTIMER_HEADERS} DESTINATION include/kaztimer)


