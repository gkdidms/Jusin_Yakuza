// ��ɾ�			�ɼ�		���� ������ �ִ� ��ġ					�纻 ������ ������ ��ġ

xcopy			/y		.\Engine\Public\*.*					.\EngineSDK\Inc\
xcopy			/y/s		.\Engine\Public\*.*					.\EngineSDK\Inc\

xcopy			/y		.\Engine\Bin\Engine.dll				.\Client\bin\
xcopy			/y		.\Engine\Bin\Engine.dll				.\MapTool\bin\
xcopy			/y		.\Engine\Bin\Engine.lib				.\EngineSDK\Lib\

xcopy			/y		.\Engine\Bin\ShaderFiles\*.*				.\Client\bin\ShaderFiles\
xcopy			/y		.\Engine\Bin\ShaderFiles\*.*				.\MapTool\bin\ShaderFiles\