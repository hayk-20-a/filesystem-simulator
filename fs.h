#ifndef FS_H
#define FS_H

// color support
#ifndef USE_COLOR
#define USE_COLOR 1
#endif

#if USE_COLOR
    #define BLUE   "\033[1;34m"
    #define GREEN  "\033[1;32m"
    #define RED    "\033[1;31m"
    #define YELLOW "\033[1;33m"
    #define RESET  "\033[0m"
#else
    #define BLUE ""
    #define GREEN ""
    #define RED ""
    #define YELLOW ""
    #define RESET ""
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CHILDREN 100
#define SAVE_FILE "filesystem.db"

// node struct
typedef struct Node {
    char name[100];
    int isFile;
    int size;
    time_t created_at;

    struct Node* parent;
    struct Node* children[MAX_CHILDREN];
    int child_count;
} Node;

// global state
extern Node* root;
extern Node* current;

// core
Node* create_node(char* name, int isFile);
void add_child(Node* parent, Node* child);
void remove_child(Node* parent, Node* target);
Node* find_child(Node* parent, char* name);
Node* copy_node(Node* node);

// commands
void mkdir_cmd(char* name);
void touch_cmd(char* name, int size);
void ls_cmd();
void cd_cmd(char* name);
void pwd_cmd();
void tree_cmd(Node* node, char* prefix, int isLast);
void find_cmd(Node* node, char* name);
void rm_cmd(char* name);
int du_cmd(Node* node);

// extra commands
void mv_cmd(char* name, char* dest);
void cp_cmd(char* name, char* dest);

// help and persitence
void help_cmd();
void save_fs(Node* node, FILE* f, int depth);
Node* load_fs(FILE* f);

#endif