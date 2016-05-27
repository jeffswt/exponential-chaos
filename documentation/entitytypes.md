
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

| Tag                         | Purpose                                                                                                                         |
| :-------------------------- | :------------------------------------------------------------------------------------------------------------------------------ |
| Properties.Name             | The indexed name as TypeName in entities, typically used as ResourcePackName::EntityTypeName.                                   |
| Properties.Type             | The entity type class, can be arbitrary string, but functional at a smaller set of strings.                                     |
| Properties.Description      | The pretty name shown in inventory, does not affect other mechanisms.                                                           |
| Properties.ShowInCreative   | Whether to display in creative inventory or not. If false, creative players would not be able to see this in their inventories. |
| Physics.PhysicsEnabled      | If set to false, related entities would be by default static unless specially requested and vice versa.                         |
| Physics.CollisionEnabled    | If set to true, related entities would calculate collision with potential entities and vice versa.                              |
| Physics.LengthX             | The length of the collision box of the entity, centered at its position.                                                        |
| Physics.LengthY             | The height of the collision box of the entity, centered at its position.                                                        |
| Physics.Mass                | The mass of the entity, used to calculate collision between entities that are both static.                                      |
| Physics.FrictionFactor(.*?) | The factor given to the described face, as a matter of friction.                                                                |
| Physics.ElasticFactor(.*?)  | The factor given to the described face, as a matter of elasticity (the severity how it bounces back)                            |
| Physics.BlastResistance     | If a projectile deployed nearby has a blast power larger than this, this entity would be destroyed.                             |
| Graphics.RenderEnabled      | Whether to render graphics for this entity by default.                                                                          |
| Graphics.LengthX            | The visual length of this entity.                                                                                               |
| Graphics.LengthY            | The visual height of this entity.                                                                                               |
| Graphics.AnimationInterval  | The time need to spend in rendering a round of animation.                                                                       |
| Graphics.TextureOnHand      | The texture shown in inventories and hotbars.                                                                                   |
| Graphics.TextureList        | The textures, given in sequence, displaying the animated texture sequence one by one.                                           |

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
    "Properties" : {
        "Name" : "Grass",
        "Type" : "Block",
        "Description" : "Grass Block",
        "ShowInCreative" : true
    },
    "Physics" : {
        "PhysicsEnabled" : false,
        "CollisionEnabled" : true,
        "LengthX" : 1,
        "LengthY" : 1,
        "Mass" : 256,
        "FrictionFactorTop" : 0.9,
        "FrictionFactorBottom" : 1.2,
        "FrictionFactorLeft" : 0.3,
        "FrictionFactorRight" : 0.3,
        "ElasticFactorTop" : 0.13,
        "ElasticFactorBottom" : 0.26,
        "ElasticFactorLeft" : 0.5,
        "ElasticFactorRight" : 0.5,
        "BlastResistance" : 3
    },
    "Graphics" : {
        "RenderEnabled" : true,
        "LengthX" : 1,
        "LengthY" : 1,
        "AnimationInterval" : 3,
        "TextureOnHand" : "block/grass.png",
        "TextureList" : [
            "block/grass.png"
        ]
    }
}
```

Following is an entity describing a TNT, which has extended tags associated
with the class "Projectile". Need to mention that this entity type is one of
the few built-in entity types that are equipped with trigger programming.

```JSON
{
	"Properties" : {
		"Name" : "TNT",
		"Type" : "Projectile",
		"Description" : "TNT",
		"SpecificProperties" : {
			"LaunchSpeed" : 9.0,
			"LaunchInterval" : 4.0,
			"DeployDelay" : 5.0,
			"DeployRadius" : 4.0,
			"DeployPowerDamage" : 700.0,
			"DeployPowerMotion" : 32.0,
			"DeployPowerBlast" : 15.0
		},
		"ShowInCreative" : true
	},
	"Physics" : {
		"PhysicsEnabled" : true,
		"CollisionEnabled" : true,
		"LengthX" : 0.4,
		"LengthY" : 0.4,
		"Mass" : 20.0,
		"FrictionFactorLeft" : 0.8,
		"FrictionFactorRight" : 0.8,
		"FrictionFactorTop" : 0.95,
		"FrictionFactorBottom" : 0.95,
		"ElasticFactorLeft" : 0.9,
		"ElasticFactorRight" : 0.9,
		"ElasticFactorTop" : 0.5,
		"ElasticFactorBottom" : 0.5,
		"BlastResistance" : 10000.0,
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
	"Graphics" : {
		"RenderEnabled" : true,
		"LengthX" : 0.4,
		"LengthY" : 0.4,
		"TextureList" : [
			"item/tnt.png"
		]
	}
}
```

This snippet described a standard player, which has all the desired properties
that a player has to have in order to proceed. Forking this method developers
may create player models with far more interesting interfaces.

```JSON
{
	"Properties" : {
		"Name" : "Steve",
		"Type" : "Player",
		"SpecificProperties" : {
			"MoveSpeed" : 15.0,
			"JumpSpeed" : 4.5,
			"MaxLife" : 10000.0
		},
		"ShowInCreative" : false
	},
	"Physics" : {
		"PhysicsEnabled" : true,
		"CollisionEnabled" : true,
		"LengthX" : 0.8,
		"LengthY" : 1.70,
		"Mass" : 50.0,
		"FrictionFactorLeft" : 0.7,
		"FrictionFactorRight" : 0.7,
		"FrictionFactorTop" : 0.1,
		"FrictionFactorBottom" : 0.9,
		"ElasticFactorLeft" : 0.2,
		"ElasticFactorRight" : 0.2,
		"ElasticFactorTop" : 0.9,
		"ElasticFactorBottom" : 0.9,
		"BlastResistance" : 100000.0
	},
	"Graphics" : {
		"RenderEnabled" : true,
		"LengthX" : 0.84,
		"LengthY" : 1.80,
		"TextureList" : [
			"player/steve.png"
		]
	}
}
```

Particles have few specific properties, and thus would not be shown hitherto,
which is located in the resources folder, and is easy to access. Looking at
**the source code** would also help understand the extended tags of particles.
