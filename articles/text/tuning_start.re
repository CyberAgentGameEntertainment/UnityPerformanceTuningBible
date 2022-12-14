={start} パフォーマンスチューニングを始めよう
本章ではパフォーマンスチューニングを行うにあたり必要な事前準備や、取り組む際のフローに関して解説します。

まずはパフォーマンスチューニングを始める前に決めておくべきことや、考慮すべきことについて取り上げます。
プロジェクトがまだ初期フェーズの場合はぜひ目を通してみてください。
ある程度プロジェクトが進んでいたとしても、記載されている内容が考慮されているか改めてチェックするのもよいでしょう。
次に性能低下が発生しているアプリケーションに対して、どのように取り組んでいくべきかを解説します。
原因の切り分け方とその解決手法を学ぶことで、パフォーマンスチューニングの一連のフローを実践できるようになるでしょう。

=={start_ready} 事前準備
パフォーマンスチューニングの前に達成したい指標を決めましょう。言葉にすると簡単ですが実は難易度が高い作業です。
それは世の中にはさまざまなスペックの端末が溢れかえっており、低スペック端末を使っているユーザーを無視することはできないからです。
そのような状況の中でゲーム仕様やターゲットユーザー層、海外展開の有無など、さまざまなことを考慮する必要があります。
この作業はエンジニアだけでは完結しません。他職種の方と協議しながらクオリティラインを決める必要があり、技術検証も必要になるでしょう。

これらの指標は負荷を測るほどの機能実装やアセットが存在しない初期フェーズから決めることは難易度が高いです。
そのためある程度プロジェクトが進んでから決めるのも1つの手です。
ただしプロジェクトが@<em>{量産フェーズに入る前までに必ず決める}ように心掛けておきましょう。
一度量産を開始すると変更コストが莫大なものになるためです。
本節で紹介する指標を決めるには時間がかかりますが、焦らずしっかり進めていきましょう。

====[column] 量産フェーズ後の仕様変更の恐ろしさ

いま量産フェーズ後でありながら、低スペック端末で描画にボトルネックのあるプロジェクトがあるとします。
メモリはすでに限界近い使用量なので、距離によって低負荷モデルに切り替える手法も使えません。
そのためモデルの頂点数を削減することにしました。

まずはデータをリダクションするために発注し直します。新たに発注書が必要になるでしょう。
次にディレクターが品質をチェックし直す必要があります。そして最後にデバッグする必要もあります。
簡易的に書きましたが、実際にはより細かいオペレーションやスケジュール調整があるでしょう。

上記のような対応を必要とするアセットが、量産後ともなると数十〜数百個はあるでしょう。
これは時間と労力が大変掛かるため、プロジェクトにとって致命傷となりかねません。

このような事態を防ぐために@<em>{もっとも負荷の掛かるシーンを作成}し、指標を満たしているか@<em>{事前に検証を行う}ことがとても大事なのです。

====[/column]

==={start_ready_objective} 指標を決める
指標を決めると目指すべき目標が定まります。逆に指標がないといつまで経っても終わりません。
@<table>{table_object_tuning}に決めるとよい指標を取り上げます。
//table[table_object_tuning][指標]{
項目	要素
--------------------
フレームレート		常時どのぐらいのフレームレートを目指すか
メモリ	どの画面でメモリが最大になるか試算し、限界値を決める
遷移時間	遷移時間待ちはどれぐらいが適切か
熱	連続プレイX時間で、どのぐらいの熱さまで許容できるか
バッテリー	連続プレイX時間で、どれぐらいのバッテリー消費が許容できるか
//}

@<table>{table_object_tuning}の中でも、とくに@<em>{フレームレート}と@<em>{メモリ}は重要な指標なので必ず決めましょう。
この時点では低スペック端末のことは置いておきましょう。
まずはボリュームゾーンにある端末に対して指標を決めることが大事です。

//info{
ボリュームゾーンの定義はプロジェクト次第です。
ベンチマークになる他タイトルや市場調査を行って決めるのもよいでしょう。
もしくはモバイルデバイスのリプレイスが長期化した背景から、4年ぐらい前のミドルレンジをひとまず指標にしてもよいでしょう。
根拠は少し曖昧でも目指すべき旗を立てましょう。そこから調整していけばよいのです。
//}

