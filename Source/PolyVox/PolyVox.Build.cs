// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PolyVox : ModuleRules
{

	public PolyVox(TargetInfo Target)
	{
		PublicIncludePaths.AddRange(
			new string[] {
				"PolyVox/Public",
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				"PolyVox/Private",
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Projects",
				"ProceduralMeshComponent"
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"GameplayTags",
				// ... add private dependencies that you statically link with here ...	
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
