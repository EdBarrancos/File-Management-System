#include "operations.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../er/error.h"
#include "../thr/threads.h"

#define DEBUG 1


/* Given a path, fills pointers with strings for the parent path and child
 * file name
 * Input:
 *  - path: the path to split. ATENTION: the function may alter this parameter
 *  - parent: reference to a char*, to store parent path
 *  - child: reference to a char*, to store child file name
 */
void split_parent_child_from_path(char * path, char ** parent, char ** child) {

	int n_slashes = 0, last_slash_location = 0;
	int len = strlen(path);

	// deal with trailing slash ( a/x vs a/x/ )
	if (path[len-1] == '/') {
		path[len-1] = '\0';
	}

	for (int i=0; i < len; ++i) {
		if (path[i] == '/' && path[i+1] != '\0') {
			last_slash_location = i;
			n_slashes++;
		}
	}

	if (n_slashes == 0) { // root directory
		*parent = "";
		*child = path;
		return;
	}

	path[last_slash_location] = '\0';
	*parent = path;
	*child = path + last_slash_location + 1;

}


/*
 * Initializes tecnicofs and creates root node.
 */
void init_fs() {
	inode_table_init();

	int root = inode_create(T_DIRECTORY);

	if (root != FS_ROOT)
		errorParse("Error: failed to create node for tecnicofs root\n");
}


/*
 * Destroy tecnicofs and inode table.
 */
void destroy_fs() {
	inode_table_destroy();
}


/*
 * Checks if content of directory is not empty.
 * Input:
 *  - entries: entries of directory
 * Returns: SUCCESS or FAIL
 */

int is_dir_empty(DirEntry *dirEntries) {
	if (dirEntries == NULL) {
		return FAIL;
	}
	for (int i = 0; i < MAX_DIR_ENTRIES; i++) {
		if (dirEntries[i].inumber != FREE_INODE) {
			return FAIL;
		}
	}

	return SUCCESS;
}


/*
 * Looks for node in directory entry from name.
 * Input:
 *  - name: path of node
 *  - entries: entries of directory
 * Returns:
 *  - inumber: found node's inumber
 *  - FAIL: if not found
 */
int lookup_sub_node(char *name, DirEntry *entries) {
	if (entries == NULL) {
		if(DEBUG)
			printf("FAILED\n");
		return FAIL;
	}
	for (int i = 0; i < MAX_DIR_ENTRIES; i++) {
        if (entries[i].inumber != FREE_INODE && strcmp(entries[i].name, name) == 0) {
            return entries[i].inumber;
        }
    }
	return FAIL;
}


/*
 * Creates a new node given a path.
 * Input:
 *  - name: path of node
 *  - nodeType: type of node
 * Returns: SUCCESS or FAIL
 */
int create(char *name, type nodeType, list *List){

	int parent_inumber, child_inumber;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType;
	union Data pdata;

	pthread_rwlock_t *lock_aux;

	strcpy(name_copy, name);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);

	if(DEBUG){
		printf("Lets Create: %s/%s\n", parent_name, child_name);
	}

	parent_inumber = lookup(parent_name, List);


	if (parent_inumber == FAIL) {
		printf("failed to create %s, invalid parent dir %s\n",
		        name, parent_name);
		return FAIL;
	}

	/* Lock Write Node */
	lock_aux = getLastItem(List);
	unlockRW(lock_aux);
	lockInumberWrite(parent_inumber);
	addList(List, getLockInumber(parent_inumber));

	inode_get(parent_inumber, &pType, &pdata);

	if(pType != T_DIRECTORY) {

		printf("failed to create %s, parent %s is not a dir\n",
		        name, parent_name);
		return FAIL;
	}

	if (lookup_sub_node(child_name, pdata.dirEntries) != FAIL) {
		printf("failed to create %s, already exists in dir %s\n",
		       child_name, parent_name);
		return FAIL;
	}

	/* create node and add entry to folder that contains new node */
	child_inumber = inode_create(nodeType);
	if (child_inumber == FAIL) {
		printf("failed to create %s in  %s, couldn't allocate inode\n",
		        child_name, parent_name);
		return FAIL;
	}

	if (dir_add_entry(parent_inumber, child_inumber, child_name) == FAIL) {
		printf("could not add entry %s in dir %s\n",
		       child_name, parent_name);
		return FAIL;
	}

	if(DEBUG)
		printf("Finished Create %s/%s\n", parent_name, child_name);
	
	return SUCCESS;
}

/*
 * moves a node given a path to another path.
 * Input:
 *  - nodeOrigin: the original path of the node
 * 	- nodeDestination: the final path of the node
 * Returns: SUCCESS or FAIL
 */
