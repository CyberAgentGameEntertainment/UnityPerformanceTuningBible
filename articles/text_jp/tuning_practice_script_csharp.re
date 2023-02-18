={practice_script_csharp} Tuning Practice - Script (C#)

本章では、主にC#コードのパフォーマンスチューニング手法について実例を交えながら紹介します。
C#の基礎的な記法についてはここでは扱わず、パフォーマンスを要求されるようなゲームの開発において
意識すべき設計や実装について解説します。

=={practice_script_csharp_sample} GC.Allocするケースと対処法

@<hd>{basic|basic_csharp_gc}で紹介しましたが、
本節では具体的にどのような処理を行ったときにGC.Allocをするのか、まずは理解していきましょう。

==={practice_script_csharp_new} 参照型のnew

まずはとてもわかりやすくGC.Allocが発生するケースです。

//listnum[simple_list_alloc][毎フレームGC.Allocするコード][csharp]{
private void Update()
{
    const int listCapacity = 100;
    // List<int>のnewでGC.Alloc
    var list = new List<int>(listCapacity);
    for (var index = 0; index < listCapacity; index++)
    {
        // 特に意味はないけどindexをListに詰めていく
        list.Add(index);
    }
    // listから値をランダムに取り出す
    var random = UnityEngine.Random.Range(0, listCapacity);
    var randomValue = list[random];
    // ... ランダムな値から何かする ...
}
//}
このコードの大きな問題点は、毎フレーム実行されるUpdateメソッドで、
@<code>{List<int>}を@<code>{new}している点です。

こちらを修正するには、@<code>{List<int>}を事前に生成して使い回すことで毎フレームのGC.Allocを
回避することが可能です。

//listnum[simple_list_nonalloc][毎フレームのGC.Allocを無くしたコード][csharp]{
private static readonly int listCapacity = 100;
// 事前にListを生成しておく
private readonly List<int> _list = new List<int>(listCapacity);

private void Update()
{
    _list.Clear();
    for (var index = 0; index < listCapacity; index++)
    {
        // 特に意味はないけどindexをListに詰めていく
        _list.Add(index);
    }
    // listから値をランダムに取り出す
    var random = UnityEngine.Random.Range(0, listCapacity);
    var randomValue = _list[random];
    // ... ランダムな値から何かする ...
}
//}

こちらのサンプルコードのような無意味なコードを書くことはないと思いますが、
類似した例は想像よりも見つかるケースが多いです。

====[column] GC.Allocを無くしたら

気付いた方も多いかと思いますが、上記@<list>{simple_list_nonalloc}のサンプルコードはこれだけで事足ります。
//listnum[simple_random][][csharp]{
var randomValue = UnityEngine.Random.Range(0, listCapacity);
// ... ランダムな値から何かする ...
//}
パフォーマンスチューニングにおいてGC.Allocを無くすことを考えるのは重要ですが、
無意味な計算を省くことを常に考えることが高速化の一歩に繋がります。

====[/column]


==={practice_script_csharp_lambda} ラムダ式
ラムダ式も便利な機能ですが、こちらも使い方によってはGC.Allocが発生してしまうため、
ゲームでは使用できる場面は限られます。
ここでは、次のようなコードが定義されている前提とします。
//listnum[lambda_define][ラムダ式サンプルの前提コード][csharp]{
// メンバー変数
private int _memberCount = 0;

// static変数
private static int _staticCount = 0;

// メンバーメソッド
private void IncrementMemberCount() 
{
    _memberCount++;
}

// staticメソッド
private static void IncrementStaticCount() 
{
    _staticCount++;
}

// 受け取ったActionをInvokeするだけのメンバーメソッド
private void InvokeActionMethod(System.Action action)
{
    action.Invoke();
}
//}

このとき、次のようにラムダ式内で変数を参照した場合、GC.Allocが発生します。

//listnum[lambda_member_var][ラムダ式内で変数を参照してGC.Allocするケース][csharp]{
// メンバー変数を参照した場合、Delegate Allocationが発生
InvokeActionMethod(() => { _memberCount++; });

// ローカル変数を参照した場合、Closure Allocationが発生
int count = 0;
// 上記と同じDelegate Allocationも発生
InvokeActionMethod(() => { count++; });
//}            

ただし、以下のようにstatic変数を参照すると、これらのGC.Allocを回避できます。

//listnum[lambda_static_var][ラムダ式内でstatic変数を参照してGC.Allocしないケース][csharp]{
// static変数を参照した場合、GC Allocは発生せず
InvokeActionMethod(() => { _staticCount++; });
//}            

//embed[latex]{
\clearpage
//}

ラムダ式内でのメソッド参照も記述方法によってGC.Allocのされ方が異なります。

//listnum[lambda_method][ラムダ式内でメソッドを参照してGC.Allocするケース][csharp]{
// メンバーメソッドを参照した場合、Delegate Allocationが発生
InvokeActionMethod(() => { IncrementMemberCount(); });

// メンバーメソッドを直接指定した場合、Delegate Allocationが発生
InvokeActionMethod(IncrementMemberCount);

// staticメソッドを直接指定した場合、Delegate Allocationが発生
InvokeActionMethod(IncrementStaticCount);
//}

これらを回避するためには、以下のようにステートメント形式でstaticメソッドを参照する必要があります。

//listnum[lambda_member_method_ref][ラムダ式内でメソッドを参照してGC.Allocしないケース][csharp]{
// ラムダ式内でstaticメソッドを参照した場合、GC.Allocは発生せず
InvokeActionMethod(() => { IncrementStaticCount(); });
//}            
こうすることで初回のみActionがnewされますが、内部的にキャッシュされることによって2回目以降GC.Allocが回避されます。

しかし、すべての変数やメソッドをstaticにすることはコードの安全性や可読性の面から
とても採用できるものではありません。高速化が必要なコードでは、staticを多用してGC.Allocを無くすよりも
毎フレームもしくは不定なタイミングで発火するイベントなどはラムダ式を使わずに設計する方が安全と言えるでしょう。

==={practice_script_csharp_boxing_generic} ジェネリックを使用してボックス化するケース

ジェネリックを使用した以下の場合、何が原因でボックス化する可能性があるでしょうか。

//listnum[generic_boxing][ジェネリックを使用してボックス化する可能性がある例][csharp]{
public readonly struct GenericStruct<T> : IEquatable<T>
{
    private readonly T _value;

    public GenericStruct(T value)
    {
        _value = value;
    }

    public bool Equals(T other)
    {
        var result = _value.Equals(other);
        return result;
    }
}
//}
このケースでは、プログラマーは@<code>{GenericStruct}に@<code>{IEquatable<T>}インターフェイスの実装をしましたが、
@<code>{T}に制限を設けるのを忘れてしまいました。その結果、@<code>{IEquatable<T>}インターフェイスの実装がされていない型を
@<code>{T}に指定できてしまい、@<code>{Object}型へ暗黙的にキャストされて以下の@<code>{Equals}が使われるケースが存在してしまいます。

//listnum[object_class_equals][Object.cs][csharp]{
public virtual bool Equals(object obj);
//}

たとえば@<code>{IEquatable<T>}インターフェイスの実装がされていない@<code>{struct}を@<code>{T}に指定すると、@<code>{Equals}の引数で
@<code>{object}にキャストされることになるため、ボックス化が発生します。これが起こらないように事前に対策するには、
次のように変更します。

//listnum[generic_non_boxing][ボックス化しないように制限をかけた例][csharp]{
public readonly struct GenericOnlyStruct<T> : IEquatable<T>
    where T : IEquatable<T>
{
    private readonly T _value;

    public GenericOnlyStruct(T value)
    {
        _value = value;
    }

    public bool Equals(T other)
    {
        var result = _value.Equals(other);
        return result;
    }
}
//}

@<code>{where}句（ジェネリック型制約）を用いて、@<code>{T}が受け入れられる型を@<code>{IEquatable<T>}を実装している型に制限してあげることで、
こうした予期せぬボックス化を未然に防ぐことができます。

====[column] 本来の目的を見失わない

@<hd>{basic|basic_csharp_gc}で紹介したようにゲームではランタイム中のGC.Allocを避けたい意図があるため、
構造体が選択されるケースも多く存在します。ただし、GC.Allocを削減したいあまりにすべてを構造体にしたところで高速化できるわけではありません。

よくある失敗としては、GC.Allocを避ける目的で構造体を取り入れたところ、期待通りGCに関するコストが減ったものの、
データサイズが大きいために値型のコピーコストがかかってしまい結果的に非効率な処理になってしまうようなケースが挙げられます。

また、これをさらに回避するためにメソッドの引数を参照渡しを利用することでコピーコストを削減する手法も存在します。
結果的に高速化できる可能性はありますが、この場合は最初からクラスを選択し、インスタンスを事前に生成して使い回すような実装を検討すべきでしょう。
GC.Allocを撲滅することが目的ではなく、あくまでも1フレームあたりの処理時間を短くすることが最終目的であることを忘れないようにしましょう。

====[/column]

=={practice_script_csharp_loop} for/foreachについて

@<hd>{basic|basic_algorithm}で紹介したようにループはデータ数に依存して時間がかかるようになります。
また、一見同じような処理に見えるループもコードの書き方次第で効率が変わります。

ここでは、SharpLab@<fn>{performance_tips_sharplab}を利用して、@<code>{foreach/for}を使用した
@<code>{List}や配列の中身を1つずつ取得するだけのコードをILからC#にデコンパイルした結果を見てみましょう。

//footnote[performance_tips_sharplab][@<href>{https://sharplab.io/}]

まずは@<code>{foreach}でループを回した場合を見てみましょう。@<code>{List}への値の追加などは省略してます。

//listnum[simple_foreach_list][Listをforeachで回す例][csharp]{
var list = new List<int>(128);        
foreach (var val in list)
{
}
//}

//listnum[simple_foreach_list_decompile][Listをforeachで回す例のデコンパイル結果][csharp]{
List<int>.Enumerator enumerator = new List<int>(128).GetEnumerator();
try
{
    while (enumerator.MoveNext())
    {
        int current = enumerator.Current;
    }
}
finally
{
    ((IDisposable)enumerator).Dispose();
}
//}

@<code>{foreach}で回した場合は、列挙子を取得して@<code>{MoveNext()}で次へ進めて@<code>{Current}で値を参照する実装になっていることがわかります。
さらに、list.cs@<fn>{performance_tips_ms_list}の
@<code>{MoveNext()}の実装を見ると、サイズのチェックなど各種プロパティアクセス回数が多くなっており、インデクサーによる直アクセスより
処理が多くなるように見えます。

//footnote[performance_tips_ms_list][https://referencesource.microsoft.com/#mscorlib/system/collections/generic/list.cs]


次に、@<code>{for}で回したときを見てみましょう。

//listnum[simple_for_list][Listをforで回す例][csharp]{
var list = new List<int>(128);
for (var i = 0; i < list.Count; i++)
{
    var val = list[i];
}
//}

//listnum[simple_for_list_decompile][Listをforで回した際のデコンパイル結果][csharp]{
List<int> list = new List<int>(128);
int num = 0;
while (num < list.Count)
{
    int num2 = list[num];
    num++;
}
//}

C#では@<code>{for}文は@<code>{while}文の糖衣構文であり、インデクサー（@<code>{public T this[int index]}）による参照で取得されていることがわかります。
また、この@<code>{while}文をよく見ると、条件式に@<code>{list.Count}が入っています。つまり、@<code>{Count}プロパティへの
アクセスがループを繰り返すたびに行われることになります。@<code>{Count}の数が多くなればなるほど、@<code>{Count}プロパティへのアクセス回数が比例して増加し、
数によっては無視できない負荷になっていきます。もし、ループ内で@<code>{Count}が変わらないのであれば、ループの前でキャッシュしておくことでプロパティアクセスの負荷を削減できます。

//listnum[simple_for_improve_list][Listをforで回す例: 改良版][csharp]{
var count = list.Count;
for (var i = 0; i < count; i++)
{
    var val = list[i];
}
//}

//listnum[simple_for_improve_list_decompile][Listをforで回す例: 改良版のデコンパイル結果][csharp]{
List<int> list = new List<int>(128);
int count = list.Count;
int num = 0;
while (num < count)
{
    int num2 = list[num];
    num++;
}
//}

@<code>{Count}をキャッシュすることでプロパティアクセス回数が削減され、高速化されました。
今回のループ中の比較はどちらもGC.Allocによる負荷はなく、実装内容の違いによる差分となります。

また、配列の場合は@<code>{foreach}も最適化されており、@<code>{for}で記述したものとほぼ変化はないものとなります。

//listnum[simple_foreach_array][配列をforeachで回す例][csharp]{
var array = new int[128];
foreach (var val in array)
{
}
//}

//listnum[simple_foreach_array_decompile][配列をforeachで回す例のデコンパイル結果][csharp]{
int[] array = new int[128];
int num = 0;
while (num < array.Length)
{
    int num2 = array[num];
    num++;
}
//}


検証のため、データ数10,000,000として事前にランダムな数値をアサインし、@<code>{List<int>}
データの和を計算するものとしました。検証環境はPixel 3a、Unity 2021.3.1f1で実施しました。

//table[loop_profile_diff_int][List<int>における記述方法ごとの計測結果]{
種類	Time ms
List: foreach	66.43
List: for	62.49
List: for（Countキャッシュ）	55.11
配列: for	30.53
配列: foreach	23.75
//}

@<code>{List<int>}の場合は、条件を細かく揃えて比較してみると@<code>{foreach}よりも@<code>{for}、@<code>{Count}の最適化を施した@<code>{for}のほうが
さらに速くなったことがわかります。@<code>{List}の@<code>{foreach}は、@<code>{Count}の最適化を施した@<code>{for}に書き換えることで、
@<code>{foreach}の処理における@<code>{MoveNext()}や@<code>{Current}プロパティのオーバーヘッドを削減し、高速化が可能です。

また、@<code>{List}と配列のそれぞれ最速同士で比較すると、@<code>{List}より配列のほうが約2.3倍以上高速になりました。
@<code>{foreach}と@<code>{for}でILが同じ結果になるように記述しても、@<code>{foreach}が速い結果となり、
配列の@<code>{foreach}が十分に最適化されていると言えるでしょう。

以上の結果から、データ数が多くかつ処理速度を高速にしなければならない場面については@<code>{List<T>}ではなく配列を検討すべきでしょう。
しかし、フィールドに定義した@<code>{List}をローカルキャッシュせずそのまま参照してしまうなど、書き換えが不十分な場合は高速化できないこともありますので、
@<code>{foreach}から@<code>{for}に変更する際には必ず計測をしつつ適切に書き換えましょう。

=={practice_script_csharp_reuse} オブジェクトプーリング

随所で触れてきましたが、ゲーム開発では動的にオブジェクトを生成せずに、事前生成して使い回すことが重要です。
これを@<em>{オブジェクトプーリング}と呼びます。たとえば、ゲームフェーズで使用予定のオブジェクトをロードフェーズでまとめて生成してプーリングしておき、
使用するときはプールしているオブジェクトへの代入と参照のみ行いながら扱うことで、ゲームフェーズ中のGC.Allocを避けることが可能です。

また、オブジェクトプーリングはアロケーションの削減の他にも、画面を構成するオブジェクトを都度作り直すことなく
画面遷移を可能にしておくことでロード時間の短縮を実現したり、計算コストが非常に高い処理の結果を保持しておいて重い計算を複数回実行することを避けたり、
さまざまな場面で用いられます。

ここでは広義にオブジェクトと表現しましたが、これは最小単位のデータに留まらず、@<code>{Coroutine}や@<code>{Action}などにも
該当します。たとえば、事前に@<code>{Coroutine}を想定される実行数分以上生成しておき、必要なタイミングで使用して使い潰していくようなことも検討しましょう。
2分間で終わるゲームで最大で20回実行されるようなときは@<code>{IEnumerator}をそれぞれ先に生成しておき、使用する時は@<code>{StartCoroutine}するだけにすることで
生成コストは抑えられます。

=={practice_script_csharp_string} string

@<code>{string}オブジェクトは文字列を表す@<code>{System.Char}オブジェクトのシーケンシャルコレクションです。@<code>{string}は使い方1つでGC.Allocが簡単に起こります。
たとえば、文字連結演算子@<code>{+}を利用して2つの文字列の連結をすると、新しい@<code>{string}オブジェクトを生成することになります。@<code>{string}の値は生成後に
変更できない（イミュータブル）ため、値の変更が行われているように見える操作は新しい@<code>{string}オブジェクトを生成して返しています。

//listnum[string_create][文字列結合でstringを作る場合][csharp]{
private string CreatePath()
{
    var path = "root";
    path += "/";
    path += "Hoge";
    path += "/";
    path += "Fuga";
    return path;
}
//}

上記例の場合は、各文字列結合でstringが生成されていき、合計で164Byteアロケーションが発生します。

文字列が頻繁に変更されるときは、値が変更可能な@<code>{StringBuilder}を利用することで@<code>{string}オブジェクトの大量生成を防ぐことができます。
文字連結や削除などの操作を@<code>{StringBuilder}オブジェクトで行い、最終的に値を取り出して@<code>{string}オブジェクトに@<code>{ToString()}することで、
取得時のみのメモリアロケーションに抑えることができます。また、@<code>{StringBuilder}を使用する際には、Capacityを必ず設定するようにしましょう。
未指定のときは初期値が16になり、@<code>{Append}などで文字数が増えバッファーが拡張されるときにメモリの確保と値のコピーが走るため、
不用意な拡張が発生しない適切なCapacityを設定するようにしましょう。


//listnum[string_create_builder][StringBuilderでstringを作る場合][csharp]{
private readonly StringBuilder _stringBuilder = new StringBuilder(16);
private string CreatePathFromStringBuilder()
{
    _stringBuilder.Clear();
    _stringBuilder.Append("root");
    _stringBuilder.Append("/");
    _stringBuilder.Append("Hoge");
    _stringBuilder.Append("/");
    _stringBuilder.Append("Fuga");
    return _stringBuilder.ToString();
}
//}

@<code>{StringBuilder}を用いた例では、事前に@<code>{StringBuilder}を生成（上記例の場合、生成時に112Byteアロケーション）しておけば、
以降は生成した文字列を取り出す@<code>{ToString()}時に掛かる50Byteのアロケーションで済みます。

ただし、@<code>{StringBuilder}も値の操作中にアロケーションが起こりにくいだけで、前述のように@<code>{ToString()}実行時には@<code>{string}オブジェクトを
生成することになるため、GC.Allocを避けたいときに使用するのは推奨されません。また、@<code>{$""}構文は@<code>{string.Format}に変換され、
@<code>{string.Format}の内部実装は@<code>{StringBuilder}が使われているため、結局は@<code>{ToString()}のコストは避けられません。
前項のオブジェクトの使い回しをここでも応用し、あらかじめ使用される可能性のある文字列は@<code>{string}オブジェクトを事前に生成し、それを使うようにしましょう。

しかしながらゲーム中に文字列操作と@<code>{string}オブジェクトの生成をどうしても行わなければならない場合もあります。
そういう場合には、文字列用のバッファーを事前に持っておいて、そのバッファーをそのまま使えるようにするような拡張を行う必要があります。
@<code>{unsafe}なコードを自前で実装するか、ZString@<fn>{string_zstring}のようなUnity向けの拡張機能
（たとえば@<code>{TextMeshPro}へのNonAllocな適用機能）を備えたライブラリの導入を検討しましょう。

//footnote[string_zstring][@<href>{https://github.com/Cysharp/ZString}]

=={practice_script_csharp_linq} LINQと遅延評価
本節ではLINQの使用によるGC.Allocを軽減する方法と遅延評価のポイントについて解説します。

==={practice_script_csharp_linq_gc_cause}LINQの使用によるGC.Allocを軽減する
LINQの使用では、@<list>{linq_heap_allocation_sample_code}のような場合にGC.Allocが発生します。

//listnum[linq_heap_allocation_sample_code][GC.Allocが発生する例][csharp]{
var oneToTen = Enumerable.Range(1, 11).ToArray();
var query = oneToTen.Where(i => i % 2 == 0).Select(i => i * i);
//}

@<list>{linq_heap_allocation_sample_code}でGC.Allocが発生する理由はLINQの内部実装に起因します。
加えて、LINQの一部メソッドは呼び出し側の型に合わせた最適化を行うため、呼び出し元の型によってGC.Allocのサイズが変化します。

//listnum[linq_type_optimization_query_execute][型ごとの実行速度検証][csharp]{
private int[] array;
private List<int> list;
private IEnumerable<int> ienumerable;

public void GlobalSetup()
{
    array = Enumerable.Range(0, 1000).ToArray();
    list = Enumerable.Range(0, 1000).ToList();
    ienumerable = Enumerable.Range(0, 1000);
}

public void RunAsArray()
{
    var query = array.Where(i => i % 2 == 0);
    foreach (var i in query){}
}

public void RunAsList()
{
    var query = list.Where(i => i % 2 == 0);
    foreach (var i in query){}
}

public void RunAsIEnumerable()
{
    var query = ienumerable.Where(i => i % 2 == 0);
    foreach (var i in query){}
}
//}

@<list>{linq_type_optimization_query_execute}に定義した各メソッドのベンチマークを測定すると@<img>{linq_type_optimizaiton_query_execute_result}のような結果が得られました。
この結果から@<code>{T[]} → @<code>{List<T>} → @<code>{IEnumerable<T>}の順番にヒープアロケーションのサイズが大きくなっていることがわかります。

このように、LINQを使用する場合は、実行時の型を意識することでGC.Allocのサイズを削減することができます。

//image[linq_type_optimizaiton_query_execute_result][型ごとの実行速度比較][scale=1.0]

====[column] LINQのGC.Allocの原因

LINQの使用によるGC.Allocの原因の一部は、LINQの内部実装です。
LINQのメソッドは@<code>{IEnumerable<T>}を受け取り、@<code>{IEnumerable<T>}を返すものが多く、このAPI設計によりメソッドチェーンを用いた直感的な記述ができるようになっています。
このときメソッドが返す@<code>{IEnumerable<T>}の実体は、各機能に合わせたクラスのインスタンスとなっています。
LINQは内部的に@<code>{IEnumerable<T>}を実装したクラスをインスタンス化してしまい、さらにループ処理を実現するために@<code>{GetEnumerator()}の呼び出しなどが行われるため内部的にGC.Allocが発生します。

====[/column]

==={practice_script_csharp_linq_lazy}LINQの遅延評価
LINQの@<code>{Where}や@<code>{Select}といったメソッドは、実際に結果が必要になるまで評価を遅らせる遅延評価となっています。
一方で、@<code>{ToArray}のような即時評価となるメソッドも定義されています。

ここで、下記の@<list>{linq_to_array_sample_code}のコードの場合を考えます。

//listnum[linq_to_array_sample_code][即時評価を挟んだメソッド][csharp]{
private static void LazyExpression()
{
    var array = Enumerable.Range(0, 5).ToArray();
    var sw = Stopwatch.StartNew();
    var query = array.Where(i => i % 2 == 0).Select(HeavyProcess).ToArray();
    Console.WriteLine($"Query: {sw.ElapsedMilliseconds}");

    foreach (var i in query)
    {
        Console.WriteLine($"diff: {sw.ElapsedMilliseconds}");
    }
}

private static int HeavyProcess(int x)
{
    Thread.Sleep(1000);
    return x;
}
//}

@<list>{linq_to_array_sample_code}の実行結果が@<list>{linq_to_array_sample_code_result}になります。
即時評価となる@<code>{ToArray}を末尾に追加したことで、@<code>{query}への代入時に@<code>{Where}や@<code>{Select}のメソッドを実行し値を評価した結果が返されます。
そのため@<code>{HeavyProcess}も呼び出されるので、@<code>{query}を生成するタイミングで処理時間がかかっていることがわかります。

//listnum[linq_to_array_sample_code_result][即時評価のメソッドを追加した結果]{
Query: 3013
diff: 3032
diff: 3032
diff: 3032
//}

このようにLINQの即時評価のメソッドを意図せず呼び出してしまうとその箇所がボトルネックになってしまう可能性があります。@<code>{ToArray}や@<code>{OrderBy}, @<code>{Count}などシーケンスすべてを一度見る必要があるメソッドは即時評価となるため、呼び出し時のコストを意識して使用しましょう。

==={practice_script_csharp_not_use_linq}「LINQの使用を避ける」という選択
LINQを使用した際のGC.Allocの原因や軽減方法、遅延評価のポイントについて解説しました。
本節ではLINQを使用する基準について解説します。
前提としてLINQは便利な言語機能ではありますが、使用するとヒープアロケーションや実行速度は使用しない場合に比べて悪化します。
実際にMicrosoftのUnityのパフォーマンスに関する推奨事項@<fn>{performance_tips_from_microsoft}では「Avoid use of LINQ」と明記されています。
LINQを使用した場合と、使用しない場合で同じロジックを実装した場合のベンチマークを@<list>{linq_vs_pure_benchmark}で比較してみます。

//footnote[performance_tips_from_microsoft][@<href>{https://docs.microsoft.com/en-us/windows/mixed-reality/develop/unity/performance-recommendations-for-unity#avoid-expensive-operations}]

//listnum[linq_vs_pure_benchmark][LINQの使用有無によるパフォーマンス比較][csharp]{
private int[] array;

public void GlobalSetup()
{
    array = Enumerable.Range(0, 100_000_000).ToArray();
}

public void Pure()
{
    foreach (var i in array)
    {
        if (i % 2 == 0)
        {
            var _ = i * i;
        }
    }
}

public void UseLinq()
{
    var query = array.Where(i => i % 2 == 0).Select(i => i * i);
    foreach (var i in query)
    {
    }
}
//}

結果は@<img>{linq_vs_pure_benchmark}になります。実行時間を比較するとLINQを使用しない場合に対してLINQを使った処理は19倍ほど時間がかかってしまっていることがわかります。

//image[linq_vs_pure_benchmark][LINQの使用有無によるパフォーマンス比較結果][scale=1.0]

上記の結果からLINQを使用することによるパフォーマンスの悪化は明確ですが、LINQを使用することでコーディングの意図が伝わりやすい場合などもあります。
これらの挙動を把握した上で、LINQを使用するか、使用する場合のルールなどはプロジェクト内で議論の余地があるかと思います。

=={practice_script_csharp_async_await} async/awaitのオーバーヘッドの避け方
async/awaitはC#5.0で追加された言語機能であり、非同期処理をコールバックを使わず一筋の同期的処理のように記述できるものです。

==={practice_script_csharp_async_await_compiler_generated}不要な箇所でのasyncを避ける
asyncを定義されたメソッドは、コンパイラによって非同期処理を実現するためのコードが生成されます。
そしてasyncキーワードがあれば、コンパイラによるコード生成は必ず行われます。
そのため、@<list>{possibly_not_async_sample}のように同期的に完了する可能性のあるメソッドも実際にはコンパイラによるコード生成が行われています。

//listnum[possibly_not_async_sample][同期的に完了する可能性のある非同期処理][csharp]{
using System;
using System.Threading.Tasks;

namespace A {
    public class B {
        public async Task HogeAsync(int i) {
            if (i == 0) {
                Console.WriteLine("i is 0");
                return;
            }
            await Task.Delay(TimeSpan.FromSeconds(1));
        }

        public void Main() {
            int i = int.Parse(Console.ReadLine());
            Task.Run(() => HogeAsync(i));
        }
    }
}
//}

この@<list>{possibly_not_async_sample}のような場合は、同期的に終了する可能性のある@<code>{HogeAsync}を分割し、@<list>{complete_async_method_sample}のように実装することで同期的に完了する場合に不要な@<code>{IAsyncStateMachine}実装のステートマシン構造体を生成するコストを省略できます。

//embed[latex]{
\clearpage
//}

//listnum[complete_async_method_sample][同期処理と非同期処理を分割した実装][csharp]{
using System;
using System.Threading.Tasks;

namespace A {
    public class B {
        public async Task HogeAsync(int i) {
            await Task.Delay(TimeSpan.FromSeconds(1));
        }

        public void Main() {
            int i = int.Parse(Console.ReadLine());
            if (i == 0) {
                Console.WriteLine("i is 0");
            } else {
                Task.Run(() => HogeAsync(i));
            }
        }
    }
}
//}

====[column] async/awaitの仕組み

async/await構文はコンパイル時にコンパイラによるコード生成を用いて実現されています。
asyncキーワードのついたメソッドはコンパイル時点で@<code>{IAsyncStateMachine}を実装した構造体を生成する処理が追加され、await対象の処理が完了するとステートを進めるステートマシンを管理することでasync/awaitの機能を実現しています。
また、この@<code>{IAsyncStateMachine}は@<code>{System.Runtime.CompilerServices}名前空間に定義されたインタフェースであり、コンパイラのみが使用可能なものとなっています。

====[/column]

==={practice_script_csharp_async_await_thread_context_capture}同期コンテキストのキャプチャを避ける
別スレッドに退避させた非同期処理から、呼び出し元のスレッドに復帰する仕組みが同期コンテキストであり、@<code>{await}を使用することで直前のコンテキストをキャプチャーできます。
この同期コンテキストのキャプチャは@<code>{await}のたびに行われるため、@<code>{await}ごとのオーバーヘッドが発生します。
そのため、Unityでの開発に広く利用されているUniTask@<fn>{unitask_reference}では、同期コンテキストのキャプチャによるオーバーヘッドを避けるために@<code>{ExecutionContext}と@<code>{SynchronizationContext}を使用しない実装となっています。
Unityに関してはこのようなライブラリを導入することで、パフォーマンスの改善が見られる場合があります。

//footnote[unitask_reference][@<href>{https://tech.cygames.co.jp/archives/3417/}]

=={practice_script_csharp_stackalloc} stackallocによる最適化

配列の確保は通常ヒープ領域に確保されるため、ローカル変数として配列を確保すると、都度GC.Allocが発生してスパイクの原因となります。
また、ヒープ領域への読み書きはスタック領域と比べると、少しですが効率が悪くなります。

そのためC#では、@<kw>{unsafe}コード限定で、スタック上に配列を確保するための構文が用意されています。

@<list>{how_to_use_stackalloc}のように、@<code>{new}キーワードを用いる代わりに、@<code>{stackalloc}キーワードを用いて配列を確保すると、スタック上に配列が確保されます。

//listnum[how_to_use_stackalloc][@<code>{stackalloc}を用いたスタック上への配列確保][csharp]{
// stackallocはunsafe限定
unsafe
{
    // スタック上にintの配列を確保
    byte* buffer = stackalloc byte[BufferSize];
}
//}

C# 7.2から@<code>{Span<T>}構造体を用いることで、@<list>{how_to_use_safe_stackalloc}に示すように@<kw>{unsafe}なしで@<code>{stackalloc}を利用できるようになりました。

//listnum[how_to_use_safe_stackalloc][@<code>{Span<T>}構造体を併用したスタック上への配列確保][csharp]{
Span<byte> buffer = stackalloc byte[BufferSize];
//}

Unityの場合は2021.2から標準で利用できます。それ以前バージョンの場合は@<code>{Span<T>}が存在しないため、System.Memory.dllを導入する必要があります。

@<code>{stackalloc}で確保した配列はスタック専用なため、クラスや構造体のフィールドに持てません。必ずローカル変数として使う必要があります。

スタック上への確保とはいえ、要素数が大きい配列の確保はそれなりに処理時間がかかります。
もしUpdateループ内などのヒープアロケーションを避けたい箇所で要素数の大きい配列を利用したい場合は、初期化時の事前確保か、オブジェクトプールのようなデータ構造を用意して、利用時に貸し出すような実装のほうがよいでしょう。

また、@<code>{stackalloc}で確保したスタック領域は、@<em>{関数を抜けるまで解放されない}点に注意が必要です。
たとえば@<list>{stackalloc_with_loop}に示すコードは、ループ内で確保した配列はすべて保持され、@<code>{Hoge}メソッドを抜けるときに解放されるので、ループを回しているうちにStack Overflowを起こす可能性があります。

//listnum[stackalloc_with_loop][@<code>{stackalloc}を用いたスタック上への配列確保][csharp]{
unsafe void Hoge()
{
    for (int i = 0; i < 10000; i++)
    {
        // ループ数分配列が蓄積される
        byte* buffer = stackalloc byte[10000];
    }
}
//}

=={practice_script_csharp_sealed} sealedによるIL2CPPバックエンド下でのメソッド呼び出しの最適化

UnityでIL2CPPをバックエンドとしてビルドをすると、クラスのvirtualなメソッド呼び出しを実現するために、C++のvtableのような仕組みを用いてメソッド呼び出しを行います@<fn>{il2cpp_internal_method_call}。

//footnote[il2cpp_internal_method_call][@<href>{https://blog.unity.com/technology/il2cpp-internals-method-calls}]

具体的には、クラスのメソッド呼び出しの定義ごとに、@<list>{il2cpp_method_call}に示すようなコードが自動生成されます。

//listnum[il2cpp_method_call][IL2CPPが生成するメソッド呼び出しに関するC++コード][c++]{
struct VirtActionInvoker0
{
    typedef void (*Action)(void*, const RuntimeMethod*);

    static inline void Invoke (
        Il2CppMethodSlot slot, RuntimeObject* obj)
    {
        const VirtualInvokeData& invokeData =
            il2cpp_codegen_get_virtual_invoke_data(slot, obj);
        ((Action)invokeData.methodPtr)(obj, invokeData.method);
    }
};
//}

これはvirutalなメソッドだけでなく、@<em>{コンパイル時に継承をしていない、virtualでないメソッドであっても}同様なC++コードを生成します。
このような自動生成の挙動によって、@<em>{コードサイズが肥大化したり、メソッド呼び出しの処理時間が増大します}。

この問題は、クラスの定義に@<code>{sealed}修飾子をつけることで回避できます@<fn>{il2cpp_devirtualization}。

//footnote[il2cpp_devirtualization][@<href>{https://blog.unity.com/technology/il2cpp-optimizations-devirtualization}]

@<list>{il2cpp_method_call_non_sealed}のようなクラスを定義してメソッドを呼び出した場合、IL2CPPで生成されたC++コードでは@<list>{il2cpp_method_call_non_sealed_cpp}のようなメソッド呼び出しが行われます。

//listnum[il2cpp_method_call_non_sealed][sealedを用いないクラス定義とメソッド呼び出し][csharp]{
public abstract class Animal
{
    public abstract string Speak();
}

public class Cow : Animal
{
    public override string Speak() {
        return "Moo";
    }
}

var cow = new Cow();
// Speakメソッドを呼び出す
Debug.LogFormat("The cow says '{0}'", cow.Speak());
//}

//listnum[il2cpp_method_call_non_sealed_cpp][@<list>{il2cpp_method_call_non_sealed}のメソッド呼び出しに対応するC++コード][c++]{
// var cow = new Cow();
Cow_t1312235562 * L_14 =
    (Cow_t1312235562 *)il2cpp_codegen_object_new(
        Cow_t1312235562_il2cpp_TypeInfo_var);
Cow__ctor_m2285919473(L_14, /*hidden argument*/NULL);
V_4 = L_14;
Cow_t1312235562 * L_16 = V_4;

// cow.Speak()
String_t* L_17 = VirtFuncInvoker0< String_t* >::Invoke(
    4 /* System.String AssemblyCSharp.Cow::Speak() */, L_16);
//}

@<list>{il2cpp_method_call_non_sealed_cpp}に示すように、virtualなメソッド呼び出しではないにもかかわらず@<code>{VirtFuncInvoker0< String_t* >::Invoke}を呼び出しており、virtualメソッドのようなメソッド呼び出しが行われていることが確認できます。

一方で、@<list>{il2cpp_method_call_non_sealed}の@<code>{Cow}クラスを@<list>{il2cpp_method_call_sealed}に示すように@<code>{sealed}修飾子を用いて定義すると、@<list>{il2cpp_method_call_sealed_cpp}のようなC++コードが生成されます。

//listnum[il2cpp_method_call_sealed][sealedを用いたクラス定義とメソッド呼び出し][csharp]{
public sealed class Cow : Animal
{
    public override string Speak() {
        return "Moo";
    }
}

var cow = new Cow();
// Speakメソッドを呼び出す
Debug.LogFormat("The cow says '{0}'", cow.Speak());
//}

//listnum[il2cpp_method_call_sealed_cpp][@<list>{il2cpp_method_call_sealed}のメソッド呼び出しに対応するC++コード][c++]{
// var cow = new Cow();
Cow_t1312235562 * L_14 =
    (Cow_t1312235562 *)il2cpp_codegen_object_new(
        Cow_t1312235562_il2cpp_TypeInfo_var);
Cow__ctor_m2285919473(L_14, /*hidden argument*/NULL);
V_4 = L_14;
Cow_t1312235562 * L_16 = V_4;

// cow.Speak()
String_t* L_17 = Cow_Speak_m1607867742(L_16, /*hidden argument*/NULL);
//}

このように、メソッド呼び出しが@<code>{Cow_Speak_m1607867742}を呼び出しており、直接メソッドを呼び出していることが確認できます。

ただし、比較的最近のUnityでは、このような最適化では一部自動で行われていることをUnity公式で明言しています@<fn>{improved_devirtualization_forum}。

//footnote[improved_devirtualization_forum][@<href>{https://forum.unity.com/threads/il2cpp-is-sealed-not-worked-as-said-anymore-in-unity-2018-3.659017/#post-4412785}]

つまり、@<code>{sealed}を明示的に指定しない場合でも、このような最適化が自動で行われている可能性があります。

しかし、「[il2cpp] Is `sealed` Not Worked As Said Anymore In Unity 2018.3?」
@<m>{\footnotemark[8]}というフォーラムで言及している通り、2019年4月の段階で、この実装は完全というわけではありません。

このような現状から、IL2CPPの生成するコードを確認しながら、プロジェクトごとにsealed修飾子の設定を決めるとよいでしょう。

より確実に直接的なメソッド呼び出しを行うために、また、今後のIL2CPPの最適化を期待して、最適化可能なマークとして@<code>{sealed}修飾子を設定するのもよいかもしれません。

=={practice_script_csharp_inline} インライン化による最適化

メソッド呼び出しには多少のコストがかかります。
そのため、C#に限らず一般的な最適化として、比較的小さいメソッドの呼び出しは、コンパイラなどによってインライン化という最適化が行われます。

具体的には、@<list>{inline_sample_no_inline}のようなコードに対して、インライン化によって@<list>{inline_sample_inline}のようなコードが生成されます。

//listnum[inline_sample_no_inline][インライン化前のコード][csharp]{
int F(int a, int b, int c)
{
    var d = Add(a, b);
    var e = Add(b, c);
    var f = Add(d, e);

    return f;
}

int Add(int a, int b) => a + b;
//}

//listnum[inline_sample_inline][@<list>{inline_sample_no_inline}に対してインライン化を行ったコード][csharp]{
int F(int a, int b, int c)
{
    var d = a + b;
    var e = b + c;
    var f = d + e;

    return f;
}
//}

インライン化は@<list>{inline_sample_inline}のように、@<list>{inline_sample_no_inline}の@<code>{Func}メソッド内での@<code>{Add}メソッドの呼び出しを、メソッド内の内容をコピーして展開することで行われます。

IL2CPPでは、コード生成時にはとくにインライン化による最適化は行われません。

しかし、Unity 2020.2からメソッドに@<code>{MethodImpl}属性を指定し、そのパラメーターに@<code>{MethodOptions.AggressiveInlining}を指定することで、生成されるC++コードの対応する関数に@<code>{inline}指定子が付与されるようになりました。
つまり、C++のコードレベルでのインライン化が行えるようになりました。

インライン化のメリットは、メソッド呼び出しのコストが削減されるだけでなく、メソッド呼び出し時に指定した引数のコピーも省けることです。

たとえば算術系のメソッドは、@<code>{Vector3}や@<code>{Matrix}のような、比較的サイズの大きい構造体を複数個引数に取ります。
構造体はそのまま引数として渡すと、すべて値渡しとしてコピーされてメソッドに渡されるため、引数の個数や渡す構造体のサイズが大きいと、メソッド呼び出しと引数のコピーでかなりの処理コストがかかる可能性があります。
また、物理演算やアニメーションの実装など、定期処理などで利用されることが多いため、メソッド呼び出しが処理負荷として見逃せないケースになることがあります。

このようなケースでは、インライン化による最適化は有効です。実際に、Unityの新しい算術系ライブラリである@<kw>{Unity.Mathmatics}では、いたるメソッド呼び出しに@<code>{MethodOptions.AggressiveInlining}が指定されています@<fn>{unity_mathmatics_inline}。

一方で、インライン化はメソッド内の処理を展開する処理のため、展開した分コードサイズが増大するというデメリットがあります。

そのため、とくに1フレームで頻繁に呼び出されホットパスとなるようなメソッドに対して、インライン化を検討するとよいでしょう。
また、属性を指定すると必ずインライン化が行われるわけではない点にも注意が必要です。

インライン化されるメソッドは、その中身が小さいものに限定されるため、インライン化を行いたいメソッドは処理を小さく保つ必要があります。

また、Unity 2020.2以前では属性指定に対して@<code>{inline}指定子がつかないのと、C++の@<code>{inline}指定子を指定してもインライン化が確実に行われる保証はありません。

そのため確実にインライン化を行いたい場合、可読性は落ちますがホットパスとなるメソッドは手動でのインライン化も検討するとよいでしょう。

//footnote[unity_mathmatics_inline][@<href>{https://github.com/Unity-Technologies/Unity.Mathematics/blob/f476dc88954697f71e5615b5f57462495bc973a7/src/Unity.Mathematics/math.cs#L1894}]
