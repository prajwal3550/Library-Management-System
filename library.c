#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DATA_FILE "library_records.txt"
#define TITLE_LEN 100
#define AUTHOR_LEN 100
#define ISBN_LEN 30
#define LINE_LEN 512

typedef struct {
    int id;
    char title[TITLE_LEN];
    char author[AUTHOR_LEN];
    char isbn[ISBN_LEN];
    int available;
} Book;

static void trim_newline(char *s);
static char *str_tolower_dup(const char *s);
static int prompt_int(const char *msg);
static void prompt_string(const char *msg, char *buf, int buflen);

int loadBooks(Book **out_books);
void saveAllBooks(Book *books, int count);
int getNextId(Book *books, int count);
void appendBookToFile(const Book *b);

void addBook();
void displayBooks();
void searchBook();
void updateBook();
void deleteBook();

int main(void) {
    int choice;
    while (1) {
        printf("\n====== Library Book Management System ======\n");
        printf("1. Add New Book\n");
        printf("2. Search Book (by title/author)\n");
        printf("3. Update Book Record (by ID)\n");
        printf("4. Delete Book Record (by ID)\n");
        printf("5. Display All Books\n");
        printf("6. Exit\n");
        printf("============================================\n");
        choice = prompt_int("Enter your choice: ");

        switch (choice) {
            case 1: addBook(); break;
            case 2: searchBook(); break;
            case 3: updateBook(); break;
            case 4: deleteBook(); break;
            case 5: displayBooks(); break;
            case 6:
                printf("Exiting. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
    return 0;
}

static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n == 0) return;
    if (s[n-1] == '\n') s[n-1] = '\0';
    if (n >= 2 && s[n-2] == '\r') s[n-2] = '\0';
}

static char *str_tolower_dup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *d = malloc(n + 1);
    if (!d) return NULL;
    for (size_t i = 0; i < n; ++i) d[i] = (char)tolower((unsigned char)s[i]);
    d[n] = '\0';
    return d;
}

static int prompt_int(const char *msg) {
    char buf[64];
    long val;
    while (1) {
        printf("%s", msg);
        if (!fgets(buf, sizeof buf, stdin)) return 0;
        trim_newline(buf);
        if (sscanf(buf, "%ld", &val) == 1) return (int)val;
        printf("Please enter a valid integer.\n");
    }
}