int move(char* nodeOrigin, char* nodeDestination, list *List){

	int parent_inumber_orig, child_inumber_orig;
	int parent_inumber_dest, child_inumber_dest;
	char *parent_name_orig, *child_name_orig, name_copy_orig[MAX_FILE_NAME];
	char *parent_name_dest, *child_name_dest, name_copy_dest[MAX_FILE_NAME];
	
	//pthread_rwlock_t *lock_aux_orig;
	//pthread_rwlock_t *lock_aux_dest;

	type pType_orig, cType_orig;
	union Data pdata_orig, cdata_orig;


	type pType_dest, cType_dest;
	union Data pdata_dest, cdata_dest;

 	/* //char full_path[MAX_FILE_NAME];
	char delim[] = "/";

	// Split child from path 
	strcpy(name_copy_orig, nodeOrigin);
	split_parent_child_from_path(name_copy_orig, &parent_name_orig, &child_name_orig);

	// start at root node 
	parent_inumber_orig = FS_ROOT;

	// Lock Root 
	lockInumberRead(parent_inumber_orig);
	addList(List, getLockInumber(parent_inumber_orig));

	// get root inode data 
	inode_get(parent_inumber_orig, &pType_orig, &pdata_orig);

	parent_name_orig = strtok(name_copy_orig, delim);

	// search for all sub nodes 
	while (parent_name_orig != NULL && (parent_inumber_orig = lookup_sub_node(parent_name_orig, pdata_orig.dirEntries)) != FAIL) {
		lockInumberRead(parent_inumber_orig);
		addList(List, getLockInumber(parent_inumber_orig));

		inode_get(parent_inumber_orig, &pType_orig, &pdata_orig);
		parent_name_orig = strtok(NULL, delim);
	}

	if (parent_inumber_orig == FAIL) {
		printf("failed to move %s, invalid parent dir %s\n",
		        child_name_orig, parent_name_orig);
		return FAIL;
	}

	inode_get(child_inumber_orig, &cType_orig, &cdata_orig);

	if(lookup_sub_node(parent_name_orig, cdata_orig.dirEntries) != FAIL){

		lockInumberRead(parent_inumber_orig);
		addList(List, getLockInumber(parent_inumber_orig));
	}

		
	if(lookup_sub_node(parent_name_orig, cdata_orig.dirEntries) == FAIL){
		printf("failed to move %s, inexistent child in parent dir %s\n",child_name_orig, parent_name_orig);
		return FAIL;		
	}

	//DESTINATION
	
	// Split child from path 
	strcpy(name_copy_dest, nodeDestination);
	split_parent_child_from_path(name_copy_dest, &parent_name_dest, &child_name_dest);

	// start at root node 
	parent_inumber_dest = FS_ROOT;

	// get root inode data 
	inode_get(parent_inumber_dest, &pType_dest, &pdata_dest);
	inode_get(child_inumber_dest, &cType_dest, &cdata_dest);


	parent_name_dest = strtok(name_copy_dest, delim);

	// search for all sub nodes 
	while (parent_name_dest != NULL && (parent_inumber_dest = lookup_sub_node(parent_name_dest, pdata_dest.dirEntries)) != FAIL) {
		if(searchList(getLockInumber(parent_inumber_dest), List)==0){
			lockInumberRead(parent_inumber_dest);
			addList(List, getLockInumber(parent_inumber_dest));
		}

		if(lookup_sub_node(parent_name_dest, cdata_dest.dirEntries)!=FAIL){
			printf("failed to move %s, invalid format dir %s\n",child_name_dest, parent_name_dest);
			return FAIL;
		}

		inode_get(parent_inumber_dest, &pType_dest, &pdata_dest);
		parent_name_dest = strtok(NULL, delim);
	}

	if (parent_inumber_dest == FAIL) {
		printf("failed to move %s, invalid parent dir %s\n",
		        child_name_dest, parent_name_dest);
		return FAIL;
	}
		
	if(lookup_sub_node(parent_name_dest, cdata_dest.dirEntries) != FAIL){
		printf("failed to move %s, already exists child in parent dir %s\n",child_name_dest, parent_name_dest);
		return FAIL;		
	}

	deleteList(List, getLockInumber(parent_inumber_orig));
	unlockRW(getLockInumber(parent_inumber_orig));

	if (tryLockWrite(getLockInumber(parent_inumber_orig))!=0){

		return WAIT;

	}

	addList(List, getLockInumber(parent_inumber_dest));

	deleteList(List, getLockInumber(parent_inumber_dest));
	unlockRW(getLockInumber(parent_inumber_dest));

	if (tryLockWrite(getLockInumber(parent_inumber_dest))!=0){

		return WAIT;

	}

	addList(List, getLockInumber(parent_inumber_dest));

	dir_reset_entry(parent_inumber_orig, child_inumber_orig);

	dir_add_entry(parent_inumber_dest,child_inumber_dest,child_name_dest);

	return SUCCESS;  */


	/* // Splits Child From Path 
	strcpy(name_copy_dest, nodeDestination);
	split_parent_child_from_path(name_copy_dest, &parent_name_dest, &child_name_dest);

	parent_inumber_dest = lookup(parent_name_dest, List);

	if (parent_inumber_dest == FAIL) {
		printf("failed to move %s, invalid parent dir %s\n",
		        child_name_dest, parent_name_dest);
		return FAIL;
	}

	// Unlock Last Node 
	lock_aux_dest = getLastItem(List);
	unlockRW(lock_aux_dest);

	if(!strcmp(child_name_orig, child_name_dest)){
		printf("failed to move %s, invalid destiny path %s\n ",
			child_name_orig, child_name_dest);
		return FAIL;
	}

	// Lock Write 
	if(parent_inumber_orig < parent_inumber_dest){
		lockInumberWrite(parent_inumber_orig);
		addList(List, getLockInumber(parent_inumber_orig));

		lockInumberWrite(parent_inumber_dest);
		addList(List, getLockInumber(parent_inumber_dest));
	}
	else{
		lockInumberWrite(parent_inumber_dest);
		addList(List, getLockInumber(parent_inumber_dest));

		lockInumberWrite(parent_inumber_orig);
		addList(List, getLockInumber(parent_inumber_orig));
	}

	// Verify if parent is directory 
	inode_get(parent_inumber_orig, &pType_orig, &pdata_orig);

	if(pType_orig != T_DIRECTORY) {
		printf("failed to move %s, parent %s is not a dir\n",
		        child_name_orig, parent_name_orig);
		return FAIL;
	}

	// Verify if child origin exists 
	child_inumber_orig = lookup_sub_node(child_name_orig, pdata_orig.dirEntries);

	if (child_inumber_orig == FAIL) {
		printf("could not move %s, does not exist in dir %s\n",
		       child_name_orig, parent_name_orig);
		return FAIL;
	}

	return SUCCESS; */

	//check locks in list
	//sort out how to index nodes
}


