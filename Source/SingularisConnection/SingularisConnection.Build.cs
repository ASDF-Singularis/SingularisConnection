using UnrealBuildTool;

public class SingularisConnection : ModuleRules
{
	public SingularisConnection(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			[
				"Core",
				"CoreUObject",
				"Engine"
			]
		);
	}
}
