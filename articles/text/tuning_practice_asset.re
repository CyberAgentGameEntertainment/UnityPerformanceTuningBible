={practice_asset} Tuning Practice - Asset
ゲーム製作ではテクスチャやメッシュ、アニメーション、サウンドなどさまざまな種類のアセットを大量に扱います。
そこで本章ではそれらのアセットに関して、パフォーマンスチューニングを行う上で気をつけるべき設定項目など、実践的な知識についてまとめます。

=={practice_asset_texture} Texture
テクスチャの元となる画像データはゲーム製作において欠かせない存在です。
その一方でメモリ消費量は比較的多くなるため設定は適切に行う必要があります。

==={practice_asset_texture_import} インポート設定
@<img>{texture_settings}はUnityにおけるテクスチャのインポート設定です。
//image[texture_settings][テクスチャ設定][scale=0.75]

この中でもとくに注意すべきものについて紹介します。

==={practice_asset_texture_read_write} Read/Write
このオプションはデフォルトでは無効になっています。無効な状態であればGPUメモリにしかテクスチャは展開されません。
有効にした場合はGPUメモリだけでなくメインメモリにもコピーされるため、2倍の消費量になってしまいます。
そのため@<code>{Texture.GetPixel}や@<code>{Texture.SetPixel}といったAPIを使用せずShaderでしかアクセスしない場合、必ず無効にしましょう。

またランタイムで生成したテクスチャは、@<list>{makeNoLongerReadable}で示すようにmakeNoLongerReadableをtrueに設定することで、メインメモリへのコピーを回避できます。
//listnum[makeNoLongerReadable][makeNoLongerReadableの設定][csharp]{
texture2D.Apply(updateMipmaps, makeNoLongerReadable: true)
//}

//info{
GPUメモリからメインメモリへのテクスチャ転送は時間がかかるため、読み取り可能な場合は、どちらにもテクスチャを展開することでパフォーマンスの向上が図られています。
//}

==={practice_asset_texture_mipmap} Generate Mip Maps
Mip Mapの設定を有効にするとテクスチャのメモリ使用量が約1.3倍になります。
この設定は3D上のオブジェクトに対して行うことが一般的で、遠くのオブジェクトに対して、ジャギ軽減やテクスチャ転送量削減を目的に設定します。
2DスプライトやUI画像に対しては基本的に不要なので、無効にしておきましょう。

==={practice_asset_texture_aniso} Aniso Level
Aniso Levelはオブジェクトを浅い角度で描画した際に、テクスチャの見栄えをボケずに描画するための機能です。
この機能は主に地面や床など遠くまで続いているオブジェクトに使用されます。
Aniso Level値が高いほどその恩恵を受けられますが、処理コストはかさみます。
//image[aniso_level_compare][Aniso Level適応イメージ][scale=1.0]

設定値は0~16までありますが少し特殊な仕様となっています。

 * 0: プロジェクト設定によらず必ず無効
 * 1: 基本的には無効。ただし、プロジェクト設定がForced Onの場合は、9~16の値にクランプされる
 * それ以外: その値での設定

テクスチャをインポートするとデフォルトでは値が1になっています。
そのためハイスペック端末が対象でない限りForced Onの設定は推奨できません。
Forced Onの設定は「Project Settings -> Quality」のAnisotropic Texturesから設定可能です。

//image[project_settings_aniso][Forced Onの設定]

Aniso Levelの設定が効果のないオブジェクトで有効になっていないか、もしくは効果のあるオブジェクトに対して無闇に高い設定値になっていないかを確認しましょう。

//info{
Aniso Levelによる効果は、線形ではなく段階で切り替わっている挙動になっています。
筆者が検証した結果では、0~1、2〜3、4~7、8以降という4段階で変化していました。
//}

