#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    double normalizer( double a, double b, double c ) {
        return 100 * ( c - b ) / ( a - b );
    }

    double dnormalizer( double a, double b, double c ) {
        return ( c * ( a - b ) / 100 ) + b;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void buy_crypto( string_t name  ) { console::log( "<<", name ); }

    void sell_crypto( string_t name ) { console::log( ">>", name ); }

    void analize_price( string_t name, object_t arg ) {

        sqlite_t db( process::env::get("MODEL") );

        auto   d=arg["new"].as<array_t<double>>();
        double a=arg["max"][4].as<double>();
        double b=arg["min"][4].as<double>();
        double c=d[1];

        /*
        console::log("-- New --");
        console::log( "o->", normalizer( a, b, d[0] ) );
        console::log( "c->", normalizer( a, b, d[1] ) );
        console::log( "h->", normalizer( a, b, d[2] ) );
        console::log( "l->", normalizer( a, b, d[3] ) );
        console::log( "m->", normalizer( a, b, d[4] ) );

        console::log( "-- Min --" );
        console::log( "o->", normalizer( a, b, arg["min"][0].as<double>() ) );
        console::log( "c->", normalizer( a, b, arg["min"][1].as<double>() ) );
        console::log( "h->", normalizer( a, b, arg["min"][2].as<double>() ) );
        console::log( "l->", normalizer( a, b, arg["min"][3].as<double>() ) );
        console::log( "m->", normalizer( a, b, arg["min"][4].as<double>() ) );

        console::log("-- Max --");
        console::log( "o->", normalizer( a, b, arg["max"][0].as<double>() ) );
        console::log( "c->", normalizer( a, b, arg["max"][1].as<double>() ) );
        console::log( "h->", normalizer( a, b, arg["max"][2].as<double>() ) );
        console::log( "l->", normalizer( a, b, arg["max"][3].as<double>() ) );
        console::log( "m->", normalizer( a, b, arg["max"][4].as<double>() ) );

        console::log("-- Mid --");
        console::log( "o->", normalizer( a, b, arg["mid"][0].as<double>() ) );
        console::log( "c->", normalizer( a, b, arg["mid"][1].as<double>() ) );
        console::log( "h->", normalizer( a, b, arg["mid"][2].as<double>() ) );
        console::log( "l->", normalizer( a, b, arg["mid"][3].as<double>() ) );
        console::log( "m->", normalizer( a, b, arg["mid"][4].as<double>() ) );
        */

        console::log( string::format( "-- %s --", name.get() ) );

        console::log( "->", normalizer( a, b, c ) );
        console::log( "->", normalizer( a, b, arg["min"][0].as<double>() ) );
        console::log( "->", normalizer( a, b, arg["min"][1].as<double>() ) );
        console::log( "->", normalizer( a, b, arg["min"][2].as<double>() ) );
        console::log( "->", normalizer( a, b, arg["min"][3].as<double>() ) );
        console::log( "->", normalizer( a, b, arg["min"][4].as<double>() ) );

        console::log( "->", arg["rsi"][3].as<double>() );

        console::log( string::format( "-- %s --", name.get() ) );

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

namespace controller { namespace analize {

    void user() { sqlite_t db (process::env::get("MODEL")); try {

        auto wait = type::bind( new uint(0) );

        db.exec( R"( SELECT * FROM TOKENS WHERE ARGS!='NULL' )", [=]( sql_item_t item ){

            auto uri = url_t();
                 uri.origin = process::env::get("BITGET_API");
                 uri.path   = "/api/v2/spot/market/candles";
                 uri.query  = query_t();

                 uri.query["symbol"]      = item["NAME"];
                 uri.query["granularity"] = "1M";
                 uri.query["limit"]       = "1";

            fetch_t args; ssl_t ssl;
                    args.method = "GET";
                    args.url    = url::format(uri);

            agent_t agent;
                    agent.conn_timeout = 10000;
                    agent.recv_timeout = 10000;
                    agent.send_timeout = 10000;

            https::fetch( args, &ssl, &agent ).then([=]( https_t cli ){
            try { if( cli.status != 200 ){ throw ""; }

                auto x   = json::parse( stream::await(cli) )["data"];
                auto arg = json::parse( item["ARGS"] );
                ptr_t<double> _act_ ( 5, 0.0 );

                if( x.empty() ){ throw ""; }

                _act_[0] = string::to_double( x[0][1].as<string_t>() );
                _act_[1] = string::to_double( x[0][4].as<string_t>() );
                _act_[2] = string::to_double( x[0][2].as<string_t>() );
                _act_[3] = string::to_double( x[0][3].as<string_t>() );
                _act_[4] = ( _act_[0] + _act_[1] + _act_[2] + _act_[3] ) / 4;

                arg["new"] = array_t<double>( _act_ ); analize_price( item["NAME"], arg );

            } catch(...) {} *wait-=1;
            }).fail([=]( except_t err ){ *wait-=1; }); *wait+=1;

           if( *wait<=5 ){ return; }
        while( *wait>=5 ){ process::next(); }
        while( *wait!=0 ){ process::next(); } });

    } catch(...) {} timer::timeout([=](){ user(); }, TIME_HOURS(6) ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/
