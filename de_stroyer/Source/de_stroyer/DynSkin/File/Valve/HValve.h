#ifndef _HVALVE_H_
#define _HVALVE_H_

#pragma once

#include "..\HFile.h"

#pragma warning(disable : 4227)

namespace File
{
    namespace Valve
    {
        class CLevel;

        typedef std::unordered_map< std::string, CLevel* >        mapLevel;
        typedef std::unordered_map< std::string, std::string >    mapVariable;

        class CLevel
        {
        public:

                                                CLevel( szPos& start, CFile* file );
                                                ~CLevel( void );


        public:

            inline const mapLevel& const        GetSubLevels( void ) { return _sublevels; }
            inline const mapVariable& const     GetVariables( void ) { return _variables; }


        protected:

            szPos                               _start = 0;            // abs. pos start
            szPos                               _end   = 0;            // abs. pos end

            mapLevel                            _sublevels;            // Sublevel
            mapVariable                         _variables;            // Variables
        };

        class CConfig : public CFile
        {
        public:

                                                CConfig( void );
                                                ~CConfig( void );


        private:

            bool                                Parse( void );


        public:

            inline CLevel*                      GetLevel( void ) { return _level; }


        protected:

            CLevel*                             _level = 0;            // Level
        };
    }
}

#pragma warning(default : 4227)

#endif /* _HVALVE_H_ */