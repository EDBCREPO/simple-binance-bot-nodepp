#pragma once

/*────────────────────────────────────────────────────────────────────────────*/

namespace controller { namespace analize {

    uint set_mode( double rsi, uint prv ) {
        uint mode = rsi<20? 0: rsi>80? 2: 3;
        if ( mode ==0 || mode==2 )           { return mode; }
        if ( mode ==3 && ( prv==1||prv==3 ) ){ return prv;  }
        if ( mode ==3 && prv ==0 )           { return 1; }
        if ( mode ==3 && prv ==2 )           { return 3; }
        return 0;
    }

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
