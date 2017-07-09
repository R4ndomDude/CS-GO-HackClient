#ifndef _HSKINS_H_
#define _HSKINS_H_

#pragma once

#include "..\File\Valve\HValve.h"

#pragma warning(disable : 4227)

namespace Skins
{
    struct SkinInfo_t
    {
        unsigned int    _id;        // index u know for what

        std::string     _shortname; // shortname
        std::string     _name;      // full skin name
    };

    typedef std::vector<SkinInfo_t> vecSkinInfo;
    typedef std::unordered_map<std::string, vecSkinInfo> mapSkinInfo;

    class CSkins
    {
    public:

                                            CSkins( void );
                                            ~CSkins( void );


    public:

        bool                                Load( const std::string& gamePath, const std::string& gameShortName );
        void                                Release( void );


    public:

		void                                DumpFile(std::ofstream& dump);
		void                                Dump();
		mapSkinInfo& const					GetData();


    public:

        inline const mapSkinInfo& const GetSkinInfo( void )     { return _skininfo; }
        inline const vecSkinInfo& const GetSkinInfoByWeapon( const std::string& weapon )
        {
			static vecSkinInfo empty;
            auto& ret = _skininfo.find( weapon );
            if( ret != _skininfo.end( ) )
                return ret->second;
			return empty;
        }


    protected:

        File::Valve::CConfig*               _items_game       = nullptr; // config file by valve
        File::Valve::CConfig*               _csgo_english     = nullptr; // config file by valve

        mapSkinInfo                         _skininfo;                   // Holds Skinsinformation
    };
}

#endif /* _HSKINS_H_ */
