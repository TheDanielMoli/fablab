#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "users.h"

FILE *primaryIndexFile;
FILE *usernameIndexFile;
FILE *file;

static int primaryIndex[SIZE];
static char usernameIndex[BUFFER_SIZE][SIZE];
static int maxPrimaryIndex;
static int maxId;

// for internal use, within the users library
static void writePrimaryIndex() {
    primaryIndexFile = fopen(USERS_PRIMARY_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(primaryIndexFile, "%d\n", primaryIndex[i]);
    }
    fprintf(primaryIndexFile, "%d", primaryIndex[maxPrimaryIndex]);
    fclose(primaryIndexFile);
}

// for internal use, within the users library
static void writeNameIndex() {
    usernameIndexFile = fopen(USERS_USERNAME_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(usernameIndexFile, "%s\n", usernameIndex[i]);
    }
    fprintf(usernameIndexFile, "%s", usernameIndex[maxPrimaryIndex]);
    fclose(usernameIndexFile);
}

// for internal use, within the users library
static void writeRecord(struct User* user) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, USERS_DATA_PATH, user->id);
    file = fopen(buffer, "w+");

    fprintf(
            file,
            "%s\n%s\n%s\n%s\n%d\n%f\n%f\n%d\n%f",
            user->username,
            user->password,
            user->firstName,
            user->lastName,
            user->isAdmin,
            user->credits,
            user->nextMonthPayment,
            user->active,
            user->bookedCredits
            );

    fclose(file);
}

// for internal use, within the users library
static int findIndex(int id) {
    int index = -1;

    for(int i = 0; i <= maxPrimaryIndex; i++){
        if (primaryIndex[i] == id) {
            index = i;
            break;
        }
    }

    return index;
}

extern struct User* checkUser(int id) {
    struct User *user = (struct User*) malloc(sizeof(struct User));

    bool found = false;
    for (int i = 0; i <= maxPrimaryIndex; i++) {
        if (id == primaryIndex[i]) {
            found = true;
        }
    }
    if (!found) {
        user->id = -1;
        return user;
    }

    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, USERS_DATA_PATH, id);
    file = fopen(buffer, "r");

    user->id = id;

    fgets(user->username, BUFFER_SIZE, (FILE*)file);
    user->username[strcspn(user->username, "\n")] = 0;

    fgets(user->password, BUFFER_SIZE, (FILE*)file);
    user->password[strcspn(user->password, "\n")] = 0;

    fgets(user->firstName, BUFFER_SIZE, (FILE*)file);
    user->firstName[strcspn(user->firstName, "\n")] = 0;

    fgets(user->lastName, BUFFER_SIZE, (FILE*)file);
    user->lastName[strcspn(user->lastName, "\n")] = 0;

    int isAdmin;
    fscanf(file, "%d", &isAdmin);
    if (isAdmin == 1) {
        user->isAdmin = true;
    } else {
        user->isAdmin = false;
    }

    fscanf(file, "%f", &user->credits);
    fscanf(file, "%f", &user->nextMonthPayment);

    int available;
    fscanf(file, "%d", &available);
    if (available == 1) {
        user->active = true;
    } else {
        user->active = false;
    }

    fscanf(file, "%f", &user->bookedCredits);

    fclose(file);

    return user;
}

extern int addUser(
        char username[BUFFER_SIZE],
        char firstName[BUFFER_SIZE],
        char lastName[BUFFER_SIZE],
        char password[BUFFER_SIZE],
        bool isAdmin
) {
    maxId = maxId + 1;
    maxPrimaryIndex = maxPrimaryIndex + 1;
    primaryIndex[maxPrimaryIndex] = maxId;
    strcpy(usernameIndex[maxPrimaryIndex], username);

    struct User* user = (struct User*) malloc(sizeof(struct User));

    user->id = maxId;
    strcpy(user->username, username);
    strcpy(user->password, password);
    strcpy(user->firstName, firstName);
    strcpy(user->lastName, lastName);
    user->isAdmin = isAdmin;
    user->credits = MONTHLY_CREDITS;
    user->nextMonthPayment = MONTHLY_PRICE;
    user->active = 1;
    user->bookedCredits = 0;

    writeRecord(user);

    fclose(file);

    writePrimaryIndex();
    writeNameIndex();

    return maxPrimaryIndex;
}

