
# Triggers

## Brief

Triggers are what script the entities to make the game map alive and automatic.
Using triggers can improve the interaction between the players and the game, and
between the players and players. By choosing trigger types among the countable
ones, one may produce almost everything in this game, ranging from portals,
minecarts to **even computers**.

## Definitions

The appliances of triggers can be seen defined in the entities and entity types.
In these documentations you would see how to implement triggers. Triggers are
defined with a preliminary event and a consequence. Once the preliminary event
is detected, the game would call a procedure to process the consequence, as seen
in the code snippet:

```C++
class Trigger
{
public:
    double                   DelayTime;
    std::string              PreliminaryAction;
    std::vector<std::string> PreliminaryObject;
    std::string              ConsequentialAction;
    std::vector<std::string> ConsequentialObject;
    bool    ImportFromJson(
            rapidjson::Value&  Config);
    bool    ExportToJson(
            rapidjson::Value&  Config,
            rapidjson::Document::
                AllocatorType& CAlloc);
    bool    ProcessConsequence(
            std::vector<void*> Args);
};
```

Therefore there are two strings, ```PreliminaryAction``` and ```ConsequentialAction```,
each representing the name of the invoked function. The two array of strings,
which are ```PreliminaryObject``` and ```ConsequentialObject```, represent the
arguments of the actions.

### Preliminary Actions

 * ```BeforeDestruction```
   * Argument 1 : Number, time before estimated destruction.

 * ```OnDestruction```
    * Takes no arguments.

 * ```OnCollision```
    * Takes no arguments.

 * ```OnGeneration```
    * Takes no arguments.

### Consequential Actions

 * ```CreateEntity (1 argument)```
    * Argument 1 : String, name of ```EntityType```. Will be created right here.

 * ```CreateEntity (3 arguments)```
    * Argument 1 : String, name of ```EntityType```.
    * Argument 2 : Number, ```Physics.PosX```.
    * Argument 3 : Number, ```Physics.PosY```.

 * ```DeductPlayerLife```
    * Argument 1 : Number, the life deducted every phase.
    * Argument 2 : String, name of player:
       * ```__ZwTrigger7All``` for all players
       * ```__ZwTrigger7Nearest``` for nearest player
       * Otherwise the name of the player

 * ```DeployProjectile```
    * Takes no arguments, effective immediately if the entity is a projectile.

 * ```TeleportEntity (0 argument)```
    * Takes no arguments, will teleport the owner here.

 * ```TeleportEntity (2 arguments)```
    * Teleports the owner to the **specified** destination.
    * Argument 1 : Number, ```Physics.PosX```.
    * Argument 2 : Number, ```Physics.PosY```.

 * WorldEdit.Fill.Pick (no argument)
    * Takes no arguments, the block in this area would be considered the block to
      be used to be filled with. If no block present, 'fill' command would become
      'erase' command.
    * For security reasons, this trigger can only be used by the server operator.

 * WorldEdit.Fill.Begin (no argument)
    * Takes no arguments, pended fill operation will begin here.

 * WorldEdit.Fill.End (no argument)
    * Takes no arguments, pended fill operation will start upon this trigger. The
      worldedit manager will consider the chosen block as the object to choose.
      All blocks would be **replaced** with the chosen block, instead of its area
      begin ignored by WorldEdit.
    * For security reasons, this trigger can only be used by the server operator.

 * WorldEdit.Copy.Begin (no argument)
    * Takes no arguments, pended copy / move operation will begin here.
    * For security reasons, this trigger can only be used by the server operator.

 * WorldEdit.Copy.End (no argument)
    * Takes no arguments, pended copy / move operation will stop sampling here.
      Id est, this area would be marked as the target to be copied.
    * For security reasons, this trigger can only be used by the server operator.

 * WorldEdit.Copy.ApplyCopy (no argument)
    * Takes no arguments, pended copy operation will start upon this trigger. The
      worldedit manager will consider this point positional equivalent to the
      'begin' position. Areas shall not overlap, otherwise the operation would
      be cancelled.
    * For security reasons, this trigger can only be used by the server operator.

 * WorldEdit.Copy.ApplyMove (no argument)
    * Takes no arguments, pended move operation will start upon this trigger. The
      worldedit manager will consider this point positional equivalent to the
      'begin' position. Areas may overlap.
    * For security reasons, this trigger can only be used by the server operator.

## Techniques

However, **scripting triggers can be a nuisance**. As entities are abundant in the
game map JSONs, and coding these can cause real confusion, there are a few ways
to simplify this confusion.

 1. Write yourself a code to solve the situation. Extract all entities from the
    given area, pre-process their X-axis and Y-axis values et cetera. Then do
    the things you really want to do.
 2. If possible, you may write a compiler to compile the specific type of code
    into the trigger style. **This may look silly**, though.
 3. Gather all the programmable entities into one place and do not save the
    map using the built-in map save procedure. Progress with the programming
    progressively.

There are also some tricks that does not inflict brutal trigger scripting
confusion. For example, spawning entities called 'Redstone Gist' regularly
around the Redstone blocks, and Redstone-activated blocks may detect those
'Redstone Gist' and transform into visible results.

The built-in entity "Ender Pearl" also does a nice job. It teleports entities
immediately after it crashes into the wall, and makes teleportation easier than
ever. This trick can be seen in the entity type configuration.

Other tricks, such as pickaxes, which deploy invisible projectiles, and explode
as soon as it collides with something. This can be used to mine things, and is
nonetheless a very good idea.

Tricks are necessary in programming in Exponential Chaos. To make good use of
the mechanism of this game, or to create astounding or astonishing projects or
mini-games in this game, developers would have to use their imagination and
creativity to create strange or devious methods to achieve their goals. Therefore,
the art of this game is not at playing it, instead it is at how to make use of
its mechanism, which often does not need to be implemented, but to be thought
over merely.