ここで実例を考えてみましょう。
いま次のような目標を掲げているプロジェクトがあるとします。

 * 競合アプリケーションのよくない点はすべて改善したい
 * とくにインゲームは滑らかに動かしたい
 * 上記以外は競合と同等ぐらいでよい

この曖昧な目標をチームで言語化すると次のような指標が生まれました。

 * フレームレート
 **	インゲームは60フレーム、アウトゲームはバッテリー消費の観点から30フレームとする。
 * メモリ
 ** 遷移時間の高速化のために、インゲーム中にアウトゲームの一部リソースも保持しておく設計とする。最大使用量を1GBとする。
 * 遷移時間
 ** インゲームやアウトゲームへの遷移時間は競合と同じレベルを目指す。時間にすると3秒以内。
 * 熱
 ** 競合と同じレベル。検証端末で連続1時間は熱くならない。（充電していない状態）
 * バッテリー
 ** 競合と同じレベル。検証端末で連続1時間でバッテリー消費は20%ほど。

このように目指すべき指標が決まれば基準となる端末で触ってみましょう。
まったく目標に届かないという状態でなければ指標としてはよいでしょう。

====[column] ゲームジャンルによる最適化

今回は滑らかに動くことがテーマだったのでフレームレートを60フレームとしました。
他にもリズムアクションゲームや、FPS（ファーストパーソンシューティング）のような判定がシビアなゲームも高フレームレートが望ましいでしょう。
しかし高フレームレートにはデメリットもあります。それはフレームレートが高いほどバッテリーを消費することです。
他にもメモリは使用量が多いほどサスペンド時にOSからキルされやすくなります。
このようなメリット・デメリットを考慮しゲームジャンルごとに適切な目標を決めましょう。

====[/column]

==={start_ready_memory} メモリの最大使用量を把握する
この節ではメモリの最大使用量について焦点をあてます。
メモリの最大使用量を知るためには、まずはサポート対象となるデバイスがメモリをどれぐらい確保できるかを把握しましょう。
基本的には動作を保証する端末のうち最低スペックのデバイスで検証するのがよいでしょう。
ただしOSバージョンによってメモリ確保の仕組みが変更されている可能性があるため、出来ればメジャーバージョンが違う端末を複数用意するのがよいでしょう。
また計測するツールによっても計測ロジックが違うため、使用するツールは必ず1つに限定しましょう。

参考までに筆者がiOSにて検証した内容を記載しておきます。
検証プロジェクトではTexture2Dをランタイムで生成し、どれぐらいでクラッシュするかを計測しました。
コードは次のとおりです。
//listnum[texture_leak][検証コード][csharp]{
private List<Texture2D> _textureList = new List<Texture2D>();
...
public void CreateTexture(int size) {
	Texture2D texture = new Texture2D(size, size, TextureFormat.RGBA32, false);
	_textureList.Add(texture);
}
//}

//embed[latex]{
\clearpage
//}

検証結果は@<img>{memory_crash_ios}のようになりました。
//image[memory_crash_ios][クラッシュのしきい値]

検証環境はUnity 2019.4.3、Xcode 11.6を用いて、XcodeのDebug NavigatorのMemoryセクションの数値を参考にしています。
この検証結果からiPhone6Sや7などの搭載メモリが2GBの端末では、メモリを1.3GB以内におさめておくのがよいと言えるでしょう。
またiPhone6のような搭載メモリが1GBの端末をサポートする場合、メモリ使用量の制約がかなり厳しくなるのも見て取れます。
他にも特徴的なのはiOS11の場合、メモリ管理の仕組みが違うためかメモリ使用量が突出していることです。
検証する際にはこのようなOSによる差分がまれにあることを注意してください。

