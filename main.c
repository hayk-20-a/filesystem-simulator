#include "fs.h"

// prompt
void print_prompt() {
    printf("fs:/> ");
}

int main() {
    char cmd[100], arg1[100];
    int size;

    // root init
    root = create_node("/", 0);
    current = root;

    FILE* f = fopen(SAVE_FILE, "r");
    if (f != NULL) {
    load_fs(f);   // rebuilding tree
    fclose(f);
    }   
    
    // welcome
    printf("=====================================\n");
    printf("   Simple File System Simulator\n");
    printf("   Type 'help' for commands\n");
    printf("=====================================\n");

    while (1) {
        print_prompt();
        scanf("%s", cmd);

        if (!strcmp(cmd, "mkdir")) {
            scanf("%s", arg1);
            mkdir_cmd(arg1);
        }

        else if (!strcmp(cmd, "touch")) {
            scanf("%s %d", arg1, &size);
            touch_cmd(arg1, size);
        }

        else if (!strcmp(cmd, "ls")) ls_cmd();
        else if (!strcmp(cmd, "cd")) { scanf("%s", arg1); cd_cmd(arg1); }
        else if (!strcmp(cmd, "pwd")) pwd_cmd();

        else if (!strcmp(cmd, "tree")) tree_cmd(root, "", 1);
        else if (!strcmp(cmd, "find")) { scanf("%s", arg1); find_cmd(root, arg1); }

        else if (!strcmp(cmd, "rm")) { scanf("%s", arg1); rm_cmd(arg1); }

        else if (!strcmp(cmd, "du")) printf("Total: %d\n", du_cmd(root));

        else if (!strcmp(cmd, "help")) help_cmd();

        else if (!strcmp(cmd, "exit")) {
            FILE* f = fopen(SAVE_FILE, "w");
            save_fs(root, f, 0);
            fclose(f);

            printf("Filesystem saved. Goodbye.\n");
            break;
        }

        else {
            printf(RED "Unknown command\n" RESET);
        }
    }

    return 0;
}