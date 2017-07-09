#include "HFile.h"


namespace File
{
    CFile::CFile( void ) : _instance( this )
    {
    }

    CFile::~CFile( void )
    {
    }

    bool CFile::Load( const std::string& path, bool isunicode /* = false*/ )
    {
        Release( );

        if( GetFileAttributesA( path.c_str( ) ) == 0xFFFFFFFF )//sanity check
            return false;

        _path = path;

        std::ifstream ifs( _path, std::ios::binary | std::ios::ate );
        auto pos = ifs.tellg( );

        auto tmp = vecFile( szPos( pos ) );

        ifs.seekg( 0, std::ios::beg );
        ifs.read( &tmp.at( 0 ), pos );

        for( auto i = 0; i < 1000; ++i ) {
            std::cout << tmp.at( i );
        }

        if( isunicode ) {
            for( unsigned int i = 0; i < pos; i += 2 ) {
                _file.push_back( tmp.at( i ) );
            }
        }
        else {
            _file = tmp;
        }

        return Parse( );
    }

    void CFile::Release( void )
    {
        if( !_file.empty( ) ) {
            _file.clear( );
        }

        _path.clear( );
    }

    szPos CFile::FindFirstOf( const std::string& szSearch, szPos start, szPos end )
    {
        auto isInverse = bool( start > end );

        if( _file.size( ) <= ( isInverse ? start : end ) )
            return _file.size( );

        if( isInverse ) {
            for( auto r = start; r > end; --r ) {
                if( CompareBytes( static_cast< char* >( &_file.at( r ) ), const_cast< char* >( &szSearch.at( 0 ) ) ) )
                    return r;
            }
        }
        else {
            for( auto r = start; r < end; ++r ) {
                if( CompareBytes( static_cast< char* >( &_file.at( r ) ), const_cast< char* >( &szSearch.at( 0 ) ) ) )
                    return r;
            }
        }

        return _file.size( );
    }

    vecPos CFile::FindAllOf( const std::string& szSearch, szPos start, szPos end, vecPos pos /*= vecPos( )*/ )
    {
        auto isInverse = bool( start > end );

        if( _file.size( ) <= ( isInverse ? start : end ) )
            return pos;

        if( isInverse ) {
            for( auto r = start; r > end; --r ) {
                if( CompareBytes( static_cast< char* >( &_file.at( r ) ), const_cast< char* >( &szSearch.at( 0 ) ) ) )
                    pos.push_back( r );
            }
        }
        else {
            for( auto r = start; r < end; ++r ) {
                if( CompareBytes( static_cast< char* >( &_file.at( r ) ), const_cast< char* >( &szSearch.at( 0 ) ) ) )
                    pos.push_back( r );
            }
        }

        return pos;
    }

    std::string CFile::GetStringAt( szPos start, szSize length )
    {
        if( _file.size( ) <= ( start + length ) )
            return std::string( "" );

        return std::string( static_cast< char* >( &_file.at( start ) ), length );
    }

    std::string CFile::GetStringBetween( szPos start, szPos end )
    {
        auto isInverse = bool( start > end );

        if( _file.size( ) <= ( isInverse ? start : end ) )
            return std::string( "" );

        auto rstart = isInverse ? end : start;
        auto rend = isInverse ? start : end;

        return std::string( static_cast< char* >( &_file.at( rstart ) ), szPos( rend - rstart ) );
    }
}
