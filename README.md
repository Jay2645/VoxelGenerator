# Voxel Generator
A system for generating 3D voxel terrain in Unreal Engine 4, using the PolyVox library (used under the MIT license).

# How to Use
The "default" Actor you want to place is the `APagedVolume` actor. A `PagedVolume` is an area full of "chunks" (`APagedChunk`), which contain voxels in a 3-dimensional array. When the game needs to access a certain voxel in a chunk, it has to "page" in that chunk, spawning a `APagedChunk` actor in the world. From here, it can access any voxel stored in that chunk. This means you can store large worlds in the PagedVolume and only access the parts of the world that you need, on a chunk-by-chunk basis.

As a chunk gets spawned, it calls the `PageIn()` method on the `Pager` class. The `Pager` class is designed to be overridden by the user -- the default class does nothing. The user can override the `PageIn()` method to add their own logic when chunks spawn (for infinite worlds, as an example). An example of this is the `FlatPager` class included inside the plugin, which simply spawns an "infinite" flat plane.

The `Pager` class is good for manipulating the voxel data stored in the PagedVolume, but if you have a large amount of voxel data that you've created in advance (a heightmap, for example), you should set it on the PagedVolume itself.

There are a few methods to this effect -- `SetVoxel()`, which sets a single voxel at the specified coordinates; `SetRegionHeightmap()`, which takes an array of floats and converts them into a voxel representation, leaving them the default material; `SetRegionMaterials()`, which sets the materials of already-existing voxels; and `SetRegionVoxels()`, which combines both `SetRegionHeightmap()` and `SetRegionMaterial()`.

If both the heightmap and the materials that need to be used are known before any voxels are created, `SetRegionVoxels()` is the best method to use.

If you do not know the heightmap or the materials you are using in advance, you should make a custom `Pager` class which generates the voxels as they are being paged in.

Once you have set some voxels in whatever volume you're using, you can call `CreateMarchingCubesMesh()` on the volume to automatically page in the required chunks and generate a mesh in Unreal using the "Marching Cubes" algorithm. You can use the `CreateMarchingCubesMesh()` function to generate a large region of voxels at once, but keep in mind that large regions can be slow.

Alternatively, you can use a PagedVolume and call `PageInChunksAroundPlayer()`, which automatically will create a mesh around the player. This will allow you to generate only the chunks around the player, and by hooking it up to one of Unreal's timers, you can generate fresh chunks for the player as the player moves around in the world. This is the method that should be used in large environments or "infinite" *Minecraft*-like worlds.
