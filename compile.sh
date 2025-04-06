#!/usr/bin/env bash

time g++ -o ./Build/main ./Service/process.cpp -lssl -lcrypto -lsqlite3 -lz -I./Modules
./Build/main
