# Shoot-to-open shortcuts

The `STATIC_TEETH` instance ID (`0x70`) supports two behaviours:

| Instance name | Behaviour | Barrier terrain |
| --- | --- | --- |
| Starts with `break_`, e.g. `break_wall` | First weapon hit disables collision immediately, fades the model out without playing its animation, and leaves it hidden for the race. | Water (`4`) |
| Any other name | Original animated Tiger Temple door that closes again. | Dirt (`1`) |

Set the **instance name**, not just the mesh or model name. The prefix is
case-sensitive. Keep the `break_` prefix within the exported instance name.

For either barrier, enable **Wall**, **Tiger Temple Door**, and **Invisible
Trigger** on its collision quadblocks, then apply the terrain setting to all
barrier quadblocks. Keep normal terrain on the drivable floor.

On door quadblocks the terrain byte is an access bitmask. Do not use Hardpack
(`11`) for the permanent barrier: it overlaps the original door's bit. All
Water door barriers share bit `4`; they unlock together. Separate permanent
shortcuts that must unlock independently need additional access bits.

The permanent variant ignores repeat hits while fading and after opening.
Its old instance hitbox is excluded from collision checks as soon as access
opens, including while the model is still fading. `TEETH_FADE_DURATION_MS` in
`game/231/RB_Teeth.c` controls the fade duration.
Starting a new race recreates the instances and clears door access flags.
In Relic Race, both variants are hidden and accessible from the start, matching
the original door's behaviour.

To verify in a normal race, shoot each door separately. The permanent barrier
should become passable immediately and fade away, remain
open after waiting or hitting it again, and be closed again in a new race.
The animated door should still open and close without unlocking the permanent
barrier. Also test a potion against one door while the other is open.

Moving bombs, missiles, and airborne potions test their full movement segment
against the marked door quadblocks, with a small weapon radius. This catches
edge hits and fast shots even when the instance's original spherical hitbox
does not cover the full barrier. Blast and potion-shatter radii also test these
quadblocks. Detection uses both sides of the geometry without changing the
wall's own collision orientation. Only Dirt (`1`) and Water (`4`) door quads
participate; ordinary terrain is unaffected.

The standalone `tools/tests/teeth_collision_test.c` harness runs the actual
game collision and GTE routines against an exported level passed as its first
argument. It checks barrier centres, edges, both approach directions, splash
hits, misses, access-mask independence, and crossing an open permanent
instance's centre while fading. Its compile command is at the top of the file.

`tools/tests/teeth_fade_render_test.c` checks actual ghost drawing packets for
coloured level instances with deliberately stale scratch data. The renderer
must initialise the fade mask command even when `colorRGBA` is nonzero;
otherwise unrelated scratch data can be interpreted as GPU commands. The test
covers ordinary and clipped triangles, textured and untextured, throughout
the fade. VRAM copies also reject invalid sizes and coordinates before copying
host memory.
