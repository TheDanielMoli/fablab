#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "meetings.h"
#include "users.h"

FILE *primaryIndexFile;
FILE *nameIndexFile;
FILE *file;

int primaryIndex[SIZE];
char nameIndex[BUFFER_SIZE][SIZE];
int maxPrimaryIndex;
int maxId;

// for internal use, within the users library
static void writePrimaryIndex() {
    primaryIndexFile = fopen(MEETINGS_PRIMARY_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(primaryIndexFile, "%d\n", primaryIndex[i]);
    }
    fprintf(primaryIndexFile, "%d", primaryIndex[maxPrimaryIndex]);
    fclose(primaryIndexFile);
}

// for internal use, within the users library
static void writeNameIndex() {
    nameIndexFile = fopen(MEETINGS_NAME_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(nameIndexFile, "%s\n", nameIndex[i]);
    }
    fprintf(nameIndexFile, "%s", nameIndex[maxPrimaryIndex]);
    fclose(nameIndexFile);
}

// for internal use, within the users library
static void writeRecord(struct Meeting* meeting) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, MEETINGS_DATA_PATH, meeting->id);
    file = fopen(buffer, "w+");

    fprintf(
            file,
            "%s\n%s\n%s%d\n\n%f",
            meeting->name,
            meeting->date,
            meeting->time,
            meeting->user,
            meeting->credits
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

extern struct Meeting* checkMeeting(int id) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, MEETINGS_DATA_PATH, id);
    file = fopen(buffer, "r");

    struct Meeting *meeting = (struct Meeting*) malloc(sizeof(struct Meeting));

    meeting->id = id;

    fgets(meeting->name, BUFFER_SIZE, (FILE*)file);
    meeting->name[strcspn(meeting->name, "\n")] = 0;

    fgets(meeting->date, BUFFER_SIZE, (FILE*)file);
    meeting->date[strcspn(meeting->date, "\n")] = 0;

    fgets(meeting->time, BUFFER_SIZE, (FILE*)file);
    meeting->time[strcspn(meeting->time, "\n")] = 0;

    fscanf(file, "%d", &meeting->user);
    fscanf(file, "%f", &meeting->credits);

    fclose(file);

    return meeting;
}

extern int addMeeting(
        int user,
        char name[BUFFER_SIZE],
        char date[BUFFER_SIZE],
        char time[BUFFER_SIZE],
        float credits
) {
    maxId = maxId + 1;
    maxPrimaryIndex = maxPrimaryIndex + 1;
    primaryIndex[maxPrimaryIndex] = maxId;
    strcpy(nameIndex[maxPrimaryIndex], name);

    struct Meeting* meeting = (struct Meeting*) malloc(sizeof(struct Meeting));

    meeting->id = maxId;
    strcpy(meeting->name, name);
    meeting->user = user;
    strcpy(meeting->date, date);
    strcpy(meeting->time, time);
    meeting->credits = credits;

    writeRecord(meeting);

    fclose(file);

    writePrimaryIndex();
    writeNameIndex();

    return maxPrimaryIndex;
}

extern void removeMeeting(int id) {
    int index = findIndex(id);

    if (index != -1) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, MEETINGS_DATA_PATH, id);
        remove(buffer);

        for(int i = index; i <= maxPrimaryIndex; i++){
            primaryIndex[i] = primaryIndex[i + 1];
            strcpy(nameIndex[i], nameIndex[i + 1]);
        }
        maxPrimaryIndex = maxPrimaryIndex - 1;

        writePrimaryIndex();
        writeNameIndex();
    }
}

extern void displayMeetings() {
    for(int i = 0; i <= maxPrimaryIndex; i++) {
        printf(
                "%d) %s\n",
                primaryIndex[i],
                nameIndex[i]
        );
    }
    printf("\n");
}

extern void displayMeetingsDetailed() {
    for(int i = 0; i <= maxPrimaryIndex; i++) {
        struct Meeting* meeting = checkMeeting(primaryIndex[i]);
        struct User* user = checkUser(meeting->user);
        printf(
                "%d) %s (%s %s): %s at %s (%f credits)\n",
                primaryIndex[i],
                meeting->name,
                user->firstName,
                user->lastName,
                meeting->date,
                meeting->time,
                meeting->credits
        );
    }
    printf("\n");
}

extern void loadMeetings() {
    for (int i = 0; i < SIZE; i++) {
        primaryIndex[i] = -1;
    }

    primaryIndexFile = fopen(MEETINGS_PRIMARY_INDEX_FILE, "r");
    nameIndexFile = fopen(MEETINGS_NAME_INDEX_FILE, "r");

    for (int i = 0; !feof(primaryIndexFile); i++) {
        fscanf(primaryIndexFile, "%d", &primaryIndex[i]);

        fgets(nameIndex[i], BUFFER_SIZE, (FILE*)nameIndexFile);
        nameIndex[i][strcspn(nameIndex[i], "\n")] = 0;

        maxId = primaryIndex[i];
        maxPrimaryIndex = i;
    }

    fclose(primaryIndexFile);
    fclose(nameIndexFile);
}