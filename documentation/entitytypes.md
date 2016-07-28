
# Entity Types

## Brief

Entity types are encoded in **strict JSON markup language**. Definitions about their
physics and graphical properties can be found in the JSON, and thus defined
through such methods. **Semantically invalid** entity types are ignored throughout
the procedure of the game, and are ignored whatsoever their properties are.

Methods have been implemented to prevent such devastating situations such that
no entity types exist. This would throw a probable ```NullPointerException```.

Entity types may be encoded with default trigger actions. These actions may not
be overridden by entities inheriting these types, be can be supported with
addendum to these trigger actions, id est these trigger actions may not be
deleted but only retained. **For details about trigger programming, see the
documentation about trigger programming for details.**

## Standard Properties

| Tag                           | Purpose                                                                                                                         |
| :---------------------------- | :------------------------------------------------------------------------------------------------------------------------------ |
| Properties.Name               | The indexed name as TypeName in entities, typically used as ResourcePackName::EntityTypeName.                                   |
| Properties.Type               | The entity type class, can be arbitrary string, but functional at a smaller set of strings.                                     |
| Properties.Description        | The pretty name shown in inventory, does not affect other mechanisms.                                                           |
| Properties.ShowInCreative     | Whether to display in creative inventory or not. If false, creative players would not be able to see this in their inventories. |
| Physics.PhysicsEnabled        | If set to false, related entities would be by default static unless specially requested and vice versa.                         |
| Physics.CollisionEnabled      | If set to true, related entities would calculate collision with potential entities and vice versa.                              |
| Physics.LengthX               | The length of the collision box of the entity, centered at its position.                                                        |
| Physics.LengthY               | The height of the collision box of the entity, centered at its position.                                                        |
| Physics.Mass                  | The mass of the entity, used to calculate collision between entities that are both static.                                      |
| Physics.FrictionFactor(.*?)   | The factor given to the described face, as a matter of friction.                                                                |
| Physics.ElasticFactor(.*?)    | The factor given to the described face, as a matter of elasticity (the severity how it bounces back)                            |
| Physics.BlastResistance       | If a projectile deployed nearby has a blast power larger than this, this entity would be destroyed.                             |
| Graphics[i].RenderEnabled     | Whether to render graphics for this entity by default.                                                                          |
| Graphics[i].LengthX           | The visual length of this entity.                                                                                               |
| Graphics[i].LengthY           | The visual height of this entity.                                                                                               |
| Graphics[i].AnimationInterval | The time need to spend in rendering a round of animation.                                                                       |
| Graphics[i].TextureOnHand     | The texture shown in inventories and hotbars.                                                                                   |
| Graphics[i].TextureList       | The textures, given in sequence, displaying the animated texture sequence one by one.                                           |
| Graphics[i].TexRotation       | The texture's rotation, for details see the appendix in this section.                                                           |

### Graphics Properties

Since version `0.12.6.7`, the entity type JSON format officially starts to support
varieties of graphical displays in one single entity. This allows players to
reduce the size of the inventory view and be able to create simultaneously
relative or principally similar entities at a scroll of the mouse wheel and the
hold of Ctrl (for details see section *Controls*).

Each *object* in the *array* Graphics represents a block state. The number
indicating the block states are relevant to their absolute position in this array.
The number is 0 for the first item in the array et cetera.

To see texture rotations and mirrorings, see the following table for details on
using the 8 states of texture rotations on the tag *TexRotation*.

| Indicator | Purpose                                                          |
| :-------- | :--------------------------------------------------------------- |
| 0         | Do nothing.                                                      |
| 1         | Rotate 90 degrees clockwise.                                     |
| 2         | Rotate 180 degrees clockwise.                                    |
| 3         | Rotate 90 degrees counter-clockwise.                             |
| 4         | Mirror against y axis.                                           |
| 5         | Mirror against y axis, then rotate 90 degrees clockwise.         |
| 6         | Mirror against y axis, then rotate 180 degrees clockwise.        |
| 7         | Mirror against y axis, then rotate 90 degrees counter-clockwise. |

## Extended Properties

All properties are described in the object under the index ```Properties.SpecificProperties```,
as shown below in the second and third example. These specific properties are
strictly depending on the classes of the entity types. If specific properties
are not given, they would be set to default, instead of aborting the import
procedure.

### Projectiles

| Tag               | Purpose                                                                                                              |
| :---------------- | :------------------------------------------------------------------------------------------------------------------- |
| LaunchSpeed       | The initial speed of the launch of the projectile, depends on the distance between the mouse pointer and the player. |
| LaunchInterval    | The time needed to wait before launching another projectile of this type.                                            |
| DeployDelay       | The maximum time that this projectile has to wait since launch until it deploys.                                     |
| DeployRadius      | The maximum effective range of this projectile.                                                                      |
| DeployPowerDamage | The damage dealt to nearby players, applied with a correlating ratio of the distance to the range radius.            |
| DeployPowerMotion | The speed given to nearby non-static entities, applied with a correlating ratio of the distance to the range radius. |
| DeployPowerBlast  | The blast power given to all nearby entities in the effective range, which is related to the distance.               |

### Players

| Tag       | Purpose                                                                                                                         |
| :-------- | :------------------------------------------------------------------------------------------------------------------------------ |
| MoveSpeed | The speed of moving applied horizontally.                                                                                       |
| JumpSpeed | The initial speed of the jump applied to players in survival mode, and the speed of moving applied vertically in creative mode. |
| MaxLife   | The maximum health points that all players are allowed to have.                                                                 |

