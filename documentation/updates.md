
# Version History

## 0.12.x Community Update

### 0.12.4.36 | 24/07/16
 * Worldedit now supports copy and paste.
 * Added trigger function `WorldEdit.Copy.Begin`.
 * Added trigger function `WorldEdit.Copy.End`.
 * Added trigger function `WorldEdit.Copy.ApplyCopy`.
 * Added trigger function `WorldEdit.Copy.ApplyMove`.

### 0.12.4.21 | 24/07/16
 * Add block filling functions by providing structure blocks.
 * Added trigger function `WorldEdit.Fill.Begin`.
 * Added trigger function `WorldEdit.Fill.End`.
 * Fixed texture load fail when using multi texture packs.
 * Fixed worldedit not filling everywhere desired.
 * Fixed worldedit remaining `WorldEdit::Fill.End` after fill.
 * WorldEdit now fills places covered with air, not something else.

### 0.12.4.6 | 24/07/16
 * Rewritten the structure of Trigger and thoroughly tested.

### 0.12.3.32 | 24/07/16
 * Updated decease message list.

### 0.12.3.31 | 24/07/16
 * Projectile destroyed items return to the players who deployed the projectile.
 * Fixed projectiles could destroy themselves when deploying.

### 0.12.3.26 | 24/07/16
 * Restored chat window and moved to the lower part of the window.
 * Fixed debug window not resizing and chat window not resizing according to the
   resizing of the windows.

### 0.12.3.21 | 24/07/16
 * Adding "radar" debugging view for players to locate other players, sorted
   in the order of health points from high to low. (Activate with F3)
 * Rearrangement of local save menu interface.
 * Now in survival mode, health points no longer flicker (flash rapidly).

### 0.12.2.16 | 24/07/16
 * Integrated input modules into main pipeline. Users no longer have to receive
   ANR (Application Not Responding) dialogues while loading.
 * Input dialogues have a wider range of input characters.
 * Input dialogues now support continuous input (i.e. generating large numbers
   of characters while pressing a key and holding).
 * Removed rename and description editing in local saves screen.
 * Fixed when input area is null, SIGSEGV will occur when entering backspace.
 * Fixed 5 seconds limit when inputting things.
 * Fixed escape sends null message in chat window.
 * Fixed SIGSEGV when sending a chat message.

### 0.12.2.11 | 24/07/16
 * Integrated loading modules into main pipeline. Users no longer have to receive
   ANR (Application Not Responding) dialogues while loading.
 * While loading, users may now resize the windows.
 * Loading dialogues now have better appearances.

### 0.12.2.6 | 24/07/16
 * Got rid of `GetAsyncKeyState()` and a couple other functions in favour of
   freeGLUT APIs. This may further ensure the portability of this project.
 * Removed deprecated and unused functions `kGetKeyLastPressedTime()` and other
   sort of things.
 * Fixed sometimes inventory menu closing immediately after opening. This is a
   direct yet unintended result of the fix of the keyboard API (Reserving a
   self-defined API has such benefits, you see).

### 0.12.2.1 | 23/07/16
 * Migrated development workspace to Git repository folder.
 * Created testing-oriented resource pack (literally useless)

### 0.12.1 | 22/05/16
 * Fixed people who can't wait can't throw things out.
 * Fixed physics simulation terminate when inputting chat message.
 * Created installer.

## 0.11.x Open Source Update

### 0.11.4 Release Candidate | 22/05/16
 * Updated version numbers to `0.11.x`.
 * Fixed players exporting twice.
 * Arrange the main menu buttons to make them look more friendly.
 * **Added comments for particular functions in GNU style.**

### 0.11.3 Release Candidate | 22/05/16
 * **Added `GNU GPL v3` licenses for all source codes.**

### 0.11.2 Release Candidate | 22/05/16
 * Checked resource manager intactness and is proper.

### 0.11.1 Release Candidate | 22/05/16
 * Gathered player entities all into the level configuration. This would increase
   the ease of manipulating player data directly through JSON.

## 0.10.x Experience Update

### 0.10.45 Zeta | 22/05/16
 * Fixed input dialog acting abnormal when pushing backspace.
 * Fixed null messages could be sent as message.
 * Fixed inresponsiveness for input dialogs, which has a timeout of 5 seconds.
   Input must be completed within 5 seconds.
 * **This project is now functional complete**.

