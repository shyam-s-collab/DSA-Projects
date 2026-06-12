#include "inverted_search.h"

/* Parse one file character-by-character, insert each word into hash table */
void read_datafile(FILE *fp, HashTable *ht, const char *filename) {
    char word[256];
    int  wlen = 0, ch;

    while ((ch = fgetc(fp)) != EOF) {
        if (isalpha(ch)) {
            if (wlen < 255)
                word[wlen++] = (char)tolower((unsigned char)ch);
        } else {
            if (wlen > 0) {
                word[wlen] = '\0';
                hash_insert(ht, word, filename);
                wlen = 0;
            }
        }
    }
    if (wlen > 0) {                /* flush last word */
        word[wlen] = '\0';
        hash_insert(ht, word, filename);
    }
}

/* Iterate file_list, open each file, index into hash table */
void create_database(FileListNode *file_list, HashTable *ht) {
    if (!file_list) {
        printf("  [ERROR] No files in list. Add files first (option 3).\n");
        return;
    }
    for (FileListNode *fl = file_list; fl; fl = fl->next) {
        FILE *fp = fopen(fl->filename, "r");
        if (!fp) {
            printf("  [WARNING] Cannot open '%s', skipping.\n", fl->filename);
            continue;
        }
        read_datafile(fp, ht, fl->filename);
        fclose(fp);
        printf("  Successful: Creation of DATABASE for file: %s\n",
               fl->filename);
    }
}