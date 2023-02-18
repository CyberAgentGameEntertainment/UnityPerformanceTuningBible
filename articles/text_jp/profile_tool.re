={tool} プロファイリングツール
プロファイリングツールはデータを収集・解析し、ボトルネックの特定やパフォーマンスの指標を決める際に使用します。
これらのツールはUnityエンジンが提供しているものだけでもいくつかあります。
他にもXcodeやAndroid Studioと言ったネイティブ準拠のツールや、GPUに特化したRenderDocなど、さまざまなツールが存在します。
そのためそれぞれのツールの特徴を理解し、適切に取捨選択することが重要になるでしょう。
この章ではそれぞれのツールの紹介とプロファイル方法について取り上げ、適切に使用できる状態を目指します。

==={before_profiling} 計測時の注意点
Unityはエディター上でアプリケーションを実行できるため「実機」と「エディター」のどちらでも計測が可能です。
計測を行うにあたって、それぞれの環境での特徴を押さえておく必要があります。

エディターで計測する場合、トライ＆エラーを素早くできることが最大の強みでしょう。
しかしエディター自身の処理負荷や、エディターが使用しているメモリ領域も計測されるため、計測結果にノイズが多くなります。
また実機とはスペックがまったく違うので、ボトルネックがわかりにくく結果が違うこともあるでしょう。

そのためプロファイリングは基本的には@<em>{実機での計測を推奨}します。
ただし「どちらの環境でも発生する」場合に限り、作業コストが低いエディターだけで作業を完結させるのが効率的です。
大体はどちらの環境でも再現しますが、まれにどちらかの環境でしか再現しないことがあります。
そのため、まずは実機で現象を確認します。次にエディターでも再現を確認した上で、エディター上で修正するというフローがよいでしょう。
もちろん最後に実機での修正確認は必ず行いましょう。

=={tool_unity_profiler} Unity Profiler
Unity ProfilerはUnityエディターに組み込まれているプロファイリングツールです。
このツールは1フレームごとに情報を収集できます。
計測できる項目は多岐に渡り、それぞれの項目をプロファイラーモジュールと呼び、Unity 2020のバージョンでは14項目も存在します。
このモジュールはいまなお更新されており、Unity 2021.2では新たにAssetに関するモジュールや、File I/Oに関するモジュールも追加されました。
このようにUnity Profilerは、さまざまなモジュールがあるため、ざっくりとパフォーマンスの外観を掴むのに最適なツールと言えるでしょう。
モジュール一覧は@<img>{profiler_module_list}のようになります。

//image[profiler_module_list][プロファイラーモジュール一覧]

これらのモジュールはプロファイラー上に表示するかどうかの設定が可能です。
ただし表示していないモジュールは計測もされていません。逆に言うとすべてを表示している場合、それだけエディターに負荷がかかります。
//image[profiler_modules][Profiler Modulesの表示/非表示機能]

また、プロファイラーツール全体で共通する便利な機能を紹介します。
//image[profiler_often_use_command][Profilerの機能説明]

@<img>{profiler_often_use_command}において「①」は各モジュールが計測している項目が列挙されています。
この項目をクリックすることで右側のタイムラインへの表示・非表示の切り替えができます。
必要な項目だけを表示することでビューが見やすくなるでしょう。
またこの項目はドラッグで並び替えることもでき、右側のグラフはその並び順で表示されます。
「②」は計測したデータの保存、読み込みの機能です。必要であれば計測結果を保存しておくとよいでしょう。
保存されるデータはプロファイラー上に表示しているデータのみです。

本書籍では@<img>{profiler_module_list}のうち、使用頻度が高いCPU UsageとMemoryモジュールについて解説を行います。

==={how_to_use_unity_profiler} 計測方法
ここではUnity Profilerで実機を用いた計測方法について取り上げます。
ビルド前に行う作業とアプリケーション起動後に行う作業の2つに分けて解説します。
なお、エディターでの計測方法は実行中に計測ボタンを押下するだけなので詳細は割愛します。

===={how_to_use_unity_profiler_for_build} ビルド前に行う作業
ビルド前に行う作業は@<kw>{Development Build}を有効にすることです。
この設定が有効化されることでプロファイラーと接続ができるようになります。

また、より詳細に計測を行う@<kw>{Deep Profile}というオプションもあります。
このオプションを有効にすると、すべての関数コールの処理時間が記録されるため、ボトルネックとなる関数の特定が容易にできます。
欠点としては計測自体に非常に大きなオーバーヘッドを要するため、動作が重くなり、メモリも大量に消費します。
そのため処理にとても時間がかかっているように見えても、通常のプロファイルではそれほどでもないこともあるので注意してください。
基本的には通常のプロファイルでは情報が足りない場合にのみ使用します。
//info{
Deep Profileは大規模プロジェクトなどでメモリを多く使用している場合、メモリ不足で計測ができないこともあるでしょう。
その場合は@<hd>{cpu_module_unity_profiler}節の@<hd>{cpu_module_sampler}を参考に、独自に計測処理を追加するしかありません。
//}

これらの設定方法はスクリプトから明示的に指定する方法と、GUIから行う方法があります。
まずスクリプトから設定する方法を紹介します。

//listnum[setting_development_build_by_script][スクリプトからのDevelopment Buildを設定する方法][csharp]{
BuildPlayerOptions buildPlayerOptions = new BuildPlayerOptions();
/* シーンやビルドターゲットの設定は省略 */

buildPlayerOptions.options |= BuildOptions.Development;
// Deep Profileモードを有効にしたい場合のみ追加
buildPlayerOptions.options |= BuildOptions.EnableDeepProfilingSupport;

BuildReport report = BuildPipeline.BuildPlayer(buildPlayerOptions);
//}
@<list>{setting_development_build_by_script}で重要な点は@<code>{BuildOptions.Development}を指定することです。

次にGUIから設定する場合は、Build Settingsから@<img>{development_build_setting}のようにDevelopment Buildにチェックしてビルドをします。
//image[development_build_setting][Build Settings]

===={how_to_use_unity_profiler_for_connect}アプリケーション起動後に行う作業
アプリケーション起動後にUnity Profilerと接続する方法は「リモート接続」と「有線（USB）接続」の2種類があります。
リモート接続は有線接続に比べて環境の制約が増え、プロファイルが思ったようにできないこともあります。
たとえば同じWifiネットワークへの接続が必須だったり、Androidのみモバイル通信を無効にする必要があったり、他にもポート解放が必要だったりします。
そのため、本節では手順がシンプルで確実にプロファイルができる有線接続について取り上げます。
もしリモート接続をしたい場合は、公式ドキュメントを見ながら試してみてください。

はじめにiOSの場合、プロファイラーへの接続は次のような手順で行います。

 1. Build SettingsからTarget PlatformをiOSに変更しておく
 2. デバイスをPCに接続し、Development Buildのアプリケーションを起動する
 3. Unity Profilerから接続先の端末を選択する (@<img>{connect_profiler_by_iphone})
 4. Recordを開始する

//image[connect_profiler_by_iphone][接続先端末の選択]

//info{
計測を行うUnityエディターは、ビルドしたプロジェクトでなくても構いません。
計測用に新規プロジェクトを作成すると動作も軽量なのでオススメです。
//}

次にAndroidの場合は、iOSに比べ少し手順が増えます。

 1. Build SettingsからTarget PlatformをAndroidに変更しておく
 2. デバイスをPCに接続し、Development Buildのアプリケーションを起動する
 3. @<code>{adb forward}コマンドを入力する。（コマンドの詳細は後述）
 4. Unity Profilerから接続先の端末を選択する
 5. Recordを開始する

@<code>{adb forward}コマンドには、アプリケーションのPackage Nameが必要になります。
たとえばPackage Nameが「jp.co.sample.app」だった場合、以下のように入力します。
//listnum[adb_tunnel_command][adb forwardコマンド]{
adb forward tcp:34999 localabstract:Unity-jp.co.sample.app
//}

adbが認識されていない場合は、adbのパス設定を行ってください。
設定方法に関してはWeb上に解説してある情報がいくつもあるため割愛します。

簡易トラブルシューティングとして、接続できない場合は以下を確認してください。

 * 両デバイス共通
 ** 実行したアプリケーションの右下にDevelopment Buildという表記があるか
 * Androidの場合
 ** 端末のUSBデバッグを有効化しているか
 ** @<code>{adb forward}コマンドの入力したパッケージ名が正しいか
 ** @<code>{adb devices}コマンドを入力した際に、デバイスが正常に認識されているか

 補足となりますがBuild And Runで直接アプリケーションを実行した場合、上述の@<code>{adb forward}コマンドが内部的に行われます。
 そのため計測時にコマンド入力は必要ありません。

====[column]{autoconnect_profiler} Autoconnect Profiler

ビルド設定にはAutoconnect Profilerというオプションがあります。
このオプションはアプリ起動時にエディターのプロファイラーに自動接続するための機能です。
そのためプロファイルに必須の設定ではありません。リモートプロファイル時も同様です。
WebGLのみ、このオプションがないとプロファイルできませんが、モバイルに関してはそれほど重宝するオプションではないでしょう。

