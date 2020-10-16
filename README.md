# SO Project 2020-21

## Authors

Rita Duarte

Eduardo Barrancos

## Exercise 1 Docs

### Folder *.*

- [main.c](./main.c)
- [tecnicofs-afs-constants.h](./tecnicofs-api-constants.h)

#### *main* file

The main file.
Handles the input arguments and processes it turning into commands.
Handle commands.
Call Functions in other files.

#### *tecnicofs-api-constants* file

Given as part of the base code.

A lot of constants, still not sure what they all do.


### Folder *er*

- [error.c](./er/error.c)
- [error.h](./er/error.h)

#### *error* files

Handle error code and related functions.

### Folder *fs*

- [operations.c](./fs/operations.c)
- [operations.h](./fs/operations.h)
- [state.c](./fs/state.c)
- [state.h](./fs/state.h)

#### *operations* files

Given as part of the base code.

The bridge between the functions *state files* and the more abstract code in the [main](./main.c).
Handling calls to create files or folders, destroy them, search for them, and initialize the file tree.

#### *state* files

Given as part of the base code.

Low level of abstraction code.
Defines the structures behind files and nodes and handles those functionalities.

### Folder *fh*

- [fileHandling.c](./fh/fileHandling.c)
- [fileHandling.h](./fh/fileHandling.h)

#### *fileHandling* files

More abstract functions to open and close files.

### Folder *thr*

- [threads.c](./thr/threads.c)
- [threads.h](./thr/threads.h)

#### *threds* files

Thread code. Handling threads and pools of it, the locks and the synch strategy. As well as the functionalities that come with it.

## Exercise 2

We are ready for you
