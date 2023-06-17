#ifndef FABLAB_EQUIPMENT_H
#define FABLAB_EQUIPMENT_H

#define SIZE 2048
#define BUFFER_SIZE 2048
#define EQUIPMENT_PRIMARY_INDEX_FILE "../database/equipment/primary-index"
#define EQUIPMENT_NAME_INDEX_FILE "../database/equipment/name-index"
#define EQUIPMENT_AVAILABLE_INDEX_FILE "../database/equipment/available-index"
#define EQUIPMENT_DATA_PATH "../database/equipment/data/%d"

#include <stdbool.h>
#include <time.h>

struct Equipment {
    int id;
    char name[BUFFER_SIZE];
    float hourlyCredits;
    bool available;
    int user;
};

extern void loadEquipment();

extern int addEquipment(char name[BUFFER_SIZE], float hourlyCredits);
extern void removeEquipment(int id);
extern struct Equipment* readEquipment(int id);
extern void borrowEquipment(int id, int user);
extern void returnEquipment(int id);
extern void displayEquipment();

#endif //FABLAB_EQUIPMENT_H
