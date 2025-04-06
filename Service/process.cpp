#include <nodepp/nodepp.h>
#include <sqlite/sqlite.h>
#include <nodepp/https.h>
#include <nodepp/timer.h>
#include <nodepp/json.h>
#include <redis/tcp.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

#include "../Controller/user.cpp"
#include "../Controller/symbol.cpp"
#include "../Controller/crypto.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() { process::env::init(".env");

    sqlite_t db (process::env::get("MODEL")); try {

        db.exec(_STRING_( CREATE TABLE TOKENS(
            ID   INTEGER PRIMARY KEY AUTOINCREMENT,
            NAME TEXT            NOT NULL,
            ARGS TEXT            NOT NULL
        ); ));

        db.exec(_STRING_( CREATE TABLE USERS(
            ID   INTEGER PRIMARY KEY AUTOINCREMENT,
            NAME TEXT            NOT NULL,
            KEY  TEXT            NOT NULL
        ); ));

        db.exec(_STRING_( create TABLE POINT(
            ID     INTEGER PRIMARY KEY AUTOINCREMENT,
            SYMBOL TEXT    NOT NULL,
            ARGS   TEXT    NOT NULL
        ); ));

    } catch(...) {}

    controller::analize::user();
    //controller::analize::crypto();
    //controller::analize::symbol();

}

/*────────────────────────────────────────────────────────────────────────────*/
