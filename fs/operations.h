#ifndef FS_H
#define FS_H
#include "state.h"
#include "threads.h"
#include "../lst/list.h"
#include <pthread.h>

extern inode_t *inode_table;

void init_fs();
void destroy_fs();
int is_dir_empty(DirEntry *dirEntries);
int create(char *name, type nodeType, list *List);
int delete(char *name, list *List);
int lookup(char *name, list *List);
void print_tecnicofs_tree(FILE *fp);

#endif /* FS_H */
