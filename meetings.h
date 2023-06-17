#ifndef FABLAB_MEETINGS_H
#define FABLAB_MEETINGS_H

#define SIZE 2048
#define BUFFER_SIZE 2048
#define MEETINGS_PRIMARY_INDEX_FILE "../database/meetings/primary-index"
#define MEETINGS_NAME_INDEX_FILE "../database/meetings/name-index"
#define MEETINGS_DATA_PATH "../database/meetings/data/%d"
#define USERS_DATA_PATH "../database/users/data/%d"

#include <stdbool.h>
#include <time.h>

struct Meeting {
    int id;
    int user;
    char name[BUFFER_SIZE];
    char date[BUFFER_SIZE];
    char time[BUFFER_SIZE];
    float credits;
};

struct MeetingOwner {
    int id;
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char firstName[BUFFER_SIZE];
    char lastName[BUFFER_SIZE];
};

extern void loadMeetings();

extern int addMeeting(
        int user,
        char name[BUFFER_SIZE],
        char date[BUFFER_SIZE],
        char time[BUFFER_SIZE],
        float credits
);
extern bool removeMeeting(int id);
extern struct Meeting* checkMeeting(int id);
extern void displayMeetings();
extern void displayMeetingsDetailed();

#endif //FABLAB_MEETINGS_H
