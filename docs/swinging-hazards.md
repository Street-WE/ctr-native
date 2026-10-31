# Swinging hazards

Name a static level instance with the case-sensitive prefix `Swinging`, for
example `SwingingHammer` or `SwingingHammer02`. Type 0 is supported; no new
instance ID or path data is required. The prefix is checked on both the
exported instance name and the model name. This also supports exporters that
retain `Hammer` as the instance name and `SwingingHammer` as its model name.
All instances sharing a matching model receive the swinging behaviour.

The instance swings about its authored pivot on its **local Z axis**, relative
to its placed orientation. It starts at +60 degrees, reaches -60 degrees after
two seconds, and returns after four seconds. Position and scale are retained.
All matching instances start in phase. The constants at the top of
`game/231/RB_Swinging.c` control the angle, period and driver collision radius.

Collision uses a rotating box around the static model's highest-detail
vertices, expanded by a sphere approximating the kart. It includes both the
shaft and head, and is an approximation rather than triangle-level collision.
The box comes from the exported model and includes its scale and offset from
the pivot. Swing movement is sampled between frames. This does not move any
separate collision quadblocks authored into the track; omit fixed wall/ground
geometry for the moving hammer itself.

Contact calls the normal squash damage handler for players and AI, preserving
the game's shields and immunity rules. Continuous contact does not repeatedly
apply damage; the driver must leave and touch again. Starting a race recreates
the state. Matching instances take precedence over their usual type behaviour.

This handler supports static, uncompressed LEV model geometry. Animated or
delta-compressed models retain their usual type behaviour instead.

Build and run `ctr_swinging_hazard` through CTest for the focused checks.
In Royal Knights Raceway, verify the pivot and swing direction, head and shaft
contact at both extremes and through the centre, a clean miss beside the head,
AI contact, shield/mask protection, pause/resume, and race restart. The model's
box can include empty space around an irregularly shaped head; tune or refine
the collision shape if the drive-through shows overly generous hits.
