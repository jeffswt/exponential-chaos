
# Game Maps

## Brief

It is what game maps are used as ultimate indexers that index the positions and
definitions of entities. They tell the program the critical data of the game
map, often physics constants, save time et cetera.

Game maps are split into two parts, or precisely, ```n+1``` parts. These parts
describe all entities and properties, which provide us with the game experience.
Do not attempt to break internal structures, though. These parts are:

 * The level data, containing properties and players, which is convenient to index.
 * The chunk data, split into many different files for ease of reading and saving.

There is a downside in the game map loading mechanism. It is written and saved
in **strict JSON markup language**. Therefore any trivial error caused in an
individual entity would cause the entire chunk to fail to load. This is what
actually confuses me. However, for the ease of reading, editing and programming,
this mechanism retains and would remain for a comparatively long time.

## Level Format

The level format, as introduced before, contains the critical information of
the map, and indexes the players hereby. However, other entities could also be
stored here, but would not remain here after an automatic save (by the program).
Players should appear here and only here, for the ease of editing players' data.

### Definitions

| Tags              | Purposes                                                                                                                                                                                                                                                                   |
| :---------------- | :--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Name              | The name of the world, described in the left side of the world save menu.                                                                                                                                                                                                  |
| Description       | A small line of text, describing the purpose of the map (Only visible to the user, the program actively ignores this).                                                                                                                                                     |
| GravityConst      | The gravitational constant, alias *'g'* in newton physics.                                                                                                                                                                                                                 |
| RegenerationValue | A real number, by default this value would be added to all players every second if it has not deceased.                                                                                                                                                                    |
| ModifyTime        | 32-bit C style timestamp, the last time the user saved the map through the program. This defines the sequence of maps sorted in the menu.                                                                                                                                  |
| WorldBoundary(.*) | After a physics enabled entity exceeds this limit, it would appear from the other side. Exempli gratis it would appear from the bottom if it exceeds the top limit.                                                                                                        |
| ForbiddenLayers   | A list of 32-bit integers, marking the layers that a player should not move to (If a player is already at that layer, nothing would happen.), however, players could be arranged by code to stay in that layer. Using this mechanism can lock players in a specific layer. |
| Chunks            | A list of strings describing the file names of the chunk JSON files.                                                                                                                                                                                                       |
| EntityList        | A list of entities. If this file is program-generated, the list would contain only players and nothing else.                                                                                                                                                               |

### Example

```JSON
{
    "Name": "A Hello World",
    "Description": "This is a world meaning to say HELLO.",
    "GravityConst": 9.8,
    "RegenerationValue": 2.0,
    "ModifyTime": 1464000000,
    "WorldBoundaryLeft": -32767.0,
    "WorldBoundaryRight": 32767.0,
    "WorldBoundaryTop": 256.0,
    "WorldBoundaryBottom": -256.0,
    "ForbiddenLayers": [ 3, 4, 6, 7 ],
    "Chunks": [
        "chunk_-1.json",
        "chunk_0.json",
        "chunk_1.json"
    ],
    "EntityList": [
        {
            "Properties": {
                "TypeName": "Minecraft2D::Steve",
                "Layer": 5,
                "Name": "__ZwDefaultEntity7Player"
            },
            "Physics": {
                "PhysicsChanged": true,
                "CollisionChanged": true,
                "RenderDisabled": true,
                "PosX": 0.0,
                "PosY": 5.0,
                "ExtendedTags": {
                    "InventoryFocus": 1,
                    "Life": 8045.56646595997,
                    "IsCreative": true
                }
            }
        }
    ]
}
```

## Chunk Format

Specifically, chunk files only split the entities apart, in an ordered manner.
Chunks are split apart by their X-axis positions. If one has its position
positioned in the range *[-8, 8)*, then it lies in *Chunk #0*; if it has itself
positioned in the range *[8, 24)*, then it lies in *Chunk #1* and so on.

Any entity can appear in any chunk file, arbitrarily, but not arbitrarily
exported by the program. Therefore, you may expect one to definitely appear in
correlated places in one chunk, only to take some time to find it.

Chunk files are extremely simple, but any erroneous (semantically error) entity
would **cause the entire chunk to fail to load and fail to save**, causing great
loss to the game map if it is important.

### Definitions

| Tags       | Purposes                                                                    |
| :--------- | :-------------------------------------------------------------------------- |
| EntityList | A list of entities, that has no limit but to be JSON-grammatically correct. |

### Example

```JSON
{
    "EntityList": [
        {
            "Properties": {
                "TypeName": "Minecraft2D::Brick",
                "Layer": 5,
                "Name": ""
            },
            "Physics": {
                "PhysicsChanged": false,
                "CollisionChanged": false,
                "RenderDisabled": false,
                "PosX": 0.0,
                "PosY": 0.0
            }
        }
    ]
}
```

## Special Notes

The game map mechanism surely has its own rules. Most of the rules are reasonable.

 * All players (**should be only one, though**) with name ```__ZwDefaultEntity7Player```
   are considered default players. All new joined players that does not have
   itself previously inside the map inherits most important data of this default
   player. If it is not specified, the game would look at a player that already
   exists and inherit his data. If otherwise (that there had been no players at
   all), the game would generate a completely empty player and assign its data
   to the new player.

 * All entities marked with name ```__ZwProgramGeneratedEntity7``` would be
   **ignored** when saving the map. This is because this is a reserved name for
   projectiles launched actively when the player is throwing projectiles. Not
   exporting them can preserve the size of the world save and reduce lag when
   loading, reduce bugs when exporting.

 * Players' entities are moved to ```level.json``` instead of any of the chunk
   configurations. This enables the developers and server managers, single
   computer players to edit their data fast, eliminating the time needed to
   index their positions in the map.
