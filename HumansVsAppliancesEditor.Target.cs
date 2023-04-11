// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class HumansVsAppliancesEditorTarget : TargetRules
{
	public HumansVsAppliancesEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
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
