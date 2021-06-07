I got a ton of work to do.  

## Little things

* ~~Renderer & Game: separate weapon FOV (g_weaponFov) for aesthetic purposes~~
* ~~Materials: `fromRgb` parameter for material authors. You can specify RGB colours instead of a texture map for development purposes~~
* Game: trigger_bind map entity
* Game: simple breakable entity
* Dmap: fix func_static inline collision

## Rendering

* Game: vegetation system
* Game: optional day/night cycle
* Materials & Renderer: ability to specify a custom interaction shader
* Materials & Renderer: proper water
* ~~UI: render-to-texture GUIs, as an alternative to unprojected GUIs which only work on flat surfaces~~
* UI: RmlUI integration to replace SWF
* Game & Renderer: very easy-to-use rendermap entities (e.g. automatic func_monitor)

## Asset authoring

* GUI model import tool
* in-game vegetation editor unless we integrate a sprinkle tool into TrenchBroom
* enhanced light editor
* enhanced TrenchBroom support
* new model formats: md5static for static models and md5terrain for terrain, that can be chopped up by the engine internally

## Audio

* Deus Ex 1-style dynamic music system
* Cool DSP stuff for the OpenAL sound backend

## Networking-related

* Co-op
* Clientside entities

## Far far in the future, possible considerations

* PhysX or Bullet physics
* C# scripting with .NET 5 or .NET 6
* plugin system
* HRTF
