
# Entities

## Brief

Entities are the fundamental parts that composes the game maps and game levels.
Special tricks can be imposed to create certain maps dedicating to certain
purposes. Without entities, game maps would cease to function, and thus losing
the basic thoughts of the developers. Learning how to edit entities, can help
game map creators design specific entities and apply them to their maps, making
**mostly everything possible, from portals to minecarts, and to computers and
so on.** Yet knowing nothing about entity programming makes no trouble for one
to directly create maps through creative mode.

Entities are also specified in **strict JSON markup language**, any error happened
during the load of a file would certainly cause the entire document to fail.
Therefore, maintaining code readiness and correctness is necessary and mandatory
while editing game maps through text editors, exempli gratis *Atom, UltraEdit,
gedit, Kate*.

## Standard properties

If any of the following properties are dismissed, they would be ignored and set
to default. Some critical dismisses would cause the entity to be completely
ignored and not injected into the game map. Some other dismisses, like
the tag ```Physics.ExtendedTags``` when it's not a special entity and the
tag ```Physics.TriggerList``` if it does not require extra triggers.

| Tags                     | Purposes                                                                                                                                     |
| :----------------------- | :------------------------------------------------------------------------------------------------------------------------------------------- |
| Properties.TypeName      | The entity type's name of this entity. Formatted as ```ResourcePackName::EntityName```.                                                      |
| Properties.Layer         | The layer containing this entity, could be theoretically any 32-bit integer if necessary.                                                    |
| Properties.Name          | The given name of this entity, could be an arbitrary Unicode string, indicates the player name if the entity type's class is a ```Player```. |
| Physics.PhysicsChanged   | Whether the original ```Physics.PhysicsEnabled``` has its value reversed.                                                                    |
| Physics.CollisionChanged | Whether the original ```Physics.CollisionEnabled``` has its value reversed.                                                                  |
| Physics.RenderDisabled   | If this is true, then the entity would be forcefully ignored in the renderer.                                                                |
| Physics.PosX             | The X-axis position of the entity.                                                                                                           |
| Physics.PosY             | The Y-axis position of the entity.                                                                                                           |
| Physics.ExtendedTags     | Tags specified for specific classes of entity types, id est ```Player```.                                                                    |
| Physics.TriggerList      | The list of triggers, added to the original triggers of the entity type. **See the documentation related to triggers for details**.          |

## Extended Properties

The following are extended tags that are designed for special entity type classes
and are commonly mandatory when loading the world. Example II describes a detailed
example of extended properties. The following tags lie under ```Physics.ExtendedTags```.

### Players

| Tags               | Purposes                                                                                        |
| :----------------- | :---------------------------------------------------------------------------------------------- |
| Inventory          | A list of inventory items, describing the player's inventory. Individual definitions see below. |
| Inventory[i].Item  | The string of the type name of the entity, such as ```MyResourcePack::MyEntityType```.          |
| Inventory[i].Count | A 32-bit integer which is larger than 0, describing the amount of such entities in the slot.    |
| InventoryFocus     | The slot that the hotbar is pointing at at the moment.                                          |
| Life               | A double precision number indicating the current health points of the player.                   |
| IsCreative         | True if the player is currently creative and vice versa.                                        |

## Examples

The following is a standard entity configuration, that contains all and none
beyond the fundamental properties. It is an entity specially marked up by the
administrator for purposes that we don't know.

```JSON
{
    "Properties": {
        "TypeName": "Minecraft2D::Stone.Diorite",
        "Layer": 4,
        "Name": "Admin's Diorite"
    },
    "Physics": {
        "PhysicsChanged": false,
        "CollisionChanged": false,
        "RenderDisabled": false,
        "PosX": -105.0,
        "PosY": -4.0
    }
}
```

This is an entity specifying a player. It contains much more data than the
abovementioned contains, but merely extended tags.

```JSON
{
    "Properties": {
        "TypeName": "Minecraft2D::Steve",
        "Layer": 5,
        "Name": "Administrator"
    },
    "Physics": {
        "PhysicsChanged": false,
        "CollisionChanged": false,
        "RenderDisabled": false,
        "PosX": -85.09189669046399,
        "PosY": 3.5880223011481595,
        "ExtendedTags": {
            "Inventory": [
                {
                    "Type": "Minecraft2D::Bedrock",
                    "Count": 64
                },
                {
                    "Type": "Minecraft2D::SugerCane",
                    "Count": 64
                }
            ],
            "InventoryFocus": 2,
            "Life": 8045.597,
            "IsCreative": true
        }
    }
}
```