もう少し踏み込んだ話をすると、このオプションが有効な場合、ビルド時にエディターのIPアドレスがバイナリへ書き込まれ、起動時にそのアドレスへ接続を試みるようになります。
専用のビルドマシンなどでビルドしている場合、そのマシンでプロファイルをしない限りは不要です。
むしろアプリケーション起動時に自動接続がタイムアウトする（8秒ほどの）待ち時間が増えるだけになります。

スクリプトからは@<code>{BuildOptions.ConnectWithProfiler}というオプション名になっており、必須のように勘違いしやすいので気をつけましょう。

====[/column]

==={cpu_module_unity_profiler} CPU Usage
CPU Usageは@<img>{profiler_cpu_usage}のように表示されます。
//image[profiler_cpu_usage][CPU Usageモジュール (Timeline表示)]

このモジュールの確認方法は大きく分けて次の2つになります。

 * Hierarchy (Raw Hierarchy)
 * Timeline

 まずはHierarchyビューに関して、表示内容と使い方について解説します。

===={cpu_module_hierarchy} 1. Hierarchyビュー
Hierarchyビューは@<img>{profiler_hierarchy_view}のような表示になります。
//image[profiler_hierarchy_view][Hierarchyビュー]

このビューの特徴としては、リスト形式で計測結果が並んでおり、ヘッダーの項目でソートが可能なことです。
調査を行う際はリストの中から気になる項目を開いていくことで、ボトルネックが判別できます。
ただし、表示されている情報は「選択しているスレッド」で費やされた時間の表示です。
たとえば、Job Systemやマルチスレッドレンダリングを使用している場合、別スレッドでの処理時間は含まれません。
確認したい場合は@<img>{profiler_hieracrhy_thread}のようにスレッドを選択することで可能です。
//image[profiler_hieracrhy_thread][スレッド選択]

//embed[latex]{
\clearpage
//}

次にヘッダーの項目について解説します。
//table[hierarchy_header][Hierarchyヘッダー情報]{
ヘッダー名	説明
--------------------
Overview	サンプル名。
Total	この関数の処理にかかった合計時間。（％表示）
Self	この関数自体の処理時間。サブ関数の処理時間は含まれません。（％表示）
Calls	1フレーム内で呼ばれた回数。
GC Alloc	この関数で割り当てたスクリプトのヒープメモリ。
Time ms	Totalをmsで表示したもの。
Self ms	Selfをmsで表示したもの。
//}

Callsは複数回の関数呼び出しを1つの項目としてまとめるのでビューとして見やすくなります。
しかし、すべてが等しい処理時間なのか、うち1つだけ処理時間が長いのかはわかりません。
このような場合に@<kw>{Raw Hierarchyビュー}を利用します。
Raw Hierarchyビューは、必ずCallsが1に固定されるという点でHierarchyビューと違います。
@<img>{profile_raw_hierarchy}ではRaw Hierarchyビューにしているため、同じ関数呼び出しが複数表示されています。
//image[profile_raw_hierarchy][Raw Hierarchyビュー]

今までの内容をまとめるとHierarchyビューは次のような用途で使用します。

 * 処理落ちしているボトルネックの把握、最適化 (Time ms、Self ms)
 * GCアロケーションの把握、最適化 (GC Allocation)

これらの作業を行う際は、目的の項目ごとに降順ソートを行ってから確認することを推奨します。

//info{
項目を開いていく際、深い階層になっていることがよくあります。
この時、Macの場合はOptionキー（Windowsの場合はAltキー）を押下しながら開くことで、すべての階層が開けます。
逆にキーを押しながら項目を閉じると、その階層以下が閉じた状態になります。 
//}

===={cpu_module_timeline} 2. Timelineビュー
もう1つの確認方法となるTimelineビューは次のような表示になります。
//image[profiler_timeline][Timelineビュー]

Timelineビューでは、Hierarchyビューの項目がボックスとして可視化されているため、一目で全体を見たときに負荷のかかっている箇所が直感的にわかります。
そしてマウスによる操作が可能なため、階層が深い場合もドラッグするだけで全容が把握できます。
さらにTimelineではわざわざスレッドを切り替える必要がなく、すべて表示されています。
そのため各スレッドでいつどのような処理が行われているかも簡単に把握できます。
このような特徴から、主に次のような用途で使用します。

 * 処理負荷を全体俯瞰してみたい
 * 各スレッドの処理負荷を把握、チューニングしたい

Timelineが向いていないのは、重い処理順を把握するためのソート操作や、アロケーション総量を確認する場合などです。
そのためアロケーションのチューニングに関してはHierarchyビューの方が向いているでしょう。

===={cpu_module_sampler} 補足：Samplerについて
関数単位で処理時間を計測する場合、2通りの方法があります。
1つは先に紹介したDeep Profileモードです。もう1つはスクリプトに直接埋め込む方法です。

直接埋め込む場合は次のように記載します。
//listnum[profile_begin_samle][Begin/EndSampleを用いた方法]{
using UnityEngine.Profiling;
/* ...省略... */
private void TestMethod()
{
	for (int i = 0; i < 10000; i++)
	{
		Debug.Log("Test");
	}
}

private void OnClickedButton()
{
	Profiler.BeginSample("Test Method")
	TestMethod();
	Profiler.EndSample()
}
//}

埋め込んだサンプルはHierarchyビュー、Timelineビューのどちらにも表示されます。
//image[sampler_display_example][Samplerの表示]

もう1つ特筆すべき特徴があります。それはプロファイリングのコードはDevelopment Buildでない場合、呼び出し側が無効化されるためオーバーヘッドはゼロになることです。
今後処理負荷が増えそうな箇所には事前に仕込んでおくのも手かもしれません。

BeginSampleメソッドはstatic関数なので気軽に使えますが、似たような機能を持ったCustomSamplerというのも存在します。
こちらはUnity 2017以降に追加され、BeginSampleより計測のオーバーヘッドが少ないため、より正確な時間が計測できるという特徴があります。

//embed[latex]{
\clearpage
//}

//listnum[profile_custom_sampler][CustomSamplerを用いた方法]{
using UnityEngine.Profiling;
/* ...省略... */
private CustomSampler _samplerTest = CustomSampler.Create("Test");

private void TestMethod()
{
	for (int i = 0; i < 10000; i++)
	{
		Debug.Log("Test");
	}
}

private void OnClickedButton()
{
	_samplerTest.Begin();
	TestMethod();
	_samplerTest.End();
}
//}

違いとしては事前にインスタンスを生成しておく必要があるところです。
CustomSamplerは計測後、スクリプト内で計測時間を取得できるのも特徴です。
より正確性が必要な場合や、処理時間に応じて警告などを出したい場合は、CustomSamplerを使用するのがよいでしょう。

==={memory_module_unity_prfoiler} Memory
メモリモジュールは@<img>{profiler_memory}のように表示されます。
//image[profiler_memory][Memoryモジュール]

このモジュールの確認方法は次の2つになります。

 * Simpleビュー
 * Detailedビュー

まずはSimpleビューに関して、表示内容と使い方について解説します。

===={memory_module_simple_view} 1. Simpleビュー
Simpleビューは@<img>{profiler_memory_simple}のような表示になります。
//image[profiler_memory_simple][Simpleビュー]

ビューに記載されている項目について説明します。

 : Total Used Memory
    Unityが割当済み（使用中）のメモリ合計量。
 : Total Reserved Memory
    Unityが現在確保しているメモリの合計量。
	OS側にあらかじめ一定量の連続メモリ領域をプールとして確保しておき、必要になったタイミングで割り当てていきます。
	プール領域が足りなくなると再度OS側に要求し拡張します。
 : System Used Memory
    アプリケーションで使用しているメモリの合計量。
	この項目では、Total Reservedで計測されていない項目（プラグインなど）も計測されます。
	ただし、これでもすべてのメモリ割り当ては追跡できません。
	正確に把握する場合は、Xcodeなどネイティブ準拠のプロファイリングツールを使う必要があります。

@<img>{profiler_memory_simple}のTotal Used Memoryの右側に記載されている項目の意味は次の通りです。

//table[simple_view_total_detail][Simple View用語説明]{
用語名	説明
--------------------
GC	ヒープ領域で使用しているメモリ量。GC Allocなどが要因で増加します。
Gfx	Texture、Shader、Meshなどで確保しているメモリ量。
Audio	オーディオ再生のために使用しているメモリ量。
Video	Video再生のために使用しているメモリ量。
Profiler	プロファイリングを行うために使用しているメモリ量。
//}
用語名に関する補足ですが、Unity 2019.2以降から「Mono」は「GC」に、「FMOD」は「Audio」に表記が変更されました。

@<img>{profiler_memory_simple}には他にも、次に示すアセットの使用個数とメモリ確保量も記載されています。

 * Texture
 * Mesh
 * Material
 * Animation Clip
 * Audio Clip

