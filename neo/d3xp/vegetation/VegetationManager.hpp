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

#pragma once

class VegetationObject;

// How to handle positioning vegetation objects on the ground
enum VegetationTestTypes
{
	VTT_None = 0,
	VTT_CastDown,
	VTT_CastUp
};

//===============================================================================
// 
// VegetationManager
// 
// Handles all vegetation objects, spawns them, deletes them, and so on.
// Vegetation objects are static, they should never move.
// 
//===============================================================================
class VegetationManager final
{
public:
	void				Init();
	void				Shutdown();

	// Populates the map with vegetation
	void				Populate( const idMapFile* mapFile );
	// @returns How many vegetation objects there are in the map
	int					Count() const;
	// @returns Whether className belongs to the vegetation system or not
	// This is checked by the game to skip spawning vegetation entities
	// and let them be handled by the vegetation system instead
	bool				IsVegetationClass( const char* className ) const;

	// Spawns vegetation object(s) from a map entity
	// It can call SpawnCircle
	void				SpawnFromMapEntity( const idDict& spawnArgs );

	// Spawns vegetation objects scattered in a circle
	// Raycasts downwards from the circle area to check for the ground
	// The starting position should ideally be somewhere above the terrain,
	// but NOT so that the sky clips the circle
	void				SpawnCircle( const idDict& spawnArgs );

	// Spawns an individual vegetation object, optionally raycasts downwards
	// by the "plantDown" keyvalue
	void				SpawnIndividual( const idDict& spawnArgs );

	// @returns A vegetation object if the index is valid, nullptr otherwise
	VegetationObject*	GetVegetationObject( int index );
	
	static constexpr size_t MaxVegetationObjects = BIT( 16U );

private:
	static constexpr size_t MaxModelVariations = 8;
	using VariationList = idStaticList<idStr, MaxModelVariations>;

private:
	// Calculates vegetation transforms based on keyvalues
	// @returns Whether it can spawn or not
	bool				CalculateTransformData( const idDict& spawnArgs, idVec3& outOrigin, idMat3& outAxis );
	// Extracts model variations from keyvlaues
	// @returns The number of model variations
	int					ModelVariations( const idDict& spawnArgs, VariationList& outStringList );
	// Actually spawns a vegetation object
	void				SpawnObject( const idVec3& objectOrigin, const idMat3& objectAxis, const idDict& spawnArgs );
	// Finds a free vegetation slot
	VegetationObject&	FindFree();

	VegetationObject	objects[MaxVegetationObjects];
};
