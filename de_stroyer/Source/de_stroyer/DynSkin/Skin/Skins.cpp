#include "HSkins.h"
#include "../../sdk.h"

namespace Skins
{
    CSkins::CSkins( void )
    {
    }

    CSkins::~CSkins( void )
    {
        delete _items_game;
        delete _csgo_english;
    }

    bool CSkins::Load( const std::string& gamePath, const std::string& gameShortName )
    {
        Release( );

        _items_game = new File::Valve::CConfig( );
        if( !_items_game->Load( gamePath + "/scripts/items/items_game.txt", false ) )
            return false;

        _csgo_english = new File::Valve::CConfig( );
        if( !_csgo_english->Load( gamePath + "/resource/" + gameShortName + "_english.txt", true ) )
            return false;

        auto& vt = _csgo_english->GetLevel( )->GetSubLevels( ).at( "Tokens" )->GetVariables( );
        auto& ai_wi = _items_game->GetLevel( )->GetSubLevels( ).at( "alternate_icons2" )->GetSubLevels( ).at( "weapon_icons" )->GetSubLevels( );

        for( auto& pk : _items_game->GetLevel( )->GetSubLevels( ).at( "paint_kits" )->GetSubLevels( ) ) {
            auto& pkid = pk.first;

            if( pkid == "9001" )
                continue;

            auto& pkname = pk.second->GetVariables( ).at( "name" );
            auto& pkdesctag = std::string( pk.second->GetVariables( ).at( "description_tag" ) );

            auto& res = vt.find( pkdesctag.substr( 1, pkdesctag.size( ) + 1 ) );
            if( res == vt.end( ) ) {
                pkdesctag[ 6 ] = 'k';//FUCK IT DAMN VALVE L2SPELL FFS
                res = vt.find( pkdesctag.substr( 1, pkdesctag.size( ) + 1 ) );
                if( res == vt.end( ) )
                    continue;
            }

            auto& skname = res->second;
            if( skname == "-" )
                continue;

            for( auto& wi : ai_wi ) {
                auto& vip = wi.second->GetVariables( ).at( "icon_path" );
                if( vip[ vip.size( ) - 7 ] != '_' )
                    continue;

                auto res = vip.find( pkname );
                if( res == std::string::npos )
                    continue;

                _skininfo[ vip.substr( 23, res - 24 ) ].push_back( { (unsigned int)atoi( pkid.c_str( ) ), pkname, skname } );
            }
        }

        return true;
    }

    void CSkins::Release( void )
    {
        if( _items_game )
            _items_game->Release( );
        if( _csgo_english )
            _csgo_english->Release( );
    }

	void CSkins::DumpFile(std::ofstream& dump)
	{
		for (auto& w : _skininfo) {
			dump << "[ " << w.first.c_str() << " ]" << std::endl;
			for (auto& s : w.second) {
				dump << " -> " << s._name.c_str() << " | " << s._shortname.c_str() << " | " << "[" << s._id << "]" << std::endl;
			}
		}
	}

	void CSkins::Dump()
	{
		for (auto& w : _skininfo) {
			Util::PrintMessage("[ %s ]\n", w.first.c_str());
			for (auto& s : w.second) {
				Util::PrintMessage(" -> %s | %s | [%d]\n", s._name.c_str(), s._shortname.c_str(), s._id);
			}
		}
	}

	mapSkinInfo& CSkins::GetData()
	{
		return _skininfo;
	}
}