### 0.10.43 Zeta | 22/05/16
 * Fixed backspace not usable in `GuiInputDialog`.
 * Fixed chat available when game is paused.

### 0.10.42 Zeta | 22/05/16
 * Added chat messages that are merely informative.
 * Added chat messages that are custom sent by users.

### 0.10.41.3 Zeta | 22/05/16
 * Fixed player disappearing after reconnection to server.

### 0.10.41.2 Zeta | 22/05/16
 * Fixed chat messages flooding the screen when players are deceased.
 * Fixed in some cases chat messages appear twice.
 * Fixed in some cases informative messages not appearing.

### 0.10.41.1 Zeta | 22/05/16
 * Fundamentally added chat messages (mostly informative things).

### 0.10.37 Zeta | 22/05/16
 * Fixed loading menu not displaying properly.
 * Added "saving" information when saving maps.

### 0.10.36 Zeta | 22/05/16
 * Fixed `SIGSEGV` when deleting an invalid entity.
 * Added world boundaries for `GameMap`.

### 0.10.35 Zeta | 22/05/16
 * Fixed not importing configuration data upon boot.
 * Fixed textures not loading which throws `GL_INVALID_OPERATION`.
 * Designed a new loading interface which is quite visually competitive.

### 0.10.29 Zeta | 22/05/16
 * Design a new GUI texture set to avoid certain issues.
 * Not displaying entity type names statically (not tooltip, the letters that
   are fixed on the inventory) any more.

### 0.10.17 Zeta | 21/05/16
 * Fixed `SIGSEGV` when changing pages through inventory pages.
 * Added resource pack support by putting entities and textures into
   different folders and loading them distinctly.
 * Inventory now displays description instead of hashed name.
 * Highest optimisation (O3) would be banned to avoid certain errors.

### 0.10.15 Zeta | 21/05/16
 * Updated entity folder and texture folder hierarchy.
 * Updated entity generation procedure.

### 0.10.13 Zeta | 21/05/16
 * Fixed `TUSNIL` not working when crossing different computers. This is caused
   by a flaw that when creating UDP servers clients attempt to create
   servers on the target machine, which is not proper.

### 0.10.12 Zeta | 18/05/16
 * Support entity-type generation from CSVs.
 * **Added over 160 types of entities, some of them are interactive.**

### 0.10.4 Zeta | 18/05/16
 * Fixed player flickering after death in multiplayer.
 * Fixed a minor problem causing directory not to iterate.
 * Added recursive directory iteration for entities.

### 0.10.3.15 Zeta | 17/05/16
 * Updated a few JSON-related functions in `GameMap`.
 * Game headers are now broadcasted through the network.

### 0.10.3.11 Zeta | 17/05/16
 * Fixed a `std::bad_alloc` when exiting game in deflate library.
 * Improved network consumption by over 96% through compression of network
   data and flow. Duplicate information is surpressed.
 * Supported lagless multiplayer!

### 0.10.3.1 Zeta | 17/05/16
 * Added inflation and deflation libraries for network process.

### 0.10.1 Zeta | 15/05/16
 * Initial release, would add a few importation tools in this major version.
   Post by the coder: The weekend has passed among the confusing network
   languages and protocols, which tortured the writer quite much.
 * Version number update.
 * Console window no longer shown except in debug mode.

## 0.9.x Multiplayer Update

### 0.9.43 Epsilon | 15/05/16
 * **Supported multiplayer for now.**
 * Now synchronising time between clients and servers.

### 0.9.32.103 Epsilon | 15/05/16
 * Fixed projectiles disappearing mysteriously in server end.
 * Fixed `SIGSEGV` when client first joining server.

### 0.9.32.101 Epsilon | 15/05/16
 * Re-written data-intactness checking in procedures (coding related). This
   enables us to again shorten the code and simplify the procedures.
 * Fixed TNTs can deduct creative players' life. and not deducting survival
   players' life ( a boolean value, so confusing... )
 * Fixed projectiles doubling in multiplayer.

### 0.9.32.93 Epsilon | 15/05/16
 * Fixed players falling into the void after chunk synchoronise.
 * Fixed players' life not changing if not synchoronising.
 * Added `<class_table.h>` for debugging easiness.
 * Added debugging information for entities and entity types.
 * **Change for loops to foreach loops.**

