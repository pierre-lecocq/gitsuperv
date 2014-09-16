/*
 * File: gitsuperv.c
 * Time-stamp: <2014-09-16 23:07:00 pierre>
 * Copyright (C) 2014 Pierre Lecocq
 * Description: Git supervisor
 */

#include "gitsuperv.h"

/*
 * Get repository status
 */
int repository_status(st_repoinfo repoinfo)
{
    int error_code;
    git_repository *repo = NULL;

    /* Open repository */
    error_code = git_repository_open(&repo, repoinfo.path);
    if (error_code < 0)
        last_git_error(error_code);

    /* Statuses */
    files_status(&repoinfo, repo);
    remote_status(&repoinfo, repo);

    /* Free */
    git_repository_free(repo);

    /* Display */
    display_results(repoinfo);

    return(0);
}

/*
 * Get remote status
 */
void remote_status(st_repoinfo *repoinfo, git_repository *repo)
{
    int error_code;
    git_remote *remote;
    git_signature *signature;
    git_revspec rs;

    repoinfo->remote_status = REMOTE_STATUS_CLEAN;

    /* fetch origin */
    /*
    error_code = git_remote_load(&remote, repo, repoinfo->name);
    if (error_code < 0) {
        error_code = git_remote_create_anonymous(&remote, repo, repoinfo->name, NULL);
        if (error_code < 0) {
            git_remote_free(remote);
            last_git_error(error_code);
        }
    }

    error_code = git_signature_default(&signature, repo);
    if (error_code < 0)
        last_git_error(error_code);

    error_code = git_remote_fetch(remote, signature, NULL);
    if (error_code != 0)
        last_git_error(error_code);

*/
    /* rev-parse */

    error_code = git_repository_open_ext(&repo, repoinfo->path, 0, NULL);
    if (error_code != 0)
        last_git_error(error_code);

    error_code = git_revparse(&rs, repo, "HEAD");
    if (error_code != 0)
        last_git_error(error_code);

    printf(" flags = %d | %d | %d | %d | \n",
           rs.flags,
           GIT_REVPARSE_SINGLE,
           GIT_REVPARSE_RANGE,
           GIT_REVPARSE_MERGE_BASE);

    /* if ((rs.flags & GIT_REVPARSE_SINGLE) != 0) { */
    /*     printf("up-to-date\n"); */
    /* } else if ((rs.flags & GIT_REVPARSE_RANGE) != 0) { */
    /*     if ((rs.flags & GIT_REVPARSE_MERGE_BASE) != 0) { */
    /*         printf("Need to push\n"); */
    /*     } */

    /*     printf("Need update\n"); */
    /* } else { */
    /*     printf("Invalid results from git_revparse"); */
    /* } */
}

/*
 * Get files status
 */
void files_status(st_repoinfo *repoinfo, git_repository *repo)
{
    int x;
    int error_code;
    git_status_list *status;
    size_t nb_entries;
    const git_status_entry *se;

    /* Check files status */
    error_code = git_status_list_new(&status, repo, &config.status_opts);
    if (error_code < 0)
        last_git_error(error_code);

    nb_entries = git_status_list_entrycount(status);

    /* Init files counters */
    repoinfo->total_files = nb_entries;
    repoinfo->modified_files = 0;
    repoinfo->created_files = 0;
    repoinfo->deleted_files = 0;
    repoinfo->untracked_files = 0;

    /* Count files */
    for (x = 0; x < nb_entries; x++) {
        se = git_status_byindex(status, x);
        if (se->status == GIT_STATUS_CURRENT) {
            continue;
        }

        if (se->status & GIT_STATUS_INDEX_NEW) {
            repoinfo->created_files++;
        }

        if (
            se->status & GIT_STATUS_INDEX_MODIFIED
            || se->status & GIT_STATUS_INDEX_RENAMED
            || se->status & GIT_STATUS_INDEX_TYPECHANGE
            || se->status & GIT_STATUS_WT_MODIFIED
            || se->status & GIT_STATUS_WT_RENAMED
            || se->status & GIT_STATUS_WT_TYPECHANGE
        ) {
            repoinfo->modified_files++;
        }

        if (
            se->status & GIT_STATUS_INDEX_DELETED
            || se->status & GIT_STATUS_WT_DELETED
        ) {
            repoinfo->deleted_files++;
        }

        if (se->status & GIT_STATUS_WT_NEW) {
            repoinfo->untracked_files++;
        }
    }
}

/*
 * Display results
 */
void display_results(st_repoinfo repoinfo)
{
    char *fmt;
    char *sign;
    char *status;
    int numpad = 3;

    if (config.use_colors == 1) {
        fmt = "%s%-25s % *d files"
            COLOR_CREATED
            " % *d+ "
            COLOR_MODIFIED
            " % *d~ "
            COLOR_DELETED
            " % *d- "
            COLOR_UNTRACKED
            " % *d?"
            COLOR_RESET
            "\t[%s]\n"
            ;
    } else {
        fmt = "%s%-25s % *d files % *d+ % *d~ % *d- % *d?\t[%s]\n";
    }

    if (repoinfo.total_files > 0 || repoinfo.remote_status != REMOTE_STATUS_CLEAN) {
        sign = " * ";
    } else {
        sign = "   ";
    }

    switch (repoinfo.remote_status) {
    case REMOTE_STATUS_PUSH:
        status = "Need to push";
        break;

    case REMOTE_STATUS_PULL:
        status = "Need to pull";
        break;

    default:
        status = "Up to date";
    }

    printf(fmt,
           sign,
           repoinfo.name,
           numpad,
           repoinfo.total_files,
           numpad,
           repoinfo.created_files,
           numpad,
           repoinfo.modified_files,
           numpad,
           repoinfo.deleted_files,
           numpad,
           repoinfo.untracked_files,
           status
    );
}

