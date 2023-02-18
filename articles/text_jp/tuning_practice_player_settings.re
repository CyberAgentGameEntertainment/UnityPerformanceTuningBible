={practice_project_settings} Tuning Practice - Player Settings

この章では、パフォーマンスに影響を与えるProject SettingsにあるPlayerの項目について紹介します。

=={practice_player_settings_scripting_backend} Scripting Backend

Unityでは、AndroidやStandalone (Windows、macOS、Linux) といったプラットフォームで、Scripting BackendをMonoかIL2CPPのどちらかから選択できます。
@<chapref>{basic}の@<hd>{basic|basic_unity_output_binary_il2cpp}で述べたとおりパフォーマンスが向上するため、IL2CPPを選択することを推奨します。

//image[scripting_backend][Scripting Backendの設定][scale=0.75]

また、Scripting BackendをIL2CPPに変更すると、一部プラットフォームを除いて@<kw>{C++ Compiler Configuration}が選択できるようになります。

//image[compiler_configuration][C++ Compiler Configurationの設定][scale=0.75]

ここではDebug、Release、Masterから選べますが、それぞれビルド時間と最適化度合いのトレードオフがありますので、ビルドの目的に応じて使い分けるとよいでしょう。

==={practice_player_settings_compiler_configuration_debug} Debug

最適化が行われないためランタイムでのパフォーマンスは良くありませんが、ビルド時間は他の設定と比較してもっとも短くなります。

==={practice_player_settings_compiler_configuration_release} Release

最適化によりランタイムのパフォーマンスが向上し、ビルドしたバイナリのサイズもより小さくなりますが、ビルドに要する時間は伸びます。

==={practice_player_settings_compiler_configuration_master} Master

そのプラットフォームで利用可能なすべての最適化が有効化されます。
たとえばWindows向けのビルドでは、リンク時コード生成 (LTCG) が使用されるなど、よりアグレッシブな最適化が行われます。
その代わりとしてビルド時間はRelease設定よりもさらに伸びますが、それが許容できる場合、製品版のビルドにはMaster設定を使用することをUnityは推奨しています。


=={practice_player_settings_code_stripping} Strip Engine Code / Managed Stripping Level

@<kw>{Strip Engine Code}はUnityの機能から、@<kw>{Managed Stripping Level}はC#をコンパイルして生成されるCILバイトコードから、
それぞれ使用されないコードを取り除くことにより、ビルドしたバイナリのサイズを削減する効果が期待できます。

しかしながら、あるコードが使用されているかどうかの判定は静的解析に強く依存しているため、
コード中で直接参照されていない型や、リフレクションで動的に呼び出しているコードが誤って除去されてしまう場合があります。

その場合には@<kw>{link.xml}ファイルや、@<code>{Preserve}属性を指定することにより除去されることを回避できます。@<fn>{managed_code_stripping}

//footnote[managed_code_stripping][@<href>{https://docs.unity3d.com/2020.3/Documentation/Manual/ManagedCodeStripping.html}]

=={practice_player_settings_accelerometer} Accelerometer Frequency (iOS)

iOS固有の設定で、加速度センサーのサンプリング周波数を変更できます。
初期設定では60Hzになっているため、適切な周波数に設定しましょう。
とくに加速度センサーを利用していない場合は、必ず設定を無効化しましょう。

//image[ios_accelerometer_frequency][サンプリング周波数の設定][scale=0.75]