==={practice_asset_texture_compress} 圧縮設定
テクスチャは特別な理由がない限り圧縮するべきでしょう。
もしプロジェクト内に無圧縮なテクスチャが存在した場合、ヒューマンエラーかレギュレーションがない可能性があります。早急に確認しましょう。
圧縮設定に関する詳細は@<hd>{basic|basic_asset_data_compression}にてご確認ください。

このような圧縮設定に関してはヒューマンエラーが起きないようにTextureImporterを利用し自動化することを推奨します。
//listnum[textureImporter][TextureImporterの自動化例][csharp]{
using UnityEditor;

public class ImporterExample : AssetPostprocessor
{
    private void OnPreprocessTexture()
    {
        var importer = assetImporter as TextureImporter;
        importer.isReadable = false; // Read/Writeの設定なども可能

        var settings = new TextureImporterPlatformSettings();
        // Android = "Android", PC = "Standalone"を指定
        settings.name = "iPhone";
        settings.overridden = true;
        settings.textureCompression = TextureImporterCompression.Compressed;
        settings.format = TextureImporterFormat.ASTC_6x6; // 圧縮形式を指定
        importer.SetPlatformTextureSettings(settings);
    }
}
//}

またすべてのテクスチャが同じ圧縮形式である必要はありません。
たとえばUI画像の中でも全体的にグラデーションがかかっている画像は、圧縮による品質低下が目立ちやすいです。
そのような場合は対象となる一部の画像だけ、圧縮率を低めに設定すると良いでしょう。
逆に3Dモデルなどのテクスチャは品質低下がわかりにくいため、圧縮率を高くするなど適切な設定を探るのがよいでしょう。

=={practice_asset_mesh} Mesh
Unityにインポートしたメッシュ（モデル）を扱う場合の注意点を紹介します。インポートしたモデルデータは設定次第でパフォーマンスは上がります。注意すべきポイントは次の4つです。

 * Read/Write Enabled
 * Vertex Compression
 * Mesh Compression
 * Optimize Mesh Data

==={practice_asset_mesh_read_write_enabled} Read/Write Enabled
Meshの注意点1つ目はRead/Write Enabledです。
モデルのインスペクターにあるこのオプションはデフォルトでは無効になっています。
//image[practice_asset_mesh_inspector][Read/Write設定]
ランタイム中、メッシュにアクセスする必要がなければ無効にしておきましょう。具体的にはモデルをUnity上に配置して、AnimationClipを再生するくらいの使い方であれば、Read/Write Enabledは無効で問題ありません。

有効にすると、CPUでアクセス可能な情報をメモリに保持するためメモリを2倍消費します。無効にするだけでメモリの節約になるためぜひ確認してみてください。

==={practice_asset_vertex_compression} Vertex Compression
Vertex Compressionはメッシュの頂点情報の精度をfloatからhalfに変更するオプションです。
これによって@<em>{ランタイム時のメモリ使用量とファイルサイズを小さくすることが可能}です。
「Project Settings -> Player」のOtherで設定ができ、デフォルト設定では次のようになっています。
//image[practice_asset_vertex_compression_settings][Vertex Compressionのデフォルト設定]

ただし、このVertex Compressionは次のような条件に当てはまると無効化されるので気をつけましょう。

 * Read/Writeが有効
 * Mesh Compressionが有効
 * Dynamic Batchingが有効かつ適応可能なメッシュ（頂点数が300以下、頂点属性が900以下）

==={practice_asset_mesh_compression} Mesh Compression
Mesh Compressionはメッシュの圧縮率を変更できます。圧縮率が高いほどファイルサイズが小さくなりストレージの占める割合が削減されます。
圧縮されたデータは実行時に展開されます。そのため@<em>{ランタイム時のメモリ使用量には影響がありません}。

Mesh Compresssionの圧縮設定には4つの選択肢があります。

 * Off: 非圧縮
 * Low: 低圧縮
 * Medium: 中程度の圧縮
 * High: 高圧縮

//image[practice_asset_mesh_compression][Mesh Compression]

