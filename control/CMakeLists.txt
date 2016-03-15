cmake_minimum_required(VERSION 2.6)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(ANGORT_SOURCE_DIR $ENV{HOME}/angort)

project(ROVER)

set(SOURCES protocol.cpp regsauto.cpp)

set(WORDFILELIST)
# this macro generates a custom build step which takes the files passed in,
# passes them through Angort's 'makeWords.pl' script, and compiles *those*.
# The files generated are added to WORDFILELIST, which should then be put
# into the SOURCES. So, Angort word files should be added with this.

macro(add_words_files)
    add_custom_command(
        OUTPUT wordslist
        COMMAND rm wordslist
        )
    foreach(file ${ARGV})
        add_custom_command(
            OUTPUT ${file}.cpp
            DEPENDS ${file}
            COMMAND pwd
            COMMAND perl  ${ANGORT_SOURCE_DIR}/makeWords.pl  ${CMAKE_SOURCE_DIR}/${file}>${file}.cpp
            COMMAND cat words >>wordslist
        )
        set(WORDFILELIST ${WORDFILELIST} ${file}.cpp)
    endforeach(file)
endmacro(add_words_files)

add_words_files(words.cpp)

link_directories(${ANGORT_SOURCE_DIR}/lib)
add_library(rover SHARED ${WORDFILELIST} ${SOURCES})
add_library(roverctrl STATIC ${SOURCES})
add_executable(test test.cpp ${SOURCES})

add_custom_command(TARGET rover
    POST_BUILD
    COMMAND pwd
    COMMAND cp librover.so rover.angso
)   


