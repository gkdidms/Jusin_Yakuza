// 명령어			옵션		원본 파일이 있는 위치					사본 파일을 저장할 위치
//이펙트
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_Trail.hlsl			.\Client\Bin\ShaderFiles\Shader_Trail.hlsl	
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_VtxInstance_Point.hlsl	.\Client\Bin\ShaderFiles\Shader_VtxInstance_Point.hlsl
xcopy			/y		.\EffectTool\Bin\ShaderFiles\Shader_Aura.hlsl			.\Client\Bin\ShaderFiles\Shader_Aura.hlsl		

xcopy			/y		.\EffectTool\Public\Effect.h					.\Client\Public\Effect.h	
xcopy			/y		.\EffectTool\Public\TRailEffect.h				.\Client\Public\TRailEffect.h	
xcopy			/y		.\EffectTool\Public\Particle_Point.h				.\Client\Public\Particle_Point.h	
xcopy			/y		.\EffectTool\Public\Aura.h					.\Client\Public\Aura.h	


xcopy			/y		.\EffectTool\Private\Effect.cpp				.\Client\Private\Effect.cpp	
xcopy			/y		.\EffectTool\Private\TRailEffect.cpp				.\Client\Private\TRailEffect.cpp	
xcopy			/y		.\EffectTool\Private\Particle_Point.cpp			.\Client\Private\Particle_Point.cpp
xcopy			/y		.\EffectTool\Private\Aura.cpp				.\Client\Private\Aura.cpp



//UI
xcopy			/y		.\UITool\Bin\ShaderFiles\Shader_VtxUI.hlsl			.\Client\Bin\ShaderFiles\Shader_VtxUI.hlsl			

xcopy			/y		.\UITool\Public\UI_Object.h					.\Client\Public\UI_Object.h	
xcopy			/y		.\UITool\Public\Group.h						.\Client\Public\Group.h	
xcopy			/y		.\UITool\Public\UI_Texture.h					.\Client\Public\UI_Texture.h	
xcopy			/y		.\UITool\Public\Image_Texture.h					.\Client\Public\Image_Texture.h			
xcopy			/y		.\UITool\Public\Text.h						.\Client\Public\Text.h	
xcopy			/y		.\UITool\Public\Btn.h						.\Client\Public\Btn.h	
xcopy			/y		.\UITool\Public\UI_Effect.h						.\Client\Public\UI_Effect.h	

xcopy			/y		.\UITool\Private\UI_Object.cpp					.\Client\Private\UI_Object.cpp	
xcopy			/y		.\UITool\Private\Group.cpp					.\Client\Private\Group.cpp	
xcopy			/y		.\UITool\Private\UI_Texture.cpp					.\Client\Private\UI_Texture.cpp	
xcopy			/y		.\UITool\Private\Image_Texture.cpp					.\Client\Private\Image_Texture.cpp			
xcopy			/y		.\UITool\Private\Text.cpp						.\Client\Private\Text.cpp	
xcopy			/y		.\UITool\Private\Btn.cpp						.\Client\Private\Btn.cpp	
xcopy			/y		.\UITool\Private\UI_Effect.cpp					.\Client\Private\UI_Effect.cpp						