@<hd>{practice_asset_vertex_compression}で触れましたが、このオプションを有効にすると@<em>{Vertex Compressionが無効化されます}。
とくにメモリ使用量の制限が厳しいプロジェクトでは、このデメリットを把握した上で設定をしてください。

==={practice_asset_optimize_mesh_data} Optimize Mesh Data
Optimize Mesh Dataはメッシュに不要な頂点データを自動で削除する機能です。
不要な頂点データの判定には使用しているShaderから自動判定されます。
これはランタイム時のメモリ、ストレージともに削減効果があります。
「Project Settings -> Player」のOtherで設定が可能です。
//image[practice_asset_optimize_mesh_data][Optimize Mesh Dataの設定][scale=0.8]

ただしこのオプションは頂点データが自動削除されるので便利ですが、予期せぬ不具合を引き起こす可能性があるので注意しましょう。
たとえばランタイムでMaterialやShaderを切り替えた際、アクセスしたプロパティが削除されており描画結果がおかしくなることがあります。
他にもMeshのみをアセットバンドル化する際、Materialの設定が正しくなかった場合に不要な頂点データと判定されることもあります。
これはParticle SystemのようなMeshの参照だけを持たせる場合にありがちです。

=={practice_asset_material} Material
Material（マテリアル）はオブジェクトをどのように描画するか決める重要な機能です。
身近な機能ですが、使い方を誤ると簡単にメモリリークしてしまいます。
本節では安全なマテリアルを使用する方法を紹介します。

===={practice_asset_material_clone} パラメーターにアクセスするだけで複製される
マテリアルの最大の注意点は、パラメーターにアクセスするだけで複製されてしまうことです。そして複製されていることに気づきづらいことです。

次のコードを見てみましょう。
//listnum[material_clone][Materialの複製される例][csharp]{
Material material;

void Awake()
{
    material = renderer.material;
    material.color = Color.green;
}
//}

マテリアルのcolorプロパティにColor.greenをセットしているだけの簡単な処理です。このrendererのマテリアルは複製されています。そして複製されたオブジェクトは明示的にDestroyする必要があります。
//listnum[material_destroy][複製されたMaterialの削除例][csharp]{
Material material;

void Awake()
{
    material = renderer.material;
    material.color = Color.green;
}

void OnDestroy()
{
    if (material != null)
    {
        Destroy(material)
    }
}
//}
このように複製されたマテリアルをDestroyすることでメモリリークを回避できます。

===={practice_asset_material_instantiate} 生成したマテリアルの掃除を徹底しよう
動的に生成したマテリアルもメモリリークの原因になりやすいです。生成したマテリアルも使い終わったら確実にDestroyしましょう。

次のサンプルコードを見てみましょう。
//listnum[material_dynamic][動的に生成したMaterialの削除例][csharp]{
Material material;

void Awake()
{
    material = new Material();  // マテリアルを動的生成
}

void OnDestroy()
{
    if (material != null)
    {
        Destroy(material);  // 使い終わったマテリアルをDestroy
    }
}
//}
生成したマテリアルは使い終わったタイミング（OnDestroy）でDestroyしましょう。プロジェクトのルールや仕様に合わせて、適切なタイミングでマテリアルはDestroyしましょう。

=={practice_asset_animation} Animation
アニメーションは2D、3D問わず多く使用されるアセットです。
本節ではAnimation ClipやAnimatorに関するプラクティスを紹介します。

==={practice_asset_animation_influence} スキンウェイト数の調整
モーションは内部的にはそれぞれの頂点がどの骨からどれぐらい影響を受けているかを計算して位置を更新しています。
この位置計算に加味する骨の数をスキンウェイト数、またはインフルエンス数と呼びます。
そのためスキンウェイト数を調整することで負荷削減ができます。
ただしスキンウェイト数を減らすと見た目がおかしくなる可能性があるので調整する際には検証しましょう。

