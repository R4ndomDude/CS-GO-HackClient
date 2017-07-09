#include "HValve.h"

namespace File
{
    namespace Valve
    {
        CLevel::CLevel( szPos& start, CFile* file ) :
            _start( start ),
            _end( 0 )
        {
            auto size = file->GetFileSize( );

            szPos vpos[ 4 ] = { 0 };
            unsigned int qcount = 0;

            for( ++start; start < size; ++start ) {
                const auto& p = file->at( start );
                if( p == '\"' ) {
                    if( qcount < 4 )
                        vpos[ qcount ] = start;
                    ++qcount;
                }
                else if( p == '\n' ) {
                    if( qcount == 4 )
                        _variables[ std::string( const_cast< char* >( &file->at( vpos[ 0 ] + 1 ) ), vpos[ 1 ] - vpos[ 0 ] - 1 ) ] = std::string( const_cast< char* >( &file->at( vpos[ 2 ] + 1 ) ), vpos[ 3 ] - vpos[ 2 ] - 1 );
                    qcount = 0;
                }
                else if( qcount != 1 && qcount != 3 ) {
                    if( p == '{' ) {
                        vpos[ 0 ] = file->FindFirstOf( "\"", start, 0 ) - 1;
                        vpos[ 1 ] = file->FindFirstOf( "\"", vpos[ 0 ], 0 );

                        _sublevels[ std::string( const_cast< char* >( &file->at( vpos[ 1 ] + 1 ) ), vpos[ 0 ] - vpos[ 1 ] ) ] = new CLevel( start, file );
                    }
                    else if( p == '}' )
                        break;
                }
            }
            _end = start;
        }

        CLevel::~CLevel( void )
        {
            for( auto& p : _sublevels ) {
                delete p.second;
            }
        }

        CConfig::CConfig( void ) : CFile( )
        {
        }

        CConfig::~CConfig( void )
        {
            delete _level;
        }

        bool CConfig::Parse( void )
        {
            auto rstart = FindFirstOf( "{", 0, 200 );
            _level = new CLevel( rstart, _instance );

            return !_level->GetSubLevels( ).empty( );
        }
    }
}
