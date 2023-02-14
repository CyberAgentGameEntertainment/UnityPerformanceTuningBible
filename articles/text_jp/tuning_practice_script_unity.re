={practice_script_unity} Tuning Practice - Script (Unity)

Unityで提供されている機能を何気なく使っていると思わぬ落とし穴にはまることがあります。
本章では、Unityの内部実装に関連したパフォーマンスチューニング手法について実例を交えながら紹介します。

=={practice_script_unity_event} 空のUnityイベント関数
@<code>{Awake}、@<code>{Start}、@<code>{Update}などUnityが提供しているイベント関数が定義されている場合、
実行時にUnity内部のリストにキャッシュされて、リストのイテレーションによって実行されます。

関数内で何も処理を行っていなくとも、定義されているだけでキャッシュ対象となるため、
不要なイベント関数を残したままにするとリストが肥大化し、イテレーションのコストが増大します。

たとえば下記サンプルコードのようにUnity上で新規生成したスクリプトには@<code>{Start}、@<code>{Update}が最初から定義されていますが、
これらの関数が不要であれば必ず削除しておきましょう。

//listnum[new_script][Unity上で新規生成したスクリプト][csharp]{
public class NewBehaviourScript : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}
//}

=={practice_script_unity_tag_name} tagやnameのアクセス
@<code>{UnityEngine.Object}を継承したクラスには@<code>{tag}プロパティと@<code>{name}プロパティが提供されています。
オブジェクトの識別に便利なこれらプロパティですが、実はGC.Allocが発生しています。

それぞれの実装をUnityCsReferenceから引用しました。
どちらもネイティブコードで実装された処理を呼び出していることが分かります。

UnityではスクリプトをC#で実装しますが、Unity自体はC++で実装されています。
C#メモリ空間とC++メモリ空間は共有できないため、C++側からC#側に文字列情報を受け渡すためにメモリの確保が行われます。
これは呼び出すたびに行われるので、複数回アクセスする場合はキャッシュしておきましょう。

Unityの仕組みとC#とC++間のメモリに関する詳細は@<hd>{basic|basic_unity_output_binary_runtime}を参照してください。

//listnum[get_tag][UnityCsReference GameObject.bindings.cs@<fn>{UnityCsReference_GameObject}から引用][csharp]{
public extern string tag
{
    [FreeFunction("GameObjectBindings::GetTag", HasExplicitThis = true)]
    get;
    [FreeFunction("GameObjectBindings::SetTag", HasExplicitThis = true)]
    set;
}
//}

//listnum[get_name][UnityCsReference UnityEngineObject.bindings.cs@<fn>{UnityCsReference_UnityEngineObject}から引用][csharp]{
public string name
{
    get { return GetName(this); }
    set { SetName(this, value); }
}

[FreeFunction("UnityEngineObjectBindings::GetName")]
extern static string GetName([NotNull("NullExceptionObject")] Object obj);
//}

//footnote[UnityCsReference_GameObject][@<href>{https://github.com/Unity-Technologies/UnityCsReference/blob/c84064be69f20dcf21ebe4a7bbc176d48e2f289c/Runtime/Export/Scripting/GameObject.bindings.cs}]
//footnote[UnityCsReference_UnityEngineObject][@<href>{https://github.com/Unity-Technologies/UnityCsReference/blob/c84064be69f20dcf21ebe4a7bbc176d48e2f289c/Runtime/Export/Scripting/UnityEngineObject.bindings.cs}]

=={practice_script_unity_component_cache} コンポーネントの取得
同じ@<code>{GameObject}にアタッチされている他のコンポーネントを取得する@<code>{GetComponent()}も注意が必要な1つです。

前節の@<code>{tag}プロパティや@<code>{name}プロパティ同様にネイティブコードで実装された処理を呼び出していることもそうですが、
指定した型のコンポーネントを「検索する」コストがかかることにも気をつけなければなりません。

下記サンプルコードでは毎フレーム@<code>{Rigidbody}コンポーネントを検索するコストがかかることになります。
頻繁にアクセスする場合は、あらかじめキャッシュしたものを使い回すようにしましょう。

//listnum[get_component][毎フレームGetComponent()するコード][csharp]{
void Update()
{
    Rigidbody rb = GetComponent<Rigidbody>();
    rb.AddForce(Vector3.up * 10f);
}
//}

=={practice_script_unity_transform} transformへのアクセス
@<code>{Transform}コンポーネントは位置や回転、スケール（拡大・縮小）、親子関係の変更など頻繁にアクセスするコンポーネントです。
下記サンプルコードのように複数の値を更新することも多いでしょう。

