#include "inverted_search.h"
#include <unistd.h>

int insert_file(FileListNode **head, const char *filename) {
    /* 1. Try to open the file exactly as the user typed it */
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        char cwd[512];
        printf("  [ERROR] Cannot open '%s'.\n", filename);
        if (getcwd(cwd, sizeof(cwd)))
            printf("          (program is running from: %s)\n"
                   "          Make sure the file is in that folder.\n", cwd);
        return 0;
    }

    /* 2. Check non-empty */
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fclose(fp);
    if (sz == 0) {
        printf("  [ERROR] File '%s' is empty.\n", filename);
        return 0;
    }

    /* 3. Check for duplicates */
    FileListNode *cur = *head;
    while (cur) {
        if (strcmp(cur->filename, filename) == 0) {
            printf("  [WARNING] File '%s' is already in the list.\n", filename);
            return -1;
        }
        cur = cur->next;
    }

    /* 4. Insert at tail */
    FileListNode *node = (FileListNode *)malloc(sizeof(FileListNode));
    if (!node) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(node->filename, filename, 255);
    node->filename[255] = '\0';
    node->next = NULL;

    if (!*head) {
        *head = node;
    } else {
        cur = *head;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
    printf("  Successful: inserting file name '%s' into file linked list.\n", filename);
    return 1;
}

void display_file_list(FileListNode *head) {
    if (!head) { printf("  (no files loaded)\n"); return; }
    int i = 1;
    for (FileListNode *cur = head; cur; cur = cur->next, i++)
        printf("  %d. %s\n", i, cur->filename);
}

void free_file_list(FileListNode **head) {
    FileListNode *cur = *head, *nxt;
    while (cur) { nxt = cur->next; free(cur); cur = nxt; }
    *head = NULL;
}