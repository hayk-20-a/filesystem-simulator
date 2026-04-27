#include "fs.h"

// global state
Node* root;
Node* current;


// node
Node* create_node(char* name, int isFile) {
    Node* node = malloc(sizeof(Node));

    strcpy(node->name, name);
    node->isFile = isFile;
    node->size = 0;
    node->created_at = time(NULL);

    node->parent = NULL;
    node->child_count = 0;

    return node;
}

// tree ops
void add_child(Node* parent, Node* child) {
    if (parent->child_count >= MAX_CHILDREN) {
        printf(RED "Max children reached\n" RESET);
        return;
    }

    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

Node* find_child(Node* parent, char* name) {
    for (int i = 0; i < parent->child_count; i++) {
        if (strcmp(parent->children[i]->name, name) == 0)
            return parent->children[i];
    }
    return NULL;
}

void remove_child(Node* parent, Node* target) {
    int idx = -1;

    for (int i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == target) {
            idx = i;
            break;
        }
    }

    if (idx == -1) return;

    for (int i = idx; i < parent->child_count - 1; i++) {
        parent->children[i] = parent->children[i + 1];
    }

    parent->child_count--;
}

// mkdir command
void mkdir_cmd(char* name) {
    if (find_child(current, name)) {
        printf(YELLOW "mkdir: exists\n" RESET);
        return;
    }

    add_child(current, create_node(name, 0));
    printf(GREEN "Directory created\n" RESET);
}

// touch command
void touch_cmd(char* name, int size) {
    if (find_child(current, name)) {
        printf(YELLOW "touch: exists\n" RESET);
        return;
    }

    Node* f = create_node(name, 1);
    f->size = size;

    add_child(current, f);

    printf(GREEN "File created\n" RESET);
}

// ls command
void ls_cmd() {
    for (int i = 0; i < current->child_count; i++) {
        Node* n = current->children[i];

        if (n->isFile)
            printf(GREEN "[file] %s (%d)\n" RESET, n->name, n->size);
        else
            printf(BLUE "[dir ] %s\n" RESET, n->name);
    }
}

// cd command
void cd_cmd(char* name) {
    if (!strcmp(name, "/")) {
        current = root;
        return;
    }

    if (!strcmp(name, "..")) {
        if (current->parent) current = current->parent;
        return;
    }

    Node* t = find_child(current, name);

    if (!t || t->isFile) {
        printf(RED "cd: not found\n" RESET);
        return;
    }

    current = t;
}

// pwd command
void pwd_cmd() {
    Node* temp = current;
    char path[1000] = "";

    while (temp && temp != root) {
        char buffer[1000];
        snprintf(buffer, sizeof(buffer), "/%s%s", temp->name, path);
        strcpy(path, buffer);
        temp = temp->parent;
    }

    printf("%s\n", path[0] ? path : "/");
}

// tree command
void tree_cmd(Node* node, char* prefix, int isLast) {
    printf("%s", prefix);
    printf(isLast ? "└── " : "├── ");

    if (node->isFile)
        printf(GREEN "%s (%d)\n" RESET, node->name, node->size);
    else
        printf(BLUE "%s\n" RESET, node->name);

    char newPrefix[1000];
    snprintf(newPrefix, sizeof(newPrefix), "%s%s", prefix, isLast ? "    " : "│   ");

    for (int i = 0; i < node->child_count; i++) {
        tree_cmd(node->children[i], newPrefix, i == node->child_count - 1);
    }
}

// find command
void find_cmd(Node* node, char* target) {
    if (strcmp(node->name, target) == 0)
        printf(GREEN "Found: %s\n" RESET, node->name);

    for (int i = 0; i < node->child_count; i++)
        find_cmd(node->children[i], target);
}

// rm command

void delete_tree(Node* node) {
    for (int i = 0; i < node->child_count; i++)
        delete_tree(node->children[i]);

    free(node);
}

void rm_cmd(char* name) {
    Node* t = find_child(current, name);

    if (!t) {
        printf(RED "rm: not found\n" RESET);
        return;
    }

    remove_child(current, t);
    delete_tree(t);

    printf(GREEN "removed\n" RESET);
}

// du command
int du_cmd(Node* node) {
    int total = node->size;

    for (int i = 0; i < node->child_count; i++)
        total += du_cmd(node->children[i]);

    return total;
}

// copy command 
Node* copy_node(Node* node) {
    Node* new_node = create_node(node->name, node->isFile);
    new_node->size = node->size;

    for (int i = 0; i < node->child_count; i++) {
        Node* child_copy = copy_node(node->children[i]);
        add_child(new_node, child_copy);
    }

    return new_node;
}

void cp_cmd(char* name, char* dest) {
    Node* node = find_child(current, name);
    if (!node) {
        printf(RED "cp: source not found\n" RESET);
        return;
    }

    Node* target = find_child(current, dest);
    if (!target || target->isFile) {
        printf(RED "cp: invalid destination\n" RESET);
        return;
    }

    Node* copy = copy_node(node);
    add_child(target, copy);

    printf(GREEN "Copied %s -> %s\n" RESET, name, dest);
}

// mv command
void mv_cmd(char* name, char* dest) {
    Node* node = find_child(current, name);
    if (!node) {
        printf(RED "mv: source not found\n" RESET);
        return;
    }

    Node* target = find_child(current, dest);
    if (!target || target->isFile) {
        printf(RED "mv: invalid destination\n" RESET);
        return;
    }

    remove_child(current, node);
    add_child(target, node);

    printf(GREEN "Moved %s -> %s\n" RESET, name, dest);
}

// help command
void help_cmd() {
    printf(YELLOW "\nCommands:\n" RESET);
    printf(" mkdir <name>\n");
    printf(" touch <name> <size>\n");
    printf(" ls\n cd\n pwd\n tree\n find\n rm\n du\n mv\n cp\n exit\n");
}


// save system

void save_fs(Node* node, FILE* f, int depth) {
    if (!node) return;

    fprintf(f, "%d %s %d %d\n",
        depth,
        node->name,
        node->isFile,
        node->size
    );

    for (int i = 0; i < node->child_count; i++)
        save_fs(node->children[i], f, depth + 1);
}


// load system

Node* node_stack[100];
int depth_stack[100];
int top = -1;

Node* load_fs(FILE* f) {
    char name[100];
    int depth, isFile, size;

    while (fscanf(f, "%d %s %d %d", &depth, name, &isFile, &size) != EOF) {

        Node* node = create_node(name, isFile);
        node->size = size;

        if (depth == 0) {
            root = node;
            current = root;
            node_stack[0] = node;
            depth_stack[0] = 0;
            top = 0;
            continue;
        }

        while (top >= 0 && depth_stack[top] >= depth)
            top--;

        Node* parent = node_stack[top];

        add_child(parent, node);

        node_stack[++top] = node;
        depth_stack[top] = depth;
    }

    return root;
}