//listnum[sample_transform_NG][transformにアクセスする例][csharp]{
void SetTransform(Vector3 position, Quaternion rotation, Vector3 scale)
{
    transform.position = position;
    transform.rotation = rotation;
    transform.localScale = scale;
}
//}

@<code>{transform}を取得するとUnity内部では@<code>{GetTransform()}という処理が呼び出されます。
前節の@<code>{GetComponent()}に比べて最適化されていて高速です。
しかしキャッシュした場合よりは遅いので、これも下記サンプルコードのようにキャッシュしてアクセスしましょう。
位置と回転の2つは@<code>{SetPositionAndRotation()}を使うことで関数呼び出し回数を減らすこともできます。

//listnum[sample_transform_OK][transformをキャッシュする例][csharp]{
void SetTransform(Vector3 position, Quaternion rotation, Vector3 scale)
{
    var transformCache = transform;
    transformCache.SetPositionAndRotation(position, rotation);
    transformCache.localScale = scale;
}
//}

=={practice_script_unity_destroy} 明示的な破棄が必要なクラス
UnityはC#で開発を行うため、GCによって参照されなくなったオブジェクトは解放されます。
しかしUnityのいくつかのクラスは明示的に破棄する必要があります。
代表的な例としては@<code>{Texture2D}、 @<code>{Sprite}、@<code>{Material}、@<code>{PlayableGraph}などです。
@<code>{new}や専用の@<code>{Create}関数で生成した場合、必ず明示的に破棄を行いましょう。

//listnum[sample_create][生成と明示的な破棄][csharp]{
void Start()
{
    _texture = new Texture2D(8, 8);
    _sprite = Sprite.Create(_texture, new Rect(0, 0, 8, 8), Vector2.zero);
    _material = new Material(shader);
    _graph = PlayableGraph.Create();
}

void OnDestroy()
{
    Destroy(_texture);
    Destroy(_sprite);
    Destroy(_material);

    if (_graph.IsValid())
    {
        _graph.Destroy();
    }
}
//}

=={practice_script_unity_keyword_access} 文字列指定
@<code>{Animator}の再生するステートの指定、@<code>{Material}の操作するプロパティの指定に文字列を使うのは避けましょう。

//embed[latex]{
\clearpage
//}

//listnum[sample_keyword_string][文字列指定の例][csharp]{
_animator.Play("Wait");
_material.SetFloat("_Prop", 100f);
//}

これらの関数の内部では@<code>{Animator.StringToHash()}や@<code>{Shader.PropertyToID()}を実行して、文字列から一意な識別値に変換をしています。
何回もアクセスする場合に都度変換が行われるのはムダなので、識別値をキャッシュしておいて使い回すようにしましょう。
下記サンプルのようにキャッシュした識別値の一覧となるクラスを定義しておくと、取り回しがよいでしょう。

//listnum[sample_keyword_cache][識別値のキャッシュの例][csharp]{
public static class ShaderProperty
{
    public static readonly int Color = Shader.PropertyToID("_Color");
    public static readonly int Alpha = Shader.PropertyToID("_Alpha");
    public static readonly int ZWrite = Shader.PropertyToID("_ZWrite");
}
public static class AnimationState
{
    public static readonly int Idle = Animator.StringToHash("idle");
    public static readonly int Walk = Animator.StringToHash("walk");
    public static readonly int Run = Animator.StringToHash("run");
}
//}

=={practice_script_unity_json_utility} JsonUtilityの落とし穴
UnityではJSONのシリアライズ/デシリアライズのために@<code>{JsonUtility}というクラスが提供されています。
公式ドキュメント@<fn>{Unity_JSONSerialization}にもC#標準のものよりも高速であることが記載されていて、パフォーマンスを意識した実装をするなら利用することも多いでしょう。
//footnote[Unity_JSONSerialization][@<href>{https://docs.unity3d.com/ja/current/Manual/JSONSerialization.html}]

//quote{
JsonUtilityは(機能は .NET JSON より少ないですが)、よく使用されている .NET JSON よりも著しく早いことが、ベンチマークテストで示されています。
//}

しかしパフォーマンスに関わることでひとつ気をつけるべきことがあります。
それは「@<code>{null}の扱い」です。

下記サンプルコードでシリアライズ処理とその結果を示しています。
クラスAのメンバーb1を明示的に@<code>{null}にしているにもかかわらず、クラスBおよびクラスCをデフォルトコンストラクターで生成した状態でシリアライズされているのが分かります。
このようにシリアライズ対象となるフィールドに@<code>{null}があった場合、JSON化の際にダミーオブジェクトが@<code>{new}されるので、そのオーバーヘッドは考慮しておいたほうがよいでしょう。

