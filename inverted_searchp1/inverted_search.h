#ifndef INVERTED_SEARCH_H
#define INVERTED_SEARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

/* ═══════════════════════════════════════════════════════════════
   DATA STRUCTURES
   ═══════════════════════════════════════════════════════════════ */

/* One node per file that contains a given word */
typedef struct FileNode {
    char            filename[256];
    int             count;          /* occurrences of word in this file */
    struct FileNode *next;
} FileNode;

/*
 * One node per unique word.
 * Used both as:
 *   - a node in the hash table's chaining linked list (next = chain)
 *   - a node in the sorted linked list fallback
 */
typedef struct WordNode {
    char             word[256];
    int              file_count;
    FileNode        *file_list;
    struct WordNode *next;          /* chain in hash bucket OR sorted list */
} WordNode;

/* ── Hash Table ──────────────────────────────────────────────── */
#define HASH_SIZE  1024            /* number of buckets (power of 2) */

typedef struct {
    WordNode *buckets[HASH_SIZE];  /* array of bucket heads */
    int       word_count;
} HashTable;

/* Node in the file-list (list of files to be indexed) */
typedef struct FileListNode {
    char               filename[256];
    struct FileListNode *next;
} FileListNode;

/* ═══════════════════════════════════════════════════════════════
   FUNCTION PROTOTYPES
   ═══════════════════════════════════════════════════════════════ */

/* Hash table */
HashTable  *hash_create(void);
void        hash_free(HashTable *ht);
WordNode   *hash_search(HashTable *ht, const char *word);
WordNode   *hash_insert(HashTable *ht, const char *word,
                        const char *filename);
void        hash_display(HashTable *ht);
void        hash_write(HashTable *ht, const char *dbfile);
HashTable  *hash_load(const char *dbfile);

/* File-list */
int         insert_file(FileListNode **head, const char *filename);
void        display_file_list(FileListNode *head);
void        free_file_list(FileListNode **head);

/* Indexing */
void        create_database(FileListNode *file_list, HashTable *ht);
void        read_datafile(FILE *fp, HashTable *ht, const char *filename);

/* Search */
void        search_word(const char *word, HashTable *ht);

/* Utility */
void        to_lower(char *str);
int         is_stopword(const char *word);
unsigned int hash_fn(const char *word);

#endif /* INVERTED_SEARCH_H */