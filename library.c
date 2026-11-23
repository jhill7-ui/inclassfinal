/*
    Ronin: Main Loop, switch cases, borrowBook, displayActiveLoans, displayStatistics
    Mau: switch cases, addBook, displayStatistics, returnBook, testing
    Jeremiah: displayBooks, findBookRecursive, countActiveLoansRecursive, sumAvailableCopiesRecursive
*/
#include <string.h>
#include <stdio.h>

#define MAX_BOOKS 30
#define MAX_LOANS 50

typedef struct {
    int id;
    char title[50];
    int totalCopies;
    int availableCopies;
    int reservedBy;
} Book;
typedef struct {
    int bookId;
    int borrowerId;
    int active;
} Loan;

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Adds a book in book with given id title and copies
*/
int addBook(Book books[], int *count, int id, const char title[], int copies) {
    // Check for exceeding MAX_BOOKS
    if (*count >= MAX_BOOKS) {
        return 0; 
    }
    // Check for invalid copy count
    if (copies <= 0) {
        return -2;
    }
    // Check for duplicate ID
    for (int i = 0; i < *count; i++) {
        if (books[i].id == id) {
            return -1; 
        }   
    }
    // Populate the new book struct at the current count index
    books[*count].id = id;
    strncpy(books[*count].title, title, sizeof(books[*count].title) - 1);
    books[*count].title[sizeof(books[*count].title) - 1] = '\0'; // Ensure null-termination
    books[*count].totalCopies = copies;
    books[*count].availableCopies = copies;
    books[*count].reservedBy = 0; // 0 indicates no active reservation
    (*count)++; // Increment the total book count
    return 1;
}

/*
displayBooks: prints ID, title, total copies, available copies, and
reservation status for every book. Does not modify anything.*/
void displayBooks(const Book books[], int bookCount)
{
    if (bookCount == 0) {
        printf("No books have been added yet.\n");
        return;
    }

    printf("\n%-6s %-30s %6s %10s  %s\n",
           "ID", "Title", "Total", "Available", "Reservation");
    printf("--------------------------------------------------------------------\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%-6d %-30s %6d %10d  ",
               books[i].id, books[i].title,
               books[i].totalCopies, books[i].availableCopies);
        if (books[i].reservedBy != 0) {
            printf("Reserved by %d\n", books[i].reservedBy);
        } else {
            printf("None\n");
        }
    }
}

/*Attempts to borrow a book from books using book id. 
Saves the loan at borrowerId.
Fails if active loan already exists or in no avaliible copies are left.
returns 1 for sucsess 0 for failure.
*/
int borrowBook(Book books[], int bookCount, Loan loans[], int *loanCount, int bookId, int borrowerId) {
    int exists = 0;
    int index;
    for (int i = 0; i < bookCount; i++) {
        if(books[i].id == bookId) {
            exists = 1;
            index = i;
            break;
        }
    }
    if (!exists) {
        printf("No such book exists in library!\n");
        return 0;
    }
    if (books[index].availableCopies <= 0) {
        printf("No copies left!\n");
        return 0;
    }
    for (int i = 0; i < *loanCount; i++) {
        if(loans[i].bookId == bookId && loans[i].borrowerId == borrowerId) {
            if (loans[i].active) {
                printf("User %d already has an active loan for this book!\n", borrowerId);
                return 0;
            } else {
                books[index].availableCopies--;
                loans[i].active = 1;
                return 1;
                
            }
        }
    }
    books[index].availableCopies--;
    Loan newLoan = {bookId, borrowerId, 1};
    loans[*loanCount] = newLoan;
    (*loanCount)++;
    return 1;
}

/* If an active loan is found with borrower id and book id the book is returned to books.
returns 1 for succsess and 0 for fail
*/
int returnBook(Book books[], int bookCount, Loan loans[], int loanCount, int bookId, int borrowerId) {
    // Reject invalid (non-positive) book or borrower IDs immediately
    if (bookId <= 0 || borrowerId <= 0) {
        return 0;
    }
    // Search the loans array for an active loan matching both book and borrower
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == bookId) {
            for (int j = 0; j < loanCount; j++) {
                if (loans[j].bookId == bookId && loans[j].borrowerId == borrowerId && loans[j].active == 1) {
                    loans[j].active = 0; // flag to inactive
                    books[i].availableCopies++;
                    return 1;
                }
            }
            // Book exists, but no matching active loan was found
            return 0;
        }
    }
    // Book ID was not found in the books array
    return 0;
}