//listnum[sample_json][シリアライズの挙動][csharp]{
[Serializable] public class A { public B b1; }
[Serializable] public class B { public C c1; public C c2; }
[Serializable] public class C { public int n; }

void Start()
{
    Debug.Log(JsonUtility.ToJson(new A() { b1 = null, }));
    // {"b1":{"c1":{"n":0},"c2":{"n":0}}}
}
//}

=={practice_script_unity_material_leak} RenderやMeshFilterの落とし穴
@<code>{Renderer.material}で取得したマテリアル、@<code>{MeshFilter.mesh}で取得したメッシュは複製されたインスタンスなので
使い終わったら明示的な破棄が必要です。
公式ドキュメント@<fn>{Unity_Renderer_material}@<fn>{Unity_MeshFilter_mesh}にもそれぞれ下記のように明記されています。
//footnote[Unity_Renderer_material][@<href>{https://docs.unity3d.com/ja/current/ScriptReference/Renderer-material.html}]
//footnote[Unity_MeshFilter_mesh][@<href>{https://docs.unity3d.com/ja/current/ScriptReference/MeshFilter-mesh.html}]

//quote{
If the material is used by any other renderers, this will clone the shared material and start using it from now on.
//}

//quote{
It is your responsibility to destroy the automatically instantiated mesh when the game object is being destroyed.
//}

取得したマテリアルやメッシュはメンバー変数に保持しておき、然るべきタイミングで破棄するようにしましょう。

//listnum[sample_destroy_copy_material][複製されたマテリアルの明示的な破棄][csharp]{
void Start()
{
    _material = GetComponent<Renderer>().material;
}

void OnDestroy()
{
    if (_material != null) {
        Destroy(_material);
    }
}
//}


=={practice_script_unity_log_delete} ログ出力コードの除去
Unityでは@<code>{Debug.Log()}、@<code>{Debug.LogWarning()}、@<code>{Debug.LogError()}といったログ出力用の関数が提供されています。
便利な機能ではありますがいくつかの問題点もあります。

 * ログ出力自体がそこそこ重たい処理
 * リリースビルドでも実行される
 * 文字列の生成や連結によってGC.Allocが発生する

