# Voxel Generator
A system for generating 3D voxel terrain in Unreal Engine 4, based on the PolyVox library (used under the MIT license).

# How to Use
The "default" Actor you want to place is the `APagedVolume` actor. This actor has a `PagedVolumeComponent`, which is an area full of "chunks" (`APagedChunk`), which contain voxels in a 3-dimensional array. When the game needs to access a certain voxel in a chunk, it has to "page" in that chunk, spawning a `APagedChunk` actor in the world. From here, it can access any voxel stored in that chunk. This means you can store large worlds in the PagedVolume and only access the parts of the world that you need, on a chunk-by-chunk basis.

As a chunk gets spawned, it calls the `PageIn()` method on the `Pager` class. The `Pager` class is designed to be overridden by the user -- the default class does nothing. The user can override the `PageIn()` method to add their own logic when chunks spawn (for infinite worlds, as an example). An example of this is the `FlatPager` class included inside the plugin, which simply spawns an "infinite" flat plane.

The `Pager` class is good for manipulating the voxel data stored in the PagedVolume, but if you have a large amount of voxel data that you've created in advance (a heightmap, for example), you should set it on the PagedVolume itself.

There are a few methods to this effect -- `SetVoxel()`, which sets a single voxel at the specified coordinates; `SetRegionHeightmap()`, which takes an array of floats and converts them into a voxel representation, leaving them the default material; `SetRegionMaterials()`, which sets the materials of already-existing voxels; and `SetRegionVoxels()`, which combines both `SetRegionHeightmap()` and `SetRegionMaterial()`. These are all to be set on the `PagedVolumeComponent` class, which is accessible through methods on the `APagedVolume` actor.

If both the heightmap and the materials that need to be used are known before any voxels are created, `SetRegionVoxels()` is the best method to use.

If you do not know the heightmap or the materials you are using in advance, you should make a custom `Pager` class which generates the voxels as they are being paged in.

Once you have set some voxels in whatever volume you're using, you can call `CreateMarchingCubesMesh()` on the volume to automatically page in the required chunks and generate a mesh in Unreal using the "Marching Cubes" algorithm. You can use the `CreateMarchingCubesMesh()` function to generate a large region of voxels at once, but keep in mind that large regions can be slow.

Alternatively, you can use a PagedVolume and call `PageInChunksAroundPlayer()`, which automatically will create a mesh around the player. This will allow you to generate only the chunks around the player, and by hooking it up to one of Unreal's timers, you can generate fresh chunks for the player as the player moves around in the world. This is the method that should be used in large environments or "infinite" *Minecraft*-like worlds.

#Installation

First, make a `Plugins` folder at your project root (where the .uproject file is), if you haven't already. Then, clone this project into a subfolder in your Plugins directory. After that, open up your project's .uproject file in Notepad (or a similar text editor), and change the `"AdditionalDependencies"` and `"Plugins"` sections to look like this:

```
	"Modules": [
		{
			"Name": "YourProjectName",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"AdditionalDependencies": [
				<OTHER DEPENDENCIES GO HERE>
				"PolyVox"
			]
		}
	],
	"Plugins": [
		<OTHER PLUGINS GO HERE>
		{
			"Name": "PolyVox",
			"Enabled": true
		}
	]
```

You can now open up your project in Unreal. You might be told that your project is out of date, and the editor will ask if you want to rebuild them. You do. After that, open up the Plugins menu, scroll down to the bottom, and ensure that the "PolyVox" plugin is enabled.

After that, you're done! You can spawn in a raw `APagedVolume` actor and start messing around with it, or subclass it in C++ or Blueprints and tweak it to your heart's desire!

# To-Do

At the moment, the only supported volume is the `PagedVolume` class. There isn't really any equivalent to PolyVox's `RawVolume` or the like, although recreating that class is a goal in the future. Additionally, the only mesh generation implemented is support for the "Marching Cubes" mesh, not blocky "*Minecraft*-like" cubes.

Additional features like tesselation for smoother terrain are also on the radar, as is support for native flowing liquids (water, lava, etc.).