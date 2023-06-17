#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "equipment.h"

FILE *primaryIndexFile;
FILE *nameIndexFile;
FILE *availableIndexFile;
FILE *file;

static int primaryIndex[SIZE];
static char nameIndex[BUFFER_SIZE][SIZE];
static bool availableIndex[SIZE];
static int maxPrimaryIndex;
static int maxId;

// for internal use, within the equipment library
static void writePrimaryIndex() {
    primaryIndexFile = fopen(EQUIPMENT_PRIMARY_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(primaryIndexFile, "%d\n", primaryIndex[i]);
    }
    fprintf(primaryIndexFile, "%d", primaryIndex[maxPrimaryIndex]);
    fclose(primaryIndexFile);
}

// for internal use, within the equipment library
static void writeNameIndex() {
    nameIndexFile = fopen(EQUIPMENT_NAME_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(nameIndexFile, "%s\n", nameIndex[i]);
    }
    fprintf(nameIndexFile, "%s", nameIndex[maxPrimaryIndex]);
    fclose(nameIndexFile);
}

// for internal use, within the equipment library
static void writeAvailableIndex() {
    availableIndexFile = fopen(EQUIPMENT_AVAILABLE_INDEX_FILE, "w+");
    for (int i = 0; i < maxPrimaryIndex; i++) {
        fprintf(availableIndexFile, "%d\n", availableIndex[i]);
    }
    fprintf(availableIndexFile, "%d", availableIndex[maxPrimaryIndex]);
    fclose(availableIndexFile);
}

// for internal use, within the equipment library
static void writeRecord(struct Equipment* equipment) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, EQUIPMENT_DATA_PATH, equipment->id);
    file = fopen(buffer, "w+");

    fprintf(
            file,
            "%s\n%f\n%d\n%d\n%d",
            equipment->name,
            equipment->hourlyCredits,
            equipment->available,
            equipment->user,
            equipment->hoursBooked
            );

    fclose(file);
}

// for internal use, within the equipment library
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

extern struct Equipment* checkEquipment(int id) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, BUFFER_SIZE, EQUIPMENT_DATA_PATH, id);
    file = fopen(buffer, "r");

    struct Equipment *equipment = (struct Equipment*) malloc(sizeof(struct Equipment));

    equipment->id = id;

    fgets(equipment->name, BUFFER_SIZE, (FILE*)file);
    equipment->name[strcspn(equipment->name, "\n")] = 0;

    fscanf(file, "%f", &equipment->hourlyCredits);

    int available;
    fscanf(file, "%d", &available);
    if (available == 1) {
        equipment->available = true;
    } else {
        equipment->available = false;
    }

    fscanf(file, "%d", &equipment->user);
    fscanf(file, "%d", &equipment->hoursBooked);

    fclose(file);

    return equipment;
}

extern int addEquipment(char name[BUFFER_SIZE], float hourlyCredits) {
    maxId = maxId + 1;
    maxPrimaryIndex = maxPrimaryIndex + 1;
    primaryIndex[maxPrimaryIndex] = maxId;
    strcpy(nameIndex[maxPrimaryIndex], name);
    availableIndex[maxPrimaryIndex] = true;

    struct Equipment* equipment = (struct Equipment*) malloc(sizeof(struct Equipment));

    equipment->id = maxId;
    strcpy(equipment->name, name);
    equipment->hourlyCredits = hourlyCredits;
    equipment->available = 1;
    equipment->user = -1;
    equipment->hoursBooked = 0;

    writeRecord(equipment);

    fclose(file);

    writePrimaryIndex();
    writeNameIndex();
    writeAvailableIndex();

    return maxPrimaryIndex;
}

extern bool removeEquipment(int id) {
    int index = findIndex(id);

    if (index != -1) {
        char buffer[BUFFER_SIZE];
        snprintf(buffer, BUFFER_SIZE, EQUIPMENT_DATA_PATH, id);
        remove(buffer);

        for(int i = index; i <= maxPrimaryIndex; i++){
            primaryIndex[i] = primaryIndex[i + 1];
            strcpy(nameIndex[i], nameIndex[i + 1]);
            availableIndex[i] = availableIndex[i + 1];
        }
        maxPrimaryIndex = maxPrimaryIndex - 1;

        writePrimaryIndex();
        writeNameIndex();
        writeAvailableIndex();

        return true;
    } else {
        return false;
    }
}

extern void borrowEquipment(int id, int hours, int user) {
    int index = findIndex(id);
    availableIndex[index] = false;
    writeAvailableIndex();

    struct Equipment* equipment = checkEquipment(id);
    equipment->available = false;
    equipment->user = user;
    equipment->hoursBooked = hours;
    writeRecord(equipment);
}

extern void giveBackEquipment(int id) {
    int index = findIndex(id);
    availableIndex[index] = true;
    writeAvailableIndex();

    struct Equipment* equipment = checkEquipment(id);
    equipment->available = true;
    equipment->user = -1;
    equipment->hoursBooked = 0;
    writeRecord(equipment);
}

extern void displayEquipment() {
    for(int i = 0; i <= maxPrimaryIndex; i++) {
        printf("%d) %s: %s\n", primaryIndex[i], nameIndex[i], availableIndex[i] ? "available" : "not available");
    }
    printf("\n");
}

extern void displayEquipmentDetailed() {
    for(int i = 0; i <= maxPrimaryIndex; i++) {
        struct Equipment* equipment = checkEquipment(primaryIndex[i]);
        printf(
                "%d) %s: %f (%s - %d hours booked)\n",
                primaryIndex[i],
                equipment->name,
                equipment->hourlyCredits,
                equipment->available ? "available" : "not available",
                equipment->hoursBooked
        );
    }
    printf("\n");
}

extern void loadEquipment() {
    for (int i = 0; i < SIZE; i++) {
        primaryIndex[i] = -1;
    }

    primaryIndexFile = fopen(EQUIPMENT_PRIMARY_INDEX_FILE, "r");
    nameIndexFile = fopen(EQUIPMENT_NAME_INDEX_FILE, "r");
    availableIndexFile = fopen(EQUIPMENT_AVAILABLE_INDEX_FILE, "r");

    for (int i = 0; !feof(primaryIndexFile); i++) {
        fscanf(primaryIndexFile, "%d", &primaryIndex[i]);

        int available;
        fscanf(availableIndexFile, "%d", &available);
        if (available == 1) {
            availableIndex[i] = true;
        } else {
            availableIndex[i] = false;
        }

        fgets(nameIndex[i], BUFFER_SIZE, (FILE*)nameIndexFile);
        nameIndex[i][strcspn(nameIndex[i], "\n")] = 0;

        maxId = primaryIndex[i];
        maxPrimaryIndex = i;
    }

    fclose(primaryIndexFile);
    fclose(nameIndexFile);
}