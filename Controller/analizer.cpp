#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void buy_crypto( string_t name  ) {
        console::log( "<<", name );
        throw -1;
    }

    void sell_crypto( string_t name ) {
        console::log( ">>", name );
        throw -1;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void classifier( string_t name, double _max_, double _min_, double _new_, double _rsi_, object_t& obj ) {

        auto _prv_ = obj["prv"].as<double>();
        auto _buy_ = obj["buy"].as<double>();

        auto _nrm_ = normalizer( _max_, _min_, _new_ );
        auto _mde_ = set_mode( _rsi_, obj["mode"].as<uint>() );
        auto _gin_ = _buy_==0.0 ? gain( _new_, _prv_ ) : gain( _new_, _buy_ );

        console::log( "<>", name, _rsi_, _gin_, _nrm_ );

    try {

        if( _mde_==BUY_MODE::BUY && _nrm_ < -1.0 && _buy_ != 0.0 )                   { sell_crypto( name ); throw 0; }
      elif( _mde_==BUY_MODE::BUY && _gin_ < 90.0 && _buy_ != 0.0 )                   { sell_crypto( name ); throw 0; }
      elif( _mde_==BUY_MODE::AFS && _gin_ < obj["gin"].as<double>() && _buy_ != 0.0 ){ sell_crypto( name ); throw 0; }

        if( _mde_==BUY_MODE::AFB && _nrm_ < 10.0 && _buy_ == 0.0 )                   { buy_crypto( name );  throw 1; }
      elif( _mde_==BUY_MODE::AFS && _nrm_ < 10.0 && _buy_ == 0.0 )                   { buy_crypto( name );  throw 1; }

    } catch( int mode ) {

        if( mode==0 ){ obj["buy"] = _new_; }
        if( mode==1 ){ obj["buy"] = type::cast<double>(0.0); }

    }

        obj["gin"] = _gin_; obj["mode"]= _mde_;
        obj["rsi"] = _rsi_; obj["prv"] = _new_;

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    void analize_price( string_t name, object_t arg ) {

        sqlite_t db( process::env::get("POINT") );

        double a = arg["max"].as<double>(), b = arg["min"].as<double>(),
               e = arg["mid"].as<double>(), c = arg["new"].as<double>(),
               d = arg["rsi"].as<double>();

        if( gain(c,e)<150 || gain(c,e)>500 ) { return; } try {

            console::log("A");

            auto data = db.exec( regex::format( R"(
                SELECT * FROM POINT WHERE NAME='${0}'
            )", name )); if( data.size()==0 ) { throw ""; }

            console::log("B");

            auto obj = json::parse( data[0]["ARGS"] );
            classifier( name, a, b, c, d, obj );

            console::log("C");

            db.exec( regex::format( R"(
                UPDATE POINT SET ARGS='${1}' WHERE NAME='${0}'
            )", name, json::stringify(obj) ));

            console::log("D");

        } catch(...) {

            uint mode = set_mode( d, BUY_MODE::AFB );

            auto obj = object_t({
                { "rsi", d }, { "buy", type::cast<double>(0) },
                { "prv", c }, { "gin", gain( c, c ) },
                { "mode", type::cast<uint>( mode ) }
            });

            db.exec( regex::format( R"(
                INSERT INTO POINT ( NAME, ARGS )
                VALUES ( '${0}', '${1}' );
            )", name, json::stringify(obj) ));

        }

    }

}}

/*────────────────────────────────────────────────────────────────────────────*/
