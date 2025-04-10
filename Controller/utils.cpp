#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    enum BUY_MODE {
         BUY = 0, // Buy Mode
         SELL= 2, // Sell Mode
         AFB = 1, // After Buy Mode
         AFS = 3  // After Sell Mode
    };

} }

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    uint set_mode( double rsi, uint prv ) {
        uint mode = rsi<20? BUY_MODE::BUY : rsi>80? BUY_MODE::SELL : BUY_MODE::AFS ;
        if ( mode ==BUY_MODE::AFS && (prv==BUY_MODE::AFB||prv==BUY_MODE::AFS) ){ return prv;  }
        if ( mode ==BUY_MODE::BUY || mode==BUY_MODE::SELL )                    { return mode; }
        if ( mode ==BUY_MODE::AFS && prv ==BUY_MODE::BUY  )           { return BUY_MODE::AFB; } return mode;
    }

}}

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    template< class V, ulong N >
    double mean( const V (&item) [N] ) { double res = 0.0;
        for( auto x=0; x<N; x++ ){ res += item[x]/N; }
        return res;
    }

    double normalizer( double a, double b, double c ) {
        return 100 * ( c - b ) / ( a - b );
    }

    double dnormalizer( double a, double b, double c ) {
        return ( c * ( a - b ) / 100 ) + b;
    }

    double strength( double a, double b ) {
        return 100 - ( 100 / ( 1 + ( a / b ) ) );
    }

    double gain( double a, double b ) {
        return ( 100 / a ) * b;
    }

    ptr_t<uint> wait ( new uint(0) );

}}

/*────────────────────────────────────────────────────────────────────────────*/
