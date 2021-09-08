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

constexpr int sizeOfVeg = sizeof( VegetationObject );
constexpr int sizeOfRE = sizeof( renderEntity_t );

renderEntity_t VegetationObject::RenderEntity = renderEntity_t{};

// ================
// VegetationObject::ctor
// ================
VegetationObject::VegetationObject()
{
	flags.isActive = false;
}

// ================
// VegetationObject::Activate
// ================
void VegetationObject::Activate( const idVec3& objectOrigin, const idMat3& objectAxis, 
								 const idDict& spawnArgs, uint16_t objectIndex )
{
	index = objectIndex;
	flags.isActive = true;

	origin = objectOrigin;
	axis = objectAxis;

	flags.isStatic = spawnArgs.GetBool( "static", true );
	flags.isSolid = spawnArgs.GetBool( "solid", false );
	flags.canInstantiate = spawnArgs.GetBool( "instanced", true );
	flags.canStepOn = spawnArgs.GetBool( "canStep", false );

	windEffect = spawnArgs.GetInt( "windEffect", 0 );
	windDirection = spawnArgs.GetFloat( "windYaw", 0.0f );
	renderDistance = spawnArgs.GetInt( "renderDistance", 2048 );

	if ( renderHandle == -1 )
	{
		const char* modelName = spawnArgs.GetString( "model" );
		SetupRenderEntity( renderModelManager->FindModel( modelName ) );
	}
}

// ================
// VegetationObject::Deactivate
// ================
void VegetationObject::Deactivate()
{
	flags.isActive = false;

	if ( renderHandle != -1 )
	{
		gameRenderWorld->FreeEntityDef( renderHandle );
		renderHandle = -1;
	}
}

// ================
// VegetationObject::UpdateRenderEntity
// ================
void VegetationObject::UpdateRenderEntity( renderEntity_t* renderEntity )
{
	const idPlayer* player = gameLocal.GetLocalPlayer();
	const idVec3& playerOrigin = player->GetPhysics()->GetOrigin();

	renderEntity->customShader = nullptr;

	/*if ( (origin - playerOrigin).Length() > renderDistance )
	{
		// A bit of a hack for now :)
		renderEntity->customShader = declManager->FindMaterial( "textures/tools/nodraw" );
	}*/
}

// ================
// VegetationObject::ModelCallback
// ================
bool VegetationObject::ModelCallback( renderEntity_t* renderEntity, const renderView_t* renderView )
{
	VegetationObject* veg = gameLocal.vegetationManager.GetVegetationObject( renderEntity->entityNum );
	if ( nullptr != veg )
	{
		veg->UpdateRenderEntity( renderEntity );
		return true;
	}

	return false;
}

// ================
// VegetationObject::SetupRenderEntity
// ================
void VegetationObject::SetupRenderEntity( idRenderModel* model )
{
	memset( &RenderEntity, 0, sizeof( RenderEntity ) );

	RenderEntity.origin = origin;
	RenderEntity.axis = axis;
	RenderEntity.callback = ModelCallback;
	RenderEntity.hModel = model;
	RenderEntity.entityNum = index;
	RenderEntity.bounds = model->Bounds( &RenderEntity );

	if ( renderHandle == -1 )
	{
		renderHandle = gameRenderWorld->AddEntityDef( &RenderEntity );
	}
	else
	{
		gameRenderWorld->UpdateEntityDef( renderHandle, &RenderEntity );
	}
}