スキンウェイト数は「Project Settings -> Quality」のOtherから設定が可能です。
//image[practice_asset_animation_influence][スキンウェイトの調整]
この設定はスクリプトから動的に調整することも可能です。
そのため低スペック端末はSkin Weightsを2に設定し、高スペック端末は4に設定するなどの微調整が可能です。
//listnum[skinweight_settings][SkinWeightの設定変更][csharp]{
// QualitySettingsを丸ごと切り替える方法 
// 引数の番号は設定画面のLevelsの並び順で上から0、1..となっています
QualitySettings.SetQualityLevel(0);

// SkinWeightsだけ変更する方法
QualitySettings.skinWeights = SkinWeights.TwoBones;
//}

==={practice_asset_animation_key_frame_reduction} キーの削減
アニメーションファイルはキーの数に依存してストレージとランタイム時のメモリを圧迫します。
キー数を削減する方法の1つとしてAnim. Compressionという機能があります。
このオプションはモデルのインポート設定からアニメーションタブを選択することで表示されます。
Anim. Compressionを有効にするとアセットインポート時に不要なキーが自動で削除されます。
//image[practice_asset_key_frame_reduction][Anim. Compression設定画面]
Keyframe Reductionは値の変化が少ない場合にキーが削減されます。
具体的には直前の曲線と比較して誤差（Error）範囲内に収まっていた場合に削除されます。
この誤差範囲は調整することが可能です。
//image[practice_asset_animation_error][Errorの設定]
少しややこしいですがErrorの設定は項目によって値の単位が違います。
Rotationは角度、PositionとScaleがパーセントです。
キャプチャした画像の許容誤差はRotationが0.5度、PositionとScaleは0.5%となります。
詳しいアルゴリズムはUnityのドキュメント@<fn>{animation_error_tolerance}にあるので気になる方は覗いてみてください。
//footnote[animation_error_tolerance][@<href>{https://docs.unity3d.com/Manual/class-AnimationClip.html#tolerance}]

Optimalはさらにわかりにくいのですが、Dense Curveというフォーマットと、Keyframe Reductionの2つの削減方法を比較し、データが小さくなる方を採用します。
押さえておくべきポイントとしては、DenseはKeyframe Reductionと比べるとサイズは小さくなります。
ただしノイジーになりやすいためアニメーションクオリティが低下する可能性があります。
この特性を把握した上で、あとは実際のアニメーションを目視して許容できるか確認していきましょう。

==={practice_asset_animation_update_reduction} 更新頻度の削減
Animatorはデフォルト設定では画面に映っていなくても毎フレーム更新を行います。
この更新方法を変更できるカリングモードというオプションがあります。
//image[practice_asset_animator_cull][カリングモード]

それぞれのオプションの意味は次のようになります。
//table[animator_culling_mode][カリングモードの説明]{
種類	意味
--------------------
Always Animate	画面外にいても常に更新を行います。（デフォルト設定）
Cull Update Transform	画面外にいるときにIKやTransformの書き込みを行ないません。@<br>{}ステートマシンの更新は行います。
Cull Completely	画面外にいるとステートマシンの更新を行ないません。@<br>{}アニメーションが完全に止まります。
//}
それぞれのオプションについて注意点があります。
まずCull Completelyを設定する場合、Rootモーションを利用している際は注意が必要です。
たとえば画面外からフレームインするようなアニメーションの場合、画面外にいるためアニメーションは即座に停止されます。
その結果いつまでたってもフレームインしなくなります。

次にCull Update Transformです。
これはTransformの更新がスキップされるだけなので、とても使い勝手のよいオプションのように感じます。
しかし揺れものといったTransformに依存した処理がある場合は注意が必要です。
たとえばキャラクターがフレームアウトすると、そのタイミングのポーズから更新がされなくなります。
そして再びフレームインした際に新たなポーズに更新されるため、その弾みで揺れものが大きく動く可能性があります。
各オプションの一長一短を把握した上で設定を変更するとよいでしょう。

