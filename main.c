#include <stdio.h>
#include <string.h>
#include "entities.h"

int main() {
    // initialize dao (data access object) entities
    struct equipment Equipment = EquipmentEntity();
    struct meetings Meetings = MeetingsEntity();
    struct users Users = UsersEntity();

    Equipment.load();
    Users.load();
    Meetings.load();

    printf("Welcome to the FabLab!\n\n");

    // sign in

    bool signedIn = false; // user id
    struct User* user;

    while (!signedIn) {
        printf("Sign in to access your account.\n");
        printf("username: ");
        char username[BUFFER_SIZE];
        scanf("%s", username);
        printf("password: ");
        char password[BUFFER_SIZE];
        scanf("%s", password);

        printf("\n");

        struct Response* response = Users.signIn(username, password);

        switch (response->status) {
            case 200:
                signedIn = true;
                user = response->user;
                printf("Welcome, %s!\n\n", user->firstName);
                break;
            case 404:
                printf("User does not exist, try again!\n\n");
                break;
            case 401:
                printf("Wrong password!\n\n");
                break;
            default:
                printf("Unexpected status from sign in, exiting\n\n");
                return -1;
        }
    }

    // print menu

    while(1) {
        printf("Available commands:\n");
        printf("- exit\n");
        if (user->isAdmin) {
            printf("- list-users\n");
            printf("- list-equipment\n");
            printf("- list-meetings\n");
            printf("- add-user\n");
            printf("- add-equipment\n");
            printf("- add-meeting\n");
        }
        printf("\n");

        printf("command: ");
        char command[BUFFER_SIZE];
        scanf("%s", command);
        printf("\n");

        if (strcmp(command, "exit") == 0) {
            printf("Bye!\n");
            return 0;
        } else if (strcmp(command, "list-users") == 0) {
            Users.listWithFullDetails();
        } else if (strcmp(command, "list-equipment") == 0) {
            Equipment.listWithFullDetails();
        } else if (strcmp(command, "list-meetings") == 0) {
            Meetings.listWithFullDetails();
        }
    }
}