また、次に示すようなオブジェクト数やGC Allocationに関する情報もあります。

 : Asset Count
    ロード済みのアセット総数。
 : Game Object Count
    シーンに存在するゲームオブジェクト数。
 : Scene Object Count
    シーンに存在するコンポーネントやゲームオブジェクトなどの総数。
 : Object Count
    アプリケーションで生成した、ロードしたすべてのオブジェクト総数。
	この値が増えている場合、何かのオブジェクトがリークしている可能性が高いです。
 : GC Allocation in Frame
    1フレーム内でAllocationの発生した回数と総量。

最後に、これらの情報からSimpleビューの利用ケースについてまとめます。

 * ヒープ領域やReservedの拡張タイミングの把握・監視
 * 各種アセットやオブジェクトがリークしていないかの確認
 * GC Allocationの監視

//info{
Unity 2021以降のSimpleビューはUIが大きく改善され、表示項目が見やすくなりました。
内容そのものには大きな変更はないため、ここで紹介した知識はそのまま使えます。
ただし名称が一部変更されているので注意してください。
たとえばGCがManaged Heapという名称に変更されました。
//image[profile_memory_2021][2021以降のシンプルビュー]
//}

===={memory_module_detailed_view}2. Detailedビュー
Detailedビューは@<img>{profiler_memory_detailed}のような表示になります。
//image[profiler_memory_detailed][Detailedビュー]
この表示結果は「Take Sample」ボタンを押下することで、その時点でのメモリスナップショットを取得できます。
Simpleビューと違いリアルタイム更新ではないため、表示を更新したい場合は再度Take Sampleを行う必要があります。

@<img>{profiler_memory_detailed}の右側に「Referenced By」という項目があります。
これは現在選択中のオブジェクトを参照しているオブジェクトが表示されます。
リークしているアセットが存在する場合、オブジェクトの参照元の情報が解決の糸口になるでしょう。
この表示は「Gather object references」を有効にしている場合のみ表示されます。
この機能を有効にすると、Take Sample時の処理時間が伸びますが、基本的には有効にしておくとよいでしょう。
//info{
Referenced Byには@<code>{ManagedStaticReferences()}という表記を見かけることがあります。
これは何かしらのstaticなオブジェクトから参照されているということです。
プロジェクトに精通している方なら、この情報だけである程度目星がつくかもしれません。
そうでない場合は@<hd>{tool_heap_explorer}を使用することを推奨します。
//}

Detailedビューのヘッダー項目は、見たとおりの意味なのでここでは解説を行いません。
操作方法は@<hd>{cpu_module_unity_profiler}の@<hd>{cpu_module_hierarchy}と同じで、
ヘッダーごとのソート機能があり、項目が階層表示になっています。
ここではName項目に表示されるトップノードについて解説を行います。

//table[memory_detailed_top_node][Detailedのトップノード]{
名前	説明
--------------------
Assets	シーンに含まれていないロードしたアセット。
Not Saved	コードによって実行時に生成されたアセット。@<br>{}たとえば、new Materiala()など、コードから生成したオブジェクトのこと。
Scene Memory	ロードしたシーンに含まれているアセット。
Others	上記以外のオブジェクトで、Unityがさまざまなシステムで使用するものへの割り当て。
//}

トップノードの中でもOthersの中に記載されている項目は普段馴染みがないものばかりでしょう。
その中でも知っておくとよいものを次に取り上げます。

//embed[latex]{
\clearpage
//}

 : System.ExecutableAndDlls
    バイナリやDLLなどに使用された割り当て量を示します。
	プラットフォームや端末によって取得できない場合があり、その場合は0Bとして扱われます。
	共通フレームワークを使用している他のアプリケーションと共有している場合もあり、
	プロジェクトに対するメモリ負荷は記載されている数値ほど大きくはありません。
	この項目の削減に躍起になるより、Assetの改善をするほうがよいでしょう。
	削減するには、DLLや不要なScriptを削減するのが効果的です。もっとも簡単な方法としては、Stripping Levelの変更です。
	ただし、実行時に型やメソッドが欠落するリスクがあるのでデバッグは入念に行いましょう。
 : SerializedFile
    AssetBundle内のオブジェクトのテーブルや、型情報となるType Treeなどのメタ情報を示します。
	これはAssetBundle.Unload(true or false)で解放することが可能です。
	もっとも効率が良いのはアセットロード後、このメタ情報のみ解放するUnload(false)を行うことですが、
	解放タイミングやリソースの参照管理をシビアに行わないと、リソースを二重ロードしたり、容易にメモリリークを引き起こすので注意してください。
 : PersistentManager.Remapper
    メモリ上とディスク上のオブジェクトの関連性を管理しています。Remapperはメモリプールを利用し、足りなくなると倍々に拡張していき減少することはありません。
	拡張されすぎないように気をつけましょう。具体的にはAssetBundleを大量にロードすると、マッピング領域が足りず拡張されます。
	そのため同時にロードされるファイル数を削減するために、不要なAssetBundleをアンロードするのがよいでしょう。
	また、1つのAssetBundleにその場で不要なアセットが大量に含まれている場合は分割するのがよいでしょう。

最後に、ここまで紹介してきた内容からDetailedビューの利用するケースをまとめます。

 * 特定タイミングでのメモリの詳細な把握、チューニング
 ** 不要なアセットがないか、想定外なものがないか
 * メモリリークの調査

=={tool_profile_analyzer} Profile Analyzer
Profile AnalyzerはProfilerのCPU Usageで取得したデータをより細かく分析するためのツールです。
Unity Profilerでは1フレーム単位のデータしか見られなかったのに対して、Profile Analyzerは指定したフレームの区間をもとに平均値や中央値、最小、最大値を取得できます。
フレームごとにばらつきのあるデータを適切に扱えるため、最適化を行った際に改善効果をより明確に示せるようになるでしょう。
またCPU Usageでは出来なかった計測データ同士の比較機能もあるため、最適化の結果を比較・可視化するのに非常に便利なツールです。

//image[profile_analyzer][Profile Analyzer]

==={profile_analyzer_install} 導入方法
このツールはPackage Managerからインストールが可能です。
Unityが公式でサポートしているので、PackagesをUnity Registryに変更し、検索ボックスに「Profile」と入力すると表示されます。
インストール後「Window -> Analysis -> Profile Analyzer」でツールが起動できます。

//image[profile_analyzer_install][PackageManagerからのインストール]

==={profile_analyzer_top} 操作方法
Profile Analyzerは起動直後は@<img>{profile_analyzer_top_view}のようになっています。
//image[profile_analyzer_top_view][起動直後]

機能として「Single」と「Compare」の2つのモードがあります。
Singleモードは1つの計測データを分析する場合に使用し、Compareモードは2つの計測データを比較する場合に利用します。

「Pull Data」はUnity Profilerで計測したデータを解析させ、結果を表示できます。
事前にUnity Profilerで計測をしておきましょう。

「Save」「Load」はProfile Analyzerで分析したデータの保存・読み込みができます。
もちろんUnity Profilerのデータだけを保持しておいても問題はありません。
その場合はUnity Profilerでデータをロードし、Profile Analyzerで毎回Pull Dataを行う必要があります。
その手順が面倒な場合に専用のデータとして保存しておくのがよいでしょう。

==={profile_analyzer_display_single} 解析結果（Singleモード）
解析結果の画面は次のような構成になっています。
ここではマーカーという単語が出てきますが、処理の名称（メソッド名）を指しています。

 * 分析区間の設定画面
 * 表示項目のフィルター入力画面
 * マーカーの中央値Top10
 * マーカーの分析結果
 * フレームのサマリ
 * スレッドのサマリ
 * 選択中マーカーのサマリ

それぞれの表示画面について見ていきましょう。

===={prfoile_analyzer_frame_segment} 1. 分析区間の設定画面
各フレームの処理時間が表示されており、初期状態では全フレームが選択された状態になっています。
フレーム区間を@<img>{profile_anlyzer_frame_segment}のようにドラッグで変更できるので、必要であれば調整しましょう。
//image[profile_anlyzer_frame_segment][フレーム区間の指定]

===={prfoile_analyzer_filters} 2. フィルター入力画面
フィルター入力画面は分析結果のフィルタリングができます。
//image[profile_analyzer_filter][フィルター入力画面]
それぞれの項目は次のようになります。

//table[analyzer_filter_explain][Filtersの項目]{
項目名	説明
--------------------
Name Filter	検索したい処理名でフィルターします。
Exclude Filter	検索から除外したい処理名でフィルターします。
Thread	選択対象のスレッドが分析結果に表示されます。@<br>{}他スレッドの情報が必要であれば追加しましょう。
Depth Slice	CPU Usageで紹介したHierarchy表示の階層数です。@<br>{}たとえばDepthが3の場合は、階層が3つ目のものが表示されます。
Analysis Type	CPU Usageヘッダー項目にあったTotalとSelfの切り替えができます。
Units	時間表示をミリ秒かマイクロ秒に変更できます。
Marker Columns	分析結果のヘッダー表示を変更できます。
//}

Depth SliceがAllの場合、PlayerLoopというトップノードが表示されたり、同じ処理の階層違いが表示されるため、見づらいときがあります。
その場合、Depthを2~3ぐらいに固定し、レンダリングやアニメーション、物理演算などのサブシステムが表示されるくらいに設定するとよいでしょう。

