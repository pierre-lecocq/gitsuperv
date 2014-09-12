# Gitsuperv

Watch your git repositories status.

[![Build Status](https://travis-ci.org/pierre-lecocq/gitsuperv.svg?branch=master)](https://travis-ci.org/pierre-lecocq/gitsuperv)

Do not leave your work unrevisioned!

*gitsuperv* reports the status of your git repositories from your work base directory

## Install

### Prerequisites

Your system must have the *libgit2* library installed. To do so, simply use your system's package manager (apt-get, yum, ...):

    apt-get install libgit2-dev

If the library is not available for your system, you can compile it yourself:

    git clone https://github.com/libgit2/libgit2
    cd libgit2 && mkdir build && cd build
    cmake .. && sudo cmake --build . --target install && cd ../..
    rm -rf libgit2

### Download the sources

Clone the repository

    git clone https://github.com/pierre-lecocq/gitsuperv
    cd gitsuperv

### Compile

Use the standard commands:

    make && sudo make install

## Run

### Launch the binary

Simply run this command:

    gitsuperv -d /path/your/work/directory

Note the */path/your/work/directory* is the directory that contains your git repositories.

### Setup config file (optional)

Create a *~/.gitsuperv* file and add an absolute path of the folder that host your repositories (without a trailing slash)

    dir=/path/your/work/directory

This will allow you to permanently set the *dir* (-d) option. Now, just run:

    gitsuperv

### Sample output

[![Sample](https://raw.githubusercontent.com/pierre-lecocq/gitsuperv/master/man/sample.png)](https://raw.githubusercontent.com/pierre-lecocq/gitsuperv/master/man/sample.png)

Legend:

- *+n* is the number of created files
- *~n* is the number of modified files
- *-n* is the number of deleted files
- *?n* is the number of untracked files


## Todolist

- Improve output
