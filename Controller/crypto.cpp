#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    struct price_t {
        ulong   otime;
        double oprice;
        double hprice;
        double lprice;
        double cprice;
        ulong   ctime;
        double volume;
    };

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void crypto() { sqlite_t db (process::env::get("MODEL")); try {

        db.exec( R"( SELECT * FROM TOKENS )", [=]( sql_item_t item ){

            auto uri = url_t();
                 uri.origin = process::env::get("BITGET_API");
                 uri.path   = "/api/v2/spot/market/candles";
                 uri.query  = query_t();

                 uri.query["symbol"]      = item["NAME"];
                 uri.query["granularity"] = "1week";
                 uri.query["limit"]       = "100";

            fetch_t args; ssl_t ssl;
                    args.method = "GET";
                    args.url    = url::format(uri);

            agent_t agent;
                    agent.conn_timeout = 10000;
                    agent.recv_timeout = 10000;
                    agent.send_timeout = 10000;

            https::fetch( args, &ssl, &agent ).then([=]( https_t cli ){
            try { if( cli.status != 200 ) { throw ""; }

                ptr_t<double> _min_ ( 5, type::cast<double>( (ulong)-1 ) );
                ptr_t<double> _mid_ ( 5, 0.0 );
                ptr_t<double> _max_ ( 5, 0.0 );
                ptr_t<double> _rsi_ ({ _min_[0], 0.0, 0.0, 0.0 });

                auto data = json::parse( stream::await(cli) );
                auto list = data["data"].as<array_t<object_t>>();

                if ( list.size()<100 ) { throw ""; } for ( auto x: list.slice(30) ) {
                if ( regex::test( x[1].as<string_t>(), "[.]0{5}" ) ){ throw ""; }

                   price_t raw; memset( &raw, 0, sizeof( price_t ) );
                           raw.oprice= string::to_double( x[1].as<string_t>() );
                           raw.hprice= string::to_double( x[2].as<string_t>() );
                           raw.lprice= string::to_double( x[3].as<string_t>() );
                           raw.cprice= string::to_double( x[4].as<string_t>() );

                    _mid_[0] += raw.oprice/list.size();
                    _mid_[1] += raw.cprice/list.size();
                    _mid_[2] += raw.hprice/list.size();
                    _mid_[3] += raw.lprice/list.size();
                    _mid_[4] = mean({ _mid_[0], _mid_[1], _mid_[2], _mid_[3] });

                    _min_[0] = min( _min_[0], raw.oprice );
                    _min_[1] = min( _min_[1], raw.cprice );
                    _min_[2] = min( _min_[2], raw.hprice );
                    _min_[3] = min( _min_[3], raw.lprice );
                    _min_[4] = mean({ _min_[0], _min_[1], _min_[2], _min_[3] });

                    _max_[0] = max( _max_[0], raw.oprice );
                    _max_[1] = max( _max_[1], raw.cprice );
                    _max_[2] = max( _max_[2], raw.hprice );
                    _max_[3] = max( _max_[3], raw.lprice );
                    _max_[4] = mean({ _max_[0], _max_[1], _max_[2], _max_[3] });

                }

                for( auto x=0; x<=list.size()-10; x++ ){ double data=0, datb=0;
                for( auto  &y:  list.slice( x, x+10 ) ){

                    price_t raw; memset( &raw, 0, sizeof( price_t ) );
                            raw.oprice= string::to_double( y[1].as<string_t>() );
                            raw.hprice= string::to_double( y[2].as<string_t>() );
                            raw.lprice= string::to_double( y[3].as<string_t>() );
                            raw.cprice= string::to_double( y[4].as<string_t>() );

                     double z =( raw.cprice -raw.oprice ), w = abs(
                     normalizer( raw.hprice, raw.lprice, raw.cprice ) -
                     normalizer( raw.hprice, raw.lprice, raw.oprice )
                     ) / 100.0;

                     if( z<0 ){ datb += string::to_double( y[6].as<string_t>() )*w / 8; }
                     else     { data += string::to_double( y[6].as<string_t>() )*w / 8; }

                }

                    _rsi_[1] = strength( data, datb );
                    _rsi_[0] = min( _rsi_[0], _rsi_[1] );
                    _rsi_[2] = max( _rsi_[2], _rsi_[1] );
                    _rsi_[3] = normalizer( _rsi_[2], _rsi_[0], _rsi_[1] );

                }

                db.exec( regex::format( R"(
                    UPDATE TOKENS SET ARGS='${1}' WHERE NAME='${0}';
                )", item["NAME"], json::stringify( object_t({
                    { "max", _max_[4] }, { "min", _min_[4] },
                    { "mid", _mid_[4] }, { "rsi", _rsi_[3] }
                }) ) ));

            } catch(...) {} *wait-=1;
            }).fail([=]( except_t err ){ *wait-=1; }); *wait+=1;

           if( *wait<=5 ){ return; }
        while( *wait>=5 ){ process::next(); }
        while( *wait!=0 ){ process::next(); } });

    } catch(...) {} timer::timeout([=](){ crypto(); }, TIME_DAYS(1) ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/