===={prfoile_analyzer_median_top10} 3. マーカーの中央値Top10
この画面は各マーカーの処理時間を中央値で並び替え、上位10個のマーカーのみを示したものです。
上位10個のマーカーがそれぞれどのくらい処理時間を占めているのかが一目でわかります。
//image[profile_analyzer_median_top10][マーカーの中央値Top10]

===={prfoile_analyzer_marker} 4. マーカーの分析結果
それぞれのマーカーの分析結果が表示されています。
Marker Nameに記載されている処理名と、Median（中央値）、Mean（平均値）の値から改善すべき処理を分析するのがよいでしょう。
ヘッダー項目にマウスポインターを合わせると、その項目の説明が表示されるので、内容が分からない場合は参照してみてください。

//image[profile_analyzer_marker_detail][各処理の分析結果]

====[column]{median_mean} 平均値と中央値

平均値はすべての値を足し合わせ、データ数で割った値です。
それに対して中央値は、ソートしたデータの中央に位置する値のことを指します。データが偶数個の場合、中央にある前後のデータから平均値を取ります。

平均値は極端に値が離れているデータがあると影響を受けやすい性質があります。
頻繁にスパイクが発生していたり、サンプリング数が十分でない場合、中央値を参考にする方がよい場合があるでしょう。

@<img>{mean_median}は中央値と平均値で差が大きくでる例です。
//image[mean_median][中央値と平均値]
この2つの値の特徴を知った上でデータを分析しましょう。

====[/column]

===={prfoile_analyzer_frame_summary} 5. フレームのサマリ
この画面には計測したデータのフレーム統計情報が表示されています。
//image[profile_analyzer_frame_summary][フレームサマリ画面][scale=0.5]

分析しているフレームの区間情報や、値のばらつき具合を箱ひげ図（Boxplot）やヒストグラムを用いて表示しています。
箱ひげ図は四分位数を理解する必要があります。
四分位数とはデータをソートした状態で、@<table>{interquatile}のように値を定めたものです。

//table[interquatile][四分位数]{
名前	説明
--------------------
最小値 (Min)	最小の値
第1四分位数 (Lower Quartile)	最小値から25%の位置にある値
中央値 (Median)	最小値から50%の位置にある値
第3四分位数 (Upper Quartile)	最小値から75%の位置にある値
最大値 (Max)	最大の値
//}

25%から75%の区間を箱で囲ったものを箱ひげグラフと言います。
//image[box_plot][箱ひげグラフ]

ヒストグラムは横軸に処理時間、縦軸にデータ数を示しており、こちらもデータの分布を見るのに便利です。
フレームサマリではカーソルを合わせることで、区間とフレーム数を確認できます。
//image[histogram][ヒストグラム]

これらの図の見方を理解した上で、特徴を分析するとよいでしょう。

===={prfoile_analyzer_thread_summary} 6. スレッドのサマリ
この画面は選択しているスレッドの統計情報が表示されています。
各スレッドに関する箱ひげ図を見ることができます。
//image[profile_analyzer_thread_summary][フレームサマリ画面]

===={prfoile_analyzer_marker_summary} 7. 選択中マーカーのサマリ
@<hd>{prfoile_analyzer_marker}画面で選択しているマーカーに関するサマリです。
選択中のマーカーに関する処理時間が箱ひげ図やヒストグラムで表されます。
//image[profile_analyzer_marker_summary][選択中マーカーのサマリ]

==={profile_analyzer_display_compare} 解析結果（Compareモード）
このモードでは2つのデータを比較できます。
上下それぞれのデータごとに解析する区間の設定ができます。
//image[profile_analyzer_compare_frame_segment][比較データの設定]

画面の使い方はSingleモードとほとんど変わりませんが、@<img>{profile_analyzer_comapre_marker}のように「Left」と「Right」という単語が色々な画面に出てきます。
//image[profile_analyzer_comapre_marker][マーカーの比較]
これはどちらのデータであるかを示しており、@<img>{profile_analyzer_compare_frame_segment}で表示されている色と一致しています。
Leftは上部のデータ、Rightは下部のデータです。
このモードを利用することで、チューニング結果の良し悪しを簡単に分析することができるでしょう。

=={tool_frame_debugger} Frame Debugger
Frame Debuggerは現在表示されている画面がどのような流れでレンダリングされているかを分析できるツールです。
このツールはデフォルトでエディターにインストールされており「Window -> Analysis -> Frame Debugger」で開きます。

エディターや実機での使用が可能です。
実機で使用する際はUnity Profilerと同様「Development Build」でビルドされたバイナリが必要です。
アプリケーションを起動し、デバイス接続先を選択した上で「Enable」を押下すると描画命令が表示されます。
//image[frame_debugger_connect][FrameDebugger 接続画面]

==={frame_debugger_enable} 分析画面
「Enable」を押下すると次のような画面になります。
//image[frame_debugger_enable][FrameDebugger キャプチャ]

左枠は1項目が1つの描画命令になっており、上から順番に発行された命令が並んでいます。
右枠は描画命令に関する詳細情報です。どのShaderがどのようなプロパティと共に処理されたかがわかります。
この画面を見ながら次のようなことを意識して分析を行います。

 * 不要な命令がないか
 * 描画バッチングが適切に効いているか、まとめられないか
 * 描画先の解像度が高すぎないか
 * 意図しないShaderを使用していないか

==={frame_debugger_detailed} 詳細画面
前節で紹介した@<img>{frame_debugger_enable}の右枠の内容を詳細に解説します。

==== 操作パネル
まずは上段部分にある操作パネルについてです。
//image[frame_debugger_header_tool][上段の操作パネル]
RT0と記載されている部分は複数のレンダリングターゲットが存在する場合に変更できます。
とくにマルチレンダーターゲットを利用している際に、それぞれのレンダリング状態を確認する際に重宝するでしょう。
Channelsは、RGBAすべてか、いずれかの1チャンネルのみで表示するかの変更ができます。
Levelsはスライダーになっており、描画結果の明るさを調節できます。
たとえばアンビエントや間接照明など描画結果が暗い際に、明るさを調整して見やすくするのに便利です。

==== 描画概要
この領域では描画先の解像度やフォーマットの情報がわかります。明らかに解像度の高い描画先があればすぐに気付くことができるでしょう。
他にも、使用しているShader名、CullなどのPass設定、使用しているキーワードなどがわかります。
下部に記載されている「Why this~」という文章は、描画のバッチングができなかった理由が書かれています。
@<img>{frame_debugger_shader_syntax}の場合、最初の描画コールを選択しているのでバッチングできないと記載されています。
このように細かく原因が記載されているので、バッチングを工夫したい場合、この情報を頼りに調整するとよいでしょう。
//image[frame_debugger_shader_syntax][中段の描画概要]

==== Shaderプロパティの詳細情報
この領域は描画しているShaderのプロパティ情報が記載されています。
デバッグ時に重宝します。
//image[frame_debugger_shader_property][下段のShaderプロパティの詳細情報]

//info{
プロパティ情報に表示されているTexture2Dがどのような状態になっているか詳細に確認したいときがあります。
その際、Macの場合はCommandキー (Windowsの場合Controlキー)を押下しながら画像をクリックすると拡大できます。
//image[frame_debugger_preview][Texture2Dのプレビューを拡大する]
//}

=={tool_memory_profiler} Memory Profiler
Memory ProfilerはPreview PackageとしてUnityが公式で提供しているツールです。
Unity ProfilerのMemoryモジュールと比較して、主に以下のような点で優れています。

 * キャプチャしたデータがスクリーンショット付きでローカルに保存される
 * 各カテゴリのメモリ占有量がビジュアライズされ、わかりやすい
 * データの比較が可能

Memory Profilerはv0.4以降とそれ未満でUIが大きく変わりました。
本書籍では執筆時点で最新版となるv0.5を使用します。
またv0.4以降のバージョンで、すべての機能を利用する場合Unity 2020.3.12f1以降のバージョンが必要になります。
さらにv0.4とv0.5は一見同じように見えますが、v0.5で大幅に機能がアップデートされました。
とくにオブジェクトの参照関係がとても追いやすくなったので、基本的にはv0.5以降の利用を推奨します。

//image[memory_profiler_top][Memory Profiler]

==={memory_profiler_install} 導入方法
Unity 2020では、Preview版のパッケージは「Project Settings -> Package Manager」から「Enable Preview Packages」を有効にする必要があります。
//image[enable_preview_package][Preview Packagesの有効化]

その後、Unity RegistryのPackageからMemory Profilerをインストールします。
インストール後「Window -> Analysis -> Memory Profiler」でツールが起動できます。
//image[memory_profiler_install][PackageManagerからインストール]

またUnity 2021以降では、パッケージの追加方法が変更されています。
追加するには「Add Package by Name」を押下し「com.unity.memoryprofiler」と入力する必要があります。
//image[memory_profiler_add_package_2021][2021以降の追加方法]

