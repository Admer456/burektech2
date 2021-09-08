/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.
Copyright (C) 2021 Admer Šuko

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "precompiled.h"
#pragma hdrstop

#include "../Game_local.h"

// ================
// ClientGame::Init
// ================
void ClientGame::Init()
{
	for ( auto& ent : entities )
	{
		ent = nullptr;
	}
}

// ================
// ClientGame::Shutdown
// ================
void ClientGame::Shutdown()
{

}

// ================
// ClientGame::IsClientClass
// ================
bool ClientGame::IsClientClass( const char* className ) const
{
	return idStr::Icmpn( className, "cs_", 3 ) == 0;
}

// ================
// ClientGame::RegisterEntity
// ================
void ClientGame::RegisterEntity( ClientEntity* ent, const idDict& spawnArgsToCopy )
{
	for ( int i = 0; i < MAX_CENTITIES; i++ )
	{
		if ( nullptr == entities[i] )
		{
			ent->entityNumber = i;
			entities[i] = ent;
			return;
		}
	}

	// Remove the entity next frame
	ent->PostEventMS( &EV_Remove, 0 );

	common->Warning( "Client entities full! (%d)", MAX_CENTITIES - 1 );
}

// ================
// ClientGame::UnregisterEntity
// ================
void ClientGame::UnregisterEntity( ClientEntity* ent )
{
	int number = ent->entityNumber;

	if ( number < 0 || number >= MAX_CENTITIES )
	{
		return common->Warning( "Tried unregistering client entity with number %d", number );
	}

	if ( nullptr == entities[number] )
	{
		return common->Warning( "Tried unregistering client entity that doesn't exist (%d)", number );
	}

	// Unlink this entity from our list
	entities[number] = nullptr;
}

// ================
// ClientGame::RunFrame
// ================
void ClientGame::RunFrame()
{
	for ( auto& ent : entities )
	{
		if ( nullptr == ent )
			continue;

		ent->Think();
	}
}
