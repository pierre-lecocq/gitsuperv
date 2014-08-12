/*
 * File: gitsuperv.c
 * Time-stamp: <>
 * Copyright (C) 2014 Pierre Lecocq
 * Description: Git supervisor
 */

#include "gitsuperv.h"

/*
 * Check a single repository status accordin to its path
 */
void check_repository_status(char *repo_path)
{
    int error_code = 0;
    git_repository *repo = NULL;
    git_status_list *status;

    /* Open repository */
    error_code = git_repository_open(&repo, repo_path);
    if (error_code < 0) {
        get_last_error(error_code);
    }

    /* Check status */
    error_code = git_status_list_new(&status, repo, &opts.status_opts);
    if (error_code < 0) {
        get_last_error(error_code);
    }

    printf("Size: %zu\n", git_status_list_entrycount(status));

    /* Free */
    git_status_list_free(status);
    git_repository_free(repo);
}

/*
 * Get last error according to a returning value
 */
void get_last_error(int error_code)
{
    const git_error *e = giterr_last();
    int len = 15 + strlen(e->message);
    char *str = (char *) malloc(len * sizeof(char));

    sprintf(str, "ERROR %d/%d: %s", error_code, e->klass, e->message);
    print_message(str, MESSAGE_ERROR);
    free(str);

    exit(error_code);
}

/*
 * Print a message accoring to a message type and verbosity level
 */
void print_message(char *str, int message_type)
{
    FILE *stream;

    if (message_type == 2) {
        stream = stderr;
    } else {
        stream = stdout;
    }

    // TODO: take care of opts.verbosity

    fprintf(stream, "%s\n", str);
}

/*
 * Main
 */
int main(int ac, char **av)
{
    opts.verbosity = 1;
    opts.repo_basedir = "/Users/pierre/work/";

    opts.status_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.status_opts.version = 1;
    opts.status_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
        GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
        GIT_STATUS_OPT_SORT_CASE_SENSITIVELY;

    check_repository_status("/Users/pierre/work/color-theme-darkmine");

    return(0);
}
