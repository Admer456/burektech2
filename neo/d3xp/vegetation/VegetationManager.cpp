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

#include "../game_local.h"

// ================
// VegetationManager::Init
// ================
void VegetationManager::Init()
{

}

// ================
// VegetationManager::Shutdown
// ================
void VegetationManager::Shutdown()
{

}

// ================
// VegetationManager::Populate
// ================
void VegetationManager::Populate( const idMapFile* mapFile )
{
	int numEntities = mapFile->GetNumEntities();

	for ( int i = 1; i < numEntities; i++ )
	{
		const idDict& dict = mapFile->GetEntity( i )->epairs;
		SpawnFromMapEntity( dict );
	}
}

// ================
// VegetationManager::Count
// ================
int VegetationManager::Count() const
{
	int number = 0;

	for ( const auto& veg : objects )
	{
		if ( veg.IsActive() )
		{
			number++;
		}
	}

	return number;
}

// ================
// VegetationManager::IsVegetationClass
// ================
bool VegetationManager::IsVegetationClass( const char* className ) const
{
	return idStr::Icmpn( className, "veg_", 4 ) == 0;
}

// ================
// VegetationManager::SpawnFromMapEntity
// ================
void VegetationManager::SpawnFromMapEntity( const idDict& spawnArgs )
{
	if ( idStr::Icmp( spawnArgs.GetString( "classname" ), "veg_generic" ) )
	{
		return;
	}

	int amount = spawnArgs.GetInt( "amount", 1 );
	if ( amount <= 0 )
	{
		amount = 1;
	}

	if ( amount == 1 )
	{
		SpawnIndividual( spawnArgs );
	}
	else
	{
		SpawnCircle( spawnArgs );
	}
}

// ================
// VegetationManager::SpawnCircle
// ================
void VegetationManager::SpawnCircle( const idDict& spawnArgs )
{
	idRandom rand( 0x455 );
	VariationList modelStringList;
	int numModelVariations = 0;

	idVec3 origin;
	idMat3 axis;

	idDict spawnArgsCopy = spawnArgs;
	int amount = spawnArgs.GetInt( "amount", 200 );
	float radius = spawnArgs.GetFloat( "radius", 500.0f );
	const idVec3& spawnerOrigin = spawnArgs.GetVector( "origin" );

	// The mapper can set multiple models, like:
	// model  : models/tree1.ase
	// model2 : models/tree2.ase
	// model3 : models/tree3.ase
	numModelVariations = ModelVariations( spawnArgs, modelStringList );

	for ( int i = 0; i < amount; i++ )
	{
		origin = spawnerOrigin;

		// Randomise the origin a little bit
		origin.x += rand.CRandomFloat() * radius;
		origin.y += rand.CRandomFloat() * radius;

		if ( CalculateTransformData( spawnArgs, origin, axis ) )
		{
			// Set a random model
			spawnArgsCopy.Set( "model", modelStringList[rand.RandomInt( numModelVariations )] );

			SpawnObject( origin, axis, spawnArgsCopy );
		}
	}
}

// ================
// VegetationManager::SpawnIndividual
// ================
void VegetationManager::SpawnIndividual( const idDict& spawnArgs )
{
	// CalculateTransformData assumes origin is set
	idVec3 origin = spawnArgs.GetVector( "origin" );
	idMat3 axis;

	if ( CalculateTransformData( spawnArgs, origin, axis ) )
	{
		SpawnObject( origin, axis, spawnArgs );
	}
}

// ================
// VegetationManager::GetVegetationObject
// ================
VegetationObject* VegetationManager::GetVegetationObject( int index )
{
	if ( index < 0 || index >= MaxVegetationObjects )
	{
		return nullptr;
	}

	return &objects[index];
}

