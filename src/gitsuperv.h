/*
 * File: gitsuperv.h
 * Time-stamp: <2014-09-13 20:14:04 pierre>
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
#define VERSION "1.7"
#define DESCRIPTION "Watch your git repositories status"

#define MAXBUF 1024

#define REMOTE_STATUS_CLEAN     0
#define REMOTE_STATUS_PUSH      1
#define REMOTE_STATUS_PULL      2

#define COLOR_UNTRACKED         "\x1b[31m"
#define COLOR_CREATED           "\x1b[32m"
#define COLOR_MODIFIED          "\x1b[33m"
#define COLOR_DELETED           "\x1b[35m"
#define COLOR_RESET             "\x1b[0m"

#define LOG_WARNING             "WARNING"
#define LOG_ERROR               "ERROR"
#define LOG_FATAL               "FATAL"

/*
 * Types
 */

typedef struct config {
    char                *dir;
    int                 use_colors;
    git_status_options  status_opts;
} st_config;

typedef struct repoinfo {
    char                *path;
    char                *name;
    int                 total_files;
    int                 modified_files;
    int                 created_files;
    int                 deleted_files;
    int                 untracked_files;
    int                 remote_status;
} st_repoinfo;

/*
 * Globals
 */

static st_config config;

/*
 * Synopsis
 */

int repository_status(st_repoinfo repoinfo);
void remote_status(st_repoinfo *repoinfo, git_repository *repo);
void files_status(st_repoinfo *repoinfo, git_repository *repo);
void display_results(st_repoinfo repoinfo);

st_repoinfo *get_repositories();
void last_git_error(int error_code);
void chomp(const char *s);
void load_config_file(char *config_file_path);
void usage(int exit_code);
void version();