また、これらの設定を用いても動的にアニメーションの更新頻度を細かく変更することはできません。
たとえばカメラから距離が離れているオブジェクトのアニメーションの更新頻度を半分にするなどの最適化です。
その場合はAnimationClipPlayableを利用するか、Animatorを非アクティブにしたうえで自身でAnimator.Updateを呼ぶ必要があります。
どちらも自前でスクリプトを書く必要がありますが、前者に比べ後者の方が簡単に導入できるでしょう。

=={practice_asset_particle} Particle System
ゲームエフェクトはゲーム演出に欠かせません。UnityではParticle Systemをよく使います。
本章ではパフォーマンスチューニング観点で、Particle Systemの失敗しない使い方、注意点について紹介します。

大事なことは次の2点です。

 * パーティクルの個数を抑える
 * ノイズは重いので注意する

==={practice_asset_emit_count} パーティクルの個数を抑える
パーティクルの個数は負荷につながります。Particle SystemはCPUで動作するパーティクル（CPUパーティクル）のため、パーティクルの個数が多ければ多いほどCPU負荷は上がります。
基本方針として必要最低限のパーティクル数に設定しましょう。必要に応じて個数を調整してみてください。

パーティクルの個数を制限する方法は2つです。

 * Emissionモジュールの放出個数の制限
 * メインモジュールのMax Particlesで最大放出個数の制限

//image[practice_asset_emit_count][Emissionモジュールで放出個数の制限][scale=1.0]

 * Rate over Time: 毎秒放出する個数
 * Bursts > Count: バーストタイミングで放出する個数

これらの設定を調整して、必要最低限のパーティクル数になるよう設定してください。

//image[practice_asset_max_particles][Max Particlesで放出個数の制限][scale=1.0]
もう1つの方法はメインモジュールのMax Particlesです。上の例では1000個以上のパーティクルは放出されなくなります。

===={practice_asset_subemitters} Sub Emittersにも注意
パーティクルの個数を抑える上で、Sub Emittersモジュールも注意が必要です。
//image[practice_asset_subemitters][Sub Emittersモジュール][scale=1.0]
Sub Emittersモジュールは特定のタイミング（生成時、寿命が尽きた時など）で任意のParticle Systemを生成します。Sub Emittersの設定によっては、一気にピーク数まで到達してしまうため使用の際には注意しましょう。

==={practice_asset_noise} ノイズは重いので注意
//image[practice_asset_noise][Noiseモジュール][scale=1.0]

Noise（ノイズ）モジュールのQualityは負荷が上がりやすいです。
ノイズは有機的なパーティクルを表現可能で、お手軽にエフェクトのクオリティをあげられるためよく使われます。
よく使う機能だからこそパフォーマンスには気をつかいたい所です。

//image[practice_asset_noise_quality][NoiseモジュールのQuality][scale=1.0]

 * Low（1D）
 * Midium（2D）
 * High（3D）

Qualityは次元が上がるほど負荷は上がります。もしNoiseが不要であればNoiseモジュールをオフにしましょう。また、ノイズを使う必要があれば、Qualityの設定はLowを優先し、要求に応じてQualityをあげていきましょう。

=={practice_asset_audio} Audio
サウンドファイルをインポートしたデフォルト状態はパフォーマンス的には改善ポイントがあります。設定項目は次の3点です。

 * Load Type
 * Compression Format
 * Force To Mono

これらをゲーム開発でよく使うBGM、効果音、ボイスで適切な設定にしましょう。

==={practice_asset_audio_load_type} Load Type
サウンドファイル（AudioClip）をロードする方法は3種類あります。
//image[practice_asset_audio_load_type][AudioClip LoadType][scale=1.0]

 * Decompress On Load
 * Compressed In Memory
 * Streaming

===={practice_asset_audio_decomplress_on_load} Decompress On Load
Decompress On Loadは、非圧縮でメモリにロードします。
CPU負荷が低いため待機時間が小さく再生されます。
その反面、メモリを多く使用します。

