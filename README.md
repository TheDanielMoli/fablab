# FabLab

FabLab is a C99 project for a university exam @Uniba. It emulates database storage for three different entities: Users, Equipment and Meetings.

Users can sign in with their username and password.

There are two user roles: admin and standard.

Standard users can see a list of equipment, check their own credit, borrow and return equipment.

Admin users can list, add and remove users; list, add and remove equipment; list, add and remove meetings.

## Running the app

This app was developed on macOS Ventura 13.4 using JetBrains CLion 2023.1.3.

Instructions:
* Open the project.
* Click the `Run` button in the top-right corner of CLion.

It works both on macOS x86 (Intel) and macOS arm64 (Apple Silicon).

It has never been tested on Windows. The recommended IDE for this exam (Dev-++) only runs on Windows. Therefore, I have no idea if this will ever run on Dev-C++, or even on Windows.

## User accounts

FabLab has the following default (test) accounts:

* Standard account

```
username: john
password: password
```

* Admin account

```
username: admin
password: admin
```