#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "users.h"

FILE *primaryIndexFile;
FILE *usernameIndexFile;
FILE *file;

int primaryIndex[SIZE];
char usernameIndex[BUFFER_SIZE][SIZE];
int maxPrimaryIndex;
int maxId;

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
            "%s\n%s\n%s\n%s\n%d\n%f\n%f\n%d",
            user->username,
            user->password,
            user->firstName,
            user->lastName,
            user->isAdmin,
            user->credits,
            user->nextMonthPayment,
            user->active
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
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, USERS_DATA_PATH, id);
    file = fopen(buffer, "r");

    struct User *user = (struct User*) malloc(sizeof(struct User));

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
    user->credits = 100;
    user->nextMonthPayment = 50;
    user->active = 1;

    writeRecord(user);

    fclose(file);

    writePrimaryIndex();
    writeNameIndex();

    return maxPrimaryIndex;
}

extern void removeUser(int id) {
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
    }
}

extern void addCredits(int id, float credits) {
    struct User* user = checkUser(id);
    user->credits = user->credits + credits;
    writeRecord(user);
}

extern void removeCredits(int id, float credits) {
    struct User* user = checkUser(id);
    user->credits = user->credits - credits;
    writeRecord(user);
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
        printf(
                "%d) %s %s (%s): %s, %f credits, €%f due next month\n",
                primaryIndex[i],
                user->firstName,
                user->lastName,
                usernameIndex[i],
                user->active ? "active" : "inactive",
                user->credits,
                user->nextMonthPayment
        );
    }
    printf("\n");
}

extern int signIn(char username[BUFFER_SIZE], char password[BUFFER_SIZE]) {
    bool found = false;
    bool ok = false;
    for (int i = 0; i <= maxPrimaryIndex; i++) {
        if (strcmp(username, usernameIndex[i]) == 0) {
            found = true;
            struct User* user = checkUser(primaryIndex[i]);
            if (strcmp(password, user -> password) == 0) {
                ok = true;
            }
            break;
        }
    }
    if (!found) {
        return 404;
    }
    if (!ok) {
        return 401;
    }
    return 200;
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