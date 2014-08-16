/*
 * File: gitsuperv.h
 * Time-stamp: <>
 * Copyright (C) 2014 Pierre Lecocq
 * Description: Git supervisor include file
 */

/*
 * Includes
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <git2.h>

/*
 * Defines
 */
#define MAXBUF 1024

#define COLOR_UNTRACKED         "\x1b[31m"
#define COLOR_CREATED           "\x1b[32m"
#define COLOR_MODIFIED          "\x1b[33m"
#define COLOR_DELETED           "\x1b[35m"
#define COLOR_RESET             "\x1b[0m"

/*
 * Types
 */

typedef struct config {
    char                *basedir;
    git_status_options  status_opts;
} st_config;

typedef struct result {
    char                *path;
    int                 total;
    int                 modified;
    int                 created;
    int                 deleted;
    int                 untracked;
} st_result;

/*
 * Synopsis
 */

char **get_repositories_paths();
int check_repository_status(char *repo_path);
void dump_current_status(st_result result);
st_result get_current_status(char *path, git_status_list *status);
void get_last_git_error(int error_code);
void load_config_from_file(char *config_file_path);

/*
 * Globals
 */

static st_config config;
static int use_colors;
