/*
*
*   File Name       hashed_path.h
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*
*   Title           2018-1 System programming #2-3
*   Description     Struct for hashed url path
*                   - dir_name:     first 3 letters of hashed url
*                   - file_name:    remaining 37 letters of hashed url
*                   - full_path:    complete form of url - "dir_name/file_name"
*/

#ifndef __HASHED_PATH_H__
#define __HASHED_PATH_H__

#define MAX_URL_LENGTH      2048
#define DIR_NAME_LENGTH     3
#define FILE_NAME_LENGTH    37
#define HASH_PATH_LENGTH    41

typedef struct {
    char url[MAX_URL_LENGTH];

    // first 3 letters of checksum
    char dir_name[DIR_NAME_LENGTH + 1];

    // last 37 letters of checksum
    char file_name[FILE_NAME_LENGTH + 1];

    // ex. "d8b/99f68b208b5453b391cb0c6c3d6a9824f3c3a"
    char full_path[HASH_PATH_LENGTH + 1];
} hashed_path;

#endif /* __HASHED_PATH_H__ */
