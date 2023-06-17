#include <stdio.h>
#include "equipment.h"
#include "users.h"
#include "meetings.h"

int main() {
    printf("Welcome to the FabLab!\n\n");

    loadEquipment();
    displayEquipmentDetailed();

    loadUsers();
    displayUsersDetailed();

    loadMeetings();
    displayMeetingsDetailed();

    printf("Bye!");

    return 0;
}