尺が短くすぐに再生してほしい効果音にオススメです。
BGMや尺の長いボイスファイルでの使用は、メモリを多く使用してしまうため注意が必要です。

===={practice_asset_audio_complress_in_memory} Compressed In Memory
Compressed In Memoryは、AudioClipを圧縮した状態でメモリにロードします。
再生するタイミングで展開するということです。
つまりCPU負荷が高く、再生遅延が起きやすいです。

ファイルサイズが大きく、メモリにそのまま展開したくないサウンドや、多少の再生遅延に問題ないサウンドが適しています。
ボイスで使うことが多いです。

===={practice_asset_audiostreaming} Streaming
Streamingは、その名の通りロードしながら再生する方式です。
メモリ使用量は少ない代わりにCPU負荷が高くなります。
尺の長いBGMでの使用がオススメです。


//table[loadtype][ロード方法と主な使用用途まとめ]{
種類	用途
--------------------
Decompress On Load	効果音
Compressed In Memory	ボイス
Streaming	BGM
//}

==={practice_asset_audio_compression_format} Compression Format
Compression formatとは、AudioClip自体の圧縮フォーマットです。

//image[practice_asset_audio_compression_format][AudioClip Compression Format][scale=1.0]

===={practice_asset_pcm} PCM
非圧縮で、メモリを大量に消費します。音質によほどクオリティを求めない限り設定することはありません。

===={practice_asset_adpcm} ADPCM
PCMに比べてメモリ使用量は70%減りますが、クオリティは低くなります。CPU負荷がVorbisと比較して格段に小さいのが特徴です。
つまり展開スピードが速いため、即時再生や大量に再生するサウンドに適しています。
具体的には、足音や衝突音、武器などのノイズを多く含む且つ、大量に再生する必要のあるサウンドです。

===={practice_asset_vorbis} Vorbis
非可逆圧縮フォーマットのため、PCMよりクオリティは下がりますが、ファイルサイズは小さくなります。唯一Qualityを設定できるため、微調整も可能です。
全サウンド（BGM、効果音、ボイス）でもっとも使われる圧縮形式です。

//table[compressionformat][圧縮方法と主な使用用途まとめ]{
種類	用途
--------------------
PCM	使用しない
ADPCM	効果音
Vorbis	BGM、効果音、ボイス
//}

==={practice_asset_sample_rate} サンプルレートの指定
サンプルレートを指定してクオリティの調節できます。全圧縮フォーマットに対応しています。
Sample Rate Settingから3種類の方法を選べます。

//image[practice_asset_audio_sample_rate_setting][Sample Rate Settings][scale=1.0]

===={practice_asset_preserve_sample_rate} Preserve Sample Rate
デフォルト設定です。元音源のサンプルレートが採用されます。

===={practice_asset_optimize_sample_rate} Optimize Sample Rate
Unity側で解析され、最高周波数の成分に基づいて自動で最適化されます。

===={practice_asset_override_sample_rate} Override Sample Rate
元音源のサンプルレートを上書きします。8,000〜192,000Hzまで指定可能です。元の音源より高くしても品質は上がりません。元音源よりサンプルレートを下げたい場合に使用します。


==={practice_asset_audio_force_to_mono} 効果音はForce To Monoを設定
Unityはデフォルト状態でステレオ再生しますが、Force To Monoを有効にすることでモノラル再生になります。モノラル再生を強制することで左右それぞれのデータを持たなくて良くなるため、ファイルサイズとメモリサイズは半分になります。
//image[practice_asset_audio_force_to_mono][AudioClip Force To Mono][scale=1.0]

効果音はモノラル再生でも問題ないことが多いです。また3Dサウンドもモノラル再生の方が良い場合もあります。検討した上でForce To Monoを有効にするとよいでしょう。
パフォーマンスチューニング効果も塵も積もれば山となります。モノラル再生で問題ない場合は積極的にForce To Monoを活用しましょう。