/* Displays the active loan data
*/
void displayActiveLoans(const Loan loans[], int loanCount) {
    int foundbooks = 0;
    for(int i = 0; i < loanCount; i++) {
        if(loans[i].active) {
            printf("Book ID: %d Borrower ID: %d\n", loans[i].bookId, loans[i].borrowerId);
            foundbooks = 1;
        }
    }
    if(!foundbooks) {
        printf("No active Loans!\n");
    }
}

/* Displays:
Number Of Registered Books
Number of book copies across all registered books
Number of currently available copies
Number of active loans
Number of pending reservations: 0
*/
void displayStatistics(const Book books[], int bookCount, const Loan loans[], int loanCount) {
    printf("Number Of Registered Books: %d\n", bookCount);
    int copies = 0;
    int availCopies = 0;
    for(int i = 0; i < bookCount; i++) {
        copies += books[i].totalCopies;
        availCopies += books[i].availableCopies;
    }
    printf("Number of book copies across all registered books: %d\n", copies);
    printf("Number of currently available copies: %d\n", availCopies);
    int activeLoans = 0;
    for (int i = 0; i < loanCount; i++) {
        if (loans[i].active) {
            activeLoans++;
        }
    }
    printf("Number of active loans: %d\n", activeLoans);
    printf("Number of pending reservations: 0");
}

/*
 * findBookRecursive: searches books[index .. bookCount-1] for bookId.
 * Base case 1: index >= bookCount -> every book has been examined, return -1.
 *              (With an empty array, bookCount is 0, so the first call
 *              returns -1 immediately without touching the array.)
 * Base case 2: books[index].id == bookId -> found, return index.
 * Recursive step: search the rest of the array starting at index + 1.
 * Each call moves one element closer to bookCount, so the recursion ends.
 */
int findBookRecursive(const Book books[], int bookCount, int bookId, int index)
{
    if (index < 0 || index >= bookCount) {
        return -1;
    }
    if (books[index].id == bookId) {
        return index;
    }
    return findBookRecursive(books, bookCount, bookId, index + 1);
}

/*
 * countActiveLoansRecursive: counts active loans in loans[index .. loanCount-1].
 * Base case: index >= loanCount -> no records left, return 0.
 * Recursive step: this record contributes 1 if active, 0 if it was returned;
 * add that to the count for the remaining records (index + 1).
 * The active field must be checked because loanCount also includes
 * returned (inactive) records that stay in the array for reuse.
 */
int countActiveLoansRecursive(const Loan loans[],
                              int loanCount, int index)
{
    if (index < 0 || index >= loanCount) {
        return 0;
    }
    int current = loans[index].active ? 1 : 0;
    return current + countActiveLoansRecursive(loans, loanCount, index + 1);
}

/*
 * sumAvailableCopiesRecursive: totals availableCopies for
 * books[index .. bookCount-1].
 * Base case: index >= bookCount -> no books left, return 0.
 * Recursive step: this book's availableCopies plus the sum for the
 * remaining books (index + 1). The result must equal the "Available
 * copies" line of displayStatistics().
 */
int sumAvailableCopiesRecursive(const Book books[],
                                int bookCount, int index)
{
    if (index < 0 || index >= bookCount) {
        return 0;
    }
    return books[index].availableCopies +
           sumAvailableCopiesRecursive(books, bookCount, index + 1);
}