### 0.9.32.79 Epsilon | 15/05/16
 * **Now the multiplayer support is extended from 2-player to n-players!**
 * Fixed a small lag when players are moving because of a problem in broadcasting,
   this is fixed through a small trick in the forest of codes.
 * Fixed potential `SIGSEGV` when saving game.
 * Fixed always throwing `GUID = 0` entities in multiplayer.
 * Fixed particles not disappearing after lifetime reached.

### 0.9.32.76 Epsilon | 15/05/16
 * Fixed entities not inserted into the world bug, which was totally a
   catastrophe and now it is fixed!

### 0.9.32.69 Epsilon | 15/05/16
 * New GUI for paused menu for multiplayer (Save & Exit removed).
 * Fixed `SIGSEGV` when directly leaving from multiplayer by delimiting termination
   procedures upon closing windows.
 * Fixed `SIGSEGV` when opening a world immediately after leaving another by also
   imposing memory locking techniques upon events.

### 0.9.32.63 Epsilon | 15/05/16
 * Fixed server not broadcasting chunk data.
 * Fixed players mysteriously disappearing in multiplayer.

### 0.9.32.61 Epsilon | 15/05/16
 * Fixed `SIGSEGV` when importing unknown entity types in singleplayer.
 * Fixed `SIGSEGV` when importing unknown entity types in multiplayer.
 * Greatly improved physics simulation performance by using an algorithm that
   was previously used, with a higher (*O(n)*) time complexity compared to
   the last algorithm (*O(logn)*), but *1/10* the constant processing speed
   which abandoned the time-consuming red-black-tree.
 * Extended chunk data broadcast period.

### 0.9.32.52 Epsilon | 15/05/16
 * Fixed players remaining in map after disconnection.
 * Fixed players appeared to be deceased after a few seconds of "Game Paused".
 * Now chunk data is announced immediately a client is joined into the server.

### 0.9.32.51 Epsilon | 15/05/16
 * Data intactness verification now includes `NullPointerException`, so `NULL`
   pointers are not necessarily detected alongside with data intactness
   outside the intactness verification procedure.
 * Fixed `SIGSEGV` when inserting a player into multiplayer mode for the first time.
   However, the whether the fix was proper or not is not known yet.

### 0.9.32.47 Epsilon | 15/05/16
 * Fixed entities not synchronising between clients and servers.
 * Fixed players can't be pushed against.
 * Fixed player `GUID`s not syncing between `<netsync.cpp>` and `<engine.cpp>`.
 * Fixed client player inventory restoring to default state every time the chunk
   data is broadcasted.

### 0.9.32.41 Epsilon | 14/05/16
 * Fixed `SIGSEGV` when leaving game if any of the players triggered entity
   deletions in multiplayer mode.
 * Fixed `SIGSEGV` when playing game, in many places.
 * Fixed entities mysteriously disappearing when placing things.

### 0.9.32.34 Epsilon | 14/05/16
 * Random generation now becomes more randomly.
 * Fixed server termination immediately after a client disconnection.

### 0.9.32.33 Epsilon | 14/05/16
 * Fixed not updating `GUID` in `InsertEntity`.
 * Fixed `SIGSEGV` when importing entity JSON data in `<netsync.h>`.
 * Fixed `SIGSEGV` when invoking player in `<engine.cpp>`.
 * No longer mandatory to use spaces between numbers when inputting resolution.
 * Added game exit if connection failed between clients and servers.
 * Added synchoronisation back-end.

### 0.9.32.11 Epsilon | 14/05/16
 * Restored the MULTIPLAYER button that was supposed to be enabled.
 * The `GameMap.IsHost` value finally becomes useful!
 * Added support for server list storage.
 * Added multiplayer graphics interface.

### 0.9.32.2 Epsilon | 14/05/16
 * Completed network dispatcher for servers.

### 0.9.32.1 Epsilon | 14/05/16
 * Let vertical speed deducted by gravity before collision instead of after.
 * Added network dispatcher for servers.

### 0.9.23.6 Epsilon | 10/05/16
 * Changed a trivial JSON manipulation mechanism to refer to network standards
   predictions.

