/*
 * File: gitsuperv.c
 * Time-stamp: <2014-08-19 00:00:13 pierre>
 * Copyright (C) 2014 Pierre Lecocq
 * Description: Git supervisor
 */

#include "gitsuperv.h"

/*
 * Get repositories paths
 */
char **get_repositories_paths()
{
    int len;
    int count;
    DIR *dirp;
    char *path;
    char **paths;
    struct dirent *file;

    /* Open */
    if ((dirp = opendir(config.basedir)) == NULL) {
        printf("Can not open directory %s\n", config.basedir);
        exit(1);
    }

    /* Count */
    count = 0;
    while ((file = readdir(dirp))) {
        if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0 && file->d_type == DT_DIR)
            count++;
    }

    /* Reset */
    paths = (char **) malloc((count + 1) * sizeof(char *));
    rewinddir(dirp);
    count = 0;

    /* Fill */
    while ((file = readdir(dirp))) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0 || file->d_type != DT_DIR)
            continue;

        len = strlen(config.basedir) + 1 + strlen(file->d_name) + 1;
        path = (char *) malloc(len * sizeof(char));

        sprintf(path, "%s/%s", config.basedir, file->d_name);
        paths[count] = path;
        count++;
    }

    /* Close */
    closedir(dirp);

    return(paths);
}

/*
 * Check a single repository status according to its path
 */
int check_repository_status(char *repo_path)
{
    int error_code = 0;
    git_repository *repo = NULL;
    git_status_list *status;
    st_result result;

    /* Open repository */
    error_code = git_repository_open(&repo, repo_path);
    if (error_code < 0)
        return(1);

    /* Check status */
    error_code = git_status_list_new(&status, repo, &config.status_opts);
    if (error_code < 0)
        get_last_git_error(error_code);

    /* Get result */
    result = get_current_status(repo_path, status);

    /* Dump result */
    print_current_status(result);

    /* Free */
    git_status_list_free(status);
    git_repository_free(repo);

    return(0);
}

/*
 * Dump current status
 */
void print_current_status(st_result result)
{
    char *fmt;
    char *sign;

    if (config.use_colors == 1) {
        fmt = "%s%-25s %d files:"
            COLOR_CREATED
            " +%d "
            COLOR_MODIFIED
            " ~%d "
            COLOR_DELETED
            " -%d "
            COLOR_UNTRACKED
            " ?%d\n"
            COLOR_RESET;
    } else {
        fmt = "%s%-25s %d files: +%d ~%d -%d ?%d\n";
    }

    if (result.total > 0) {
        sign = " * ";
    } else {
        sign = "   ";
    }

    printf(fmt,
           sign,
           result.name,
           result.total,
           result.created,
           result.modified,
           result.deleted,
           result.untracked
    );
}

/*
 * Get current status
 */
st_result get_current_status(char *path, git_status_list *status)
{
    int x;
    size_t nb_entries;
    const git_status_entry *se;
    st_result result;

    nb_entries = git_status_list_entrycount(status);

    /* Path */
    result.path = (char *)malloc((strlen(path) + 1) * sizeof(char *));
    strcpy(result.path, path);

    /* Name */
    // +1 for moving pointer to the second index and remove the slash
    result.name = strrchr(path, '/') + 1;

    /* Init files counters */
    result.total = nb_entries;
    result.modified = 0;
    result.created = 0;
    result.deleted = 0;
    result.untracked = 0;

    /* Count files */
    for (x = 0; x < nb_entries; x++) {
        se = git_status_byindex(status, x);
        if (se->status == GIT_STATUS_CURRENT) {
            continue;
        }

        if (se->status & GIT_STATUS_INDEX_NEW) {
            result.created++;
        }

        if (
            se->status & GIT_STATUS_INDEX_MODIFIED
            || se->status & GIT_STATUS_INDEX_RENAMED
            || se->status & GIT_STATUS_INDEX_TYPECHANGE
            || se->status & GIT_STATUS_WT_MODIFIED
            || se->status & GIT_STATUS_WT_RENAMED
            || se->status & GIT_STATUS_WT_TYPECHANGE
        ) {
            result.modified++;
        }

        if (
            se->status & GIT_STATUS_INDEX_DELETED
            || se->status & GIT_STATUS_WT_DELETED
        ) {
            result.deleted++;
        }

        if (se->status & GIT_STATUS_WT_NEW) {
            result.untracked++;
        }
    }

    return(result);
}

/*
 * Get last error according to a returning value
 */
void get_last_git_error(int error_code)
{
    const git_error *e = giterr_last();

    printf("ERROR %d/%d: %s", error_code, e->klass, e->message);

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
void load_config_from_file(char *config_file_path)
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

            if (strcmp(key, "basedir") == 0) {
                config.basedir = (char *)malloc((strlen(value)+1) * sizeof(char));
                strcpy(config.basedir, value);
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
    printf("\ngitsuperv - Watch your git repositories status\n\n");
    printf("\t-b, --basedir=BASEDIR\n");
    printf("\n");

    if (exit_code > 0) {
        exit(exit_code);
    }
}

/*
 * Main
 */
int main(int argc, char **argv)
{
    int x;
    char **paths;
    char *config_file_path;
    struct option longopts[] = {
        {"basedir", optional_argument, NULL, 'b'},
        {0, 0, 0, 0}
    };

    config.use_colors = isatty(fileno(stdout));

    /* Get options from file */
    config_file_path = (char *)malloc((strlen(getenv("HOME")) + 12) * sizeof(char));
    sprintf(config_file_path, "%s/%s", getenv("HOME"), ".gitsuperv");
    load_config_from_file(config_file_path);

    /* Get options */
    if (argc > 1) {
        while ((x = getopt_long(argc, argv, "b:", longopts, NULL)) != -1) {
            switch (x) {
            case 'b':
                config.basedir = optarg;
                break;

            default:
                usage(1);
            }
        }
    }

    /* Check options */
    if (config.basedir == NULL) {
        usage(1);
    }

    /* Get paths */
    paths = get_repositories_paths();

    /* Get statuses */
    for (x = 0; (x < sizeof(paths) && paths[x]); x++) {
        check_repository_status(paths[x]);
    }

    return(0);
}
