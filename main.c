#include <stdio.h>
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

    int signedIn = false;

    while (!signedIn) {
        printf("Sign in to access your account.\n");
        printf("username: ");
        char username[BUFFER_SIZE];
        scanf("%s", username);
        printf("password: ");
        char password[BUFFER_SIZE];
        scanf("%s", password);

        printf("\n");

        int result = Users.signIn(username, password);

        switch (result) {
            case 200:
                printf("Sign in successful!\n\n");
                signedIn = true;
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

    printf("Bye!\n");

    return 0;
}
