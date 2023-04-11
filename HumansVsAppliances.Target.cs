// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HumansVsAppliancesTarget : TargetRules
{
	public HumansVsAppliancesTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "HumansVsAppliances" } );
		
		
		///////////////////////////////////////////////
		/// include current setting for building in MAC
		/////////////////////////////////////////////// 
		
		/*bOverrideBuildEnvironment = true;
		AdditionalCompilerArguments = "-Wno-unused-but-set-variable";
		*/

	}
}
