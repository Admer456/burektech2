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

class VegetationObject final
{
public:
	// Vegetation instances aren't allocated dynamically for performance
	VegetationObject();
	~VegetationObject() = default;

	// Sets up this vegetation object
	void		Activate( const idVec3& objectOrigin, const idMat3& objectAxis, 
						  const idDict& spawnArgs, uint16_t objectIndex );
	// Frees this vegetation object slot and frees the render entity
	void		Deactivate();
	// Updates the render entity based on flags
	void		UpdateRenderEntity( renderEntity_t* renderEntity );
	// Called by the renderer to update the associated render entity
	static bool	ModelCallback( renderEntity_t* renderEntity, const renderView_t* renderView );

	uint16_t	GetIndex() const { return index; }
	bool		IsActive() const { return flags.isActive; }

private:
	void		SetupRenderEntity( idRenderModel* model );

private:
	idVec3		origin{ vec3_origin }; // position 
	idMat3		axis{ mat3_identity }; // orientation + scale

	uint16_t	index{ 0 }; // index into VegetationManager::objects
	qhandle_t	renderHandle{ -1 }; // render entity handle

	// Mapped between 0.0 and 8.0
	// Increase by 1 means increase by 0.03125
	byte		windEffect{ 0 };
	// Encoded yaw
	// Increase by 1 means increase by 1.4 degrees
	byte		windDirection{ 0 };
	// Distance at which this disappears
	// Is mapped to in-game units
	uint16_t	renderDistance{ 2048 };

	struct		VegetationFlags
	{
		bool	isActive		: 1; // this slot is taken
		bool	isStatic		: 1; // completely static, never updates
		bool	isSolid			: 1; // player can walk over it
		bool	canInstantiate	: 1; // can use instanced rendering
		bool	canStepOn		: 1; // when the player walks over this, it bends (grass, small bushes)
	} flags;

	// sizeof(VegetationObject) was at 60, so bump it up to 64
	uint16_t	padding{ 0 };

	// Doom 3 normally stores an instance of this per each 
	// game entity, but since we don't wanna update vegetation
	// instances every frame, this can be static to save memory
	static renderEntity_t RenderEntity;
};
