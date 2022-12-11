cmake_minimum_required(VERSION 3.16.3)
project(zos_sp C)

set(CMAKE_C_STANDARD 99)

add_executable(
        zos_sp main.c
        input/inputHandler.h input/inputHandler.c
        input/checkInput.h input/checkInput.c
        output/error.h
        output/messages.h output/messages.c

        commands/fileCommands.h commands/fileCommands.c
        commands/pathCommands.h commands/pathCommands.c
        commands/dirCommands.h commands/dirCommands.c
        commands/fsCommands.h commands/fsCommands.c

        fat/fat.h fat/fat.c
        )
target_link_libraries(${PROJECT_NAME} m)