//info{
@<img>{memory_crash_ios}では検証環境が少し古いため、執筆時の最新環境を用いて一部計測し直しました。
環境はUnity 2020.3.25、2021.2.0の2バージョンとXcode13.3.1を用いて、OSバージョンが14.6と15.4.1のiPhoneXRにビルドを行いました。
結果として計測値に差異はとくにみられなかったので、まだ信頼性のあるデータかと思います。	
//}

====[column] メモリ計測ツール

計測するツールとして推奨したいのは、XcodeやAndroidStudioなどのネイティブ準拠のツールです。
たとえばUnity Profilerの計測では、プラグインなどが独自で確保したネイティブメモリ領域は計測対象外になります。
他にもIL2CPPビルドの場合、IL2CPPのメタデータ（100MBほど）も計測対象になっていません。
一方でネイティブツールのXcodeの場合、アプリで確保されたメモリはすべて計測されます。
そのためより正確に値が計測されるネイティブ準拠のツールを使うのがよいでしょう。

//image[xcode_debug_memory][Xcode Debug Navigator]

====[/column]

==={start_ready_low_spec} 動作保証端末を決める
パフォーマンスチューニングをどこまで行うかを決める指標として、最低限の動作を保証する端末を決めることも重要です。
この動作保証端末の選定は経験がないと即座に決めることは難しいですが、勢いで決めずにまずは低スペック端末の候補を洗い出すことから始めましょう。

筆者がオススメする方法としては「SoCのスペック」を計測したデータを参考にする方法です。
具体的にはベンチマーク計測アプリで計測されたデータをWeb上で探します。
まずは基準とした端末のスペックを把握し、それよりいくらか計測値の低い端末を何パターンか選定しましょう。

端末が洗い出せれば、実際にアプリケーションをインストールして動作確認をします。
動作が重くても落胆しないでください。これでやっと何を削ぎ落とすか議論できるスタートラインに立てました。
次項では機能を削ぎ落とすにあたって考えておきたい大事な仕様について紹介します。

//info{
ベンチマーク計測アプリはいくつかありますが、筆者はAntutuを基準としています。
こちらは計測データのまとめサイトが存在していますし、
有志の方も積極的に計測データを報告してくれているのが理由です。
//}

==={start_ready_quality_setting} 品質設定の仕様を決める
市場にさまざまなスペックの端末が溢れ返っているいま、1つの仕様で多くの端末をカバーするのは難しいでしょう。
そのため近年ではゲーム内にいくつかの品質設定を設けることで、さまざまな端末に対して安定した動作を保証することが主流となっています。

たとえば次のような項目を品質設定の高、中、低で切り分けるとよいでしょう。

 * 画面解像度
 * オブジェクトの表示数
 * 影の有無
 * ポストエフェクト機能
 * フレームレート
 * CPU負荷の高いスクリプトのスキップ機能など

ただし見た目のクオリティを下げる事になるので、ディレクターと相談しながら
どこまでのラインをプロジェクトとして許容できるかを一緒に探っていきましょう。

=={start_detect_degration} 未然に防ぐ
性能低下は不具合と同じく時間が経過することでさまざまな原因が絡み合い、調査の難易度が上がってしまいます。
なるべく早期に気づけるような仕組みをアプリケーションに実装しておくのがよいでしょう。
実装が簡単で効果の高い方法としては、画面上に現在のアプリケーションの状態を表示しておくことです。
少なくとも次の要素は画面上に常に表示することを推奨します。

 * 現在のフレームレート
 * 現在のメモリ使用量

//image[performance_stats][パフォーマンスの可視化]

フレームレートは体感で性能が低下していることがわかりますが、メモリはクラッシュすることでしか検知できません。
@<img>{performance_stats}のように画面に常に表示しておくだけでメモリリークが早期に発見できる確率が上がるでしょう。

この表示方法はさらに工夫することで効果が高くなります。
たとえばフレームレートの達成したい目標が30フレームなら、25〜30フレームを緑色、20〜25フレームを黄色、それ以下を赤色にしてみましょう。
そうすることで直感的にアプリケーションが基準を満たしているのか一目でわかるようになります。

