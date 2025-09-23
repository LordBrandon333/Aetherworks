// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Aetherworks : ModuleRules
{
	public Aetherworks(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Aetherworks",
			"Aetherworks/Variant_Platforming",
			"Aetherworks/Variant_Platforming/Animation",
			"Aetherworks/Variant_Combat",
			"Aetherworks/Variant_Combat/AI",
			"Aetherworks/Variant_Combat/Animation",
			"Aetherworks/Variant_Combat/Gameplay",
			"Aetherworks/Variant_Combat/Interfaces",
			"Aetherworks/Variant_Combat/UI",
			"Aetherworks/Variant_SideScrolling",
			"Aetherworks/Variant_SideScrolling/AI",
			"Aetherworks/Variant_SideScrolling/Gameplay",
			"Aetherworks/Variant_SideScrolling/Interfaces",
			"Aetherworks/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