// ================
// VegetationManager::CalculateTransformData
// ================
bool VegetationManager::CalculateTransformData( const idDict& spawnArgs, idVec3& outOrigin, idMat3& outAxis )
{
	idRandom random( 0x22 );

	outAxis = mat3_identity;

	// "angle" keyvalue sets yaw
	if ( spawnArgs.FindKey( "angle" ) )
	{
		float angle = spawnArgs.GetFloat( "angle", -360 );
		// Negative = random range from 0 to abs(angle)
		if ( angle < 0 )
		{
			angle = -angle * random.RandomFloat();
		}

		outAxis = idAngles( 0.0f, angle, 0.0f ).ToMat3();
	}
	// Unlikely that someone will set this, but still gotta consider it
	else if ( spawnArgs.FindKey( "angles" ) )
	{
		outAxis = spawnArgs.GetAngles( "angles" ).ToMat3();
	}

	// Scale
	{
		float scale = spawnArgs.GetFloat( "scale", -2.5f );
		float minScale = spawnArgs.GetFloat( "minScale", 0.2f );
		// Negative = random range from 0 to abs(scale)
		if ( scale < 0 )
		{
			scale = -scale * random.RandomFloat() + minScale;
		}

		outAxis *= scale;
	}

	// Should raycast into the ground
	{
		trace_t results;
		float normalAlignment = spawnArgs.GetFloat( "normal", 0.0f );
		int cast = spawnArgs.GetInt( "cast", VTT_CastDown );
		idVec3 direction( 0.0f, 0.0f, -1.0f );

		if ( cast != VTT_None )
		{
			if ( cast == VTT_CastUp )
			{
				direction.z = -1.0f;
			}

			// Do a simple line trace
			gameLocal.clip.TracePoint( results, outOrigin, outOrigin + direction * 1024.0f, MASK_SOLID | CONTENTS_WATER, nullptr );

			// Can't spawn in air nor intersecting with water
			if ( results.fraction == 1.0f || results.c.contents & CONTENTS_WATER )
			{
				return false;
			}

			// We know where we're supposed to be now :)
			outOrigin = results.endpos;

			// This code is untested, let's leave it out for now
#if 0
			// Also align the vegetation object along the surface normal
			const idVec3 up = idVec3( 0.0f, 0.0f, 1.0f );
			const idVec3& normal = results.c.normal;
			float normalAngle = idMath::ACos( normal * up ) * idMath::M_RAD2DEG;

			idVec3 rotationAxis = normal.Cross( up );
			rotationAxis.Normalize();

			idRotation rotation = idRotation( vec3_origin, rotationAxis, normalAngle * normalAlignment );
			outAxis *= rotation.ToMat3();
#endif
	}
}

	return true;
}

// ================
// VegetationManager::ModelVariations
// ================
int VegetationManager::ModelVariations( const idDict& spawnArgs, VariationList& outStringList )
{
	if ( !spawnArgs.FindKey( "model" ) )
	{
		return 0;
	}

	outStringList.Append( spawnArgs.GetString( "model" ) );

	int count = 2;
	while ( count < 8 )
	{
		idStr modelString = idStr( "model" ) + count;
		if ( !spawnArgs.FindKey( modelString.c_str() ) )
		{
			return count - 1;
		}

		outStringList.Append( spawnArgs.GetString( modelString ) );
		count++;
	}

	return count;
}

// ================
// VegetationManager::SpawnObject
// ================
void VegetationManager::SpawnObject( const idVec3& objectOrigin, const idMat3& objectAxis, const idDict& spawnArgs )
{
	VegetationObject& veg = FindFree();
	if ( veg.IsActive() )
	{
		gameLocal.Printf( "Reached too many vegetation objects in the level\n" );
		return;
	}

	veg.Activate( objectOrigin, objectAxis, spawnArgs, &veg - objects );
}

// ================
// VegetationManager::FindFree
// ================
VegetationObject& VegetationManager::FindFree()
{
	for ( auto& veg : objects )
	{
		if ( !veg.IsActive() )
		{
			return veg;
		}
	}

	return objects[0];
}
