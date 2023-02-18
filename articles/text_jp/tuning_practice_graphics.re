={practice_graphics} Tuning Practice - Graphics

本章では、Unityのグラフィックス機能周辺のチューニング手法について紹介します。

=={practice_graphics_resolution} 解像度の調整

レンダリングパイプラインの中でもフラグメントシェーダーのコストはレンダリングする解像度に比例して増加します。
とくに昨今のモバイルデバイスはディスプレイの解像度が高く、描画解像度を適切な値に調整する必要があります。

==={practice_graphics_resolution_dpi} DPIの設定

モバイルプラットフォームのPlayer Settingsの解像度関連の項目に含まれているResolution Scaling Modeを@<em>{Fixed DPI}に設定すると、
特定の@<kw>{DPI（dots per inch）}をターゲットに解像度を落とすことができます。

//image[fixed_dpi][Resolution Scaling Mode]

最終的な描画解像度は、Target DPIの値とQuality Settingsに含まれるResolution Scaling DPI Scale Factorの値が乗算されて決定します。

//image[dpi_factor][Resolution Scaling DPI Scale Factor]

==={practice_graphics_resolution_code} スクリプトによる解像度設定

スクリプトから描画解像度を動的に変更するには、@<code>{Screen.SetResolution}を呼び出します。

現在の解像度は@<code>{Screen.width}や@<code>{Screen.height}で取得することができ、DPIは@<code>{Screen.dpi}で取得できます。

//listnum[screen_setresolution][Screen.SetResolution][csharp]{
public void SetupResolution()
{
    var factor = 0.8f;

    // Screen.width, Screen.heightで現在の解像度を取得
    var width = (int)(Screen.width * factor);
    var height = (int)(Screen.height * factor);

    // 解像度を設定
    Screen.SetResolution(width, height, true);
}
//}

//info{
    @<code>{Screen.SetResolution}での解像度設定は実機でのみ反映されます。

    Editorでは変更が反映されないため注意しましょう。
//}


=={practice_graphics_overdraw} 半透明とオーバードロー

半透明マテリアルの使用は@<kw>{オーバードロー}の増加の大きな原因となります。
オーバードローとは画面の1ピクセルに対して複数回フラグメントの描画が行われることで、フラグメントシェーダーの負荷に比例してパフォーマンスに影響を与えます。

とくにParticle Systemなどで大量に半透明のパーティクルを発生させる場合などにオーバードローが大量に発生することが多いです。

オーバードローによる描画負荷の増加を軽減するには、以下のような方法が考えられます。

 * 不要な描画領域を削減する
 ** テクスチャが完全に透明な領域も描画対象になるためなるべく減らす
 * オーバードローが発生する可能性のあるオブジェクトにはなるべく軽量なシェーダーを使用する
 * 半透明マテリアルはなるべく使用しない
 ** 不透明マテリアルで擬似的に半透明のような見た目を再現できる@<kw>{ディザリング}という手法も検討する

Built-in Render PipelineのEditorではSceneビューのモードを@<kw>{Overdraw}に変更することでオーバードローを可視化することができ、オーバードローの調整の基準として有用です。

//image[overdraw][Overdrawモード]


//info{
    Universal Render Pipelineでは、Unity 2021.2から実装されている@<kw>{Scene Debug View Modes}によってオーバードローを可視化できます。
//}

=={practice_graphics_draw_call} ドローコールの削減

ドローコールの増加はCPU負荷にしばしば影響を与えますが、
Unityにはドローコールの数を削減するための機能がいくつか存在します。

==={practice_graphics_dynamic_batching} 動的バッチング

@<kw>{動的バッチング}は、動的なオブジェクトをランタイムでバッチングするための機能です。
この機能を使用することで、同じマテリアルを使用している動的なオブジェクトのドローコールを統合して削減できます。

使用するには、Player Settingsから@<kw>{Dynamic Batching}の項目を有効にします。

また、Universal Render PipelineではUniversal Render Pipeline Asset内の@<kw>{Dynamic Batching}の項目を有効にする必要があります。
ただUniversal Render PipelineではDynamic Batchingの使用は非推奨となっています。

//image[dynamic_batching][Dynamic Batchingの設定]

