/*
 * File: gitsuperv.c
 * Time-stamp: <>
 * Copyright (C) 2014 Pierre Lecocq
 * Description: Git supervisor
 */

#include "gitsuperv.h"

/*
 * Get repositories paths
 */
char **get_repositories_paths()
{
    char **paths;
    DIR *dirp;
    int count = 0;
    struct dirent *file;

    /* Open */
    if ((dirp = opendir(opts.repo_basedir)) == NULL) {
        perror ("Couldn't open the directory");
    }

    /* Count */
    while ((file = readdir(dirp))) {
        if (strstr(file->d_name, ".") != NULL || file->d_type != DT_DIR)
            continue;
        count++;
    }

    /* Reset */
    paths = (char **) malloc((count + 1) * sizeof(char *));
    rewinddir(dirp);
    count = 0;

    /* Fill */
    while ((file = readdir(dirp))) {
        if (strstr(file->d_name, ".") != NULL || file->d_type != DT_DIR)
            continue;

        int len = strlen(opts.repo_basedir) + 1 + strlen(file->d_name) + 1;
        char *path = (char *) malloc(len * sizeof(char));

        sprintf(path, "%s/%s", opts.repo_basedir, file->d_name);
        paths[count] = path;
        count++;
    }

    /* Close */
    closedir(dirp);

    return(paths);
}

/*
 * Check a single repository status accordin to its path
 */
int check_repository_status(char *repo_path)
{
    int error_code = 0;
    git_repository *repo = NULL;
    git_status_list *status;

    /* Open repository */
    error_code = git_repository_open(&repo, repo_path);
    if (error_code < 0)
        return(1);

    /* Check status */
    error_code = git_status_list_new(&status, repo, &opts.status_opts);
    if (error_code < 0)
        get_last_error(error_code);

    /* Dump */
    dump_current_status(repo_path, status);

    /* Free */
    git_status_list_free(status);
    git_repository_free(repo);

    return(0);
}

/*
 * Dump current status
 */
void dump_current_status(char *path, git_status_list *status)
{
    size_t nb_entries = git_status_list_entrycount(status);

    if (nb_entries  > 0) {
        printf("[%s] %zu files to commit\n", path, nb_entries);
        /* int x; */
        /* const git_status_entry *se; */
        /* for (x = 0; x < nb_entries; x++) { */
        /*     se = git_status_byindex(status, x); */
        /*     if (se->status == GIT_STATUS_CURRENT) { */
        /*         continue; */
        /*     } */
        /* } */
    }
}

/*
 * Get last error according to a returning value
 */
void get_last_error(int error_code)
{
    const git_error *e = giterr_last();

    printf("ERROR %d/%d: %s", error_code, e->klass, e->message);

    exit(error_code);
}

/*
 * Main
 */
int main(int ac, char **av)
{
    opts.verbosity = 1;
    opts.repo_basedir = "/Users/pierre/work";

    opts.status_opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.status_opts.version = 1;
    opts.status_opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
        GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
        GIT_STATUS_OPT_SORT_CASE_SENSITIVELY;

    char **paths = get_repositories_paths();
    for (int x = 0; x < sizeof(paths); x++) {
        check_repository_status(paths[x]);
    }

    return(0);
}
