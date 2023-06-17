#include <stdio.h>
#include "equipment.h"
#include "users.h"
#include "meetings.h"

int main() {
    loadEquipment();
    displayEquipment();

    loadUsers();
    displayUsersDetailed();

    loadMeetings();
    displayMeetingsDetailed();

    return 0;
}
