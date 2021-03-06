I got a ton of work to do. Stuff that is ~~crossed out~~ means it's done.   

## Little things

* ~~Renderer & Game: separate weapon FOV (g_weaponFov) for aesthetic purposes~~
* ~~Materials: `fromRgb` parameter for material authors. You can specify RGB colours instead of a texture map for development purposes~~
* Game: `trigger_bind` map entity
* Game: simple breakable entity
* Dmap: fix `func_static` inline collision
* ~~Dmap: add back `origin` brushes from Quake~~

## Game code

* Weapons: nativisation
* Weapons: tools from Odljev
* AI: nativisation
* AI: Recast navigation

## Physics

* AFs/ragdolls from model defs
    * More concrete concept of a hitbox
    * Auto-generate AF file from model def?
* Animation-driven ragdolls
    * Push bones towards the current animation pose (mode 1)
    * Push bones based on the delta (current anim frame - last anim frame transform) (mode 2)
    * Blending between mode 1 and mode 2
    * Controlling mode 2's intensity (multiply the forces)

## Rendering

* Game & Renderer: vegetation system
    * Basic vegetation instances
        * Note: first get a basic clientside entity concept
    * Batch rendering
        * Idea: `idRenderBackend::DrawSingleInteraction` binds textures & shaders and essentially renders a surface, start looking there
        * Idea: `idRenderBackend::DrawElementsWithCounters` does the actual draw call, either modify it or make a version of it that ultimately calls `glDrawElementsInstancedBaseVertex`
        * Idea: `BatchRenderEntity` which contains a render entity reference and a list of transforms & other render data for each instance
        * Note: shaders will have to be modified to support this
    * Interactive vegetation instances, that the player can 'squash'
* Game & Renderer: optional day/night cycle
    * Different envprobe sets for day & night
    * Sun flares
    * Procedural sky
* Materials & Renderer: ability to specify a custom interaction shader
    * Idea: `idRenderBackend::DrawSingleInteraction` binds the interaction shader
    * Note: there can be multiple interaction shaders, this may be tricky
* Materials & Renderer: proper water
* Materials & Renderer: terrain texture layers
* ~~UI: render-to-texture GUIs, as an alternative to unprojected GUIs which only work on flat surfaces~~
* UI: RmlUI integration to replace SWF
    * Main menus
    * GUI surfaces
    * In-game HUD
* Game & Renderer: very easy-to-use rendermap entities (e.g. automatic func_monitor)
    * ~~Automatic material generation for curved GUI surfaces~~
    * Automatic material generation for camera render targets
* Game & Renderer: dome-based sky

## Asset authoring

* GUI model import tool
    * Drag'n'drop support
    * Conveniently add/edit/remove animations, animation frame events and so on
    * Preview the model
* In-game vegetation editor unless we integrate a sprinkle tool into TrenchBroom
* Enhanced light editor
* Enhanced TrenchBroom support 
    * MD5 model support 
    * hide texture images, only show fully qualified materials
    * visualisation wireframes for light frustums and others
* New model formats:
    * md5static, for static models 
    * md5terrain, for terrain that can be chopped up by the engine internally

## Audio

* Deus Ex 1-style dynamic music system
* Cool DSP stuff for the OpenAL sound backend
    * DSP presets for rooms
    * Low-pass filters for far-away sounds
    * More options for sound shaders

## Networking-related

* Co-op
* Clientside entities
    * May only think in `idGameLocal::ClientRunFrame`
    * May never modify server/game entities
    * Are in a separate array than server/game entities
    * Can be spawned from `idGameLocal::MapPopulate`, among other places

## Far far in the future, possible considerations

* PhysX or Bullet physics
* C# scripting with .NET 5 or .NET 6
* plugin system
* HRTF
* animation-driven ragdolls for more interesting character death animations
