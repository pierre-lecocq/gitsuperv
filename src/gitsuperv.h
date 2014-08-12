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
#include <git2.h>

/*
 * Defines
 */

#define MESSAGE_NOTICE 1
#define MESSAGE_WARNING 2
#define MESSAGE_ERROR 3

#define VERBOSITY_NULL 0
#define VERBOSITY_MIN 1
#define VERBOSITY_MAX 2

/*
 * Types
 */

typedef struct options {
    int                 verbosity;
    char                *repo_basedir;
    git_status_options  status_opts;
} struct_options;

/*
 * Synopsis
 */

void check_repository_status(char *repo_path);
void get_last_error(int error_code);
void print_message(char *str, int message_type);


/*
 * Globals
 */

static struct options opts;