UnityのLogging設定をオフにした場合、スタックトレースは停止しますが、ログは出力されます。
@<code>{UnityEngine.Debug.unityLogger.logEnabled}にUnityでは@<code>{false}を設定すると、ログは出力されませんが、
関数内部で分岐しているだけなので、関数の呼び出しコストや不要なはずの文字列の生成や連結は行われてしまいます。
@<code>{#if}ディレクティブを使うという手段もありますが、すべてのログ出力処理に手を入れるのは現実的ではありません。

//listnum[sample_directive][#ifディレクティブ][csharp]{
#if UNITY_EDITOR
  Debug.LogError($"Error {e}");
#endif
//}

このような場合に活用できるのが@<code>{Conditional}属性です。
@<code>{Conditional}属性が付いた関数は、指定したシンボルが定義されていない場合、コンパイラによって呼び出し部分が除去されます。
@<list>{sample_conditional}のサンプルのように、自作のログ出力クラスを通してUnity側のログ機能を呼び出すのをルールとして、自作クラス側の各関数に@<code>{Conditional}属性を付加することで、
必要に応じて関数の呼び出しごと除去できるようにするとよいでしょう。

//embed[latex]{
\clearpage
//}

//listnum[sample_conditional][Conditional属性の例][csharp]{
public static class Debug
{
    private const string MConditionalDefine = "DEBUG_LOG_ON";

    [System.Diagnostics.Conditional(MConditionalDefine)]
    public static void Log(object message)
        => UnityEngine.Debug.Log(message);
}
//}

注意点として、指定したシンボルが関数の呼び出し側から参照できる必要があるということです。
@<code>{#define}で定義されたシンボルのスコープは、記述したファイル内に限定されてしまいます。
@<code>{Conditional}属性が付いた関数を呼び出しているすべてのファイルにシンボルを定義していくのは現実的ではありません。
UnityにはScripting Define Symbolsというプロジェクト全体に対してシンボルを定義する機能があるので活用しましょう。
「Project Settings -> Player -> Other Settings」で設定ができます。

//image[practice_script_unity_define][Scripting Define Symbols]

=={practice_script_unity_burst} Burstを用いたコードの高速化

Burst@<fn>{burst}はUnity公式が開発する、ハイパフォーマンスなC#スクリプティング行うためのコンパイラです。
//footnote[burst][@<href>{https://docs.unity3d.com/Packages/com.unity.burst@1.6/manual/docs/QuickStart.html}]

BurstではC#のサブセット言語を用いてコードを記述します。
BurstがC#コードをLLVMというコンパイラ基盤@<fn>{llvm}の中間構文であるIR（Intermediate Representation）に変換し、IRを最適化をした上で機械語に変換されます。

//footnote[llvm][@<href>{https://llvm.org/}]

このときにコードを可能な限りベクトル化し、SIMDという命令を積極的に使った処理に置き換えます。これによって、より高速なプログラムの出力が期待できます。

SIMDはSingle Instruction/Multiple Dataの略で、単一の命令を同時に複数のデータに適用するような命令を指します。
つまりSIMD命令を積極的に利用することで、1命令でデータがまとめて処理されるため、通常の命令と比べて高速に動作します。

==={practice_script_csharp_optimize_code_with_burst} Burstを用いたコードの高速化

BurstではHigh Performance C#(HPC#)@<fn>{burst_hpc}と呼ばれるC#のサブセット言語を用いてコードを記述します。
//footnote[burst_hpc][@<href>{https://docs.unity3d.com/Packages/com.unity.burst@1.7/manual/docs/CSharpLanguageSupport_Types.html}]

HPC#の特徴の1つとしてC#の参照型、つまりクラスや配列などが利用できません。そのため原則として構造体を用いてデータ構造を記述します。

配列のようなコレクションは代わりに@<code>{NativeArray<T>}などのNativeContainer@<fn>{burst_native_container}を利用します。HPC#の詳細については脚注記載のドキュメントを参考にしてください。
//footnote[burst_native_container][@<href>{https://docs.unity3d.com/Manual/JobSystemNativeContainer.html}]

BurstはC# Job Systemと組み合わせて利用します。そのため自身の処理を@<code>{IJob}を実装したジョブの@<code>{Execute}メソッド内に記述します。
定義したジョブに@<code>{BurstCompile}属性を付与することで、そのジョブがBurstによって最適化されます。

@<list>{burst_job}に、与えられた配列の各要素を二乗して@<code>{Output}配列に格納する例を示します。

//listnum[burst_job][簡単な検証用のJob実装][csharp]{
[BurstCompile]
private struct MyJob : IJob
{
    [ReadOnly]
    public NativeArray<float> Input;

    [WriteOnly]
    public NativeArray<float> Output;

    public void Execute()
    {
        for (int i = 0; i < Input.Length; i++)
        {
            Output[i] = Input[i] * Input[i];
        }
    }
}
//}

@<list>{burst_job}の14行目の各要素はそれぞれ独立して計算でき（計算に順序依存がない）、かつ出力配列のメモリアライメントは連続しているためSIMD命令を用いてまとめて計算が可能です。

コードがどのようなアセンブリに変換されるかは、@<img>{burst_inspector}のようにBurst Inspectorを用いて確認できます。

//image[burst_inspector][Burst Inspectorを用いることで、コードがどのようなアセンブリに変換されるか確認できる][scale=1.0]

@<list>{burst_job}の14行目の処理は、ARMV8A_AARCH64向けのアセンブリで@<list>{burst_job_asm_simd}に変換されます。

//listnum[burst_job_asm_simd][@<list>{burst_job}の14行目のARMV8A_AARCH64向けのアセンブリ]{
        fmul        v0.4s, v0.4s, v0.4s
        fmul        v1.4s, v1.4s, v1.4s
//}

アセンブリのオペランドに、@<code>{.4s}というサフィックスがついていることから、SIMD命令が利用されていることが確認できます。

ピュアなC#により実装されたコードとBurstにより最適化されたコードのパフォーマンスを実機で比較します。

実機にはAndroid Pixel 4a、IL2CPPをスクリプトバックエンドとしてビルドを行い比較しています。また配列のサイズは2^20 = 1,048,576としています。
計測は同じ処理を10回繰り返し、処理時間の平均をとりました。

@<table>{burst_comp}にパフォーマンス比較の計測結果を示します。

//tsize[|latex||l|r|]
//table[burst_comp][ピュアなC#実装とBurstによる最適化されたコードの処理時間の比較]{
手法	処理時間（非表示）
-------------------------------------------------------------
ピュアなC#実装	5.73ms
Burstによる実装	0.98ms
//}

ピュアなC#実装と比べて約5.8倍ほど高速化を確認できました。