==={memory_profiler_control} 操作方法
Memory Profilerは大きく分けて4つの要素から構成されています。

 * ツールバー
 * Snapshot Panel
 * 計測結果
 * Detail Panel

それぞれの領域について解説を行います。

===={memory_profiler_tool_bar} 1. ツールバー
//image[memory_profiler_tool_bar][ツールバー領域]
@<img>{memory_profiler_tool_bar}はHeaderのキャプチャを示しています。
①のボタンは計測対象を選択できます。
②のボタンは押下するとその時点のメモリを計測します。オプションで計測対象をNative Objectだけにしたり、スクリーンショットを無効にできます。
基本デフォルト設定で問題ないでしょう。
③のボタンは押下すると計測済みのデータを読み込みができます。
その他「Snapshot Panel」や「Detail Panel」を押下することで、左右にある情報パネルの表示／非表示が可能です。
Tree Mapの表示だけを見たい場合に非表示にすることがよいでしょう。
また「?」を押下することで公式ドキュメントを開くことができます。

計測に関する注意点が1つあります。
それは計測時に必要なメモリが新たに確保され、以降解放されないことです。
ただし無限には増え続けず、何度か計測するといずれ落ち着きます。計測時のメモリ確保量はプロジェクトの複雑度によるでしょう。
この前提を知らないと、膨れ上がったメモリ使用量を見てリークがあるかもと勘違いしてしまうので気をつけましょう。

===={memory_profiler_snapshot_panel} 2. Snapshot Panel
//image[memory_profiler_snapshot_panel_single][Snapshot Panel (Single)]

Snapshot Panelには計測したデータが表示され、どのデータを見るか選択できます。
このデータはアプリを起動してから終了するまでを1セッションとし、セッションごとにまとめられます。
また計測データを右クリックすることで削除や名称のリネームが可能です。

上部に「Single Snapshot」「Compare Snapshots」があります。
Compare Snapshotsを押下すると、計測データを比較するUIに表示が変わります。
//image[memory_profiler_snapshot_panel_compare][Snapshot Panel (Comapre)]

「A」がSingle Snapshotで選択したデータ、「B」がComapre Snapshotsで選択したデータです。
入れ替えボタンを押下することで「A」と「B」を入れ替えることができるので、わざわざSingle Snapshot画面に戻らずに切り替えも可能です。

===={memory_profiler_snapshot_result} 3. 計測結果
計測結果は「Summary」「Objects and Allocations」「Fragmentation」の3つのタブがあります。
本節ではよく使用するSummaryを解説し、その他は補足として簡単に取り上げます。
Summaryの画面上部はMemory Usage Overviewと呼ばれる領域で、現在のメモリの概要が表示されています。
項目を押下するとDetail Panelに解説が表示されるので、分からない項目は確認するとよいでしょう。
//image[memory_profiler_usage_overview][Memory Usage Overview]

次に画面中部はTree Mapと呼ばれており、オブジェクトのカテゴリごとにメモリ使用量をグラフィカルに表示します。
各カテゴリを選択すると、カテゴリ内のオブジェクトを確認できます。
@<img>{memory_profiler_tree_map}ではTexture2Dのカテゴリを選択している状態です。
//image[memory_profiler_tree_map][Tree Map]

そして画面下部はTree Map Tableと呼ばれています。ここではオブジェクトの一覧がテーブル形式で並んでいます。
表示項目はTree Map Tableのヘッダーを押下することで、グループ化やソート、フィルターを行うことができます。
//image[memory_profiler_tree_map_table_option][ヘッダー操作]

とくにTypeをグループ化することで分析しやすくなるため、積極的に利用しましょう。
//image[memory_profiler_tree_map_table_group][Typeによるグループ化]

またTree Mapでカテゴリを選択した場合は、そのカテゴリ内のオブジェクトだけを表示するフィルターが自動で設定されます。
//image[memory_profiler_tree_map_table][フィルターの自動設定]

最後にCompare Snapshotsを使用した場合のUIの変化について説明します。
Memory Usage Overviewにはそれぞれのオブジェクトの差分が表示されます。
//image[memory_profiler_compare_snaphost_overview][Compare SnapshotsのMemory Usage Overview]

またTree Map TableにはHeaderにDiffという項目が追加されます。
Diffには次のようなタイプがあります。

//table[compare_tree_map_view][Tree Map Table (Compare)]{
Diff	説明
--------------------
Same	A、B同一オブジェクト
Not in A (Deleted)	Aにあるが、Bにはないオブジェクト
Not in B (New)	Aにはないが、Bにあるオブジェクト
//}

これらの情報を見ながらメモリが増減しているかを確認できます。

===={memory_profiler_snapshot_detail} 4. Detail Panel
このパネルは選択しているオブジェクトの参照関係を追跡したい際に利用します。
このReferenced Byを確認することで、参照を握り続けている原因を把握できるでしょう。
//image[memory_profiler_detail_references][Referenced By]

下部のSelection Detailsという箇所には、オブジェクトの詳細な情報が載っています。
その中でも「Help」という項目には、解放するためのアドバイスなどが記載されています。
何をするべきか分からないときは一読するとよいかもしれません。
//image[memory_profiler_detail_selection][Selection Details]


===={memory_profiler_others}　補足：Summary以外の計測結果について

「Objects and Allocations」はSummaryと違い、アロケーションなどのより細かい内容がテーブル形式で確認できます。
//image[memory_profiler_objects_allocations][Table Viewの指定]
「Fragmentation」は仮想メモリの状況を可視化してくれるため、断片化の調査に利用できます。
メモリアドレスなど直感的でない情報が多いため利用する難易度は高いでしょう。
//image[memory_profiler_fragmentation][Fragmentation]

またMemory Profilerのv0.6から「Memory Breakdowns」という新機能が追加されました。
Unity 2022.1以降が必須となりますが、TreeMapをリスト表示で閲覧できたり、Unity Subsystemsなどのオブジェクト情報も見ることが可能になりました。
他にも、重複の可能性があるオブジェクトを確認できるようになりました。
//image[memory_profiler_memory_break_down][Memory Breakdowns]

=={tool_heap_explorer} Heap Explorer
Heap Explorerは個人開発者であるPeter77@<fn>{author_of_heap_explore}氏のオープンソースのツールです。
こちらはMemory Profilerと同様にメモリの調査を行う際によく利用するツールです。
Memory Profilerは0.4以前のバージョンだと、参照がリスト形式で表示されないため追跡するのに非常に労力がかかりました。
0.5以降で改善されたものの、対応していないUnityバージョンを利用している方もいるでしょう。
その際の代替ツールとしてはまだまだ利用価値が高いので今回取り上げたいと思います。
//footnote[author_of_heap_explore][@<href>{https://github.com/pschraut}]

//image[heap_explorer_top][Heap Explorer]

==={heap_explorer_install} 導入方法
本ツールはGitHubのリポジトリ@<fn>{url_of_heap_explorer}に記載されている
Package URL'sをコピーし、Package ManagerのAdd Package from Git urlから追加します。
インストール後「Window -> Analysis -> Memory Profiler」でツールが起動できます。
//footnote[url_of_heap_explorer][@<href>{https://github.com/pschraut/UnityHeapExplorer}]

==={heap_explorer_control} 操作方法
Heap Explorerのツールバーは次のようになっています。
//image[heap_explorer_header][Heap Explorerのツールバー]

 : 左右の矢印
  操作の戻る、進むができます。とくに参照を追跡する場合に便利です。
 : File
   計測ファイルの保存、読み込みが可能です。.heapという拡張子で保存されます。
 : View
   表示画面の切り替えができます。色々な種類があるので、興味があればドキュメントを見てください。
 : Capture
  計測を行います。ただし@<em>{計測対象はHeap Explorer上では変更ができません}。
  対象の切り替えはUnity ProfilerなどのUnityが提供するツール上で変更する必要があります。
  またSaveはファイルに保存した上で結果が表示され、Analyzeは保存せずに表示します。
  なお、Memory Profilerと同様に、計測する際に確保されるメモリは解放されないので注意してください。

//image[heap_explorer_target][計測対象の切り替え]

計測結果の画面は次のようになります。この画面をOverviewと呼びます。
//image[heap_explorer_capture_top][Heap Explorerの計測結果 (Overview)]
Overviewの中でとくに気にするべきカテゴリは緑の線が引いてあるNative Memory UsageとManaged Memory Usageです。
Investigateのボタンを押下するとそれぞれのカテゴリの詳細を確認できます。

以降ではカテゴリ詳細の表示について、重要な部分に絞って解説します。

===={heap_explorer_objects} 1. Object
Native MemoryをInvestigateした場合、C++ Objectsがこの領域に表示されます。
Managed Memoryの場合はC# Objectsがこの領域に表示されます。
//image[heap_explorer_objects_view][Object表示領域]

ヘッダーにいくつか見慣れない項目があるので補足しておきます。

 : DDoL
  Don't Destroy On Loadの略です。シーンを遷移しても破棄しないオブジェクトとして指定されているかがわかります。
 : Persistent
 永続オブジェクトかどうかです。起動時にUnityが自動生成するものがこれに当たります。

