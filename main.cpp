#include <nodepp/nodepp.h>
#include <nodepp/https.h>
#include <nodepp/json.h>

using namespace nodepp;

void onMain() {

    struct price {
        ulong  otime;
        double oprice;
        double hprice;
        double lprice;
        double cprice;
        ulong  ctime;
        double volume;
    };

    auto uri = url_t();
         uri.origin = "https://api.binance.com";
         uri.path   = "/api/v3/klines";
         uri.query  = query_t();

    uri.query["symbol"]   = "1INCHUSDT";
    uri.query["limit"]    = "500";
    uri.query["interval"] = "1M";

    fetch_t args; ssl_t ssl;
            args.method= "GET";
            args.url   = url::format( uri );

    https::fetch( args, &ssl ).then([=]( https_t cli ){ try {

        auto data = json::parse( stream::await( cli ) );

        ptr_t<double> _min_ ( 5, type::cast<double>( (ulong)-1 ) );
        ptr_t<double> _mid_ ( 5, 0.0 );
        ptr_t<double> _max_ ( 5, 0.0 );
        ptr_t<double> _act_ ( 5, 0.0 );

        auto list = data.as<array_t<object_t>>(); for( auto x: list ){

             price data; memset( &data, 0, sizeof( price ) );
                   data.oprice= string::to_double(x[1].as<string_t>());
                   data.hprice= string::to_double(x[2].as<string_t>());
                   data.lprice= string::to_double(x[3].as<string_t>());
                   data.cprice= string::to_double(x[4].as<string_t>());

             _mid_[0] += data.oprice/list.size();
             _mid_[1] += data.cprice/list.size();
             _mid_[2] += data.hprice/list.size();
             _mid_[3] += data.lprice/list.size();
             _mid_[4] = ( _mid_[0] + _mid_[1] + _mid_[2] + _mid_[3] ) / 4;

             _min_[0] = min( _min_[0], data.oprice );
             _min_[1] = min( _min_[1], data.cprice );
             _min_[2] = min( _min_[2], data.hprice );
             _min_[3] = min( _min_[3], data.lprice );
             _min_[4] = ( _min_[0] + _min_[1] + _min_[2] + _min_[3] ) / 4;

             _max_[0] = max( _max_[0], data.oprice );
             _max_[1] = max( _max_[1], data.cprice );
             _max_[2] = max( _max_[2], data.hprice );
             _max_[3] = max( _max_[3], data.lprice );
             _max_[4] = ( _max_[0] + _max_[1] + _max_[2] + _max_[3] ) / 4;

        } do {

            price data; memset( &data, 0, sizeof( price ) );
                  data.oprice= string::to_double(list[list.size()-1][1].as<string_t>());
                  data.hprice= string::to_double(list[list.size()-1][2].as<string_t>());
                  data.lprice= string::to_double(list[list.size()-1][3].as<string_t>());
                  data.cprice= string::to_double(list[list.size()-1][4].as<string_t>());

            _act_[0] = data.oprice;
            _act_[1] = data.cprice;
            _act_[2] = data.hprice;
            _act_[3] = data.lprice;
            _act_[4] = ( _act_[0] + _act_[1] + _act_[2] + _act_[3] ) / 4;

        } while(0); console::clear();

        uri.query["size"] = string::to_string( list.size() );
        console::log( json::stringify( uri.query ) );

        console::log( "-- min --" );
        console::log( "o->", _min_[0] );
        console::log( "c->", _min_[1] );
        console::log( "h->", _min_[2] );
        console::log( "l->", _min_[3] );
        console::log( "m->", _min_[4] );

        console::log( "-- mean --" );
        console::log( "o->", _mid_[0] );
        console::log( "c->", _mid_[1] );
        console::log( "h->", _mid_[2] );
        console::log( "l->", _mid_[3] );
        console::log( "m->", _mid_[4] );

        console::log( "-- max --" );
        console::log( "o->", _max_[0] );
        console::log( "c->", _max_[1] );
        console::log( "h->", _max_[2] );
        console::log( "l->", _max_[3] );
        console::log( "m->", _max_[4] );

        console::log( "-- act --" );
        console::log( "o->", _act_[0] );
        console::log( "c->", _act_[1] );
        console::log( "h->", _act_[2] );
        console::log( "l->", _act_[3] );
        console::log( "m->", _act_[4] );

    } catch(...) { console::log("something went wrong"); }
    });

    console::log( "wait a minute" );

}
