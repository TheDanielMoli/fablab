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
                printf("Welcome, %s!\n", user->firstName);
                if (user->isAdmin) {
                    printf("You have admin privileges.\n");
                } else {
                    printf("You are a standard user.\n");
                }
                printf("\n");
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

    printf("Use command \"help\" to get a list of all available commands\n\n");

    while(1) {
        printf("command: ");
        char command[BUFFER_SIZE];
        scanf("%s", command);
        printf("\n");

        if (strcmp(command, "exit") == 0) {
            printf("Bye!\n");
            return 0;
        } else if (strcmp(command, "help") == 0) {
            printf("Available commands:\n");
            printf("- exit\n");
            printf("- help\n");
            printf("- list-equipment\n");
            printf("- view-balance\n");
            printf("- borrow-equipment\n");
            printf("- return-equipment\n");
            printf("- list-meetings\n");

            if (user->isAdmin) {
                printf("- list-users\n");
                printf("- add-user\n");
                printf("- add-equipment\n");
                printf("- add-meeting\n");
                printf("- remove-user\n");
                printf("- remove-equipment\n");
                printf("- remove-meeting\n");
                printf("- disable-user\n");
                printf("- enable-user\n");
                printf("- renew-subscription\n");
            }

            printf("\n");
        } else if (strcmp(command, "list-equipment") == 0) {
            Equipment.listWithFullDetails();
        } else if (strcmp(command, "view-balance") == 0) {
            Users.displayBalance(user->id);
        } else if (strcmp(command, "borrow-equipment") == 0) {
            int id;
            printf("id of the equipment: ");
            scanf("%d", &id);

            int hours;
            printf("hours of the equipment: ");
            scanf("%d", &hours);

            struct Equipment* equipment = checkEquipment(id);

            if (equipment->available) {
                float credits = equipment->hourlyCredits * (float)hours;
                if (user->credits - user->bookedCredits > credits) {
                    Users.bookCredits(user->id, credits);
                    user = Users.check(user->id);
                    Equipment.borrow(id, hours, user->id);
                    printf("You just borrowed equipment %d!", id);
                } else {
                    printf("Insufficient credits for borrowing equipment %d!", id);
                }
            } else {
                printf("Equipment %d is not available!", id);
            }

            printf("\n\n");
        } else if (strcmp(command, "return-equipment") == 0) {
            int id;
            printf("id of the equipment: ");
            scanf("%d", &id);

            struct Equipment* equipment = checkEquipment(id);

            if (equipment->user == user->id && !equipment->available) {
                float credits = equipment->hourlyCredits * (float)equipment->hoursBooked;
                Users.removeCredits(user->id, credits);
                user = Users.check(user->id);
                Equipment.giveBack(id);
                printf("You just returned equipment %d and paid %f credits!", id, credits);
            } else if (equipment->available) {
                printf("Equipment %d is available, you haven't booked it!", id);
            } else {
                printf("Equipment %d has been booked by someone else!", id);
            }

            printf("\n\n");
        } else if (strcmp(command, "list-meetings") == 0) {
            Meetings.listWithFullDetails();
        } else if (user->isAdmin) {
            if (strcmp(command, "list-users") == 0) {
                Users.listWithFullDetails();
            } else if (strcmp(command, "add-user") == 0) {
                char username[BUFFER_SIZE];
                printf("username: ");
                scanf("%s", username);

                char firstName[BUFFER_SIZE];
                printf("first name: ");
                scanf("%s", firstName);

                char lastName[BUFFER_SIZE];
                printf("last name: ");
                scanf("%s", lastName);

                char password[BUFFER_SIZE];
                printf("password: ");
                scanf("%s", password);

                char adminCheck[BUFFER_SIZE];
                bool isAdmin = false;
                printf("is admin [y/n, yes/no]: ");
                scanf("%s", adminCheck);
                if (strcmp(adminCheck, "yes") == 0 || strcmp(adminCheck, "y") == 0) {
                    isAdmin = true;
                }

                Users.add(username, firstName, lastName, password, isAdmin);

                printf("User %s %s (%s) added!\n\n", firstName, lastName, username);
            } else if (strcmp(command, "add-equipment") == 0) {
                char name[BUFFER_SIZE];
                printf("name: ");
                scanf("%s", name);

                float hourlyCredits;
                printf("hourly credits: ");
                scanf("%f", &hourlyCredits);

                Equipment.add(name, hourlyCredits);

                printf("Equipment %s added!\n\n", name);
            } else if (strcmp(command, "add-meeting") == 0) {
                int id;
                printf("id of the user (presenter): ");
                scanf("%d", &id);

                char name[BUFFER_SIZE];
                printf("name: ");
                scanf("%s", name);

                char date[BUFFER_SIZE];
                printf("date [DD/MM/YYYY]: ");
                scanf("%s", date);

                char time[BUFFER_SIZE];
                printf("time [HH:mm]: ");
                scanf("%s", time);

                float credits;
                printf("bonus credits (to the presenter): ");
                scanf("%f", &credits);

                Meetings.add(id, name, date, time, credits);
                Users.updateNextPayment(id, -credits);

                printf("Meeting %s added!\n\n", name);
            } else if (strcmp(command, "remove-user") == 0) {
                int id;
                printf("id of the user: ");
                scanf("%d", &id);

                if (user->id == id) {
                    printf("You can't remove yourself!");
                } else {
                    if (Users.remove(id)) {
                        printf("User %d removed", id);
                    } else {
                        printf("Could not find user with id %d", id);
                    }
                }

                printf("\n\n");
            } else if (strcmp(command, "remove-equipment") == 0) {
                int id;
                printf("id of the equipment: ");
                scanf("%d", &id);

                if (Equipment.remove(id)) {
                    printf("Equipment %d removed", id);
                } else {
                    printf("Could not find equipment with id %d", id);
                }

                printf("\n\n");
            } else if (strcmp(command, "remove-meeting") == 0) {
                int id;
                printf("id of the meeting: ");
                scanf("%d", &id);

                struct Meeting *meeting = Meetings.check(id);
                Users.updateNextPayment(meeting->user, meeting->credits);

                if (Meetings.remove(id)) {
                    printf("Meeting %d removed", id);
                } else {
                    printf("Could not find meeting with id %d", id);
                }

                printf("\n\n");
            } else if (strcmp(command, "disable-user") == 0) {
                int id;
                printf("id of the user: ");
                scanf("%d", &id);

                struct User *user = Users.check(id);
                if (user->isAdmin) {
                    printf("Admin users can't be disabled");
                } else {
                    if (user->active) {
                        Users.disable(id);
                        printf("User %d now disabled", id);
                    } else {
                        printf("User %d already disabled", id);
                    }
                }

                printf("\n\n");
            } else if (strcmp(command, "enable-user") == 0) {
                int id;
                printf("id of the user: ");
                scanf("%d", &id);

                struct User *user = Users.check(id);
                if (user->active) {
                    printf("User %d already enabled", id);
                } else {
                    Users.enable(id);
                    printf("User %d now enabled", id);
                }

                printf("\n\n");
            } else if (strcmp(command, "renew-subscription") == 0) {
                int id;
                printf("id of the user: ");
                scanf("%d", &id);
                Users.renew(id);
                printf("Subscription renewed for user %d!\n\n", id);
            } else {
                printf("Command %s not supported.\n\n", command);
            }
        } else {
            printf("Command %s not supported, or need admin privileges.\n\n", command);
        }
    }
}
