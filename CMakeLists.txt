cmake_minimum_required(VERSION 3.23)
project(zos_sp C)

set(CMAKE_C_STANDARD 99)

add_executable(zos_sp main.c
        input/inputHandler.h input/inputHandler.c
        input/checkInput.h input/checkInput.c
        utils/error.h
        utils/messages.h utils/messages.c
        commands/copyFile.h commands/copyFile.c
        commands/removeFile.h commands/removeFile.c
        commands/moveFile.h commands/moveFile.c
        )
