/*
 * File: gitsuperv.h
 * Time-stamp: <2014-08-18 23:58:54 pierre>
 * Copyright (C) 2014 Pierre Lecocq
 * Description: Git supervisor include file
 */

/*
 * Includes
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
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
    int                 use_colors;
    git_status_options  status_opts;
} st_config;

typedef struct result {
    char                *path;
    char                *name;
    int                 total;
    int                 modified;
    int                 created;
    int                 deleted;
    int                 untracked;
} st_result;

/*
 * Globals
 */

static st_config config;

/*
 * Synopsis
 */

char **get_repositories_paths();
int check_repository_status(char *repo_path);
void print_current_status(st_result result);
st_result get_current_status(char *path, git_status_list *status);
void get_last_git_error(int error_code);
void chomp(const char *s);
void load_config_from_file(char *config_file_path);
void usage(int exit_code);
