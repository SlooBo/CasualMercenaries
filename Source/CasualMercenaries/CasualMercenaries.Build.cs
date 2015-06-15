// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class CasualMercenaries : ModuleRules
{
	public CasualMercenaries(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
            "OnlineSubsystem",
			"OnlineSubsystemUtils", 
            "Slate","SlateCore","UMG",
        });


        DynamicallyLoadedModuleNames.AddRange(
                 new string[] {
				"OnlineSubsystemNull",
				"NetworkReplayStreaming",
				"NullNetworkReplayStreaming",
				"HttpNetworkReplayStreaming"
			                    }
            );
		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystem");
        if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        {
            if (UEBuildConfiguration.bCompileSteamOSS == true)
            {
                DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
            }
        }
	}
}