=={start_tuning} パフォーマンスチューニングに取り組む
いくら未然に性能低下を防ぐ努力をしても、すべてを防ぐことは厳しいでしょう。
これは仕方ありません。開発を進める上で性能低下は切っても切り離せないものです。パフォーマンスチューニングと向き合う時は必ず来るでしょう。
以降ではどのようにしてパフォーマンスチューニングに取り組んでいくべきかを解説します。

==={start_tuning_attitude} 心構え
パフォーマンスチューニングを始める前にまずは大事な心構えを紹介します。
たとえばフレームレートが低下しているアプリケーションがあるとしましょう。明らかにリッチなモデルが複数体表示されています。
周りの人はこのモデルがきっと原因に違いないと言っています。
果たして本当にそうでしょうか、その証拠はどこにあるのかしっかり見定める必要があるでしょう。
パフォーマンスチューニングの心構えとして、必ず意識して欲しいことが2つあります。

1つ目は、@<em>{計測し、原因を特定}することです。@<em>{推測ではいけません}。

2つ目は、修正したら必ず@<em>{結果を比較}しましょう。前後のプロファイルを比較するとよいでしょう。
ポイントとしては修正箇所だけでなく、全体で性能低下が発生していないかを確認することです。
パフォーマンスチューニングの恐ろしい所は、修正箇所は高速化したが他の箇所で負荷が上がり、全体でみると性能低下しているという事がまれにあります。
これでは本末転倒です。

//image[tuning_basic_flow][パフォーマンスチューニングの心構え]
確実に原因を突き止め確実に速くなったことを確認する。それがパフォーマンスチューニングの大事な心構えなのです。

==={start_tuning_any_degration} 性能低下の種類
性能低下といってもそれぞれが指し示すものは違うでしょう。
本書では大別して以下の3つと定義します。(@<img>{degration})
//image[degration][性能低下の原因]

まずクラッシュする場合は@<em>{「メモリ超過」}か@<em>{「プログラムの実行エラー」}の2種類に大別されるでしょう。
後者についてはパフォーマンスチューニングの領域ではないため、具体的な内容は本書では取り扱いません。

次に画面の処理落ちやロードが長いのは@<em>{「CPUやGPUの処理時間」}が原因の大半を占めるでしょう。
以降では「メモリ」と「処理時間」に焦点を当てて性能低下を深掘ります。

=={start_memory} メモリ超過の原因切り分け
クラッシュの原因にはメモリ超過が考えられると取り上げました。
ここからはメモリ超過の原因をさらに分解していきましょう。

==={start_memory_leak} メモリがリークしている
メモリ超過の原因の1つとしてメモリのリークが考えられます。
これを確認するためにシーンの遷移に伴ってメモリ使用量が徐々に増えるかどうか確認しましょう。
ここでいうシーンの遷移とはただの画面遷移ではなく、大きく画面が転換する事を指します。
たとえばタイトル画面からアウトゲーム、アウトゲームからインゲームなどです。
計測する際は次の手順で進めましょう。

 1. あるシーンでのメモリ使用量をメモする
 2. 違うシーンに遷移する
 3. 「1」〜「2」の流れを3〜5回ほど繰り返す

計測した結果、メモリ使用量が純増していれば間違いなく何かがリークしています。
これは目に見えない不具合と言ってもいいでしょう。まずはリークを無くしましょう。

また「2」の遷移を行う前に、いくつか画面遷移を挟んでおくのもよいでしょう。
なぜなら特定の画面でロードしたリソースだけが、例外的にリークしていたという事もあり得るためです。

リークに確信が持てたらリークの原因を探りましょう。
@<hd>{start_leak_survey}にて具体的な調査方法について解説します。

====[column]繰り返す理由

これは筆者の経験談ですが、リソース解放後（UnloadUnusedAssets後）に
タイミングの問題でいくつかリソースが解放されていないケースがありました。
この解放されていないリソースは次のシーンに遷移すると解放されます。
これに対して、遷移を繰り返した際にメモリ使用量が徐々に増加する場合は、いずれクラッシュを引き起こします。
前者の問題と切り分けるために本書では、メモリ計測時に遷移を何度か繰り返す手法を推奨しています。

