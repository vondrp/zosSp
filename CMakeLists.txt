cmake_minimum_required(VERSION 3.23)
project(zos_sp C)

set(CMAKE_C_STANDARD 99)

add_executable(zos_sp main.c
        input/inputHandler.h input/inputHandler.c
        input/checkInput.h input/checkInput.c
        utils/error.h
        utils/messages.h utils/messages.c

        commands/pathCommands.h commands/pathCommands.c

        commands/files/copyFile.h commands/files/copyFile.c
        commands/files/removeFile.h commands/files/removeFile.c
        commands/files/moveFile.h commands/files/moveFile.c
        commands/files/fileContent.h commands/files/fileContent.c

        commands/dir/makeDir.h commands/dir/makeDir.c
        commands/dir/removeDir.h commands/dir/removeDir.c
        commands/dir/ls.h commands/dir/ls.c
        )