動的バッチングはCPUコストを使用する処理であるため、オブジェクトに適用させるには多くの条件をクリアする必要があります。
以下に主な条件を記載します。

 * 同一のマテリアルを参照している
 * MeshRendererかParticle Systemで描画を行っている
 ** SkinnedMeshRendererなどの他のコンポーネントでは動的バッチングの対象とならない
 * メッシュの頂点数が300以下である
 * マルチパスを使用していない
 * リアルタイムシャドウの影響を受けていない

//info{

    動的バッチングはCPUの定常負荷に影響を与えるため、推奨されない場合があります。
    後述する@<kw>{SRP Batcher}を使用すると動的バッチングに近い効果を得ることができます。

//}

==={practice_graphics_static_batching} 静的バッチング

@<kw>{静的バッチング}は、シーン内で動かないオブジェクトをバッチングするための機能です。
この機能を使用することで、同一マテリアルを使用している静的なオブジェクトのドローコールを削減できます。

動的バッチングと同様にPlayer Settingsから@<kw>{Static Batching}を有効にすることで使用できます。

//image[static_batching][Static Batchingの設定]

オブジェクトを静的バッチングの対象とするには、オブジェクトの@<kw>{staticフラグ}を有効にする必要があります。
具体的にはstaticフラグの中の@<kw>{Batching Static}というサブフラグを有効にする必要があります。

//image[batching_static][Batching Static]

静的バッチングは動的バッチングとは違いランタイムでの頂点の変換処理を行わないため低負荷で実行できます。
しかし、バッチ処理によって結合されたメッシュ情報を保存するためメモリを多く消費することに注意が必要です。


==={practice_graphics_instancing} GPUインスタンシング

@<kw>{GPUインスタンシング}とは同一メッシュ・同一マテリアルのオブジェクトを効率的に描画するための機能です。
草や木のように同じメッシュを複数回描画する場合のドローコールの削減が期待できます。

GPUインスタンシングを使用するには、マテリアルのInspectorから@<kw>{Enable Instancing}を有効にします。

//image[enable_instancing][Enable Instancing]

GPUインスタンシングを使用できるシェーダーを作成するためにはいくつか専用の対応が必要となります。
以下にBuilt-in Render PipelineでGPUインスタンシングを使用するための最低限の実装を行ったシェーダーコードの例を記載します。

//listnum[instancing_shader][GPUインスタンシングに対応したシェーダー]{
Shader "SimpleInstancing"
{
    Properties
    {
        _Color ("Color", Color) = (1, 1, 1, 1)
    }

    CGINCLUDE

    #include "UnityCG.cginc"

    struct appdata
    {
        float4 vertex : POSITION;
        UNITY_VERTEX_INPUT_INSTANCE_ID
    };

    struct v2f
    {
        float4 vertex : SV_POSITION;
        // フラグメントシェーダーでINSTANCED_PROPにアクセスしたい場合にのみ必要
        UNITY_VERTEX_INPUT_INSTANCE_ID
     };

    UNITY_INSTANCING_BUFFER_START(Props)
        UNITY_DEFINE_INSTANCED_PROP(float4, _Color)
    UNITY_INSTANCING_BUFFER_END(Props)

    v2f vert(appdata v)
    {
        v2f o;

        UNITY_SETUP_INSTANCE_ID(v);

        // フラグメントシェーダーでINSTANCED_PROPにアクセスしたい場合にのみ必要
        UNITY_TRANSFER_INSTANCE_ID(v, o);

         o.vertex = UnityObjectToClipPos(v.vertex);
        return o;
    }

    fixed4 frag(v2f i) : SV_Target
    {
        // フラグメントシェーダーでINSTANCED_PROPにアクセスしたい場合にのみ必要
        UNITY_SETUP_INSTANCE_ID(i);

        float4 color = UNITY_ACCESS_INSTANCED_PROP(Props, _Color);
        return color;
    }

    ENDCG

    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma multi_compile_instancing
            ENDCG
        }
    }
}
//}

GPUインスタンシングは同一のマテリアルを参照しているオブジェクトにのみ作用しますが、各インスタンスごとのプロパティを設定できます。
上記のシェーダーコードのように対象のプロパティを@<code>{UNITY_INSTANCING_BUFFER_START(Props)}と@<code>{UNITY_INSTANCING_BUFFER_END(Props)}で囲むことで個別に変更するプロパティとして設定できます。