extern bool removeUser(int id) {
    int index = findIndex(id);

    if (index != -1) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, USERS_DATA_PATH, id);
        remove(buffer);

        for(int i = index; i <= maxPrimaryIndex; i++){
            primaryIndex[i] = primaryIndex[i + 1];
            strcpy(usernameIndex[i], usernameIndex[i + 1]);
        }
        maxPrimaryIndex = maxPrimaryIndex - 1;

        writePrimaryIndex();
        writeNameIndex();

        return true;
    } else {
        return false;
    }
}

extern void addCredits(int id, float credits) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->credits = user->credits + credits;
        writeRecord(user);
    }
}

extern void bookCredits(int id, float credits) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->bookedCredits = user->bookedCredits + credits;
        writeRecord(user);
    }
}

extern void removeCredits(int id, float credits) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->bookedCredits = user->bookedCredits - credits;
        user->credits = user->credits - credits;
        writeRecord(user);
    }
}

extern void displayUsers() {
    for(int i = 0; i <= maxPrimaryIndex; i++) {
        printf(
                "%d) %s\n",
                primaryIndex[i],
                usernameIndex[i]
                );
    }
    printf("\n");
}

extern void displayUsersDetailed() {
    for(int i = 0; i <= maxPrimaryIndex; i++) {
        struct User* user = checkUser(primaryIndex[i]);
        if (user-> id != -1) {
            printf(
                    "%d) %s %s (%s, %s): %s, %f credits, €%f due next month\n",
                    primaryIndex[i],
                    user->firstName,
                    user->lastName,
                    usernameIndex[i],
                    user->isAdmin ? "admin user" : "standard user",
                    user->active ? "active" : "inactive",
                    user->credits,
                    user->nextMonthPayment
            );
        }
    }
    printf("\n");
}

extern struct Response* signIn(char username[BUFFER_SIZE], char password[BUFFER_SIZE]) {
    bool found = false;
    bool ok = false;
    int id = -1;
    for (int i = 0; i <= maxPrimaryIndex; i++) {
        if (strcmp(username, usernameIndex[i]) == 0) {
            found = true;
            struct User* user = checkUser(primaryIndex[i]);
            if (user-> id != -1) {
                if (strcmp(password, user->password) == 0) {
                    ok = true;
                }
                id = user->id;
            }
            break;
        }
    }
    struct Response *response = (struct Response*) malloc(sizeof(struct User));
    if (!found) {
        response->status = 404;
        return response;
    }
    if (!ok) {
        response->status = 401;
        return response;
    }
    response->status = 200;
    response->user = checkUser(id);
    return response;
}

extern void disableUser(int id) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->active = false;
        writeRecord(user);
    }
}

extern void enableUser(int id) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->active = false;
        writeRecord(user);
    }
}

extern void renewUser(int id) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->credits = user->credits + MONTHLY_CREDITS;
        user->nextMonthPayment = MONTHLY_PRICE;
        writeRecord(user);
    }
}

extern void updateNextPayment(int id, float credits) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        user->credits = user->nextMonthPayment + credits;
        writeRecord(user);
    }
}

extern void displayBalance(int id) {
    struct User* user = checkUser(id);
    if (user-> id != -1) {
        printf("Your balance is %f, of which %f booked by current borrowings.\n", user->credits, user->bookedCredits);
        printf("Your next monthly payment will be %f.\n\n", user->nextMonthPayment);
    }
}

extern void loadUsers() {
    for (int i = 0; i < SIZE; i++) {
        primaryIndex[i] = -1;
    }

    primaryIndexFile = fopen(USERS_PRIMARY_INDEX_FILE, "r");
    usernameIndexFile = fopen(USERS_USERNAME_INDEX_FILE, "r");

    for (int i = 0; !feof(primaryIndexFile); i++) {
        fscanf(primaryIndexFile, "%d", &primaryIndex[i]);

        fgets(usernameIndex[i], BUFFER_SIZE, (FILE*)usernameIndexFile);
        usernameIndex[i][strcspn(usernameIndex[i], "\n")] = 0;

        maxId = primaryIndex[i];
        maxPrimaryIndex = i;
    }

    fclose(primaryIndexFile);
    fclose(usernameIndexFile);
}