#include <stdio.h>
#include <string.h>
#include "entities.h"
#include "colors.h"

int main() {
    // initialize dao (data access object) entities
    struct equipment Equipment = EquipmentEntity();
    struct meetings Meetings = MeetingsEntity();
    struct users Users = UsersEntity();

    Equipment.load();
    Users.load();
    Meetings.load();

    color(CYAN);
    printf("Welcome to the FabLab!\n\n");
    color(DEFAULT);

    // sign in

    bool signedIn = false; // user id
    struct User* user;

    while (!signedIn) {
        printf("Sign in to access your account:\n");
        color(CYAN);
        printf("username: ");
        color(DEFAULT);
        char username[BUFFER_SIZE];
        scanf("%s", username);
        color(CYAN);
        printf("password: ");
        color(DEFAULT);
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
                    color(YELLOW);
                    printf("You have admin privileges.\n");
                    color(DEFAULT);
                } else {
                    color(GREEN);
                    printf("You are a standard user.\n");
                    color(DEFAULT);
                }
                printf("\n");
                break;
            case 404:
                color(RED);
                printf("User does not exist, try again!\n\n");
                color(DEFAULT);
                break;
            case 401:
                color(RED);
                printf("Wrong password!\n\n");
                color(DEFAULT);
                break;
            default:
                color(RED);
                printf("Unexpected status from sign in, exiting\n\n");
                color(DEFAULT);
                return -1;
        }
    }

    // print menu

    printf("Use command \"help\" to get a list of all available commands\n\n");

    while(1) {
        color(CYAN);
        printf("command: ");
        color(DEFAULT);
        char command[BUFFER_SIZE];
        scanf("%s", command);
        printf("\n");

        if (strcmp(command, "exit") == 0) {
            color(CYAN);
            printf("Bye!\n");
            color(DEFAULT);
            return 0;
        } else if (strcmp(command, "help") == 0) {
            printf("Available commands:\n");
            color(CYAN);
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
            color(DEFAULT);

            printf("\n");
        } else if (strcmp(command, "list-equipment") == 0) {
            Equipment.listWithFullDetails();
        } else if (strcmp(command, "view-balance") == 0) {
            Users.displayBalance(user->id);
        } else if (strcmp(command, "borrow-equipment") == 0) {
            int id;
            color(CYAN);
            printf("id of the equipment: ");
            color(DEFAULT);
            scanf("%d", &id);

            int hours;
            color(CYAN);
            printf("hours of the equipment: ");
            color(DEFAULT);
            scanf("%d", &hours);

            struct Equipment* equipment = checkEquipment(id);

            if (equipment->available) {
                float credits = equipment->hourlyCredits * (float)hours;
                if (user->credits - user->bookedCredits > credits) {
                    Users.bookCredits(user->id, credits);
                    user = Users.check(user->id);
                    Equipment.borrow(id, hours, user->id);
                    color(GREEN);
                    printf("You just borrowed equipment %d!", id);
                    color(DEFAULT);
                } else {
                    color(RED);
                    printf("Insufficient credits for borrowing equipment %d!", id);
                    color(DEFAULT);
                }
            } else {
                color(RED);
                printf("Equipment %d is not available!", id);
                color(DEFAULT);
            }

            printf("\n\n");
        } else if (strcmp(command, "return-equipment") == 0) {
            int id;
            color(CYAN);
            printf("id of the equipment: ");
            color(DEFAULT);
            scanf("%d", &id);

            struct Equipment* equipment = checkEquipment(id);

            if (equipment->user == user->id && !equipment->available) {
                float credits = equipment->hourlyCredits * (float)equipment->hoursBooked;
                Users.removeCredits(user->id, credits);
                user = Users.check(user->id);
                Equipment.giveBack(id);
                color(GREEN);
                printf("You just returned equipment %d and paid %f credits!", id, credits);
                color(DEFAULT);
            } else if (equipment->available) {
                color(YELLOW);
                printf("Equipment %d is available, you haven't booked it!", id);
                color(DEFAULT);
            } else {
                color(YELLOW);
                printf("Equipment %d has been booked by someone else!", id);
                color(DEFAULT);
            }

            printf("\n\n");
        } else if (strcmp(command, "list-meetings") == 0) {
            Meetings.listWithFullDetails();
        } else if (user->isAdmin) {
            if (strcmp(command, "list-users") == 0) {
                Users.listWithFullDetails();
            } else if (strcmp(command, "add-user") == 0) {
                char username[BUFFER_SIZE];
                color(CYAN);
                printf("username: ");
                color(DEFAULT);
                scanf("%s", username);

                char firstName[BUFFER_SIZE];
                color(CYAN);
                printf("first name: ");
                color(DEFAULT);
                scanf("%s", firstName);

                char lastName[BUFFER_SIZE];
                color(CYAN);
                printf("last name: ");
                color(DEFAULT);
                scanf("%s", lastName);

                char password[BUFFER_SIZE];
                color(CYAN);
                printf("password: ");
                color(DEFAULT);
                scanf("%s", password);

                char adminCheck[BUFFER_SIZE];
                bool isAdmin = false;
                color(CYAN);
                printf("is admin [y/n, yes/no]: ");
                color(DEFAULT);
                scanf("%s", adminCheck);
                if (strcmp(adminCheck, "yes") == 0 || strcmp(adminCheck, "y") == 0) {
                    isAdmin = true;
                }

                Users.add(username, firstName, lastName, password, isAdmin);

                color(GREEN);
                printf("User %s %s (%s) added!\n\n", firstName, lastName, username);
                color(DEFAULT);
            } else if (strcmp(command, "add-equipment") == 0) {
                char name[BUFFER_SIZE];
                color(CYAN);
                printf("name: ");
                color(DEFAULT);
                scanf("%s", name);

                float hourlyCredits;
                color(CYAN);
                printf("hourly credits: ");
                color(DEFAULT);
                scanf("%f", &hourlyCredits);

                Equipment.add(name, hourlyCredits);

                color(GREEN);
                printf("Equipment %s added!\n\n", name);
                color(DEFAULT);
            } else if (strcmp(command, "add-meeting") == 0) {
                int id;
                color(CYAN);
                printf("id of the user (presenter): ");
                color(DEFAULT);
                scanf("%d", &id);

                char name[BUFFER_SIZE];
                color(CYAN);
                printf("name: ");
                color(DEFAULT);
                scanf("%s", name);

                char date[BUFFER_SIZE];
                color(CYAN);
                printf("date [DD/MM/YYYY]: ");
                color(DEFAULT);
                scanf("%s", date);

                char time[BUFFER_SIZE];
                color(CYAN);
                printf("time [HH:mm]: ");
                color(DEFAULT);
                scanf("%s", time);

                float credits;
                color(CYAN);
                printf("bonus credits (to the presenter): ");
                color(DEFAULT);
                scanf("%f", &credits);

                Meetings.add(id, name, date, time, credits);
                Users.updateNextPayment(id, -credits);

                color(GREEN);
                printf("Meeting %s added!\n\n", name);
                color(DEFAULT);
            } else if (strcmp(command, "remove-user") == 0) {
                int id;
                color(CYAN);
                printf("id of the user: ");
                color(DEFAULT);
                scanf("%d", &id);

                if (user->id == id) {
                    color(RED);
                    printf("You can't remove yourself!");
                    color(DEFAULT);
                } else {
                    if (Users.remove(id)) {
                        color(GREEN);
                        printf("User %d removed", id);
                        color(DEFAULT);
                    } else {
                        color(YELLOW);
                        printf("Could not find user with id %d", id);
                        color(DEFAULT);
                    }
                }

                printf("\n\n");
            } else if (strcmp(command, "remove-equipment") == 0) {
                int id;
                color(CYAN);
                printf("id of the equipment: ");
                color(DEFAULT);
                scanf("%d", &id);

                if (Equipment.remove(id)) {
                    color(GREEN);
                    printf("Equipment %d removed", id);
                    color(DEFAULT);
                } else {
                    color(YELLOW);
                    printf("Could not find equipment with id %d", id);
                    color(DEFAULT);
                }

                printf("\n\n");
            } else if (strcmp(command, "remove-meeting") == 0) {
                int id;
                color(CYAN);
                printf("id of the meeting: ");
                color(DEFAULT);
                scanf("%d", &id);

                struct Meeting *meeting = Meetings.check(id);

                Users.updateNextPayment(meeting->user, meeting->credits);

                if (Meetings.remove(id)) {
                    color(GREEN);
                    printf("Meeting %d removed", id);
                    color(DEFAULT);
                } else {
                    color(YELLOW);
                    printf("Could not find meeting with id %d", id);
                    color(DEFAULT);
                }

                printf("\n\n");
            } else if (strcmp(command, "disable-user") == 0) {
                int id;
                color(CYAN);
                printf("id of the user: ");
                color(DEFAULT);
                scanf("%d", &id);

                struct User *user = Users.check(id);
                if (user->isAdmin) {
                    color(RED);
                    printf("Admin users can't be disabled");
                    color(DEFAULT);
                } else {
                    if (user->active) {
                        Users.disable(id);
                        color(GREEN);
                        printf("User %d now disabled", id);
                        color(DEFAULT);
                    } else {
                        color(YELLOW);
                        printf("User %d already disabled", id);
                        color(DEFAULT);
                    }
                }

                printf("\n\n");
            } else if (strcmp(command, "enable-user") == 0) {
                int id;
                color(CYAN);
                printf("id of the user: ");
                color(DEFAULT);
                scanf("%d", &id);

                struct User *user = Users.check(id);
                if (user->active) {
                    color(YELLOW);
                    printf("User %d already enabled", id);
                    color(DEFAULT);
                } else {
                    Users.enable(id);
                    color(GREEN);
                    printf("User %d now enabled", id);
                    color(DEFAULT);
                }

                printf("\n\n");
            } else if (strcmp(command, "renew-subscription") == 0) {
                int id;
                color(CYAN);
                printf("id of the user: ");
                color(DEFAULT);
                scanf("%d", &id);
                Users.renew(id);
                color(GREEN);
                printf("Subscription renewed for user %d!\n\n", id);
                color(DEFAULT);
            } else {
                color(YELLOW);
                printf("Command %s not supported.\n\n", command);
                color(DEFAULT);
            }
        } else {
            color(YELLOW);
            printf("Command %s not supported, or need admin privileges.\n\n", command);
            color(DEFAULT);
        }
    }
}