int main(void) {
    Book books[MAX_BOOKS];
    Loan loans[MAX_LOANS];
    int bookCount = 0;
    int loanCount = 0;
    int choice;
    char inputVerificiation;
    int keepLooping = 1;
    printf("===== LIBRARY MANAGEMENT SYSTEM =====\n1. Add a book\n2. Display all books\n3. Borrow a book\n4. Return a book\n5. Reserve a book\n6. Display active loans\n7. Display library statistics\n8. Search for a book (recursive)\n9. Display recursive summary\n0. Exit\n");
    do {
        printf("Enter your choice: ");
        if(scanf("%d%c", &choice, &inputVerificiation) != 2 || inputVerificiation != '\n') {
            printf("ERROR: Please enter an integer\n");
            clearInput();
        } else if (choice < 0 || choice > 9) {
            printf("ERROR: Invalid range, expected (0-9)\n");
        } else {
            switch (choice) {
                case 0:
                    keepLooping = 0;
                    break;
                case 1:
                    if (bookCount >= MAX_BOOKS) {
                        printf("ERROR: Library Full!\n");
                        break;
                    }
                    int id;
                    int copies;
                    char title[50];
                    printf("Enter Book ID: ");
                    if (scanf("%d", &id) != 1 || id <= 0) {
                        printf("ERROR: Invalid ID.\n");
                        clearInput();
                        break;
                    }
                    clearInput();
                    printf("Enter Book Title: ");
                    if (fgets(title, sizeof(title), stdin) != NULL) {
                        title[strcspn(title, "\n")] = '\0';
                    }
                    printf("Enter Number of Copies: ");
                    if (scanf("%d", &copies) != 1) {
                        printf("ERROR: Invalid copy count.\n");
                        clearInput();
                        break;
                    }
                    int result = addBook(books, &bookCount, id, title, copies);
                    if (result == 1) {
                        printf("Book '%s' added successfully!\n", title);
                    } else if (result == -1) {
                        printf("ERROR: A book with ID %d already exists.\n", id);
                    } else if (result == -2) {
                        printf("ERROR: Number of copies must be greater than 0.\n");
                    }
                    break;
                case 2:
                    displayBooks(books, bookCount);
                    break;
                case 3: {
                    if (loanCount >= MAX_LOANS) {
                        printf("Maximum loan records reached!\n");
                        return 0;
                    }
                    int bookId;
                    int borrowerId;
                    int validated = 0;
                    do {
                        printf("Enter Book Id: ");
                        if(scanf("%d%c", &bookId, &inputVerificiation) != 2 || inputVerificiation != '\n') {
                            printf("ERROR: Invalid ID.\n");
                            clearInput();
                        } else if (bookId <= 0) {
                            printf("ERROR: Invalid range, expected > 0\n");
                        } else {
                            validated = 1;
                        }
                    } while (!validated);
                    validated = 0;
                    do {
                        printf("Enter Borrower Id: ");
                        if(scanf("%d%c", &borrowerId, &inputVerificiation) != 2 || inputVerificiation != '\n') {
                            printf("ERROR: Invalid ID.\n");
                            clearInput();
                        } else if (borrowerId <= 0) {
                            printf("ERROR: Invalid range, expected > 0\n");
                        } else {
                            validated = 1;
                        }
                    } while (!validated);
                    if (borrowBook(books, bookCount, loans, &loanCount, bookId, borrowerId)) {
                        printf("Book Borrowed Sucsessfully!\n");
                    }
                    break;
                }
                case 4: {
                    int bookID;
                    int borrowerID;

                    printf("Enter Book ID: ");
                    if (scanf("%d", &bookID) != 1) {
                        printf("ERROR: Invalid ID.\n");
                        clearInput();
                        break;
                    }

                    printf("Enter borrower ID: ");
                    if (scanf("%d", &borrowerID) != 1) {
                        printf("Invalid borrower ID.\n");
                        clearInput();
                        break;
                    }

                    // Call returnBook() and check its return value (1 = success, 0 = failure)
                    if (returnBook(books, bookCount, loans, loanCount, bookID, borrowerID) == 1) {
                        printf("Book returned successfully.\n");
                    } else {
                        printf("Error: Could not return book (invalid ID, book not found, or no active loan).\n");
                    }
                    break;
                }
                case 5:
                    printf("Reservation Function Not Implemented\n");
                    break;
                case 6:
                    displayActiveLoans(loans, loanCount);
                    break;
                case 7:
                    displayStatistics(books, bookCount, loans, loanCount);
                    break;
                case 8: {
                    int bookId;
                    int validated = 0;
                    do {
                        printf("Enter Book Id: ");
                        if(scanf("%d%c", &bookId, &inputVerificiation) != 2 || inputVerificiation != '\n') {
                            printf("ERROR: Invalid ID.\n");
                            clearInput();
                        } else if (bookId <= 0) {
                            printf("ERROR: Invalid range, expected > 0\n");
                        } else {
                            validated = 1;
                        }
                    } while (!validated);
                    int result = findBookRecursive(books, bookCount, bookId, 0);
                    if (result == -1) {
                        printf("No book with ID '%d' found\n", bookId);
                    } else {
                        printf("Book with ID '%d' found at index '%d'\n", bookId, result);
                    }
                    break;
                }
                case 9: {
                    int activeLoans = countActiveLoansRecursive(loans, loanCount, 0);
                    int available = sumAvailableCopiesRecursive(books, bookCount, 0);
                    printf("\n===== Recursive Summary =====\n");
                    printf("Active loans:          %d\n", activeLoans);
                    printf("Available copies:      %d\n", available);
                    break;
                }
            }
        }
    } while (keepLooping);
}