ちなみに前者のような問題があった場合、リソース解放時には何かしらのオブジェクトがまだ参照を握っており、その後に解放されているのでしょう。
致命傷とはなりませんが、原因調査を行い解決しておくのが良いでしょう。

====[/column]

==={start_memory_large_used} メモリ使用量が単純に多い
リークしていない状態でメモリ使用量が多い場合、削減できる箇所を探っていく必要があります。
@<hd>{start_memory_reduction}にて、具体的な方法について解説します。

=={start_leak_survey} メモリリークを調査しよう
まずはメモリリークを再現させた上で、次に紹介するツールを用いて原因を探っていきましょう。
ここでは簡単にツールの特徴を説明します。
ツールの使い方の詳細は@<chapref>{profile_tool}にて取り扱うので、参考にしながら調査してみてください。

===={start_leak_survey_unity_memory} Profiler (Memory)
Unityエディターにデフォルトで搭載されているプロファイラーツールです。
そのため気軽に計測ができます。基本的には「Detailed」かつ「Gather object references」を設定した状態でメモリをスナップショットし、調査することになるでしょう。
このツールでの計測データは、他のツールと違いスナップショットの比較はできません。
使い方の詳細は@<hd>{profile_tool|memory_module_unity_prfoiler}を参照してください。

===={start_leak_survey_memory_profiler} Memory Profiler
こちらはPackage Managerからインストールする必要があります。
ツリーマップではグラフィカルにメモリの内容が表示されます。
Unity公式でサポートされており、現在も頻繁にアップデートされています。
v0.5以降、参照関係を追跡する方法が大きく改善されているため最新版の利用を推奨します。
使い方の詳細は@<hd>{profile_tool|tool_memory_profiler}を参照してください。

===={start_leak_survey_heap_explorer} Heap Explorer
こちらはPackage Managerからインストールする必要があります。
個人が開発したツールですが、非常に使いやすく動作も軽量です。
参照関係を追跡する際にリスト形式で見ることが可能で、v0.4以前のMemory Profilerの痒い所に手が届いたツールです。
v0.5のMemory Profilerが利用できない際の代替ツールとして利用すると良いでしょう。
使い方の詳細は@<hd>{profile_tool|tool_heap_explorer}を参照してください。

=={start_memory_reduction} メモリを削減しよう
メモリを削減するポイントは@<em>{大きな箇所から削る}ことです。
なぜなら1KBを1,000個削っても1MBの削減にしかなりません。しかし10MBのテクスチャを圧縮して2MBになると8MBも削減されます。
費用対効果を考えて、まずは大きいものから削減していくことを意識しましょう。

本節ではメモリ削減に使用するツールはProfiler(Memory)として話を進めます。
使用したことがない方は@<hd>{profile_tool|memory_module_unity_prfoiler}にて詳細を確認してください。

以降の節では、削減する際に見るべき項目を取り上げていきます。

==={start_memory_reduction_asset} Assets
Simple ViewでAssets関連が多い場合、不要なアセットやメモリリークの可能性が考えられます。
ここでいうAssets関連とは@<img>{profiler_memory_simple}の矩形で囲った部分です。
//image[profiler_memory_simple][Assets関連の項目]
この場合、調査するべきことは次の3つです。

===={start_memory_reduction_asset_unused} 不要アセット調査
不要なアセットとは、現在のシーンにまったく必要のないリソースを指します。
たとえばタイトル画面でしか使用しないBGMがアウトゲーム内でもメモリに常駐しているなどです。
まずは今のシーンに必要なものだけにしましょう。

===={start_memory_reduction_asset_duplicate} 重複アセット調査
これはアセットバンドル対応を行なった際によく発生します。
アセットバンドルの依存関係の切り分け方が良くないために、同じアセットが複数のアセットバンドルに含まれている状態です。
しかし依存関係は切り分けすぎてもダウンロードファイル数の増加や、ファイル展開コストの増加に繋がります。
この辺りは計測しながらバランス感覚を養う必要があるかもしれません。
アセットバンドルに関する詳細は、@<hd>{basic|basic_unity_assetbundle}を参照してください。

