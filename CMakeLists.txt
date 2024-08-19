cmake_minimum_required(VERSION 3.20)

project(
        zad2_sik
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -Wpedantic -Werror -pedantic -g -O2 -lboost_program_options)

set(SOURCES src)
set(INCLUDES include)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

include_directories(${INCLUDES})


# Ustawienia kompilatora
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")
#set(CMAKE_CXX_STANDARD_REQUIRED True)
# Dodanie biblioteki Common
add_library(Common
        include/common.h
        src/err.cpp
        include/err.h
        src/common.cpp
        src/Trick.cpp
        include/Trick.h
        src/Game.cpp
        include/Game.h
        src/Allgames.cpp
        include/Allgames.h
        src/Player.cpp
        include/Player.h
        src/Card.cpp
        include/Card.h
        src/common_f.cpp
        include/common_f.h
        src/Server.cpp
        include/Server.h
)

# Dodanie pliku wykonywalnego kierki-klient
add_executable(kierki-klient
        src/kierki-klient.cpp
        include/klient_f.h
        src/klient_f.cpp
)

# Powiązanie biblioteki Common z plikiem wykonywalnym kierki-klient
target_link_libraries(kierki-klient Common)

# Dodanie pliku wykonywalnego kierki-serwer
add_executable(kierki-serwer
        src/kierki-serwer.cpp
)

# Powiązanie biblioteki Common z plikiem wykonywalnym kierki-serwer
target_link_libraries(kierki-serwer Common)

# Dodanie pliku wykonywalnego main
add_executable(main
        src/main.cpp
)

# Powiązanie biblioteki Common z plikiem wykonywalnym main
target_link_libraries(main PRIVATE Common)