#ifndef _HFILE_H_
#define _HFILE_H_

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif /* WIN32_LEAN_AND_MEAN */

#include <Windows.h>

#include <fstream>
#include <vector>
#include <unordered_map>

#include <iostream>

#pragma warning(disable : 4227)

namespace File
{
    typedef unsigned int szPos;
    typedef szPos szSize;

    typedef std::vector<szPos> vecPos;
    typedef std::vector<char> vecFile;

    class CFile
    {
    public:

                                            CFile( void );
        virtual                             ~CFile( void );


    public:

        bool                                Load( const std::string& path, bool isunicode = false );
        void                                Release( void );


    public:

        szPos                               FindFirstOf( const std::string& search, szPos start, szPos end );
        vecPos                              FindAllOf( const std::string& search, szPos start, szPos end, vecPos pos = vecPos( ) );

        std::string                         GetStringAt( szPos start, szSize length );
        std::string                         GetStringBetween( szPos start, szPos end );


    private:

        virtual bool Parse( void ) = 0;


    public:

        inline const std::string& const     GetPath( void )         { return _path; }
        inline const vecFile& const         GetFile( void )         { return _file; }
        szPos								GetFileSize( void )     { return _file.size( ); }
        inline const char& const            at( std::size_t p )     { return _file.at( p ); }


    private:

        inline bool CompareBytes( char* pszData, const char* pszPattern )
        {
            for( auto i = 0; *pszPattern; ++pszPattern, ++i ) {
                if( pszData[ i ] != *pszPattern ) {
                    return false;
                }
            }
            return true;
        }


    protected:

        std::string                         _path;          // filepath

        vecFile                             _file;          // file std::binary | std::ate
        CFile*                              _instance;      // instance
    };
}

#pragma warning(default : 4227)

#endif /* _HFILE_H_ */