===={start_memory_reduction_asset_regulation} レギュレーションをチェックする
それぞれの項目のレギュレーションが守られているかを見直しましょう。
レギュレーションがない場合は、メモリの見積もりが適切にできていない可能性があるので確認しましょう。

たとえばテクスチャなら次のような内容をチェックをするとよいでしょう。

 * サイズは適切か
 * 圧縮設定は適切か
 * MipMapの設定が適切か
 * Read/Writeの設定が適切かなど

それぞれのアセットごとに気をつけるべき点は@<chapref>{tuning_practice_asset}を参照してください。

==={start_memory_reduction_gc} GC（Mono）
Simple ViewでGC（Mono）が多い場合は、一度に大きなGC.Allocが発生している可能性が高いです。
もしくは毎フレームGC.Allocが発生してメモリが断片化しているかもしれません。
これらが原因でマネージドヒープ領域が余計に拡張している可能性があります。
この場合はGC.Allocを地道に削減していきましょう。

マネージドヒープに関しては@<hd>{basic|basic_memory}を参照してください。
同様にGC.Allocに関する詳細は@<hd>{basic|basic_csharp_stack_heap}にて取り扱っています。

=====[column] バージョンによる表記違い

2020.2以降では「GC」と表示されていますが、2020.1以下のバージョンまでは「Mono」と記載されています。
どちらもマネージドヒープの占有量を指しています。

=====[/column]

==={start_memory_reduction_other} Other
Detailed Viewで怪しい項目がないかを確認しましょう。
たとえば@<em>{Other}の項目などは一度開いて調査するとよいでしょう。
//image[profiler_memory_other][Otherの項目]
筆者の経験ではSerializedFileやPersistentManager.Remapperが、かなり肥大化していた事例がありました。
複数プロジェクトで数値比較ができる場合、一度比較してみるのも良いでしょう。
それぞれの数値を見比べると異常値がわかるかもしれません。
詳細は@<hd>{profile_tool|memory_module_detailed_view}を参照してください。

==={start_memory_reduction_plugin} プラグイン
ここまではUnityの計測ツールを使って原因の切り分けをしてきました。
しかしUnityで計測できるのはUnityが管理しているメモリのみです。
つまりプラグインで独自に確保しているメモリ量などは計測されません。
ThirdParty製品で余分にメモリを確保していないかを調べましょう。

具体的にはネイティブ計測ツール（XcodeのInstruments）を利用します。
詳細は@<hd>{profile_tool|tool_instruments}を参照してください。

==={start_memory_reduction_planning} 仕様を検討する
これは最後の手段です。
ここまでに取り上げてきた内容で削れる部分がない場合、仕様を検討するしかありません。
以下に例を挙げます。

 * テクスチャの圧縮率を変更する
 ** テクスチャの一部分だけ圧縮率を一段回上げる
 * ロード・アンロードするタイミングを変更する
 ** 常駐メモリに載っているオブジェクトを解放し都度ロードにする
 * ロード仕様を変更する
 ** インゲームでロードするキャラクターのバリエーションを1種類減らす

どれも影響範囲は大きく、ゲームの面白さに根本的に関わる可能性もあります。
そのため仕様の検討は最後の手段なのです。
そうならないように早い段階でメモリの見積もり、計測はしっかり行いましょう。

=={start_process_reduction} 処理落ちの原因切り分け
ここからは処理時間を計測し最適化していく流れについて紹介します。
画面の処理落ちは「瞬間的な処理落ち」か「定常的な処理落ち」かで対処法が変わります。

//info{
瞬間的な処理落ちは針のように尖った処理負荷が計測されます。
その見た目からスパイクとも言われています。
//}

//image[process_heavy][スパイクと定常処理負荷]

@<img>{process_heavy}では、定常負荷が急に増加しており、さらに定期的なスパイクも発生している計測データです。
どちらの事象もパフォーマンスチューニングが必要でしょう。
まずは比較的シンプルな瞬間的な負荷調査を解説します。
その後、定常的な負荷調査について解説します。

