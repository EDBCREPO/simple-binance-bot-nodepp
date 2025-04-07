#pragma once

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