/*
 * Deletes a node given a path.
 * Input:
 *  - name: path of node
 * Returns: SUCCESS or FAIL
 */
int delete(char *name, list *List){

	int parent_inumber, child_inumber;
	char *parent_name, *child_name, name_copy[MAX_FILE_NAME];
	/* use for copy */
	type pType, cType;
	union Data pdata, cdata;

	pthread_rwlock_t *lock_aux;

	strcpy(name_copy, name);
	split_parent_child_from_path(name_copy, &parent_name, &child_name);
	

	if(DEBUG){
		printf("Lets Delete: %s/%s\n", parent_name, child_name);
	}

	parent_inumber = lookup(parent_name, List);

	if (parent_inumber == FAIL) {
		printf("failed to delete %s, invalid parent dir %s\n",
		        child_name, parent_name);
		return FAIL;
	}

	/* Lock Write Node */
	lock_aux = getLastItem(List);
	unlockRW(lock_aux);
	lockInumberWrite(parent_inumber);
	addList(List, getLockInumber(parent_inumber));

	inode_get(parent_inumber, &pType, &pdata);

	if(pType != T_DIRECTORY) {
		printf("failed to delete %s, parent %s is not a dir\n",
		        child_name, parent_name);
		return FAIL;
	}

	child_inumber = lookup_sub_node(child_name, pdata.dirEntries);

	if (child_inumber == FAIL) {
		printf("could not delete %s, does not exist in dir %s\n",
		       name, parent_name);
		return FAIL;
	}

	inode_get(child_inumber, &cType, &cdata);

	if (cType == T_DIRECTORY && is_dir_empty(cdata.dirEntries) == FAIL) {
		printf("could not delete %s: is a directory and not empty\n",
		       name);
		return FAIL;
	}

	/* remove entry from folder that contained deleted node */
	if (dir_reset_entry(parent_inumber, child_inumber) == FAIL) {
		printf("failed to delete %s from dir %s\n",
		       child_name, parent_name);
		return FAIL;
	}

	if (inode_delete(child_inumber) == FAIL) {
		printf("could not delete inode number %d from dir %s\n",
		       child_inumber, parent_name);
		return FAIL;
	}

	if(DEBUG)
		printf("Finished Delete %s/%s\n", parent_name, child_name);

	return SUCCESS;
}


/*
 * Lookup for a given path.
 * Input:
 *  - name: path of node
 * Returns:
 *  inumber: identifier of the i-node, if found
 *     FAIL: otherwise
 */
int lookup(char *name, list* List) {
	char full_path[MAX_FILE_NAME];
	char delim[] = "/";

	strcpy(full_path, name);

	if(DEBUG)
		printf("LookUp Strated %s\n", full_path);

	/* start at root node */
	int current_inumber = FS_ROOT;

	/* use for copy */
	type nType;
	union Data data;

	/* Lock Root */
	lockInumberRead(current_inumber);
	addList(List, getLockInumber(current_inumber));

	/* get root inode data */
	inode_get(current_inumber, &nType, &data);

	char *path = strtok(full_path, delim);

	/* search for all sub nodes */
	while (path != NULL && (current_inumber = lookup_sub_node(path, data.dirEntries)) != FAIL) {
		/* Lock node */
		lockInumberRead(current_inumber);
		addList(List, getLockInumber(current_inumber));

		inode_get(current_inumber, &nType, &data);
		path = strtok(NULL, delim);
	}

	if(DEBUG)
		printf("LookUp Finished and The Number Found: %d\n", current_inumber);

	return current_inumber;
}


/*
 * Prints tecnicofs tree.
 * Input:
 *  - fp: pointer to output file
 */
void print_tecnicofs_tree(FILE *fp){
	inode_print_tree(fp, FS_ROOT, "");
}
