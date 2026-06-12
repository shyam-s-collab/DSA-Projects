/*
   name:Shyam S Suryavanshi
   date:24/04/2026
   project:Inverted search
   Description:The Inverted Search project is designed to perform fast word searching across
               multiple text files by using a Hash Table-based Inverted Index. Instead of searching 
               every file line by line each time the user enters a word, the system first creates a 
               database that stores each unique word along with the file names in which it appears 
               and the number of times it is repeated.
*/
#include "inverted_search.h"

#define DEFAULT_DB  "database.txt"
#define CLEAR_INPUT() do { int _c; while((_c=getchar())!='\n'&&_c!=EOF); } while(0)

static void print_banner(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════╗\n");
    printf("  ║     INVERTED SEARCH ENGINE  v2.0 [Hashing]     ║\n");
    printf("  ╚══════════════════════════════════════════════════╝\n");
    printf("  Index structure : Hash Table (djb2, %d buckets,\n", HASH_SIZE);
    printf("                    separate chaining) — O(1) avg lookup\n");
}

static void print_menu(void) {
    printf("\n  Select your choice among following options:\n");
    printf("    1. Create DATABASE\n");
    printf("    2. Display Database\n");
    printf("    3. Add Files\n");
    printf("    4. Search\n");
    printf("    5. Save Database\n");
    printf("    6. Exit\n");
    printf("  Enter your choice: ");
}

int main(int argc, char *argv[]) {
    FileListNode *file_list = NULL;
    char          dbfile[256];

    if (argc >= 2) strncpy(dbfile, argv[1], 255);
    else           strncpy(dbfile, DEFAULT_DB, 255);
    dbfile[255] = '\0';

    print_banner();

    /* Load or create hash table */
    HashTable *ht = hash_load(dbfile);

    int running = 1;
    while (running) {
        print_menu();

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            CLEAR_INPUT();
            printf("  [ERROR] Invalid input. Enter a number 1-6.\n");
            continue;
        }
        CLEAR_INPUT();

        switch (choice) {

        case 1:  /* Create DATABASE */
            printf("\n  ── Create DATABASE ──────────────────────────────\n");
            if (!file_list) {
                printf("  No files added yet. Use option 3 first.\n");
                break;
            }
            hash_free(ht);
            ht = hash_create();
            create_database(file_list, ht);
            printf("  DATABASE creation complete. (%d unique words indexed)\n",
                   ht->word_count);
            break;

        case 2:  /* Display */
            printf("\n  ── Display Database ─────────────────────────────\n");
            hash_display(ht);
            break;

        case 3: {  /* Add Files */
            printf("\n  ── Add Files ────────────────────────────────────\n");
            char fname[256], again = 'y';
            while (tolower(again) == 'y') {
                printf("  Enter file name to add: ");
                if (scanf("%255s", fname) != 1) { CLEAR_INPUT(); break; }
                CLEAR_INPUT();
                int r = insert_file(&file_list, fname);
                if (r == 1) {
                    /* Index immediately into hash table */
                    FILE *fp = fopen(fname, "r");
                    if (fp) {
                        read_datafile(fp, ht, fname);
                        fclose(fp);
                        printf("  '%s' indexed. Total unique words: %d\n",
                               fname, ht->word_count);
                    }
                }
                printf("  Add another file? (y/n): ");
                scanf(" %c", &again); CLEAR_INPUT();
            }
            break;
        }

        case 4: {  /* Search */
            printf("\n  ── Search ───────────────────────────────────────\n");
            if (ht->word_count == 0) {
                printf("  Database is empty. Add and index files first.\n");
                break;
            }
            char query[256], again = 'y';
            while (tolower(again) == 'y') {
                printf("  Enter the word you want to search: ");
                if (scanf("%255s", query) != 1) { CLEAR_INPUT(); break; }
                CLEAR_INPUT();
                search_word(query, ht);
                printf("  Search another word? (y/n): ");
                scanf(" %c", &again); CLEAR_INPUT();
            }
            break;
        }

        case 5: {  /* Save */
            printf("\n  ── Save Database ────────────────────────────────\n");
            if (ht->word_count == 0) {
                printf("  Nothing to save.\n"); break;
            }
            char savefile[256];
            printf("  Enter file name to save [%s]: ", dbfile);
            if (fgets(savefile, sizeof(savefile), stdin)) {
                savefile[strcspn(savefile, "\n")] = '\0';
                if (strlen(savefile) == 0)
                    strncpy(savefile, dbfile, 255);
            } else {
                strncpy(savefile, dbfile, 255);
            }
            hash_write(ht, savefile);
            break;
        }

        case 6:  /* Exit */
            printf("\n  Saving database before exit...\n");
            if (ht->word_count > 0) hash_write(ht, dbfile);
            printf("  Goodbye!\n\n");
            running = 0;
            break;

        default:
            printf("  [ERROR] Invalid choice. Enter 1-6.\n");
        }
    }

    hash_free(ht);
    free_file_list(&file_list);
    return 0;
}