### Particles

| Tag      | Purpose                                                                                                    |
| :------- | :--------------------------------------------------------------------------------------------------------- |
| LifeTime | The time lapse that the particle remains to be existed. Until this time reaches the particle is destroyed. |

## Examples

Below is a basic entity type "Grass Block", containing no extra tags.

```JSON

{
    "Properties": {
        "Name": "Grass",
        "Type": "Block",
        "Description": "Grass Block",
        "ShowInCreative": true
    },
    "Physics": {
        "PhysicsEnabled": false,
        "CollisionEnabled": true,
        "LengthX": 1,
        "LengthY": 1,
        "Mass": 2047,
        "FrictionFactorTop": 1.65,
        "FrictionFactorBottom": 0.4,
        "FrictionFactorLeft": 0.12,
        "FrictionFactorRight": 0.12,
        "ElasticFactorTop": 0.08,
        "ElasticFactorBottom": 0.3,
        "ElasticFactorLeft": 0.04,
        "ElasticFactorRight": 0.04,
        "BlastResistance": 3
    },
    "Graphics": [
        {
            "RenderEnabled": true,
            "LengthX": 1,
            "LengthY": 1,
            "AnimationInterval": 1,
            "TextureOnHand": "blocks/grass_side.png",
            "TextureList": [
                "blocks/grass_side.png"
            ]
        }
    ]
}
```

Following is an entity describing a TNT, which has extended tags associated
with the class "Projectile". Need to mention that this entity type is one of
the few built-in entity types that are equipped with trigger programming.

Note that this entity also made use of the animation to simulate flashing when
detonating, and successfully made use of the extensibility of graphical
appearences regardless of the physical size.

```JSON
{
    "Properties": {
        "Name": "TNT",
        "Type": "Projectile",
        "Description": "TNT",
        "SpecificProperties" : {
			"LaunchSpeed" : 9.0,
			"LaunchInterval" : 1.6,
			"DeployDelay" : 5.0,
			"DeployRadius" : 4.0,
			"DeployPowerDamage" : 400.0,
			"DeployPowerMotion" : 160.0,
			"DeployPowerBlast" : 50.0
		},
		"ShowInCreative": true
    },
    "Physics": {
        "PhysicsEnabled": true,
        "CollisionEnabled": true,
        "LengthX": 0.8,
        "LengthY": 0.8,
        "Mass": 20,
        "FrictionFactorLeft" : 0.8,
		"FrictionFactorRight" : 0.8,
		"FrictionFactorTop" : 0.95,
		"FrictionFactorBottom" : 0.95,
		"ElasticFactorLeft" : 0.9,
		"ElasticFactorRight" : 0.9,
		"ElasticFactorTop" : 0.5,
		"ElasticFactorBottom" : 0.5,
		"BlastResistance" : 6000.0,
		"TriggerList": [
            {
        		"DelayTime": 2.0,
        		"PreliminaryAction": "BeforeDestruction",
        		"PreliminaryObject": [
        			"0.4"
        		],
        		"ConsequentialAction": "CreateEntity",
        		"ConsequentialObject": [
        			"Minecraft2D::ExplosionBasic"
        		]
        	}
        ]
    },
    "Graphics": [
        {
            "RenderEnabled": true,
            "LengthX": 1.6,
            "LengthY": 1.6,
            "AnimationInterval": 1.0,
            "TextureOnHand": "blocks/tnt_side.png",
            "TextureList": [
                "blocks/edit/tnt_unignited.png",
                "blocks/edit/tnt_ignited.png"
            ]
        }
    ]
}
```

This snippet described a standard player, which has all the desired properties
that a player has to have in order to proceed. Forking this method developers
may create player models with far more interesting interfaces.

```JSON
{
    "Properties": {
        "Name": "Steve",
        "Type": "Player",
        "SpecificProperties": {
            "MoveSpeed": 6.0,
            "JumpSpeed": 2.0,
            "MaxLife": 2000.0
        },
        "ShowInCreative": false
    },
    "Physics": {
        "PhysicsEnabled": true,
        "CollisionEnabled": true,
        "LengthX": 0.8,
        "LengthY": 1.70,
        "Mass": 50.0,
        "FrictionFactorLeft": 1.5,
        "FrictionFactorRight": 1.5,
        "FrictionFactorTop": 2.2,
        "FrictionFactorBottom": 3.0,
        "ElasticFactorLeft": 0.2,
        "ElasticFactorRight": 0.2,
        "ElasticFactorTop": 0.9,
        "ElasticFactorBottom": 0.9,
        "BlastResistance": 18000000.0
    },
    "Graphics": [
        {
            "RenderEnabled": true,
            "LengthX": 0.84,
            "LengthY": 1.80,
            "TextureList": [
                "player/steve.png"
            ]
        }
    ]
}
```

Particles have few specific properties, and thus would not be shown hitherto,
which is located in the resources folder, and is easy to access. Looking at
**the source code** would also help understand the extended tags of particles.

Those blocks with additional block states are also widely available in the
resources folder, and is rather easy to access. Try on crafting tables for
instance. You may have to need to write a script to gain access to the massive
amount of data while filtering them.