/*
 * Get repositories
 */
st_repoinfo *get_repositories()
{
    int len;
    int count;
    DIR *dirp;
    st_repoinfo repoinfo;
    st_repoinfo *repositories;
    struct dirent *file;

    /* Open */
    if ((dirp = opendir(config.dir)) == NULL) {
        printf("[%s] Can not open directory %s\n", LOG_FATAL, config.dir);
        exit(1);
    }

    /* Count */
    count = 0;
    while ((file = readdir(dirp))) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0 && file->d_type == DT_DIR)
            count++;
    }

    /* Reset */
    repositories = (st_repoinfo *) malloc((count) * sizeof(st_repoinfo));
    rewinddir(dirp);
    count = 0;

    /* Fill */
    while ((file = readdir(dirp))) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || file->d_type != DT_DIR)
            continue;

        repoinfo.name = (char *) malloc((strlen(file->d_name) + 1) * sizeof(char));
        strcpy(repoinfo.name, file->d_name);

        len = strlen(config.dir) + 1 + strlen(file->d_name) + 1;
        repoinfo.path = (char *) malloc(len * sizeof(char));
        sprintf(repoinfo.path, "%s/%s", config.dir, file->d_name);
        repositories[count] = repoinfo;
        count++;
    }

    /* Close */
    closedir(dirp);

    return(repositories);
}

/*
 * Get last error according to a returning value
 */
void last_git_error(int error_code)
{
    const git_error *e = giterr_last();

    printf("[%s] %d/%d: %s\n", LOG_FATAL, error_code, e->klass, e->message);

    exit(error_code);
}

/*
 * Chomp a string
 */
void chomp(const char *s)
{
    char *p;

    while (NULL != s && NULL != (p = strrchr(s, '\n'))){
        *p = '\0';
    }
}

/*
 * Get config
 */
void load_config_file(char *config_file_path)
{
    FILE *fd;
    char line[MAXBUF];
    char *key;
    char *value;

    config.status_opts.version = 1;
    config.status_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    config.status_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
        GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
        GIT_STATUS_OPT_SORT_CASE_SENSITIVELY;

    /* Open file */
    fd = fopen(config_file_path, "r");
    if (fd != NULL) {
        /* Read lines */
        while (fgets(line, sizeof(line), fd) != NULL) {
            key = strtok(line, "=");
            value = strtok(NULL, "=");
            chomp(value);

            if (strcmp(key, "dir") == 0) {
                config.dir = (char *) malloc((strlen(value) + 1) * sizeof(char));
                strcpy(config.dir, value);
            }
        }

        /* Close */
        fclose(fd);
    }
}

/*
 * Print usage and exit if needed
 */
void usage(int exit_code)
{
    printf("\ngitsuperv %s - %s\n\n", VERSION, DESCRIPTION);

    printf("     Options:\n\n");
    printf("\t-d, --dir=DIR\n");
    printf("\t-h, --help\n");
    printf("\t-v, --version\n");
    printf("\n");

    if (exit_code > 0) {
        exit(exit_code);
    }
}

/*
 * Print verion and exit
 */
void version()
{
    printf("gitsuperv %s\n", VERSION);

    exit(0);
}

/*
 * Main
 */
int main(int argc, char **argv)
{
    int x;
    int result;
    st_repoinfo *repositories;
    char *config_file_path;
    struct option longopts[] = {
        {"dir", optional_argument, NULL, 'd'},
        {"colors", optional_argument, NULL, 'c'},
        {"help", optional_argument, NULL, 'h'},
        {"version", optional_argument, NULL, 'v'},
        {0, 0, 0, 0}
    };

    /* Defaults */
    config.use_colors = 0;

    /* Get options from file */
    config_file_path = (char *) malloc((strlen(getenv("HOME")) + 12) * sizeof(char));
    sprintf(config_file_path, "%s/%s", getenv("HOME"), ".gitsuperv");
    load_config_file(config_file_path);

    /* Get options from command line (override config file) */
    if (argc > 1) {
        while ((x = getopt_long(argc, argv, "d:chv", longopts, NULL)) != -1) {
            switch (x) {
            case 'd':
                config.dir = optarg;
                break;

            case 'c':
                config.use_colors = 1;
                break;

            case 'v':
                version();
                break;

            case 'h':
                usage(1);
                break;

            default:
                usage(1);
            }
        }
    }

    /* Check options */
    if (config.dir == NULL) {
        usage(1);
    }

    if (
        config.use_colors
        && !isatty(fileno(stdout))
    ) {
        config.use_colors = 0;
        printf("[%s] Colors do not seem to be supported\n", LOG_WARNING);
    }

    /* Get infos for each repository */
    git_threads_init();
    repositories = get_repositories();
    for (x = 0; x < sizeof(repositories) - 1; x++) {
        result = repository_status(repositories[x]);
        if (result != 0) {
            printf("[%s] Can not process respository \"%s\" at %s\n", LOG_ERROR, repositories[x].name, repositories[x].path);
        }
    }
    git_threads_shutdown();

    return(0);
}