このプロパティをC#で@<kw>{MaterialPropertyBlock}のAPIを使用して変更することで個別のカラーなどのプロパティを設定できます。
ただ、あまりに大量のインスタンスに対してMaterialPropertyBlockを使用すると、MaterialPropertyBlockでのアクセスがCPUのパフォーマンスに影響を与えることがあるので気をつけましょう。

==={practice_graphics_srp_batcher} SRP Batcher

@<kw>{SRP Batcher}とは、@<kw>{Scriptable Render Pipeline}でのみ使用できる描画のCPUコストを削減するための機能です。
この機能を使用することで、同一のシェーダーバリアントを使用する複数のシェーダーのセットパスコールをまとめて処理することができるようになります。

SRP Batcherを使用するには、@<kw>{Scriptable Render Pipeline Asset}のInspectorから@<kw>{SRP Batcher}の項目を有効にします。

//image[enable_srp_batcher][SRP Batcherの有効化]

また、ランタイムでは以下のC#コードでSRP Batcherの有効・無効を変更できます。

//listnum[srp_batcher_enable][SRP Batcherの有効化][csharp]{
GraphicsSettings.useScriptableRenderPipelineBatching = true;
//}

シェーダーをSRP Batcherに対応させるには以下の2点の条件をクリアする必要があります。

 1. オブジェクトごとに定義されるビルトインのプロパティを@<kw>{UnityPerDraw}という1つのCBUFFERに定義する
 2. マテリアルごとのプロパティを@<kw>{UnityPerMaterial}という1つのCBUFFERに定義する

UnityPerDrawに関しては、Universal Render Pipelineなどのシェーダーでは基本的にデフォルトで対応されていますが、
UnityPerMaterialのCBUFFERの設定は自分で行う必要があります。

以下のように、マテリアルごとのプロパティを@<code>{CBUFFER_START(UnityPerMaterial)}と@<code>{CBUFFER_END}で囲みます。

//listnum[shader_unitypermaterial][UnityPerMaterial]{
Properties
{
    _Color1 ("Color 1", Color) = (1,1,1,1)
    _Color2 ("Color 2", Color) = (1,1,1,1)
}

CBUFFER_START(UnityPerMaterial)

float4 _Color1;
float4 _Color2;

CBUFFER_END
//}

以上の対応でSRP Batcherに対応したシェーダーを作成できますが、
Inspectorから該当のシェーダーがSRP Batcherに対応しているかどうか確認することもできます。

シェーダーのInspectorの@<kw>{SRP Batcher}の項目が@<kw>{compatible}となっていたらSRP Batcherに対応しており、
@<kw>{not compatible}となっていたら対応していないことがわかります。

//image[srp_batcher_compatible][SRP Batcherに対応しているシェーダー]

=={practice_graphis_atlas} SpriteAtlas

2DゲームやUIでは多くのスプライトを使用して画面を構築することがしばしばあります。
そういった場合に大量のドローコールを発生させないための機能が@<kw>{SpriteAtlas}です。

SpriteAtlasを使用すると、複数のスプライトを1つのテクスチャとしてまとめることでドローコールが削減できます。

SpriteAtlasを作成するには、まずPackage Managerから@<kw>{2D Sprite}をプロジェクトにインストールする必要があります。

//image[2d_sprite][2D Sprite]

インストール後、Projectビューで右クリックして「Create -> 2D -> Sprite Atlas」を選択し、SpriteAtlasのアセットを作成します。

//image[create_sprite_atlas][SpriteAtlasの作成]

アトラス化するスプライトを指定するには、SpriteAtlasのInspectorから@<kw>{Objects for Packing}の項目に@<em>{スプライトかスプライトを含むフォルダー}を設定します。

//image[atlas_packing][Objects for Packingの設定]

以上の設定を行うことでビルド時やUnity Editorの再生時にアトラス化の処理が行われ、対象のスプライトの描画の際はSpriteAtlasで統合されたテクスチャが参照されるようになります。

以下のようなコードでSpriteAtlasから直接スプライトを取得することも可能です。

