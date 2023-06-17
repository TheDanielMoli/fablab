#ifndef FABLAB_USERS_H
#define FABLAB_USERS_H

#define SIZE 2048
#define BUFFER_SIZE 2048
#define MONTHLY_PRICE 50.0f
#define MONTHLY_CREDITS 50.0f
#define USERS_PRIMARY_INDEX_FILE "../database/users/primary-index"
#define USERS_USERNAME_INDEX_FILE "../database/users/username-index"
#define USERS_DATA_PATH "../database/users/data/%d"

#include <stdbool.h>
#include <time.h>

struct User {
    int id;
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char firstName[BUFFER_SIZE];
    char lastName[BUFFER_SIZE];
    bool isAdmin;
    float credits;
    float nextMonthPayment;
    bool active;
    float bookedCredits;
};

struct Response {
    int status;
    struct User* user;
};

extern void loadUsers();

extern int addUser(
        char username[BUFFER_SIZE],
        char firstName[BUFFER_SIZE],
        char lastName[BUFFER_SIZE],
        char password[BUFFER_SIZE],
        bool isAdmin
        );
extern bool removeUser(int id);
extern struct User* checkUser(int id);
extern struct Response* signIn(char username[BUFFER_SIZE], char password[BUFFER_SIZE]);
extern void addCredits(int id, float credits);
extern void bookCredits(int id, float credits);
extern void removeCredits(int id, float credits);
extern void disableUser(int id);
extern void enableUser(int id);
extern void renewUser(int id);
extern void updateNextPayment(int id, float credits);
extern void displayBalance(int id);
extern void displayUsers();
extern void displayUsersDetailed();

#endif //FABLAB_USERS_H
