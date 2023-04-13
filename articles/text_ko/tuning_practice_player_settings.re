={practice_project_settings} Tuning Practice - Player Settings

이 장에서는 퍼포먼스에 영향을 미치는 Project Settings의 Player 항목에 대해 소개합니다. 

=={practice_player_settings_scripting_backend} Scripting Backend

Unity에서는 Android, Standalone (Windows, macOS, Linux) 등의 플랫폼에서 Scripting Backend를 Mono 또는 IL2CPP 중 하나를 선택할 수 있습니다. 
@<chapref>{basic} 의 @<hd>{basic|basic_unity_output_binary_il2cpp}에서 설명한 바와 같이 성능 향상을 위해 IL2CPP를 선택하는 것을 권장한다. 

//image[scripting_backend][Scripting Backend 설정][scale=0.75]

 또한, Scripting Backend를 IL2CPP로 변경하면, 일부 플랫폼을 제외하고는@<kw>{C++ Compiler Configuration} 를 선택할 수 있게 된다. 

//image[compiler_configuration][C++ Compiler Configuration 설정][scale=0.75]

 여기서 Debug, Release, Master를 선택할 수 있는데, 각각 빌드 시간과 최적화 정도에 따라 트레이드오프가 있으므로 빌드 목적에 따라 적절히 사용하면 좋다. 

==={practice_player_settings_compiler_configuration_debug} Debug

최적화가 이루어지지 않기 때문에 런타임 성능은 좋지 않지만, 다른 설정에 비해 빌드 시간이 가장 짧습니다. 

==={practice_player_settings_compiler_configuration_release} Release

최적화를 통해 런타임 성능이 향상되고 빌드된 바이너리의 크기가 작아지지만, 빌드 시간은 길어집니다. 

==={practice_player_settings_compiler_configuration_master} Master

해당 플랫폼에서 사용할 수 있는 모든 최적화가 활성화됩니다. 
예를 들어, Windows용 빌드에서는 링크 시 코드 생성(LTCG)을 사용하는 등 보다 공격적인 최적화가 이루어집니다. 
대신 빌드 시간은 Release 설정보다 더 길어지지만, Unity는 허용 가능한 경우 제품 버전 빌드 시 Master 설정을 사용하는 것을 권장합니다. 


=={practice_player_settings_code_stripping} Strip Engine Code / Managed Stripping Level

@<kw>{Strip Engine Code} 은 Unity의 기능 중 하나입니다,@<kw>{Managed Stripping Level} 은 C#을 컴파일하여 생성되는 CIL 바이트 코드에서 
각각 사용되지 않는 코드를 제거함으로써 빌드된 바이너리의 크기를 줄이는 효과를 기대할 수 있습니다. 

그러나 특정 코드가 사용되었는지 여부의 판단은 정적 분석에 의존하기 때문에 
코드에서 직접 참조되지 않는 타입이나 리플렉션으로 동적으로 호출되는 코드가 잘못 제거되는 경우가 있다. 

이 경우@<kw>{link.xml} 파일이나 @<code>{Preserve}속성을 지정하여 제거되는 것을 방지할 수 있습니다. @<fn>{managed_code_stripping}

//footnote[managed_code_stripping][@<href>{https://docs.unity3d.com/2020.3/Documentation/Manual/ManagedCodeStripping.html}]

=={practice_player_settings_accelerometer} Accelerometer Frequency (iOS)

iOS 전용 설정으로 가속도 센서의 샘플링 주파수를 변경할 수 있습니다. 
기본값은 60Hz로 설정되어 있으므로 적절한 주파수로 설정해 주세요. 
특히 가속도 센서를 사용하지 않는다면 반드시 설정을 비활성화해야 한다. 

//image[ios_accelerometer_frequency][샘플링 주파수 설정][scale=0.75]