これ以降に紹介する表示領域は、@<img>{heap_explorer_objects_view}のオブジェクトを選択することで更新されます。

===={heap_explorer_referenced_by} 2. Referenced by
対象オブジェクトの参照元となるオブジェクトが表示されます。
//image[heap_explorer_referenced_by][Referenced by]

===={heap_explorer_references_to} 3. References to
対象オブジェクトの参照先となるオブジェクトが表示されます。
//image[heap_explorer_references_to][References to]

===={heap_explorer_path_to_root} 4. Path to Root
対象オブジェクトを参照しているルートオブジェクトが表示されます。
メモリリークを調査する際、何が参照を握っているのか把握できるので重宝します。
//image[heap_explorer_path_to_root][Path to Root]

今までの項目をまとめると次のようなイメージになります。
//image[heap_explorer_reference_explain][参照のイメージ]

ここまで紹介したようにHeap Explorerにはメモリリークやメモリ調査に必要な機能が一式揃っています。
動作も軽快なのでぜひこのツールの利用も検討してみてください。
気に入ったら感謝の意を込めてStarもつけると尚よいでしょう。

=={tool_xcode} Xcode
XcodeはAppleが提供する統合開発環境ツールです。
UnityでターゲットプラットフォームをiOSに設定した際、そのビルド成果物がXcodeプロジェクトになります。
Unityで計測するよりも正確な数値が取れるので、厳密な検証をする際はXcodeを利用することを推奨します。
本節ではDebug Navigator、GPU Frame Capture、Memory Graphの3つのプロファイリングツールに触れていきます。

==={tool_xcode_debug_start} プロファイル方法
Xcodeからプロファイルする方法は2通りあります。
1つ目はXcodeから直接端末にビルドし、アプリケーションを実行する方法です。
@<img>{xcode_run}に示すように実行ボタンを押下するだけでプロファイルが開始されます。
ビルドを行う際の証明書などの設定は本書では割愛します。
//image[xcode_run][Xcodeの実行ボタン]

2つ目は実行中のアプリケーションをXcodeのデバッガーにアタッチする方法です。
これはアプリを実行した後に、Xcodeメニューの「Debug -> Attach to Process」から実行中のプロセスを選択することでプロファイルができます。
ただし、ビルド時の証明書が開発者用（Apple Development）でないといけません。
Ad HocやEnterpriseの証明書ではアタッチできないので注意してください。
//image[xcode_debugger_attach][Xcodeのデバッガーアタッチ]

==={tool_xcode_debug_navi} Debug Navigator
Debug NavigatorはXcodeからアプリケーションを実行するだけで、CPUやMemoryなどのデバッグゲージが確認できます。
アプリケーション実行後に@<img>{xcode_debug_navi_tab}のスプレーマークを押下することで6つの項目が表示されます。
もしくはXcodeメニューの「View -> Navigators -> Debug」から開くことも可能です。
以降ではそれぞれの項目について解説します。
//image[xcode_debug_navi_tab][Debug Navigatorの選択]

===={tool_xcode_debug_navi_cpu} 1. CPUゲージ
どれぐらいCPUを使用しているかを見ることができます。
また各スレッドごとの使用率も見ることが可能です。
//image[xcode_debug_navi_cpu][CPUゲージ]

//embed[latex]{
\clearpage
//}

===={tool_xcode_debug_navi_memory} 2. Memoryゲージ
メモリ消費量の概要を見ることができます。
内訳などの細かい分析はできません。
//image[xcode_debug_navi_memory][Memoryゲージ]

===={tool_xcode_debug_navi_enery} 3. Energyゲージ
電力消費に関する概要を見ることができます。
CPU、GPU、Networkなどの使用率の内訳が把握できます。
//image[xcode_debug_navi_energy][Energyゲージ]

===={tool_xcode_debug_navi_disk} 4. Diskゲージ
File I/Oの概要を見ることができます。
予期せぬタイミングでファイルの読み書きが行われていないかのチェックに役立つでしょう。
//image[xcode_debug_navi_disk][Diskゲージ]

===={tool_xcode_debug_navi_network} 5. Networkゲージ
ネットワーク通信の概要を見ることができます。
Diskと同じく予期しない通信をしていないかなどのチェックに役立つでしょう。
//image[xcode_debug_navi_network][Networkゲージ]

===={tool_xcode_debug_navi_fps} 6. FPSゲージ
このゲージはデフォルトでは表示されていません。
@<hd>{tool_xcode_frame_debug}で解説するGPU Frame Captureを有効化すると表示されます。
FPSはもちろんのこと、シェーダーステージの使用率、各CPUやGPUの処理時間を確認できます。
//image[xcode_debug_navi_fps][FPSゲージ]

==={tool_xcode_frame_debug} GPU Frame Capture
GPU Frame CaptureとはXcode上でフレームデバッグができるツールのことです。
UnityのFrame Debuggerと同様に、レンダリングが完了するまでの過程が確認できます。
Unityと比べシェーダーの各ステージでの情報量が多いため、ボトルネックの分析や改善に役立つかもしれません。
以下では使用方法について解説します。

===={tool_xcode_frame_debug_ready} 1. 準備
XcodeでGPU Frame Captureを有効にするにはスキームの編集が必要になります。
初めに「Product -> Scheme -> Edit Scheme」でスキーム編集画面を開きましょう。
//image[xcode_frame_debugger_scheme][スキーム編集画面]

次に「Options」タブからGPU Frame Captureを「Metal」に変更しましょう。
//image[xcode_frame_debugger_gpu_enable][GPU Frame Captureの有効化]

最後に「Diagnostics」タブからMetalの「Api Validation」を有効にしましょう。
//image[xcode_frame_debugger_api][Api Validationの有効化]

===={tool_xcode_frame_debug_capture} 2. キャプチャ
実行中にデバッグバーからカメラマークを押下することでキャプチャが行われます。
シーンの複雑度によりますが、初回のキャプチャには時間がかかるので気長に待ちましょう。
なお、Xcode13以降の場合はMetalのアイコンに変更されています。
//image[xcode_frame_debugger_capture_icon][GPU Frame Captureボタン]

キャプチャが完了すると次のようなサマリー画面が表示されます。
//image[xcode_frame_debugger_summary][サマリー画面]

このサマリー画面からは描画の依存関係やメモリなどの詳細を確認できる画面へ遷移できます。
またNavigatorエリアには描画に関するコマンドが表示されています。
この表示方法は「View Frame By Call」と「View Frame By Pipeline State」があります。
//image[xcode_frame_debugger_navigator_view][表示方法の変更]

By Call表示では描画コマンドがすべて呼び出された順に並びます。
これは描画の前準備になるバッファーの設定なども含まれるため非常に多くのコマンドが並ぶことになります。
一方、By Pipeline Stateは各シェーダーで描画されたジオメトリに関する描画コマンドだけが並びます。
どういったことを調査するかで表示を切り替えるとよいでしょう。
//image[xcode_frame_debugger_view_frame_diff][表示の違い]

Navigatorエリアに並ぶ描画コマンドは押下することで、そのコマンドに使用されているプロパティ情報が確認できます。
プロパティ情報にはテクスチャ、バッファー、サンプラー、シェーダーの関数、ジオメトリなどがあります。
それぞれのプロパティはダブルクリックすることで詳細を確認できます。
たとえばシェーダーコードそのものや、サンプラーがRepeatなのかClampなのかといったことまで把握できます。
//image[xcode_frame_debuggeer_command][描画コマンド詳細]

またジオメトリのプロパティは頂点情報がテーブル形式で表示されるだけでなく、カメラを動かして形状の確認も可能です。
//image[xcode_frame_debugger_geometry][ジオメトリビューワー]

//embed[latex]{
\clearpage
//}

次にサマリー画面のPerformance欄にある「Profile」について説明します。
このボタンを押下するとより細かい解析を開始します。
解析が終わると描画にかかった時間がNavigatorエリアに表示されるようになります。
//image[xcode_frame_debugger_navigator_profile][プロファイル後の表示]

それだけでなく解析結果をCountersという画面でより詳細に確認ができます。
この画面では各描画のVertexやRasterized、Fragmentなどの処理時間をグラフィカルに見ることが可能です。
//image[xocde_frame_debugger_counter][Counters画面]

続いてサマリー画面のMemory欄にある「Show Memory」について説明します。
このボタンを押下するとGPUで使用しているリソースが確認できる画面へ遷移します。
表示される情報は主にテクスチャとバッファーです。不要なものがないか確認するとよいでしょう。
//image[xcode_frame_debugger_memory][GPUリソース確認画面]

最後にサマリー画面のOverviewにある「Show Dependencies」について説明します。
このボタンを押下すると各レンダーパスの依存関係が表示されます。
Dependencyを見ていく際には「矢印が外向き」になっているボタン押下することで、その階層以下の依存関係をさらに開くことができます。
//image[xcode_frame_debugger_dependency_open][Dependencyを開くボタン]
この画面はどの描画が何に依存しているかを見たいときに使用しましょう。
//image[xcode_frame_debugger_dependency][階層を開いた状態]