static void prompt_string(const char *msg, char *buf, int buflen) {
    printf("%s", msg);
    if (fgets(buf, buflen, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    trim_newline(buf);
}

int loadBooks(Book **out_books) {
    FILE *f = fopen(DATA_FILE, "r");
    if (!f) {
        *out_books = NULL;
        return 0;
    }

    Book *arr = NULL;
    int cap = 0, count = 0;
    char line[LINE_LEN];

    while (fgets(line, sizeof line, f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;

        char *p = line;
        char *token;
        int field = 0;
        Book b = {0};

        token = strtok(p, "|");
        while (token) {
            switch (field) {
                case 0: b.id = atoi(token); break;
                case 1: strncpy(b.title, token, TITLE_LEN-1); b.title[TITLE_LEN-1] = '\0'; break;
                case 2: strncpy(b.author, token, AUTHOR_LEN-1); b.author[AUTHOR_LEN-1] = '\0'; break;
                case 3: strncpy(b.isbn, token, ISBN_LEN-1); b.isbn[ISBN_LEN-1] = '\0'; break;
                case 4: b.available = atoi(token); break;
                default: break;
            }
            field++;
            token = strtok(NULL, "|");
        }

        if (count >= cap) {
            int newcap = (cap == 0) ? 8 : cap * 2;
            Book *tmp = realloc(arr, newcap * sizeof(Book));
            if (!tmp) {
                perror("Memory allocation failed");
                free(arr);
                fclose(f);
                *out_books = NULL;
                return 0;
            }
            arr = tmp;
            cap = newcap;
        }
        arr[count++] = b;
    }

    fclose(f);
    *out_books = arr;
    return count;
}

void saveAllBooks(Book *books, int count) {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) {
        perror("Unable to open file for writing");
        return;
    }
    for (int i = 0; i < count; ++i) {
        fprintf(f, "%d|%s|%s|%s|%d\n",
                books[i].id,
                books[i].title,
                books[i].author,
                books[i].isbn,
                books[i].available);
    }
    fclose(f);
}

int getNextId(Book *books, int count) {
    int max = 0;
    for (int i = 0; i < count; ++i) if (books[i].id > max) max = books[i].id;
    return max + 1;
}

void appendBookToFile(const Book *b) {
    FILE *f = fopen(DATA_FILE, "a");
    if (!f) {
        perror("Unable to open file to append");
        return;
    }
    fprintf(f, "%d|%s|%s|%s|%d\n", b->id, b->title, b->author, b->isbn, b->available);
    fclose(f);
}

void addBook() {
    Book *books = NULL;
    int count = loadBooks(&books);
    int id = getNextId(books, count);

    Book b;
    b.id = id;
    prompt_string("Enter title: ", b.title, TITLE_LEN);
    prompt_string("Enter author: ", b.author, AUTHOR_LEN);
    prompt_string("Enter ISBN: ", b.isbn, ISBN_LEN);
    int avail = prompt_int("Is the book available? (1 = yes, 0 = borrowed): ");
    b.available = (avail == 0) ? 0 : 1;

    appendBookToFile(&b);
    printf("Book added with ID %d.\n", b.id);

    free(books);
}

void displayBooks() {
    Book *books = NULL;
    int count = loadBooks(&books);
    if (count == 0) {
        printf("No books found.\n");
        free(books);
        return;
    }

    printf("\n%-5s | %-30s | %-20s | %-15s | %-10s\n", "ID", "Title", "Author", "ISBN", "Status");
    printf("-------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < count; ++i) {
        printf("%-5d | %-30.30s | %-20.20s | %-15.15s | %-10s\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].isbn,
               books[i].available ? "Available" : "Borrowed");
    }

    free(books);
}

void searchBook() {
    char keyword[128];
    prompt_string("Enter keyword to search in title or author: ", keyword, sizeof keyword);
    if (strlen(keyword) == 0) {
        printf("Empty keyword. Aborting search.\n");
        return;
    }

    Book *books = NULL;
    int count = loadBooks(&books);
    if (count == 0) {
        printf("No books to search.\n");
        free(books);
        return;
    }

    char *kwlow = str_tolower_dup(keyword);
    if (!kwlow) {
        printf("Memory error.\n");
        free(books);
        return;
    }

    int found = 0;
    for (int i = 0; i < count; ++i) {
        char *tl = str_tolower_dup(books[i].title);
        char *al = str_tolower_dup(books[i].author);
        if (!tl || !al) { free(tl); free(al); continue; }

        if (strstr(tl, kwlow) || strstr(al, kwlow)) {
            if (!found) {
                printf("\nMatches:\n");
                printf("%-5s | %-30s | %-20s | %-15s | %-10s\n", "ID", "Title", "Author", "ISBN", "Status");
                printf("--------------------------------------------------------------------------------\n");
            }
            printf("%-5d | %-30.30s | %-20.20s | %-15.15s | %-10s\n",
                   books[i].id,
                   books[i].title,
                   books[i].author,
                   books[i].isbn,
                   books[i].available ? "Available" : "Borrowed");
            found++;
        }
        free(tl); free(al);
    }

    if (!found) printf("No matches found for \"%s\".\n", keyword);

    free(kwlow);
    free(books);
}

void updateBook() {
    Book *books = NULL;
    int count = loadBooks(&books);
    if (count == 0) {
        printf("No records to update.\n");
        free(books);
        return;
    }

    int id = prompt_int("Enter the ID of the book to update: ");
    int idx = -1;
    for (int i = 0; i < count; ++i) if (books[i].id == id) { idx = i; break; }
    if (idx == -1) {
        printf("Book with ID %d not found.\n", id);
        free(books);
        return;
    }

    printf("Current details:\n");
    printf("Title : %s\n", books[idx].title);
    printf("Author: %s\n", books[idx].author);
    printf("ISBN  : %s\n", books[idx].isbn);
    printf("Status: %s\n", books[idx].available ? "Available" : "Borrowed");

    char choice[8];
    prompt_string("Update title? (y/n): ", choice, sizeof choice);
    if (choice[0] == 'y' || choice[0] == 'Y') prompt_string("New title: ", books[idx].title, TITLE_LEN);

    prompt_string("Update author? (y/n): ", choice, sizeof choice);
    if (choice[0] == 'y' || choice[0] == 'Y') prompt_string("New author: ", books[idx].author, AUTHOR_LEN);

    prompt_string("Update ISBN? (y/n): ", choice, sizeof choice);
    if (choice[0] == 'y' || choice[0] == 'Y') prompt_string("New ISBN: ", books[idx].isbn, ISBN_LEN);

    prompt_string("Change availability status? (y/n): ", choice, sizeof choice);
    if (choice[0] == 'y' || choice[0] == 'Y') {
        int avail = prompt_int("Is the book available? (1 = yes, 0 = borrowed): ");
        books[idx].available = (avail == 0) ? 0 : 1;
    }

    saveAllBooks(books, count);
    printf("Book with ID %d updated.\n", id);

    free(books);
}

void deleteBook() {
    Book *books = NULL;
    int count = loadBooks(&books);
    if (count == 0) {
        printf("No records to delete.\n");
        free(books);
        return;
    }

    int id = prompt_int("Enter the ID of the book to delete: ");
    int idx = -1;
    for (int i = 0; i < count; ++i) if (books[i].id == id) { idx = i; break; }
    if (idx == -1) {
        printf("Book with ID %d not found.\n", id);
        free(books);
        return;
    }

    printf("Are you sure you want to delete the book '%s' by %s? (y/n): ", books[idx].title, books[idx].author);
    char confirm[8];
    if (!fgets(confirm, sizeof confirm, stdin)) { free(books); return; }
    trim_newline(confirm);
    if (!(confirm[0] == 'y' || confirm[0] == 'Y')) {
        printf("Delete canceled.\n");
        free(books);
        return;
    }

    for (int i = idx; i < count - 1; ++i) books[i] = books[i+1];
    count--;

    saveAllBooks(books, count);
    printf("Book with ID %d deleted.\n", id);

    free(books);
}