//listnum[load_atlas_sprite][SpriteAtlasからSpriteをロードする][csharp]{
[SerializeField]
private SpriteAtlas atlas;

public Sprite LoadSprite(string spriteName)
{
    // Spriteの名前を引数にしてSpriteAtlasからSpriteを取得する
    var sprite = atlas.GetSprite(spriteName);
    return sprite;
}
//}

SpriteAtlasに含まれるSpriteを1つロードするとアトラス全体のテクスチャが読み込まれるため、1つだけロードするよりも多くのメモリを消費します。
そのため、SpriteAtlasを適切に分割するなど注意して使用する必要があります。

//info{
    この節はSpriteAtlas V1をターゲットにして記載しています。
    SpriteAtlas V2ではアトラス化する対象のスプライトのフォルダー指定ができなかったりなど動作に大きく変更が加わる可能性があります。
//}

=={practice_graphics_culling} カリング

Unityでは、最終的に画面に表示されない部分の処理を事前に省くための@<kw>{カリング}という処理が行われます。

==={practice_graphics_frustum_culling} 視錐台カリング

@<kw>{視錐台カリング}とは、カメラの描画範囲となる視錐台の外側にあるオブジェクトを描画対象から省くための処理です。
これによりカメラの範囲外のオブジェクトは描画の計算が行われないようになります。

視錐台カリングは何も設定せずともデフォルトで行われています。
頂点シェーダーの負荷が高いオブジェクトなどの場合は、メッシュを適切に分割することでカリングの対象とし描画コストを下げる手法も有効です。

==={practice_graphics_shader_culling} 背面カリング

@<kw>{背面カリング}とは、カメラから見えない（はずの）ポリゴンの裏側を描画から省く処理です。
ほとんどのメッシュは閉じている（表側のポリゴンのみがカメラに映る）ため裏側は描画する必要がありません。

Unityではシェーダーに明記しない場合ポリゴンの背面がカリングの対象となっていますが、明記することでカリングの設定を切り替えることが可能です。
SubShader内に以下のように記述します。

//listnum[shader_cull][カリングの設定]{
SubShader
{
    Tags { "RenderType"="Opaque" }
    LOD 100

    Cull Back // Front, Off

    Pass
    {
        CGPROGRAM
        #pragma vertex vert
        #pragma fragment frag
        ENDCG
    }
}
//}

@<code>{Back}、@<code>{Front}、@<code>{Off}の3つの設定がありますが、それぞれの効果は以下のようになっています。

 * @<kw>{Back} - 視点と反対側のポリゴンを描画しない
 * @<kw>{Front} - 視点と同じ方向のポリゴンを描画しない
 * @<kw>{Off} - 背面カリングを無効化し、すべての面を描画する

==={practice_graphics_occlusion_culling} オクルージョンカリング

@<kw>{オクルージョンカリング}とは、オブジェクトに遮蔽されてカメラに映らないオブジェクトを描画対象から省く処理です。
この機能は事前にベイクした遮蔽判定用のデータをもとにランタイムでオブジェクトが遮蔽されているか判定し、遮蔽されているオブジェクトを描画対象から外します。

オブジェクトをオクルージョンカリングの対象とするには、Inspectorのstaticフラグから@<kw>{Occluder Static}または@<kw>{Occludee Static}を有効にします。
Occluder Staticを無効にしOccludee Staticを有効にした場合はオブジェクトを遮蔽する側としては判定されなくなり、@<em>{遮蔽される側のみ}として処理が行われるようになります。
逆の場合は遮蔽される側として判定されなくなり、 @<em>{遮蔽する側のみ}として処理されます。

//image[occlusion_static][オクルージョンカリングのstaticフラグ]

オクルージョンカリング用の事前ベイクを行うため、@<kw>{Occlusion Cullingウィンドウ}を表示します。
このウィンドウでは各オブジェクトのstaticフラグの変更やベイクの設定変更などを行うことができ、@<kw>{Bakeボタン}を押すことでベイクを行うことができます。

//image[occlusion_window][Occlusion Cullingウィンドウ]

オクルージョンカリングは描画コストを削減する代わりにカリング処理のためにCPUへ負荷をかけるため、各負荷のバランスを見て適切に設定を行う必要があります。

