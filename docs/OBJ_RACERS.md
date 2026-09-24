# OBJ racers

Native builds check `assets/mods/racers/<assetName>/` before
`assets/mods/racers/<assetName>.ctr`. `assetName` is the name in
`game/CharacterRegistry.c`, for both original and additional roster entries.
A folder does not itself create a new roster entry.

Example:

```text
assets/mods/racers/cortex/
    cortex.obj
    cortex.mtl
    bricks1.png
    helmet.png
    ...
assets/mods/racers/cortex.ctr
```

The loader prefers `<assetName>.obj`, then `model.obj`, then the only OBJ in
the folder. Multiple other OBJ files are considered ambiguous. Missing or
invalid geometry, materials or diffuse images use the existing `.ctr` fallback.
If that is also absent, existing driver-pack fallback behaviour is unchanged.

The OBJ's `mtllib` names its material file. If that file is absent, the loader
tries the OBJ's filename with `.mtl` instead. This handles renamed Blender
exports such as `cortex.obj` still referencing `RocketRacer - Copy.mtl`.
`map_Kd` selects PNG or BMP diffuse textures. Paths are resolved by filename
within the character folder, including references exported as absolute paths.

Textures are decoded with SDL and uploaded as ordinary OpenGL textures. They
do not occupy emulated PS1 VRAM or require CLUTs. They still use host graphics
memory. Images retain their resolution; the existing packet renderer currently
quantizes texture coordinates to 256 steps and clamps them to the image edges.

## Export conventions and current scope

- Export Y-up with the desired forward direction, origin and scale applied.
  The importer preserves the authored origin and proportions. Header scales
  follow the existing loose racer convention (axis extent multiplied by 1000).
  Very large meshes (over 32 units on an axis) or extreme origins are rejected.
- Triangles and convex polygons, positive and negative indices, UVs, `Kd`, and
  the common OBJ per-vertex RGB extension are supported. Triangulate concave
  polygons in the exporter. Back faces and edge-on triangles are culled using
  OBJ face winding (counter-clockwise when viewed from outside), with mirrored
  transforms handled automatically. Export faces oriented outward. Vertex
  shading normals do not control culling; lighting is baked into the
  material/vertex colours.
- Texture alpha supports cutouts: pixels below 50% alpha are discarded, so
  transparent backgrounds reveal the geometry behind them. Partial alpha is
  treated as a hard edge; smooth translucency is not yet supported.
- This initial path draws rigid geometry. Material transparency,
  `map_d`, normal/specular maps, texture tiling, skeletal animation, and CTR's
  special water/reflection/ghost shading are not implemented. The existing
  roster wheel settings still control the game's separately drawn wheels.
- Models use the existing per-player transforms, culling and ordering tables.
  The game's visibility callback also applies, including invisibility power-ups.
  Instance colour effects also apply, including blackening when burned and
  the game's colour recovery, for both textured and untextured parts.
  Squash and recovery use the game's instance transforms. Fully flattened
  models render both face windings because zero height removes their volume;
  normal back-face culling resumes when they regain height.
  Carried TNT uses the registry's driver-pack character for its head height,
  including additional roster slots.
  Triangles crossing the near plane are skipped. Draws stop at the normal
  primitive-buffer guard instead of overrunning it; use modest polygon counts.
- Loaded meshes and textures are cached until shutdown. Restart the game after
  editing a loaded model. No user assets are rewritten by the importer.

## Modelled wheels

For any character with `CHARACTER_FLAG_HAS_WHEELS` disabled in the registry
(including Oxide), four named objects in the same OBJ can spin:
`wheel_fl`, `wheel_fr`, `wheel_rl`, and `wheel_rr`. Named OBJ groups also work.
Other geometry stays rigid. Missing wheel objects are allowed; characters
with CTR wheels enabled keep their OBJ geometry static.

Export with Y up, +Z forward and the wheel axles parallel to X. Keep each
wheel separate and centred geometrically around its axle; the importer uses
its bounding-box centre, since OBJ does not store object pivots. Apply transforms
before exporting and preserve object names. Materials and textures work as usual.

Wheel rotation follows distance travelled along the kart's forward axis and
reverses when backing up. Sideways powerslide movement does not add rotation.
It updates once per simulation frame for players and AI, including offscreen
racers, and freezes with the game. Menu models stay still. This provides wheel
spin only, without steering or suspension animation. Restart after editing assets.
