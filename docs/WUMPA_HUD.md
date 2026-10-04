# Custom wumpa HUD assets

Native builds support optional replacements in the game's assets directory:

```text
assets/mods/models/wumpa/
    wumpa.obj
    wumpa.mtl
    texture.png
    2d/
        icon.png
```

The loader prefers `wumpa.obj`, then `model.obj`, then the only OBJ in the
folder. The OBJ's `mtllib` references its material file, and `map_Kd` in that
file selects the PNG or BMP diffuse texture. Material files and textures are
resolved by filename within the wumpa folder. If the named material file is
missing, the importer also tries the OBJ filename with an `.mtl` extension.

The 3D model replaces only `STATIC_FRUITDISP` (model ID `0x37`), the rotating
HUD fruit. World pickups keep their existing model. The OBJ preserves its
authored origin, Y-up axes, and proportions; adjust export scale to fit the
HUD. It uses the same importer and rendering limits as [OBJ racers](OBJ_RACERS.md).

The image loader prefers `2d/icon.png`, then `2d/wumpa.png`, then the only PNG
in the `2d` folder. It replaces the flying fruit pickup icon and is fitted to the
original icon's HUD rectangle and retains the existing pickup movement and
scale. Use the original aspect ratio to avoid stretching. Transparent pixels
are supported as alpha cutouts; partial alpha is not smooth translucency.

Both replacements are independent and optional. Missing or invalid assets
keep the original model or image. Assets are cached until shutdown; restart
the game after changing them. No model ID edits are needed.

The runtime model importer accepts OBJ, not glTF/BIN. Export a static pose to
OBJ when using glTF source assets. HUD spinning is applied by the game to the
whole model; animation tracks in an asset are not needed for this rotation.