//info{
    オクルージョンカリングで削減されるのはオブジェクトの描画処理のみで、リアルタイムシャドウの描画などの処理はそのまま行われます。
//}


=={practice_graphics_shader} シェーダー
シェーダーはグラフィックス表現に非常に有効ですが、しばしばパフォーマンスの問題を引き起こします。

==={practice_graphics_shader_float} 浮動小数点数型の精度を下げる
GPU（とくにモバイルプラットフォーム）の計算速度は大きいデータ型より小さいデータ型のほうが速くなります。
そのため、置き換え可能な場合は浮動小数点数型を@<kw>{float型（32bit）}から@<kw>{half型（16bit）}に置き換えることが有効です。

深度計算など精度が必要な場合はfloat型を使うべきですが、Colorの計算などでは精度を落としてしまっても結果的な見た目に大きな差異は起こりづらいです。

==={practice_graphics_shader_vertex} 頂点シェーダーで計算を行う
頂点シェーダーの処理はメッシュの頂点数分だけ実行され、フラグメントシェーダーの処理は最終的に書き込まれるピクセル数分実行されます。
一般的に頂点シェーダーの実行回数はフラグメントシェーダーよりも少ないことが多く、そのため複雑な計算は可能な限り頂点シェーダーで行うのが良いでしょう。

頂点シェーダーの計算結果はシェーダーセマンティクスを介してフラグメントシェーダーに渡されますが、ここで渡される値は補間されたものであり、フラグメントシェーダーで計算した場合と見た目が違う可能性に注意する必要があります。

//listnum[shader_vertex_factor][頂点シェーダーによる事前計算]{
CGPROGRAM
#pragma vertex vert
#pragma fragment frag

#include "UnityCG.cginc"

struct appdata
{
    float4 vertex : POSITION;
    float2 uv : TEXCOORD0;
};

struct v2f
{
    float2 uv : TEXCOORD0;
    float3 factor : TEXCOORD1;
    float4 vertex : SV_POSITION;
};

sampler2D _MainTex;
float4 _MainTex_ST;

v2f vert (appdata v)
{
    v2f o;
    o.vertex = UnityObjectToClipPos(v.vertex);
    o.uv = TRANSFORM_TEX(v.uv, _MainTex);

    // 複雑な事前計算を行う
    o.factor = CalculateFactor();

    return o;
}

fixed4 frag (v2f i) : SV_Target
{
    fixed4 col = tex2D(_MainTex, i.uv);

    // 頂点シェーダーで計算した値をフラグメントシェーダーで使用する
    col *= i.factor;

    return col;
}
ENDCG
//}

==={practice_graphis_shader_tex} テクスチャに情報を事前に仕込む 
シェーダー内の複雑な計算の結果がもし外部の値によって変化しないのであれば、テクスチャの要素として事前計算した結果を格納しておくことも有効な手段です。

方法としては、Unityで専用のテクスチャを生成するツールを実装したり各種DCCツールの拡張機能として実装するなどが考えられます。
すでに使用しているテクスチャのアルファチャンネルがもし使用されていなければそこに書き込んだり、専用のテクスチャを用意するのが良いでしょう。

たとえば、カラーグレーディングに使用される@<kw>{LUT（色対応表）}は、各ピクセルの座標が各カラーに対応するテクスチャに対して事前に色調補正をかけます。
そのテクスチャをシェーダー内で元のカラーをもとにサンプリングすることで、事前にかけた色調補正をもとのカラーに対してかけたのとほぼ同一の結果を得ることができます。

//image[lut-texture-1024][色調補正前のLUTテクスチャ（1024x32）]

==={practice_graphics_variant_collection} ShaderVariantCollection
@<kw>{ShaderVariantCollection}を使用すると、シェーダーを使用する前にコンパイルしスパイクを防ぐことができます。

ShaderVariantColletionではゲーム内で使用するシェーダーバリアントのリストをアセットとして保持できます。
Projectビューから「Create -> Shader -> Shader Variant Collection」を選択して作成します。

//image[create-shadervariant][ShaderVariantCollectionの作成]

作成したShaderVariantCollectionのInspectorビューからAdd Shaderを押下して対象のシェーダーを追加し、さらにシェーダーに対してどのバリアントを追加するかを選択します。

