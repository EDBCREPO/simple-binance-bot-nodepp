#ifndef NODEPP_SQLITE
#define NODEPP_SQLITE

/*────────────────────────────────────────────────────────────────────────────*/

#include <nodepp/nodepp.h>
#include <nodepp/event.h>
#include <sqlite3.h>

namespace nodepp { using sql_item_t = map_t<string_t,string_t>; }

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef NODEPP_SQLITE_GENERATOR
#define NODEPP_SQLITE_GENERATOR

namespace nodepp { namespace _sqlite_ { GENERATOR( cb ){
protected:

    array_t<string_t> col;
    int num_fields, x;
    int err;

public:

    template< class T, class U, class V, class Q > coEmit( T& fd, U& res, V& cb, Q& self ){
    gnStart ; coWait( self->is_used()==1 ); self->use();

        num_fields = sqlite3_column_count( res ); for( x=0; x<num_fields; x++ )
        { col.push(string_t((char*)sqlite3_column_name(res,x))); } coYield(1);

        coWait( (err=sqlite3_step(res))==SQLITE_BUSY );
        if( err != SQLITE_ROW ){ coGoto(2); } do {

            auto object = map_t<string_t,string_t>();

            for( x=0; x<num_fields; x++ ){
                 char* y = (char*) sqlite3_column_text(res,x);
                 object[ col[x] ] = y ? y : "NULL";
            }

        cb( object ); }while(0); coTry(1);
        coYield(2); self->release();
        sqlite3_finalize(res);

    gnStop
    }

};}}

#endif

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { class sqlite_t {
protected:

    struct NODE {
        sqlite3 *fd = nullptr;
        bool   used = 0;
        int   state = 1;
    };  ptr_t<NODE> obj;

public:

    event_t<> onUse;
    event_t<> onRelease;

    /*─······································································─*/

    virtual ~sqlite_t() noexcept {
        if( obj.count() > 1 || obj->fd == nullptr ){ return; }
        if( obj->state == 0 ){ return; } free();
    }

    /*─······································································─*/

    sqlite_t ( string_t db_file ) : obj( new NODE ) {
        if( sqlite3_open( db_file.data(), &obj->fd ) ) {
            process::error( "SQL Error: ", sqlite3_errmsg(obj->fd) );
        }
    }

    sqlite_t () : obj( new NODE ) { obj->state = 0; }

    /*─······································································─*/

    void exec( const string_t& cmd, const function_t<void,sql_item_t>& cb ) const {
        if( cmd.empty() || obj->state==0 || obj->fd==nullptr ){ return; }

        _sqlite_::cb task; auto self = type::bind( this );
        sqlite3_stmt *res; int rc; char* msg;

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &res, NULL ) != SQLITE_OK ) {
            string_t message ( sqlite3_errmsg( obj->fd ) );
            process::error( "SQL Error: ", message );
        }   if( res == NULL ) { return; }

        process::poll::add( task, obj->fd, res, cb, self );
    }

    array_t<sql_item_t> exec( const string_t& cmd ) const { array_t<sql_item_t> arr;
        function_t<void,sql_item_t> cb = [&]( sql_item_t args ){ arr.push(args); };
        if( cmd.empty() || obj->state==0 || obj->fd==nullptr ){ return nullptr; }

        _sqlite_::cb task; auto self = type::bind( this );
        sqlite3_stmt *res; int rc; char* msg;

        if( sqlite3_prepare_v2( obj->fd, cmd.get(), -1, &res, NULL ) != SQLITE_OK ) {
            string_t message ( sqlite3_errmsg( obj->fd ) );
            process::error( "SQL Error: ", message );
        }   if( res == NULL ) { return nullptr; }

        process::await( task, obj->fd, res, cb, self ); return arr;
    }

    /*─······································································─*/

    void use()     const noexcept { if( obj->used==1 ){ return; } obj->used=1; onUse    .emit(); }
    void release() const noexcept { if( obj->used==0 ){ return; } obj->used=0; onRelease.emit(); }
    bool is_used() const noexcept { return obj->used; }

    /*─······································································─*/

    void free() const noexcept {
        if( obj->fd == nullptr ){ return; }
        if( obj->state == 0 )   { return; }
        sqlite3_close( obj->fd );
        release(); obj->state =0;
    }

};}

/*────────────────────────────────────────────────────────────────────────────*/

#ifndef SQL_FORMAT
#define SQL_FORMAT
namespace nodepp { namespace sqlite {
    template< class V, class... T >
string_t format( const V& argc, const T&... args ){
    string_t result = string::to_string(argc); ulong n=0;

    string::map([&]( const string_t arg ){
        if( arg.empty() || result.empty()   ){ return; }
        if( regex::test(arg,"[<\'\">]|\\N") ){ result=nullptr; return; }
        string_t reg = "\\$\\{" + string::to_string(n) + "\\}";
        result = regex::replace_all( result, reg, arg ); n++;
    },  args... );

    return result;
}}}
#endif

/*────────────────────────────────────────────────────────────────────────────*/

namespace nodepp { namespace sqlite {

    sqlite_t add( string_t path ) { return sqlite_t( path ); }

}}

/*────────────────────────────────────────────────────────────────────────────*/

#endif
