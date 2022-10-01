//
// Created by vondr on 24.09.2022.
//

#ifndef ZOSSP_CHECKINPUT_H
#define ZOSSP_CHECKINPUT_H

char *getFilename(char *string_path);

/**
 * find all \ and replace them with /
 * check if input is alright
 * @param string	fileName with his path
 * @return			SUCCESS or error type of - MUST FINISH WHEN DECIDE WHAT TO WRITE
 */
int checkFilenameInput(char *string);
#endif //ZOSSP_CHECKINPUT_H