//image[shadervariant-inspector][ShaderVariantCollectionのInspector]

ShaderVariantCollectionをGraphics Settingsの@<kw>{Shader preloading}の項目内の@<kw>{Preloaded Shaders}に追加することで、
アプリケーションの起動時にコンパイルするシェーダーバリアントを設定できます。

//image[preloaded-shaders][Preloaded Shaders]

また、スクリプトから@<kw>{ShaderVariantCollection.WarmUp()}を呼び出すことで該当のShaderVariantCollectionに含まれるシェーダーバリアントを明示的に事前コンパイルすることも可能です。

//listnum[warmup_shader_variant][ShaderVariantCollection.WarmUp]{
public void PreloadShaderVariants(ShaderVariantCollection collection)
{
    // 明示的にシェーダーバリアントを事前コンパイルする
    if (!collection.isWarmedUp)
    {
        collection.WarmUp();
    }
}
//}

=={practice_graphics_lighting} ライティング
ライティングはゲームのアート表現において非常に重要な要素の1つですが、パフォーマンスに大きく影響を与えることが多いです。

==={practice_graphics_shadow} リアルタイムシャドウ
リアルタイムシャドウの生成はドローコールやフィルレートを大きく消費します。
そのため、リアルタイムシャドウを使用する際は慎重に設定を検討する必要があります。

===={practice_graphics_shadow_call} ドローコールの削減
シャドウ生成のドローコールを削減するためには、以下のような方針が考えられます。

 * シャドウを落とすオブジェクトの数を減らす
 * バッチングによりドローコールをまとめる

シャドウを落とすオブジェクトを減らす方法はいくつかありますが、シンプルな方法はMeshRendererの@<kw>{Cast Shadows}の設定をオフにすることです。
これによりオブジェクトをシャドウの描画対象から外すことができます。
この設定は通常Unityではオンになっているため、シャドウを使用しているプロジェクトでは注意するべきでしょう。

//image[mesh-castshadow][Cast Shadows]

また、オブジェクトがシャドウマップに描画される最大距離を短くすることも有効です。
Quality Settingsの@<kw>{Shadow Distance}の項目でシャドウマップの最大描画距離を変更することができ、この設定によりシャドウを落とすオブジェクトの数を必要最低限に減らすことができます。
この設定を調整することでシャドウマップの解像度に対して最低限の範囲でシャドウを描画することができるため、シャドウの解像感の低下を抑えることにも繋がります。

//image[shadow-distance][Shadow Distance]

通常の描画と同様、シャドウ描画でもバッチング処理の対象にすることでドローコールを削減できます。
バッチングの手法については@<hd>{practice_graphics_draw_call}を参照してください。


===={practice_graphics_shadow_fill} フィルレートの節約
シャドウによるフィルレートはシャドウマップの描画とシャドウの影響を受けるオブジェクトの描画の両方に左右されます。

Quality SettingsのShadowsの項目にあるいくつかの設定を調整することでそれぞれのフィルレートを節約できます。

//image[quality-shadow][Quality Settings -> Shadows]

@<kw>{Shadows}の項目ではシャドウの形式を変更することができ、@<kw>{Hard Shadows}は影の境界線がはっきりと出ますが比較的負荷が低く、@<kw>{Soft Shadows}は影の境界線をぼかしたような表現ができますが負荷が高いです。

@<kw>{Shadow Resolution}と@<kw>{Shadow Cascades}の項目はシャドウマップの解像度に影響する項目で、大きく設定するとシャドウマップの解像度が大きくなりフィルレートの消費が大きくなります。
ただ、この設定はシャドウの品質に大きく関係するところでもあるため、パフォーマンスと品質のバランスを見ながら慎重に調整する必要があります。

一部の設定は@<kw>{Light}コンポーネントのInspectorから変更できるため、個別のライトごとに設定を変えることも可能です。

//image[light-component][Lightコンポーネントのシャドウ設定]

===={practice_graphics_fake_shadow} 疑似シャドウ
ゲームジャンルやアートスタイルによっては、板ポリゴンなどでオブジェクトの影を擬似的に表現する手法も有効です。
この手法は使用上の制約が強く自由度が高いものではありませんが、通常のリアルタイムシャドウの描画手法に比べて圧倒的に軽量に描画できます。

