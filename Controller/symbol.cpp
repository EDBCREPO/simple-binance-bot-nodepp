#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void symbol() { sqlite_t db (process::env::get("MODEL")); try {

        auto uri = url_t();
             uri.origin = process::env::get("BITGET_API");
             uri.path   = "/api/v2/spot/public/symbols";

        fetch_t args; ssl_t ssl;
                args.method = "GET";
                args.url    = url::format(uri);

        agent_t agent;
                agent.conn_timeout = 10000;
                agent.recv_timeout = 10000;
                agent.send_timeout = 10000;

        https::fetch( args, &ssl, &agent ).then([=]( https_t cli ){
        auto list = type::bind( new array_t<string_t>() );
        if( cli.status != 200 ){ return; }

            cli.onData([=]( string_t data ){
                auto raw = regex::match_all( data,"symbol[^,]+" );
                forEach( x, raw ){
                    if( !regex::test( x, "USDT$" ) ){ continue; }
                    list->push( regex::remove_all( x, "symbol|[\":]+" ) );
                }
            });

            cli.onClose([=](){ forEach( x, *list ) { try {

                auto raw = db.exec( regex::format ( R"(
                    SELECT COUNT(*) FROM TOKENS WHERE NAME='${0}'
                )", x ));

                if( raw.empty() || raw[0]["COUNT(*)"]!="0" ){ throw ""; }

                db.exec( regex::format( R"(
                    INSERT INTO TOKENS ( NAME, ARGS )
                    VALUES ( '${0}', 'NULL' );
                )", x ));

            } catch(...) { continue; } } });

        stream::pipe( cli ); }).fail([=]( except_t ){});

    } catch(...) {} timer::timeout([=](){ symbol(); }, TIME_DAYS(7) ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/
