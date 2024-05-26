project(kdevelop-json)

SET(CMAKE_C_FLAGS "-Wall -Wextra -Wformat=2 -Wstrict-aliasing=2 -Wcast-align -Wwrite-strings -Wformat-nonliteral -Wconversion -Wfloat-equal -Wpointer-arith -Wswitch-enum")

SET(SOURCES
json/json.c
json/main.c)


add_executable(kdevelop-json ${SOURCES})