==={tool_xcode_memory_graph} Memory Graph
このツールではキャプチャタイミングでのメモリ状況を分析できます。
左側のNavigatorエリアにはインスタンスが表示され、そのインスタンスを選択することで参照関係がグラフで表示されます。
右側のInspectorエリアにはそのインスタンスの詳細情報が表示されます。
//image[xcode_memory_graph_top][MemoryGraph キャプチャ画面]

このツールではプラグインなどUnity上では計測できないオブジェクトのメモリ使用量を調査をする場合に利用するとよいでしょう。
以下では使用方法について解説します。

===={tool_xcode_memory_graph_ready} 1. 事前準備
Memory Graphで有益な情報を得るためにスキームの編集を行う必要があります。
「Product -> Scheme -> Edit Scheme」でスキーム編集画面を開きましょう。
そして「Diagnostics」タブから「Malloc Stack Logging」を有効にしましょう。
//image[xcode_memory_graph_malloc_stack_logging][Malloc Stack Loggingの有効化]

こちらを有効にすることでInspectorにBacktraceが表示されるようになり、どのような流れでアロケーションされたかがわかります。
//image[xcode_memory_graph_backtrace][Backtraceの表示]

===={tool_xcode_memory_graph_capture} 2. キャプチャ
アプリケーション実行中にデバッグバーから枝のようなアイコンを押下することでキャプチャが行われます。
//image[xcode_memory_graph_icon][Memory Graph Captureボタン]

またMemory Graphは「File -> Export MemoryGraph」からファイルとして保存が可能です。
このファイルに対して、vmmapコマンドやheapコマンド、malloc_historyコマンドを駆使してさらに深い調査をすることが可能です。
興味があればぜひ調べてみてください。
例としてvmmapコマンドのSummary表示を紹介します。MemoryGraphだと掴みにくかった全体像を把握できます。
//listnum[vmmap_summary][vmmap summaryコマンド]{
vmmap --summary hoge.memgraph
//}

//image[xcode_vmmap_summary][MemoryGraph Summary 表示]

=={tool_instruments} Instruments
XcodeにはInstruments（インストゥルメンツ）という詳細な計測・分析を得意とするツールがあります。
Instrumentsは「Product -> Analyze」を選択することでビルドが始まります。
完了すると次のような計測項目のテンプレートを選択する画面が開きます。
//image[instruments_menu][Instruments テンプレート選択画面]

テンプレートの多さからわかるように、Instrumentsはさまざまな内容を分析できます。
本節ではこの中からとくに利用する頻度の高い「Time Profiler」と「Allocations」を取り上げます。

==={tool_instruments_time_profiler} Time Profiler
Time Profilerはコードの実行時間を計測するツールです。
Unity ProfilerにあったCPUモジュールと同じく、処理時間を改善する際に使用します。

計測を開始するためにはTime Profilerのツールバーにある赤い丸印のレコードボタンを押下する必要があります。
//image[insturments_record_start][レコード開始ボタン]

//embed[latex]{
\clearpage
//}

計測を行うと@<img>{instruments_time_profile_result}のような表示になります。
//image[instruments_time_profile_result][計測結果]
Unity Profilerと違いフレーム単位ではなく、区間での分析を行っていきます。
下部のTree Viewには区間内の処理時間が表示されます。
ゲームロジックの処理時間を最適化する場合、Tree ViewのPlayerLoop以下の処理を分析していくとよいでしょう。

Tree Viewの表示を見やすくするために、Xcodeの下部にあるCall Treesの設定を@<img>{instruments_call_tress}のようにしておくとよいでしょう。
とくにHide System Librariesにチェックすることで、手の出せないシステムコードが非表示になり調査がしやすくなります。
//image[instruments_call_tress][Call Trees設定]

このようにして処理時間を分析し、最適化していくことができます。

//info{
Time Profilerではシンボル名がUnity Profilerと違います。
大きく変わりはないですが「クラス名_関数名_ランダムな文字列」という表記になります。
//image[xcode_time_profiler_il2cpp][Time Profilerでのシンボル名]
//}

==={tool_instruments_allocations} Allocations
Allocationsはメモリ使用量を計測するためのツールです。メモリリークや使用量の改善を行う際に使用します。
//image[instruments_allocations_view][Allocations 計測画面]

計測を行う前に「File -> Recording Options」を開き「Discard events for freed memory」にチェックを入れましょう。
//image[instruments_recording_options][オプション設定画面]

このオプションを有効にするとメモリが解放された際に記録を破棄します。
//image[instruments_recodring_options_diff][オプション設定による違い]
@<img>{instruments_recodring_options_diff}を見てわかるようにオプションの有無で見た目が大きく変わります。
オプションありの場合、メモリがアロケーションしたタイミングでのみ線が記録されます。
また記録された線は、その確保された領域が解放されると破棄されます。
つまりこのオプションを設定することで、線が残り続けていればメモリから解放されていないということになります。
たとえばシーン遷移によってメモリを解放する設計の場合、遷移前のシーン区間内に線が多く残っていた場合メモリリークの疑いがあります。
その際はTree Viewで詳細を追いましょう。

画面下部のTree ViewはTime Profilerと同様に指定した範囲の詳細が表示されます。
このTree Viewの表示方法は4種類あります。
//image[instruments_allocations_view_type][表示方法の選択]
もっともオススメの表示方法はCall Treesです。
これによってどのコードによってアロケーションが発生したのかを追うことができます。
画面下部にCall Treesの表示オプションがあるので、Time Profilerで紹介した
@<img>{instruments_call_tress}と同じようにHide System Librariesなどのオプションを設定するとよいでしょう。
@<img>{instruments_allocations_call_tree}ではCall Treesの表示をキャプチャしました。
12.05MBのアロケーションがSampleScriptのOnClickedで発生していることがわかります。
//image[instruments_allocations_call_tree][Call Tree表示]

最後にGenerationsという機能を紹介します。
Xcodeの下部に「Mark Generations」というボタンがあります。
//image[instruments_allocations_mark_generation][Mark Generationボタン]
これを押下するとそのタイミングでのメモリが記憶されます。
その後、再度Mark Generationsを押下すると前回のデータと比較して新たに確保されたメモリ量が記録されます。
//image[instruments_allocations_generations][Generations]
@<img>{instruments_allocations_generations}の各Generationsは、
詳細を見るとCall Tree形式で表示されるため、メモリ増加が何によって発生したかを追うことが可能です。

=={tool_android_studio} Android Studio
Android StudioはAndroidの統合開発環境ツールです。
このツールを用いてアプリケーションの状態を計測できます。
プロファイル可能な項目はCPU、Memory、Network、Energyの4つです。
本節ではまずプロファイル方法について紹介し、その後CPUとMemoryの計測項目について解説します。
//image[android_studio_profile][プロファイル画面]

==={tool_android_studio_debug_start} プロファイル方法
プロファイル方法は2通りあります。
1つ目はAndroid Studio経由でビルドし、プロファイルする方法です。
この方法ではまずAndroid StudioのプロジェクトをUnityから書き出します。
Build Settingsで「Export Project」にチェックをいれてビルドします。
//image[android_studio_export][プロジェクトのExport]

次に書き出されたプロジェクトをAndroid Studioで開きます。
そしてAndroid端末を接続した状態で、右上にあるゲージのようなアイコンを押下するとビルドが開始します。
ビルド完了後、アプリが立ち上がりプロファイルが開始します。
//image[android_studio_tool_bar][Profile開始アイコン]

2つ目は実行中のプロセスをデバッガーにアタッチして計測する方法です。
まずAndroid Studioメニューの「View -> Tool Windows -> Profiler」からAndroid Profilerを開きます。
//image[android_studio_profiler_menu][Android Profilerを開く]

次に開いたProfilerの@<kw>{SESSIONS}から計測対象のセッションを選択します。
セッションを接続するためには、計測するアプリケーションを起動しておく必要があります。
また@<em>{Development Buildを適応したバイナリ}でないといけません。
セッションの接続が完了するとプロファイルが開始します。
//image[android_studio_profiler_attach][プロファイルするSESSIONを選択する]

2つ目のデバッガーにアタッチする方法はプロジェクトを書き出す必要がなく、気軽に利用できるので覚えておくと良いでしょう。

//info{
厳密にはUnityのDevelopment Buildではなく、AndroidManifest.xmlにてdebuggableやprofileableの設定を行う必要があります。
UnityではDevelopment Buildを行った場合、自動的にdebuggableがtrueになります。
//}

==={tool_android_studio_cpu} CPU計測
CPU計測の画面は@<img>{android_studio_cpu_thread_select}のようになります。
この画面だけでは何がどれぐらいの処理時間を消費しているかわかりません。
より詳細に見るためには、詳細に見たいスレッドを選択する必要があります。
//image[android_studio_cpu_thread_select][CPU計測トップ画面、スレッド選択]

スレッド選択後、Recordボタンを押下することでスレッドのコールスタックが計測されます。
@<img>{android_studio_record_button}のように計測タイプがいくつかありますが「Callstack Sample Recording」で問題ないでしょう。
//image[android_studio_record_button][Record開始ボタン]

