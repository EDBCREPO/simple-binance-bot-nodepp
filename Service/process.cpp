#include <nodepp/nodepp.h>
#include <sqlite/sqlite.h>
#include <nodepp/https.h>
#include <nodepp/timer.h>
#include <nodepp/json.h>

/*────────────────────────────────────────────────────────────────────────────*/

using namespace nodepp;

/*────────────────────────────────────────────────────────────────────────────*/

#include "../Controller/utils.cpp"
#include "../Controller/price.cpp"
#include "../Controller/symbol.cpp"
#include "../Controller/crypto.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

void onMain() {  process::env::init(".env");

    try { sqlite_t db( process::env::get("MODEL") );

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

    } catch(...) {}

    try { sqlite_t db( process::env::get("POINT") );

        db.exec(_STRING_( CREATE TABLE POINT(
            ID   INTEGER PRIMARY KEY AUTOINCREMENT,
            NAME TEXT    NOT NULL,
            ARGS TEXT    NOT NULL
        ); ));

    } catch(...) {}

    controller::analize::price();
    //controller::analize::crypto();
    //controller::analize::symbol();

}

/*────────────────────────────────────────────────────────────────────────────*/
