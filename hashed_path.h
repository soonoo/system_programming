/*
*
*   File Name       hashed_path.h
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*
*   Title           2018-1 System programming #1-1
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
    char dir_name[DIR_NAME_LENGTH + 1];
    char file_name[FILE_NAME_LENGTH + 1];
    char full_path[HASH_PATH_LENGTH + 1];
} hashed_path;

#endif /* __HASHED_PATH_H__ */
