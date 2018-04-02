/*
*
*   File Name       util.c
*   Date            2018/03/27
*   OS              Ubuntu 16.04 64 bits
*   Author          Hong Soonwoo
*   Student ID      2014722023
*   
*   Title           2018-1 System programming #1-1
*   Description     Utility functions for string manipulation, getting user input
*
*/

#include "headers.h"

/*
*
*   remove_newline
*   Input           char *      string with newline at the end
*                   size_t *    size of string
*
*   Description     removes new line character at the end of string
*
*/
void remove_newline(char *string, size_t *size)
{
    if ((*size) == 0 || string == NULL) return;
    if (string[(*size) - 1] == '\n') {
        string[(*size) - 1] = '\0';
        (*size)--;
    }
}


/*
*
*   get_input
*   Input           char **     double pointer to a user input
*                   size_t *    buffer
*
*   Output          size_t      size of user input
*
*   Description     get user input using getline()
*
*/
size_t get_input(char **buf, size_t *len)
{
    printf("input URL > ");
    return getline(buf, len, stdin);
}


/*
*
*   get_hash_path
*   Input           char *          hashed url string
*                   hashed_path     pointer to path string to be calculated
*
*   Output          hashed_path *   pointer to a file path
*
*   Description     parse hashed url to path string
*
*/
hashed_path *get_hash_path(char *hashed_url, hashed_path *path)
{
    if (hashed_url == NULL || path == NULL) return NULL;

    // get directory name and file name from 160 bits checksum
    strncpy(path->dir_name, hashed_url, DIR_NAME_LENGTH);
    strncpy(path->file_name, hashed_url + DIR_NAME_LENGTH, FILE_NAME_LENGTH);

    // save full path to path->full_path e.g. "4AF/A71FFB32DC64586C1A3B65CB7B99D7F2E3185"
    strncpy(path->full_path, path->dir_name, DIR_NAME_LENGTH);
    strncpy(path->full_path + DIR_NAME_LENGTH, "/", 1);
    strncpy(path->full_path + DIR_NAME_LENGTH + 1, path->file_name, FILE_NAME_LENGTH);

    return path;
}


/*
*
*   is_hit
*   Input           hashed_path*    hashed url string
*
*   Output          bool            true if url is hit
*
*   Description     return true if url is hit
*
*/
bool is_hit(hashed_path *path)
{
    DIR *dp_cache = opendir(CACHE_DIR_NAME);
    struct dirent *dir_cache;

    while((dir_cache = readdir(dp_cache))) {
        if (strcmp(dir_cache->d_name, path->dir_name) == EQUAL) {
            chdir(CACHE_DIR_NAME);
            DIR *dp_hash = opendir(path->dir_name);
            struct dirent *dir_hash;

            while((dir_hash = readdir(dp_hash))) {
                if(strcmp(dir_hash->d_name, path->file_name) == EQUAL) {
                    closedir(dp_hash);
                    chdir("..");
                    return true;
                }
            }
            closedir(dp_hash);
            chdir("..");
        }
    }
    closedir(dp_cache);

    return false;
}