=={start_spike} 瞬間的な負荷を調査する
スパイクの調査方法としてはProfiler(CPU)を用いて原因を調査します。

ツールの詳細な使い方は@<hd>{profile_tool|cpu_module_unity_profiler}を参照してください。
まずは原因がGCによるものかどうかを切り分けましょう。
原因の切り分け自体にはDeep Profileは必要ありませんが、解決するためには必要になるでしょう。

==={start_spike_gc} GCによるスパイク
GC（ガベージコレクション）が発生している場合、GC.Allocを削減する必要があります。
どの処理がどれぐらいアロケートしているかはDeep Profileを利用するとよいでしょう。
まず削減すべきところは費用対効果の高い箇所です。
次にあげるような項目を中心に修正するとよいでしょう。

 * 毎フレームアロケートしている箇所
 * 大量のアロケートが発生している箇所

アロケーションは少ないほど良いですが、必ずゼロにすべきということではありません。
たとえば生成処理（Instantiate）などで発生するアロケートは防ぎようがないでしょう。
このような場合は都度オブエジェクトを生成せずに、オブジェクトを使い回すプーリングといった手法が有効です。
GCの詳細は@<hd>{basic|basic_csharp_gc}を参照してください。

==={start_spike_heavy_process} 重い処理によるスパイク
GCが原因でない場合は何かしらの重い処理が瞬間的に行われています。
ここでもDeep Profileを利用して何の処理がどれぐらい重いのかを調査し、もっとも処理に時間がかかっている箇所を見直していきましょう。

よくある一時的な重い処理といえば次のようなものが考えられるでしょう。

 * Instantiate処理
 * 大量のオブジェクト、あるいは階層が深いオブジェクトのアクティブ切り替え
 * 画面のキャプチャ処理など

このようにプロジェクトコードにかなり依存する部分なので、解決方法は一律にこうすればよいというものはありません。
実際に計測して原因が判明したらプロジェクトメンバーに計測結果を共有し、どのように改善すべきか検討してみてください。

=={start_heavy_process} 定常的な負荷を調査する
定常的な処理負荷を改善する際は、1フレーム内での処理をいかに削減するかが重要になります。
1フレーム内で行っている処理は大別するとCPU処理とGPU処理に分けることができます。
まずはこの2つの処理のうち、どちらがボトルネックになっているのか、もしくは同じくらいの処理負荷なのかを切り分けるとよいでしょう。

//info{
CPUにボトルネックがある状態をCPUバウンド、GPUにボトルネックがある状態をGPUバウンドと呼びます
//}

切り分ける簡単な方法として、次に当てはまる内容があればGPUバウンドの可能性が高いでしょう。

 * 画面の解像度を下げた際に処理負荷が劇的に改善する
 * Profilerで計測した際に@<kw>{Gfx.WaitForPresent}が存在する

逆にこれらがない場合はCPUバウンドの可能性があります。
以降ではCPUバウンドとGPUバウンドの調査方法について説明します。

==={start_heavy_process_cpu} CPUバウンド
CPUバウンドは前節でも取り扱ったCPU (Profiler)を利用します。
Deep Profileを利用して調査し、特定のアルゴリズムに大きな処理負荷がかかっていないかを確認します。
大きな処理負荷がない場合は均等に重いということになるので地道に改善していきましょう。
地道な改善を重ねても到底目標の削減値に及ばない場合は
@<hd>{start_ready_quality_setting}に立ち戻って再考し直すのもよいでしょう。

==={start_heavy_process_gpu} GPUバウンド
GPUバウンドの場合はFrame Debuggerを利用して調査するのがよいでしょう。
使い方の詳細は@<hd>{profile_tool|tool_frame_debugger}を参照してください。

===={start_heavy_process_gpu_resolution} 解像度が適切か
GPUバウンドの中でも解像度はGPUの処理負荷に大きく影響します。
そのため解像度を適切に設定できていない状態であれば、まずは最優先に適切な解像度にする必要があります。