### 0.9.23.3 Epsilon | 10/05/16
 * Fixed player would deal damage after teleporting to a lower place.
 * Fixed projectiles may destroy items with blast power of 0 and entities with
   blast resistance of 0 (We don't want such things to happen).

### 0.9.20.6 Epsilon | 10/05/16
 * A snippet of disabled code containing a new collision reaction system is inserted
   into the physics renderer. It still causes problems, however.

### 0.9.20.5 Epsilon | 10/05/16
 * Added overlap-based collision detection system so overlap should not happen
   any longer.

### 0.9.20.1 Epsilon | 10/05/16
 * Creative players are shown their layers instead of their life because they
   are never allowed to be deducted life.

### 0.9.17.8 Epsilon | 10/05/16
 * Fixed some trivial warnings in included headers and network libraries.
 * Set minimum required view distance to 1.

### 0.9.17.7 Epsilon | 10/05/16
 * Fixed window state changing to full screen after editing the window resolution
   twice. (That is, it acts strange after changing for odd times)
 * Fixed frame rate of rendering (On laggy computers, rendering takes up a lot
   of time, and causes confusion), we sacrifice the render time for physics
   realtime simulation.

### 0.9.17.4 Epsilon | 10/05/16
 * Mysteriously fixed a flaw: Creative inventory open causes `SIGSEGV` after entering
   a Survival map regardless of the player opened the survival inventory
   or not.

### 0.9.17.1 Epsilon | 10/05/16
 * Updated Cross Network Library sockets from GitHub.
 * Added Trusted Users' Scalable Network Interconnection Library from GitHub.
 * Added blocking queue algorithm from GitHub.
 * The fundamental requirements for creating connections have been set up
   theoretically. Further versions would focus on details on realizing the
   multiplayer communication engine.

### 0.9.5 Epsilon | 03/05/16
 * Added `CNL` (Cross Network Library) sockets.

### 0.9.2.1 Epsilon | 02/05/16
 * Fixed full-screen toggle not working properly in options menu.

### 0.9.1 Epsilon / Technical Preview 3 | 02/05/16
 * No critical updates. Only a release for combining all previous versions. This
   version is completely playable and theoretically free of bugs. However
   only single player is allowed in this version.

## 0.8.x Mechanism Update

### 0.8.29.7 Delta | 02/05/16
 * Fixed layer cascading relations not right after players' layer migration.

### 0.8.29.3 Delta | 02/05/16
 * Fixed GUI tooltip not drawing very right.
 * Fixed that `CurrentDirectory` is a fixed string, now it can be dynamically
   indexed and queried.
 * Added map sorting according to last modified time.

### 0.8.29.1 Delta | 02/05/16
 * Added last modified time to `GameMap`.
 * Added system time functions to `<native/time.h>`.
 * Added forbidden layers to `GameMap`.
 * Added layer jumping functions (Using W/S/Up/Down)

### 0.8.29.0 Delta | 02/05/16
 * Game mechanism updates are mostly completed. And meanwhile bugs have mostly
   disappeared and fixed. This is not a major update, but is a very important
   milestone however.
 * 0.8.29.x mainly focus on trivial functions, such as layer jumping, e.t.c.

### 0.8.26.9 Delta | 02/05/16
 * Added ender pearl (experimental!)
 * Fixed sometimes teleportation does not work in trigger.

### 0.8.26.6 Delta | 02/05/16
 * Fixed `SIGSEGV` appears when deploying projectiles or attempting to process
   consequences through triggers.
 * Integrated pending operations into physics renderer.
 * Added thread-safe Insert and Remove operations for `Chunk` and `GameMap`.
 * Added `DeployProjectile` action for triggers.

### 0.8.26.3 Delta | 02/05/16
 * Added inline class functions for data intact checking.
 * Added pended operations for entity insertion and removals. These will guarantee
   then intactness of the data. If not pended, iterators will behave in a
   strange manner. And mostly they will `SIGSEGV`.

### 0.8.23.7 Delta | 02/05/16
 * Added trigger preliminary action `OnGeneration`.
 * Added trigger preliminary action `OnDestruction`.
 * Added trigger preliminary action `Always`.
 * Trigger actions are now complete.

### 0.8.23.6 Delta | 02/05/16
 * Fixed an improper comparison between time in DEPLOYMENT so that animations
   display too many times.
 * Fixed `EntityType` not importing trigger data.
 * Now animations are displayed when projectiles explode. (These settings are
   defined by the configuration files themselves, not the program. You can
   define which to deploy.

### 0.8.23.0 Delta | 01/05/16
 * Fixed an improper inheritance function in `Entity` which causes different
   entities to share the same triggers.

### 0.8.20.16 Delta | 01/05/16
 * Fixed projectiles destroying particles.
 * Added teleportation functions.
 * Added exportation of triggers' JSON datum.

### 0.8.20.11 Delta | 01/05/16
 * Fixed local saves not changing when clicking on a map.
 * Fixed creative players dealt damage through projectiles.
 * Fixed creative inventory not properly hiding entities that are not supposed
   to appear in creative inventory.
 * Added trigger consequential actions.

### 0.8.20.2 Delta | 01/05/16
 * Separated animation for particles and other entities. Other entities' animation
   begins from the commence of the game map, and the particles' animation
   begins from its generation time.

### 0.8.20.1 Delta | 01/05/16
 * Fixed creative players' inventory items mysteriously disappearing after
   procedure (projectile deployment), only because we didn't consider a
   matter of creative players when we first write this snippet, and when
   we added the "creative" function, we didn't fix this.
 * Added particle destruction after deployment.

### 0.8.20.0 Delta | 01/05/16
 * The fall damage module is complete. Now if players attempt to rescue oneself
   midair through rocket boosters and succeeded, i.e. Reach positive Y-axis
   speed (comparatively to the gravity constant).

### 0.8.17.7 Delta | 01/05/16
 * Fixed players would immediately die when falling in air and colliding with
   a nearby object that has small relative velocity.

### 0.8.17.6 Delta | 01/05/16
 * Added fall damage for players (Now when jumping from high cliffs,
   players will receive their damage).

### 0.8.17.2 Delta | 01/05/16
 * Added multi-page inventory support.  
 * Fixed an indexer ignorance on font types in `<graphics/gui.cpp>`.

### 0.8.17.1 Delta | 01/05/16
 * 0.8.17.x are trivial updates that add small functions.
 * Completed inventory support for both creative players and survival players.

### 0.8.14.33 Delta | 01/05/16
 * Added hot-bar in the bottom of the screen.
 * Added inventory tabs for creative player and survival players. However, the
   creative player tabs aren't actually ready yet.

### 0.8.14.10 Delta | 01/05/16
 * Added on-hand texture for `EntityType`.

### 0.8.14.2 Delta | 01/05/16
 * Added creative player support.

### 0.8.14.1 Delta | 30/04/16
 * Added screen size toggle without having to restart.
 * Added complete support for resolution changes. All GUI objects are correctly
   rendered at their corresponding positions, even after a screen size
   change.

### 0.8.9.15 Delta | 30/04/16
 * Fixed memory leak when changing resolution. This is due to a function
   when setting properties for a font-spec, it will generate a new area of
   memory to assign the bitmaps for the font. We now index it so that no
   duplicate assignments would be made.
 * Fixed `SIGSEGV` after changing resolution. This is due to a downside when,
   the pointer indexes are not removed from the indexer after destruction.
   Also, defining R-B trees in the public area causes `std::rb_tree::decrement_pointer`
   to malfunction.

### 0.8.9.8 Delta | 30/04/16
 * Added full screen toggle without having to restart.
 * Added window size toggle without having to restart.

### 0.8.9.7 Delta | /04/16
 * Added resolution changing support. Only resolution changing have been
   supported.
 * Added dynamic update of objects when required to update crucial values.
 * Added dynamic update support in Options menu.

### 0.8.8 Delta | 30/04/16
 * Added platform independent keyboard state detection system.
 * Added ESCAPE key in game paused menu.
 * Added ESCAPE key when typing in dialogue boxes.

### 0.8.7 Delta | 30/04/16
 * Fixed entity on top layers will appear under all entities from adjacent
   chunks when this entity is on the edge of one layer.

### 0.8.5 Delta | 30/04/16
 * Fixed entity count doubles every time you save the map.
 * Fixed removed entities are not removed when you save the map.
   The abovementioned problems all come from the same issue.
 * Added particle class in `EntityType`.
 * Added delay time in `Trigger`.

## 0.7.x Performance Update

### 0.7.40 Gamma | 30/04/16
 * Fixed when projectile deploys, it does not destroy any static entity in any case.
 * Changed obfuscating iterators to AUTO.

### 0.7.33 Gamma | 30/04/16
 * Removed `CollisionOverride` to improve physics simulation efficiency: The
   computational complexity was reduced from *O(n^2)* to *O(n)*.

### 0.7.32 Gamma | 30/04/16
 * Fixed entities crossing others when simulation becomes laggy.

### 0.7.31 Gamma | 30/04/16
 * Fully re-written the physics collision system, all previously displayed
   physics simulation problems have been solved.
 * Restored some functionalities disabled during `0.7.20.x` development which
   were used to simplify the development phase.
 * Updated a few functions to comply to the new standards.

### 0.7.20.29 Gamma | 30/04/16
 * Fixed runtime-error when moving through chunks (New engine).
 * Fixed entities with duplicating position values does not appear in position
   indexer structure (by adding unique data).
 * Fixed position indexer binary search algorithm by toggling a very important
   binary operator, which affected the internal algorithm search comparison.
 * Completely implemented the binary search algorithm and greatly improved the
   performance of searching for adjacent blocks, complexity had reduced from
   *O(n)* to *O(logn)*.

### 0.7.20.23 Gamma | 29/04/16
 * Partially re-written the physics collision engine, updating the reactions to
   the collision activities. Now vibrations and various other bugs are supposed
   to be eliminated.
 * Fixed entities passing colliding objects shortly after collision by adding tolerance.
 * Fixed entities acting strange while colliding by simply swapping a mistaken
   parameter (reversing X-Axis or Y-Axis)
 * Fixed player can leap on the side of the wall.
 * Fixed player can jump through walls by achieving high speeds.

### 0.7.20.14 Gamma | 29/04/16
 * Improved performance for special collision definitions,
   i.e. `CollisionOverride`, `CollisionIgnore`.
 * Added more convenient functions for function availability judging,
   e.g. `bool PhysicsEnabled(...)`...
 * Implemented special collision availability detection system.

### 0.7.20.11 Gamma | 29/04/16
 * Partially re-written the physics collision engine with a preliminary detection
   system. This system does not judge collision after the collision happens,
   it judges it before the collision happens.
 * This version only added the judging operation, not defining what to do.

### 0.7.20.1 Gamma | 28/04/16
 * Added l/r/u/b boundaries position sorting in chunks.

### 0.7.19 Gamma | 28/04/16
 * **Fixed alpha textures import by removing `glAlphaTest(GL_GREATER, 0.1)`.**
 * Further modularized `graphics/render` and `physics/engine` modules.
   These modules were originally integrated in main functions, which made
   the code extremely obfuscated. Now they are separated into several
   source codes, which made the code nice-looking.

### 0.7.16 Gamma | 23/04/16
 * Fixed `SIGSEGV` when invoking `graphicsRenderGame(...)` (invalid Chunks).
 * Fixed `SIGSEGV` (in GDB) when invoking `GameMap::Clear(...)`.
 * Fixed `NullPointerException` in `Entity::InheritFrom(...,...)`.
 * Added animation function such animation begins from (Generation time).
 * Added player insertion for later multiplayer use.

### 0.7.14 Gamma | 23/04/16
 * **Fixed `SIGSEGV` when invoking `UpdateProjectileDeployment` (for now).**
 * Fixed viewport not moving when mouse is out of window.
 * **Fixed chunk-edge snapping (or literally, bug) by integrating all rendering chunks.**
 * Fixed projectiles sometimes run at extremely high speed with the same trajectory.
   The problem was: When you remove an entity from the structure, it's hard to
   keep the iterator in the proper place without spending a long time to code it.
   Therefore there is a batch process to delete entities marked as deleted. However,
   when it was first implemented, those that are marked as deleted weren't regarded
   as deleted by the engine itself.
 * Fixed not entirely deleting entities when invoking `GameMap::RemoveEntity(...)`.
 * Removed 10m/s limit due to most related bugs' fix.
 * **Migrated `WINTHREADAPI` to `C++11` `<thread>`.**

### 0.7.6 Gamma | 23/04/16
 * **Added custom sleep function for this project.**
 * Added nanosecond-precision sleep functions for this project.
 * Frame rate no longer dynamically change with the render burden. It is mostly fixed.
 * Physics rendering made more smoothly.
 * **Migrated to `TDM-GCC` in favour of `GTHREAD` from `MinGW`.**

## 0.6.x GUI Update

### 0.6.30 Beta / Technical Preview 2 | 23/04/16
 * Disabled button for multiplayer.

### 0.6.25 Beta | 23/04/16
 * Fixed multi players appearing in game.
 * Fixed not updating after renaming or editing description.
 * Fixed not turning page after pressing relevant buttons.
 * Fixed not updating item selection after page switching.
 * Added parameter inclusion support for `graphics/gui.h`.
 * Added game map invocation support for single player.
 * Added disabled button mode for game selection menus.
 * Added complete support for game selection menus.
 * Given a mode for object rendering support when textures are not provided.

### 0.6.23 Beta | 22/04/16
 * Fixed invalid type name assignment in `Entity::InheritFrom(...)`.
 * Fixed not properly exporting Inventory in `Entity::ExportToJson(...)`.
 * Added save `level.json` function.
 * Added save `chunk_*.JSON` function.
 * Added game map saving function.
 * Game map saving does not save computer-generated entities (i.e. new Projectiles).
 * Temporarily disabled projectile deployment due to further Runtime Errors (Fix in 0.7.x).

### 0.6.22 Beta | 18/04/16
 * Fixed player vibration when landing on land.
 * Added game pause menu and is functional.
 * Changed inventory slots from 10 to 9 and indexing from 1 to 9.

### 0.6.18 Beta | 17/04/16
 * Fixed Runtime Error when (not occasionally) deploying projectiles by serializing deployment procedure.
 * Fixed physics engine for more realistic friction simulation.
 * Fixed player vertically jumping on walls.
 * Fixed player stuck in walls when landing too fast by improving performance.
 * Added friction and elasticity configuration for all 4 sides.
 * Added different configuration for (Damage/Motion/Blast) projectile deployment power.
 * **Improved visual performance by parallel processing different routines.**

### 0.6.15 Beta | 17/04/16
 * Fixed Runtime Error when containing too many entities:
 * The upper limit of entity counts is *3.1*10^5*, which can hold at least 90 chunks.
 * Passed stress test for tons of entity insertions.
 * Ignoring physics render for unimportant (Non-player) entities when chunk is too full (>4096 entities).

### 0.6.11 Beta | 17/04/16
 * **Removed `operator=` due to `std::move` and `std::forward` operators
   in `C++11` from `rapidjson/document.h`**
 * Updated the options interface to further improve scalability.
 * Updated the `ToString()` function for better compatibility.
 * Updated `native/keystate2ascii.h` for better compatibility.

### 0.6.10 Beta | 17/04/16
 * Fixed stall and improper debugging information in `OptionsGui`.
 * **Added `GenericValue::GenericValue(std::string)` function
   in `rapidjson/document.h`**
 * Added full screen mode.
 * Added main configuration import procedure.
 * Added main configuration export procedure.

### 0.6.6 Beta | 17/04/16
 * **Fixed improper leaving procedures in `glutMainLoop()`.**
 * Added game background.
 * Added options menus.
 * **Migrated obsolete `GLUT` (x86) to `FreeGLUT` (x86/x86_64)**

### 0.6.5 Beta | 17/04/16
 * Added Loading GUI.
 * Added entity loading information for loading GUI.
 * Temporarily disabled projectile deployment for not RE-ing.

### 0.6.4 Beta | 17/04/16
 * Added GUI toolkit for building menus.
 * Added native font rendering for GUI toolkit.
 * Added Main GUI (Yet incomplete, but functional).
 * Fixed Cartesian system inproper dictation in GUIs.

### 0.6.3 Beta | 14/04/16
 * Fixed player regenerating too much and exceeding maximum allowed life.
 * Added entity inheritance from other entity types and entities.

### 0.6.2.2 Beta | 14/04/16
 * Fixed player still could launch projectiles after decease.
 * If the player deceased, he would have to stay at the original place.

### 0.6.2.1 Beta | 13/04/16
 * Added life system for players.

### 0.6.1 Beta | 13/04/16
 * Added blast resistance to blocks.
 * Added destruction of (static blocks) entities by projectiles.

## 0.5.x Chunky Update

### 0.5.19.1 Alpha | 13/04/16
 * Fixed Runtime Error when using slot *#0*.
 * Added (actually restored) projectile deployment (literally, explosion)
 * Projectile deployment now supports customization!

### 0.5.18.3 Alpha | 13/04/16
 * Added 2 more blocks for development diversity.
 * Added slot changing for in-game (static) entity insertion.

### 0.5.18.2 Alpha | 13/04/16
 * Fixed Runtime Error when going too far from spawn point (by limiting X-axis speed)
 * Fixed not consuming blocks when putting blocks

### 0.5.18.1 Alpha | 13/04/16
 * Fixed inventory not importing.
 * Added (actually restored) projectile launch.
 * Added in-game block insertion.
 * Projectile launching now supports customization!

### 0.5.17.4 Alpha | 13/04/16
 * Fixed Runtime Error when invoking more than 16 blocks at one chunk file.
 * Fixed Runtime Error when invoking `rapidjson` commands.
 * Redesigned the `ImportJsonData()` functions.
 * Added a global header where things used by everyone are declared.

### 0.5.17.3 Alpha | 13/04/16
 * Fixed Runtime Error when starting from command line since last fix.

### 0.5.17.2 Alpha | 13/04/16
 * Fixed Runtime Error when starting from Windows Explorer.

### 0.5.17.1 Alpha | 13/04/16
 * Fixed not correctly ignoring entities on X-axis.
 * Fixed `NullPointerException` when loading large amount of entities.
 * Fixed dead loop when crashing on entities because of
   small `CollidingIgnoranceValue` in `<physics/engine.cpp>`.

### 0.5.16 Alpha | 13/04/16
 * Fixed a reserved code snippet remained when changing map format.
 * Added rendering layering support.
 * Added physics simulation layering support
 * Added individual settings for entities.
 * Improved graphics performance by ignoring unanimated objects when animating.

### 0.5.15.1 Alpha | 13/04/16
 * Introduced frame-based animation rendering.

### 0.5.15 Alpha | 12/04/16
 * Fixed JSON readers not reading things other than the first key.
 * Fixed `SIGSEGV` when reaching outer chunks initially out of view distance.
 * Added individual settings for entity classes { Player, Projectile }.
 * Improved overall performance by knocking most things from *O(n)* to *O(logn)*.
 * Improved graphics performance by ignoring out-of-view objects when posting render.

### 0.5.14 Alpha | 11/04/16
 * **Renewed chunk format and level format.**
 * Changing code style and indentation (from 8 to 4 :P).
 * Changing FLOAT values into DOUBLE.
 * Improved overall performance by ignoring entity simulation out of view distance.
 * Removed old entities and reload is pended for final update.
 * Removed projectile summon and deployment support and is pended for a later update.

## 0.3.x Physics Update

### 0.3.8 Alpha / Technical Preview 1
 * Fixed memory leak in projectile deployment.
 * Updated physics engine so that player movement no longer look laggy.

### 0.3.7 Alpha
 * Updated physics engine to fix momentum collision.
 * Entities no longer stick together, they split apart when colliding.
 * Added projectile summon support.
 * Added projectile deployment (explosion) support.

### 0.3.6 Alpha
 * Updated physics engine to fix friction.
 * Game mechanism engine updated to support mouse and keyboard.

### 0.3.5 Pre-Alpha
 * Imported over 100 new entity types.
 * First playable map created.

### 0.3.0 Pre-Alpha
 * Supported momentum collision (Bugged).
 * Supported friction (Bugged).
 * Supported elasticity.

## 0.2.x Graphics Update

### 0.2.0 Pre-Alpha
 * Supporting PNG textures instead of BMP textures.
 * Supported alpha textures / transparency.
 * Renewed physics engine to support collision.

## 0.1.x Physics Update

### 0.1.0 Pre-Alpha
 * Initial commit of physics simulation engine.
 * Initial commit of OpenGL rendering engine.
 * Basic OpenGL functions introduced.
 * Importing of JSON entities.
 * Importing of JSON maps.
 * Importing of BMP textures.