Stopボタンを押下すると計測が終了し、結果が表示されます。
結果画面としてはUnity ProfilerのCPUモジュールのような表示になります。
//image[android_studio_sampling_result][コールスタック計測結果画面]

==={tool_android_studio_memory} Memory計測
Memory計測の画面は@<img>{android_studio_memory}のようになります。
この画面ではメモリの内訳を見ることはできません。
//image[android_studio_memory][Memory計測画面]

メモリの内訳を見る場合は追加で計測を行う必要があります。
計測方法は3種類あります。「Capture heap dump」は押下したタイミングでのメモリ情報が取得できます。
それ以外のボタンは計測区間中のアロケーションを分析するためのものです。
//image[android_studio_memory_record_options][Memory計測オプション]

例として@<img>{android_studio_memory_heap_dump}にHeap Dumpの計測結果をキャプチャしました。
詳細分析するには少し粒度が粗いので難易度が高いでしょう。
//image[android_studio_memory_heap_dump][Heap Dump結果]

=={tool_graphics_render_doc} RenderDoc
RenderDocはオープンソースで開発されており、無料で利用できる高品質なグラフィックスデバッガーツールです。
このツールは現時点ではWindowsとLinuxに対応していますが、Macには対応していません。
またサポートしているGraphics APIはVulkanやOpenGL(ES)、D3D11、D3D12などです。
そのためAndroidでの利用はできますがiOSでは利用できません。

本節ではAndroidアプリケーションを実際にプロファイルしてみます。
ただしAndroidのプロファイリングにはいくつか制約があるので注意してください。
まずAndroid OSバージョンは6.0以降が必須です。そして計測対象のアプリケーションはDebuggableを有効にする必要があります。
これはビルド時にDevelopment Buildを選択しておけば問題ありません。
なお、プロファイルに使用したRenderDocのバージョンはv1.18となります。

==={render_doc_ready} 計測方法
まずはRenderDocの準備を行いましょう。
公式サイト@<fn>{render_doc_url}からインストーラーをダウンロードし、ツールのインストールを行いましょう。
インストール後、RenderDocのツールを開きます。
//footnote[render_doc_url][@<href>{https://renderdoc.org/}]

//image[render_doc_top][RenderDoc 起動後の画面]

次にAndroidデバイスをRenderDocと接続します。
左下隅にある家マークを押下するとPCと接続されているデバイスリストが表示されます。
リストの中より計測したい端末を選択してください。
//image[render_doc_connection][デバイスとの接続]

次に接続したデバイスから起動するアプリケーションを選択しましょう。
右側にあるタブからLaunch Applicationを選択し、Executable Pathから実行するアプリを選択します。
//image[render_doc_launch_application_tab][Launch Applicationタブ 実行アプリ選択]

File Browserのウィンドウが開くので、今回計測するPacakge Nameを探しActivityを選択します。
//image[render_doc_launch_application_select][計測するアプリケーション選択]

最後にLaunch ApplicationからLaunchボタンを押下するとデバイスでアプリケーションが起動します。
またRenderDoc上では計測するためのタブが新たに追加されます。
//image[render_doc_capture_menu][計測用のタブ]

試しにCapture Frame(s) Immediatelyを押下すると、フレームデータがキャプチャされ、Capture collectedに並びます。
このデータをダブルクリックすることでキャプチャデータを開くことができます。

==={render_doc_capture_dislay} キャプチャデータの見方
RenderDocにはさまざまな機能がありますが、本節では重要な部分に絞って機能を説明します。
まず画面上部にはキャプチャしたフレームのタイムラインが表示されます。
それぞれの描画コマンドがどのような順番で行われたかを視覚的に捉えることができます。
//image[render_doc_timeline][タイムライン]

次にEvent Browserです。ここには各コマンドが上から順番に記載されています。
//image[render_doc_event_browser][Event Browser]

Event Browser内の上部にある「時計マーク」を押下すると、各コマンドの処理時間が「Duration」に表示されます。
計測タイミングによって処理時間が変動するのであくまで目安と考えるとよいでしょう。
また、DrawOpaqueObjectsのコマンドの内訳を見ると3つがBatch処理されており、1つだけBatchから外れた描画になっているのがわかります。

次に右側にタブで並んでいる各ウインドウについて説明します。
このタブの中にはEvent Browserで選択したコマンドの詳細情報を確認できるウインドウがあります。
とくに重要なのはMesh Viewer、Texture Viewer、Pipeline Stateの3つです。
//image[render_doc_window_tab][各ウインドウ]

初めにPipeline Stateについてです。
Pipeline Stateはそのオブジェクトが画面に描画されるまでの各シェーダーステージでどのようなパラメーターだったかを確認できます。
また使用されたシェーダーやその中身も閲覧可能です。
//image[render_doc_pipeline_state][Pipieline State]
パイプラインステートに表示されるステージ名は省略されているため正式名称を@<table>{pipeline_state_name}にまとめます。
//table[pipeline_state_name][PipielineStateの正式名称]{
ステージ名	正式名
--------------------
VTX	Vertex Input
VS	Vertex Shader
TCS	Tessellation Control Shader
TES	Tessellation Evaluation Shader
GS	Geometry Shader
RS	Rasterizer
FS	Fragment Shader
FB	Frame Buffer
CS	Compute Shader
//}

@<img>{render_doc_pipeline_state}ではVTXステージが選択されており、トポロジーや頂点入力データが確認できます。
他にも@<img>{render_doc_pipeline_state_framebuffer}のFBステージでは、アウトプット先のテクスチャの状態やBlend Stateなどの詳細を見られます。
//image[render_doc_pipeline_state_framebuffer][FB(Frame Buffer)のState]

また@<img>{render_doc_pipeline_state_fragment}のFSステージでは、フラグメントシェーダー内で利用されたテクスチャやパラメーターの確認ができます。
//image[render_doc_pipeline_state_fragment][FS(Fragment Shader)のState]
FSステージの中央にあるResourcesには、使用されたテクスチャやサンプラーが表記されています。
またFSステージの下段にあるUniform Buffersには、CBufferが表示されています。このCBufferにはfloatやcolorなど、数値情報のプロパティが格納されています。
各項目の右側には「Go」という矢印アイコンがあり、これを押下するとデータの詳細を確認できます。

FSステージの上段には使用したシェーダーが表示されています。Viewを押下するとそのシェーダーコードを確認できます。
表示をわかりやすくするためにDisassembly typeはGLSLがオススメです。
//image[render_doc_shader_code][Shaderコードの確認]

次にMesh Viewerです。これはMeshの情報を視覚的に見ることができ、最適化やデバッグに便利な機能です。
//image[render_doc_mesh_viewer][Mesh Viewer]
Mesh Viewerの上段にはメッシュの頂点情報がテーブル形式で表記されています。
下段にはカメラを動かして形状が確認できるプレビュー画面があります。
どちらもInとOutでタブが分かれており、変換の前後で値や見た目がどのように変わったかを把握できます。
//image[render_doc_mesh_view_in_out][Mesh ViewerのIn、OutのPreview表示]

最後にTexture Viewerです。
この画面ではEvent Browserで選択しているコマンドの「入力に使用したテクスチャ」と「出力結果」がわかります。
//image[texture_viewer_input_output][Texture Viewer テクスチャ確認画面]
画面右側の領域では入出力のテクスチャを確認できます。表示されているテクスチャを押下すると画面左側の領域に反映されます。
左側の画面はただ表示するだけでなく、カラーチャンネルをフィルターしたり、ツールバーの設定を適用させることができます。
//image[texture_viewer_toolbar][Texture Viewerのツールバー]
@<img>{texture_viewer_input_output}では、Overlayに「Wireframe Mesh」を選択していたので、
このコマンドで描画されたオブジェクトに黄色のワイヤーフレーム表示がされており視覚的にわかりやすくなっています。

またTexture ViewerにはPixel Contextという機能があります。
これは選択したピクセルの描画履歴を見るための機能です。
履歴がわかるということはあるピクセルに対して塗りつぶしがどれぐらいの頻度で行われているかを把握できます。
これはオーバードローの調査や最適化を行うのに便利な機能です。
ただしあくまでもピクセル単位なので全体的にオーバードローを調査するのには向いていません。
調査方法としては@<img>{texture_viewer_input_output}の左側の画面で、調査したい箇所を右クリックするとPixel Contextにその位置が反映されます。
//image[texture_viewer_pixel_context][Pixel Contextへの反映]
次にPixel ContextのHistoryボタンを押下すると、そのピクセルの描画履歴が確認できます。
//image[texture_viewer_pixel_history][ピクセルの描画履歴]
@<img>{texture_viewer_pixel_history}では4つの履歴があります。
緑色の行は深度テストなどのパイプラインのテストにすべて合格し描画されたことを示します。一部のテストに失敗し描画されなかった場合は赤色になります。
キャプチャした画像では、画面のクリア処理とカプセルの描画が成功し、PlaneとSkyboxが深度テストに失敗しています。