まずは想定の品質設定で適切な解像度になっているかを確認しましょう。
確認方法としてはFrame Debugger内で処理しているレンダーターゲットの解像度に注目するとよいでしょう。
次にあげるようなことを意図的に実装していない場合、最適化に取り組みましょう。

 * UI要素だけデバイスのフル解像度で描画されている
 * ポストエフェクト用の一時テクスチャの解像度が高いなど

===={start_heavy_process_gpu_unused} 不要なオブジェクトがあるか
Frame Debuggerで不要な描画がないかを確認しましょう。
たとえば必要のないカメラがアクティブ状態になっており、裏で関係のない描画が行われているかもしれません。
また他の遮蔽物によって直前の描画がムダになっているケースが多いのであれば、@<kw>{オクルージョンカリング}を検討するのもよいでしょう。
オクルージョンカリングの詳細は@<hd>{tuning_practice_graphics|practice_graphics_occlusion_culling}を参照してください。

//info{
オクルージョンカリングはデータを事前に準備する必要があり、そのデータをメモリに展開するためメモリ使用量が増えることにも注意しましょう。
このようにパフォーマンスを向上させるために、メモリに事前に準備した情報を構築するのはよくある手法です。
メモリとパフォーマンスは反比例することが多いので、何かを採用する際はメモリも意識すると良いでしょう。
//}

===={start_heavy_process_gpu_batching} バッチングが適切か
描画対象をまとめて描画する事をバッチングと言います。
まとめて描画されることで描画効率が向上するためGPUバウンドに効果があります。
たとえばStatic Batchingを利用すると複数の動かないオブジェクトのメッシュをまとめてくれます。

バッチングに関しては色々な方法があるため、代表的なものをいくつかあげておきます。
気になったものがあれば@<hd>{tuning_practice_graphics|practice_graphics_draw_call}を参照してみください。

 * Static Batching
 * Dynamic Batching
 * GPU Instancing
 * SRP Batcherなど

===={start_heavy_process_gpu_any} 個別に負荷をみる
まだ処理負荷が高い場合は個別にみていくしかありません。
オブジェクトの頂点数が多すぎたり、Shaderの処理に原因があるかもしれません。
これを切り分けるには個々のオブジェクトに対してアクティブを切り替え、処理負荷の変わり具合を見ていきます。
具体的には背景を消してみてどうなるか、キャラクターを消してみてどうなるか、といったようにカテゴリごとに絞っていきます。
処理負荷の高いカテゴリがわかれば、さらに次のような要素をみていくとよいでしょう。

 * 描画するオブジェクトが多すぎないか
 ** まとめて描画できないか検討する
 * 1オブジェクトの頂点数が多すぎないか
 ** リダクション、LODを考える
 * シンプルなShaderに付け替えて処理負荷が改善するか
 ** Shaderの処理を見直す

===={start_heavy_process_gpu_others} それ以外
それぞれのGPU処理が積み上がって重いと言えるでしょう。
この場合は地道に1つ1つ改善していくしかありません。

またこちらもCPUバウンドと同様に、到底目標の削減値に及ばない場合
@<hd>{start_ready_quality_setting}に立ち戻って再考するのもよいでしょう。

=={start_summary} まとめ
この章では「パフォーマンスチューニング前」と「パフォーマンスチューニング中」に気をつけて欲しい事を取り上げました。

パフォーマンスチューニング前に気をつけることは以下のとおりです。

 * 「指標」「動作保証端末」「品質設定の仕様」を決めること
 ** 量産前までに検証を行い指標を確定させること
 * 性能低下に気づきやすい仕組みを作っておくこと

パフォーマンスチューニング中に気をつけることは以下のとおりです。

 * 性能低下の原因を切り分け、適切な対処を行うこと
 * 「計測」、「改善」、「再度計測 (結果のチェック）」の一連の流れを必ず行うこと

パフォーマンスチューニングはここまで解説してきたように、計測して原因を切り分けていくことが重要です。
このドキュメントに記載されていない事例が発生しても、その基礎が守られていれば大きな問題になることはないでしょう。
一度もパフォーマンスチューニングをしたことがない方は、ぜひこの章の内容を実践してもらえると幸いです。
