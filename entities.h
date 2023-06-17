#ifndef FABLAB_ENTITIES_H
#define FABLAB_ENTITIES_H

#define SIZE 2048
#define BUFFER_SIZE 2048

#include "equipment.h"
#include "meetings.h"
#include "users.h"

typedef struct equipment {
    void (*load)();
    int (*add)(char name[BUFFER_SIZE], float hourlyCredits);
    bool (*remove)(int id);
    struct Equipment* (*check)(int id);
    void (*borrow)(int id, int hours, int user);
    void (*giveBack)(int id);
    void (*list)();
    void (*listWithFullDetails)();
} equipment;

equipment EquipmentEntity() {
    struct equipment _;
    _.load = &loadEquipment;
    _.add = &addEquipment;
    _.remove = &removeEquipment;
    _.check = &checkEquipment;
    _.borrow = &borrowEquipment;
    _.giveBack = &giveBackEquipment;
    _.list = &displayEquipment;
    _.listWithFullDetails = &displayEquipmentDetailed;
    return _;
}

typedef struct meetings {
    void (*load)();
    int (*add)(
            int user,
            char name[BUFFER_SIZE],
            char date[BUFFER_SIZE],
            char time[BUFFER_SIZE],
            float credits
            );
    bool (*remove)(int id);
    struct Meeting* (*check)(int id);
    void (*list)();
    void (*listWithFullDetails)();
} meetings;

meetings MeetingsEntity() {
    struct meetings _;
    _.load = &loadMeetings;
    _.add = &addMeeting;
    _.remove = &removeMeeting;
    _.check = &checkMeeting;
    _.list = &displayMeetings;
    _.listWithFullDetails = &displayMeetingsDetailed;
    return _;
}

typedef struct users {
    void (*load)();
    int (*add)(
            char username[BUFFER_SIZE],
            char firstName[BUFFER_SIZE],
            char lastName[BUFFER_SIZE],
            char password[BUFFER_SIZE],
            bool isAdmin
    );
    bool (*remove)(int id);
    struct User* (*check)(int id);
    struct Response* (*signIn)(char username[BUFFER_SIZE], char password[BUFFER_SIZE]);
    void (*addCredits)(int id, float credits);
    void (*bookCredits)(int id, float credits);
    void (*removeCredits)(int id, float credits);
    void (*disable)(int id);
    void (*enable)(int id);
    void (*renew)(int id);
    void (*updateNextPayment)(int id, float credits);
    void (*displayBalance)(int id);
    void (*list)();
    void (*listWithFullDetails)();
} users;

users UsersEntity() {
    struct users _;
    _.load = &loadUsers;
    _.add = &addUser;
    _.remove = &removeUser;
    _.check = &checkUser;
    _.signIn = &signIn;
    _.addCredits = &addCredits;
    _.bookCredits = &bookCredits;
    _.removeCredits = &removeCredits;
    _.disable = &disableUser;
    _.enable = &enableUser;
    _.renew = &renewUser;
    _.updateNextPayment = &updateNextPayment;
    _.displayBalance = &displayBalance;
    _.list = &displayUsers;
    _.listWithFullDetails = &displayUsersDetailed;
    return _;
}

#endif //FABLAB_ENTITIES_H
