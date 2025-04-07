#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

#include "mark.cpp"

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void price() { sqlite_t db (process::env::get("MODEL")); try {

        db.exec( R"( SELECT * FROM TOKENS WHERE ARGS!='NULL' )", [=]( sql_item_t item ){

            auto uri = url_t();
                 uri.origin = process::env::get("BITGET_API");
                 uri.path   = "/api/v2/spot/market/candles";
                 uri.query  = query_t();

                 uri.query["symbol"]      = item["NAME"];
                 uri.query["granularity"] = "1week";
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
                _act_[4] = mean({ _act_[0], _act_[1], _act_[2], _act_[3] });

                arg["new"] = array_t<double>( _act_ ); analize_price( item["NAME"], arg );

            } catch(...) {} *wait-=1;
            }).fail([=]( except_t err ){ *wait-=1; }); *wait+=1;

           if( *wait<=5 ){ return; }
        while( *wait>=5 ){ process::next(); }
        while( *wait!=0 ){ process::next(); } });

    } catch(...) {} timer::timeout([=](){ price(); }, TIME_HOURS(6) ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/
