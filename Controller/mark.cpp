#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void buy_crypto( string_t name  ) { console::log( "<<", name ); }

    void sell_crypto( string_t name ) { console::log( ">>", name ); }

    void analize_price( string_t name, object_t arg ) {

        sqlite_t db( process::env::get("MODEL") );

        double a = arg["max"].as<double>(), b = arg["min"].as<double>(),
               e = arg["mid"].as<double>(), c = arg["new"].as<double>(),
               d = arg["rsi"].as<double>();

        console::log( string::format( "-- %s --", name.get() ) );

        console::log( "->", normalizer( a, b, c ) );

        console::log( "---" );

        console::log( "->", gain( c, e ) );
        console::log( "->", d );

        console::log( string::format( "-- %s -- \n", name.get() ) );

        /*
        try {

            auto data = db.exec( regex::format( R"(
                SELECT * FROM POINT WHERE SYMBOL='${0}'
            )", name )); if( data.size()==0 ) { throw ""; }

            auto obj = json::parse( data[0]["ARGS"] );

            db.exec( regex::format( R"(
                UPDATE POINT SET ARGS='${1}' WHERE NAME='${0}'
            )", name, json::stringify(obj) ));

        } catch(...) {

            auto obj = object_t({
                { "max", c }, { "med", c }, { "min", c },
                { "mode", "buy" },
            });

            db.exec( regex::format( R"(
                INSERT INTO POINT ( NAME, ARGS )
                VALUES ( '${0}', '${1}' );
            )", name, json::stringify(obj) ));

        }
        */

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/
