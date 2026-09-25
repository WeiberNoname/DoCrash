using UnrealBuildTool;
public class PupPop : ModuleRules {
    public PupPop(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "RenderCore"});
    }
}