//image[fake-shadow][板ポリゴンによる擬似シャドウ]

==={practice_graphics_lightmap} ライトマッピング
事前にライティングの効果とシャドウをテクスチャにベイクしておくことで、リアルタイム生成よりもかなり低負荷に品質の高いライティング表現を実現できます。

ライトマップをベイクするには、まずシーンに配置したLightコンポーネントの@<kw>{Mode}の項目を@<kw>{Mixed}か@<kw>{Baked}に変更します。

//image[light-mixed][LightのMode設定]

また、ベイク対象となるオブジェクトのstaticフラグを有効化します。

//image[object-static][staticの有効化]

この状態で、メニューから「Window -> Rendering -> Lighting」を選択しLightingビューを表示します。

デフォルトの状態だと@<kw>{Lighting Settings}アセットが指定されていないため、@<kw>{New Lighting Settings}ボタンを押下して新規作成を行います。

//image[new-lighting-settings][New Lighting Settings]

ライトマップについての設定は主に@<kw>{Lightmapping Settings}タブで行います。

//image[lightmapping-settings][Lightmapping Settings]

多くの設定項目がありますが、これらの値を調整することでライトマップのベイクの速度や品質が変わります。
そのため、求める速度や品質に合わせて適切に設定する必要があります。

これらの設定の中でもっともパフォーマンスへの影響が大きいのは@<kw>{Lightmap Resolution}です。
この設定はUnityにおける1unitあたりにライトマップのテクセルをどれだけ割り当てるかという設定で、この値により最終的なライトマップのサイズが変動するため、ストレージやメモリの容量、テクスチャへのアクセス速度などに大きな影響を与えます。

//image[lightmap-resolution][Lightmap Resolution]

最後に、Inspectorビューの下部にある@<kw>{Generate Lighting}ボタンを押下することでライトマップのベイクを行うことができます。
ベイクが完了すると、シーンと同名のフォルダーにベイクされたライトマップが格納されていることを確認できます。

//image[generate-lighting][Generate Lighting]

//image[baked-lightmaps][ベイクされたライトマップ]

=={practice_graphics_lod} Level of Detail
カメラから遠い距離にあるオブジェクトをハイポリゴン・高精細に描画するのは非効率です。
@<kw>{Level of Detail（LOD）}という手法を利用することでカメラからの距離に応じてオブジェクトの詳細度を削減できます。

Unityでは、オブジェクトに@<kw>{LOD Group}コンポーネントを追加することでLODの制御を行うことができます。

//image[lod-group][LOD Group]

LOD GroupがアタッチされたGameObjectの子に各LODレベルのメッシュを持ったRendererを配置し、LOD Groupの各LODレベルに設定することでカメラに応じてLODレベルが切り替えられるようになります。
カメラの距離に対してどのLODレベルを割り当てるかをLOD Groupごとに設定することも可能です。

LODを使用することで一般に描画の負荷を削減できますが、各LODレベルのメッシュがすべてロードされるためメモリやストレージの圧迫には注意が必要です。

=={practice_graphics_texture_sreaming} テクスチャストリーミング
Unityの@<kw>{テクスチャストリーミング}を利用することで、テクスチャのために必要なメモリ容量やロード時間を削減できます。
テクスチャストリーミングとは、シーンのカメラ位置に応じてミップマップをロードすることでGPUメモリを節約するための機能です。

この機能を有効にするには、Quality Settingsの@<kw>{Texture Streaming}を有効化します。

//image[texture-streaming][Texture Streaming]

さらに、テクスチャのミップマップをストリーミングできるようにするためテクスチャのインポート設定を変更する必要があります。
テクスチャのInspectorを開き、@<kw>{Advanced}設定内の@<kw>{Streaming Mipmaps}を有効化します。

//image[streaming-mip][Streaming Mipmaps]

これらの設定により指定されたテクスチャのミップマップがストリーミングされるようになります。
またQuality Settingsの@<kw>{Memory Budget}の項目を調整することでロードするテクスチャの合計メモリ使用量を制限できます。
テクスチャストリーミングシステムはここで設定したメモリ量を超えないようにミップマップをロードします。