//info{
パフォーマンスチューニングとは話がずれますが音声ファイルは無圧縮のものをUnityに取り込みましょう。
圧縮済みのものをインポートした場合、Unity側でデコード & 再圧縮を行うので品質の低下が発生します。
//}

=={practice_asset_special_folder} Resources / StreamingAssets
Unityには特別なフォルダーが存在します。とくに次の3つはパフォーマンス観点で注意が必要です。

 * Resourcesフォルダー
 * StreamingAssetsフォルダー

通常Unityは、シーンやマテリアル、スクリプトなどから参照されたオブジェクトがビルドに含まれます。

//listnum[practice_asset_special_folder_script_reference][スクリプトで参照されたオブジェクトの例][csharp]{
[SerializeField] GameObject sample; // 参照されたオブジェクトはビルドに含まれる
//}

先の特別なフォルダーはルールが違います。格納したファイルはビルドに含まれます。つまり、リリース時に不要な不要なファイルも格納されていればビルドに含まれ、ビルドサイズの膨張につながります。

問題はプログラムから確認することができないということです。不要なファイルを目視で確認しなければならないので時間がかかります。これらのフォルダーには注意してファイル追加しましょう。

しかし、プロジェクトが進行する中で格納ファイルはどうしても増えていきます。中には使用しなくなった不要なファイルが混入することもあるでしょう。結論として、定期的な格納ファイルの見直しをオススメします。

==={practice_asset_resources} 起動時間を遅くするResourcesフォルダー
Resourcesフォルダーに大量のオブジェクトを格納すると、アプリの起動時間が伸びてしまいます。
Resourcesフォルダーは文字列参照でオブジェクトをロードできる昔ながらの便利機能です。

//listnum[practice_asset_special_folder_resources][スクリプトで参照されたオブジェクトの例][csharp]{
var object = Resources.Load("aa/bb/cc/obj");
//}
このようなコードでオブジェクトをロードできます。
Resourcesフォルダーに格納しておけば、スクリプトからオブジェクトにアクセスできるため、多用してしまいがちです。
しかし、Resourcesフォルダーに詰め込みすぎると前述の通りアプリの起動時間が伸びます。
原因はUnity起動時に、全Resourcesフォルダー内の構造を解析し、ルックアップテーブルを作成するからです。
できる限りResourcesフォルダーの使用は最小限にした方がよいでしょう。

=={practice_asset_scriptable_object} ScriptableObject

ScriptableObjectはYAMLのアセットで、テキスト形式としてファイル管理しているプロジェクトが多いと思われます。
明示的に@<code>{[PreferBinarySerialization]}Attributeを指定することで保存形式をバイナリ形式に変更できます。
おもにデータが大量になるようなアセットの場合、バイナリ形式にすることで書き込み・読み込みのパフォーマンスが向上します。

ただし、当然ながらバイナリ形式の場合マージツールなどでは扱いにくくなります。アセット更新が上書きだけで済む、
変更差分をテキストで確認する必要がないようなアセットや、ゲーム開発が完了しデータの変更が行われなくなったアセットについては
積極的に@<code>{[PreferBinarySerialization]}を指定するとよいでしょう。

//info{
ScriptableObjectを使用するにあたって陥りやすいミスは、クラス名とソースコードのファイル名の不一致です。
クラスとファイルは同名にする必要があります。
クラス作成時には命名に注意しつつ、@<code>{.asset}ファイルが正しくシリアライズされ、
バイナリ形式で保存されていることを確認しましょう。
//}

//listnum[scriptable_object_sample][ScriptableObjectの実装例][csharp]{
/*
* ソースコードのファイル名がScriptableObjectSample.csのとき
*/

// シリアライズ成功
[PreferBinarySerialization]
public sealed class ScriptableObjectSample : ScriptableObject
{
    ...
}

// シリアライズ失敗
[PreferBinarySerialization]
public sealed class MyScriptableObject : ScriptableObject
{
    ...
}
//}
