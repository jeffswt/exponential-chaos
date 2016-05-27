
# Resource Packs

## Brief

To further improve the portability and extensibility of this game, we introduced
resource packs to the game. Each resource pack has its own purposes, and are
strictly unrelated and distinct from each other. Each has its own name, and
neither has even the tiniest possibilities of conflicting with each other.

Defining a resource pack is easy, extending one is hard. You will have to create
your own textures, create your own entities, in order to make your very own
resource pack.

## Definition

First we look at an example of resource pack definition. Upon the first look
you may think that one resource pack is relatively easy to create. But you are
wrong. Technically, resource packs are composed of entities, so creating a
resource pack of 100 entities is equally hard as creative 100 entities.

```JSON
{
    "Name" : "Minecraft2D"
}
```

The hierarchy of the resource pack folder lies below.

```
exponential_chaos\
    maps\
    textures\
    ...
    resources\
        default_respack\
        somebody_s_respack\
        ...
        newest_respack\
        my_pack\
            entities\ (Mandatory)
                ...
            textures\ (Mandatory)
                ...
            resource.json (Mandatory)
```

Entities in one resource pack, only has permission to access its own textures.
This guaranteed that no mixing up would happen between resource packs. Entities
would, further, not at all mix up.

The "Name" given in ```resource.json``` defines the entity type name prefix. The
prefix is the resource pack name plus two colons. Such as an entity called ```Brick```
under the texture pack ```NewPack``` in the folder ```npack\``` would have an
actual name called ```NewPack::Brick```. This is why the name of the folder
does not matter.

## Notes

 * For maximum compatibility, please do not include Unicode characters, spaces,
   carriage return characters et cetera, which includes special control
   sequences that may cause some systems to fail.
 * Avoid using long resource pack names, either in the JSON or the folder name.
   Using long folder names might cause the **file system to malfunction**, and
   using long resource pack names **will** cause **severe redundancy** in the game
   saves, which occupies larger disk space.
 * If you have useful resource packs that will visibly improve game playability,
   you can contact me and I will after careful consideration post a link to your
   file or directly host your resource pack.
