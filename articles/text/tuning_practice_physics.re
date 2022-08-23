={practice_physics} Tuning Practice - Physics

本章では、Physics（物理演算）の最適化について紹介します。

ここでPhysicsとはPhysXによる物理演算を指します。ECSのUnity Physicsは扱っていません。

また本章では、3D Physicsをメインで取り上げていますが、2D Physicsでも参考になる箇所は多いでしょう。

=={practice_physics_you_need_physics} 物理演算のオン・オフ

Unity標準では、たとえシーン上に1つも物理演算に関するコンポーネントが配置されていなかったとしても、物理エンジンによる物理演算の処理は毎フレーム必ず実行されます。
そのため、ゲーム内で物理演算を必要としない場合は、物理エンジンをオフにしておくとよいでしょう。

物理エンジンの処理は、@<code>{Physics.autoSimulation}に値を設定することでオン・オフを切り替えることができます。
たとえば、インゲーム中のみ物理演算を用いて、それ以外では利用しない場合は、インゲーム中のみこの値を@<code>{true}に設定しておくとよいでしょう。

=={practice_physics_fixed_timestep} Fixed TimestepとFixed Updateの頻度の最適化

MonoBehaviourの@<code>{FixedUpdate}は@<code>{Update}とは違い、固定時間で実行されます。

物理エンジンは前フレームの経過時間に対して、1フレーム内でFixed Updateを複数回呼び出すことで、ゲームの世界の経過時間と物理エンジンの世界の時間をあわせます。
そのためFixed Timestepの値が小さいと、@<em>{より多くの回数Fixed Updateが呼び出され、負荷の原因となります}。

この時間は、@<img>{projectsetting_time_fixed_timestep}の示すように、Project Settingsの@<kw>{Fixed Timestep}で設定できます。この値の単位は秒となります。デフォルトでは0.02、つまり20ミリ秒が指定されています。

//image[projectsetting_time_fixed_timestep][Project SettingsのFixed Timestep項目][scale=1.0]

また、スクリプト中からは@<code>{Time.fixedDeltaTime}を操作することで変更できます。

@<kw>{Fixed Timestep}は一般に、小さいほど物理演算の精度が上がり、コリジョン抜けなどの問題が発生しにくくなります。
そのため、精度と負荷のトレードオフにはなりますが、@<em>{ゲームの挙動に不備が発生しない範囲でこの値をターゲットFPSに近い値にする}ことが望ましいです。

==={practice_physics_maximum_allowed_timestep} Maximum Allowed Timestep

前節のとおり、Fixed Updateは前フレームからの経過時間をもとに複数回呼び出されます。

「あるフレームでレンダリング処理が重たい」などの理由で前フレームの経過時間が大きくなった場合、そのフレームでは通常より多くの回数Fixed Updateが呼び出されることになります。

たとえば、@<kw>{Fixed Timestep}が20ミリ秒で前フレームに200ミリ秒かかったとき、Fixed Updateは10回呼び出されることになります。

つまり、あるフレームで処理落ちした場合は次のフレームでの物理演算のコストが高くなります。
それが原因でそのフレームも処理落ちするリスクが高くなることで、次フレームでの物理演算も重くなる、といった負のスパイラルに陥る現象が物理エンジンの世界では知られています。

この問題を解決するためにUnityでは@<img>{projectsetting_time_maximum_allowed_timestep}に示すように、Project Settingsから@<kw>{Maximum Allowed Timestep}という、1フレーム内で物理演算が利用する時間の最大値が設定できます。
この値はデフォルトで0.33秒が設定されていますが、ターゲットFPSに近い値にしてFixed Updateの呼び出し回数を制限し、フレームレートを安定させたほうがよいでしょう。

//image[projectsetting_time_maximum_allowed_timestep][Project SettingsのMaximum Allowed Timestep項目][scale=1.0]

=={practice_physics_collision} コリジョン形状の選定

一般的に複雑なコリジョン形状は、当たり判定にかかるコストが高くなります。
そのため物理演算のパフォーマンスを最適化するときに、コライダーの形状は@<em>{可能な限りシンプルな形状を採用する}ことが重要です。

たとえば、人型のキャラクター形状の近似にカプセルコライダーをよく利用しますが、ゲームとして身長が仕様に影響しない場合、スフィアコライダーに置き換えたほうが当たり判定のコストは小さくなります。

形状の選定は、当たり判定の対象や状況によってコストが異なるので一概に言えないですが、判定コストを低い順に並べるとスフィアコライダー、カプセルコライダー、ボックスコライダー、メッシュコライダーと覚えておくとよいでしょう。

=={practice_physics_collision_mesh} 衝突マトリックスとレイヤーの最適化

Physicsには、どのゲームオブジェクトのレイヤー同士が衝突可能かを定義する「衝突マトリックス」という設定があります。
この設定は@<img>{projectsetting_layer_collision_matrix}に示すように、Project SettingsのPhysics > Layer Collision Matrixから変更できます。

//image[projectsetting_layer_collision_matrix][Project SettingsのLayer Collision Matrix項目]

衝突マトリックスは、2つのレイヤーが交わる位置のチェックボックスにチェックが入っていれば、それらのレイヤーは衝突することを示します。

衝突しないレイヤー同士は@<em>{ブロードフェーズと呼ばれるオブジェクトの大雑把な当たり判定を取る前計算からも除外される}ため、この設定を適切に行うのが、@<em>{衝突する必要がないオブジェクト同士の計算を省くのに最も効率的}です。

パフォーマンスを考慮すると、@<em>{物理演算には専用のレイヤーを用意}し、@<em>{衝突する必要のないレイヤー間のチェックボックスはすべてオフ}にすることが望ましいです。

=={practice_physics_raycast} レイキャストの最適化

レイキャストは、飛ばしたレイと衝突したコライダーの衝突情報を取得できる便利な機能ですが、その反面、負荷の原因にもなるので注意が必要です。

==={practice_physics_raycast_type} レイキャストの種類

レイキャストは、線分との衝突判定を取る@<code>{Physics.Raycast}以外にも、@<code>{Physics.SphereCast}などの、その他の形状で判定を取るメソッドが用意されています。

ただし判定を取る形状が複雑になるほど、その負荷が高くなります。パフォーマンスを考慮すると、可能な限り@<code>{Physics.Raycast}の利用のみに留めるのが望ましいです。

==={practice_physics_raycast_parameter} レイキャストのパラメーターの最適化

@<code>{Physics.Raycast}は、レイキャストの始点と向きの2つのパラメーター以外に、パフォーマンスの最適化に関わるパラメーターとして、@<code>{maxDistance}と@<code>{layerMask}があります。

@<code>{maxDistance}はレイキャストの判定を行う最大の長さ、つまりレイの長さを指定します。

このパラメーターを省略すると既定値として@<code>{Mathf.Infinity}が渡され、非常に長いレイで判定を取ろうとします。
このようなレイは、ブロードフェーズに対して悪影響を与えたり、そもそも当たりを取る必要のないオブジェクトと当たり判定を取る可能性があるので、必要以上の距離を指定しないようにします。

また@<code>{layerMask}も、当たりを取る必要のないレイヤーのビットは立てないようにします。

衝突マトリックスと同様に、ビットが立っていないレイヤーとはブロードフェーズからも除外されるため、計算コストを抑えることができます。
このパラメーターを省略すると、既定値として@<code>{Physics.DefaultRaycastLayers}という、Ignore Raycast以外のすべてのレイヤーと衝突する値が指定されるため、こちらも必ず指定します。

==={practice_physics_raycastall} RaycastAllとRaycastNonAlloc

@<code>{Physics.Raycast}では、衝突したコライダーのうち1つの衝突情報が返却されますが、@<code>{Physics.RaycastAll}メソッドを利用すると、複数の衝突情報を取得できます。

@<code>{Physics.RaycastAll}は衝突情報を、@<code>{RaycastHit}構造体の配列を動的に確保して返却します。
そのため、このメソッドを呼び出すたびにGC Allocが発生し、GCによるスパイクの原因になります。

この問題を回避するために、確保済みの配列を引数に渡すと、結果をその配列に書き込んで返却する@<code>{Physics.RaycastNonAlloc}というメソッドが存在します。

パフォーマンスを考慮すると@<code>{FixedUpdate}内では、可能な限りGC Allocを発生させないようにすべきです。

@<list>{how_to_use_raycast_all_nonalloc}に示すように、結果を書き込む配列をクラスのフィールドやプーリングなどの機構で保持し、その配列を@<code>{Physics.RaycastNonAlloc}に渡すことで、配列の初期化時以外のGC.Allocを回避できます。

//listnum[how_to_use_raycast_all_nonalloc][@<code>{Physics.RaycastAllNonAlloc}の利用方法][csharp]{
// レイを飛ばす始点
var origin = transform.origin;
// レイを飛ばす方向
var direction = Vector3.forward;
// レイの長さ
var maxDistance = 3.0f;
// レイが衝突する対象のレイヤー
var layerMask = 1 << LayerMask.NameToLayer("Player");

// レイキャストの衝突結果を格納する配列
// この配列を初期化時に事前に確保したり、
// プールに確保されているものを利用する
// 事前にレイキャストの結果の最大数を決める必要がある
// private const int kMaxResultCount = 100;
// private readonly RaycastHit[] _results = new RaycastHit[kMaxResultCount];

// すべての衝突情報が配列で返ってくる
// 戻り値に衝突個数が返されるので合わせて利用する
var hitCount = Physics.RaycastNonAlloc(
    origin,
    direction,
    _results, 
    layerMask,
    query
);
if (hitCount > 0)
{
    Debug.Log($"{hitCount}人のプレイヤーとの衝突しました");

    // _resultsには配列の0番目から順に衝突情報がはいる
    var firstHit = _results[0];

    // 個数以上のインデックスを指定するとそれは無効な衝突情報なので注意
}
//}

=={practice_physics_collider_and_rigidbody} コライダーとRigidbody

UnityのPhysicsには、スフィアコライダーやメッシュコライダーなどの衝突について扱う@<code>{Collider}コンポーネントと、物理シミュレーションを剛体ベースで行うための@<code>{Rigidbody}コンポーネントがあります。
これらのコンポーネントの組み合わせとその設定によって、3つのコライダーに分類されます。

@<code>{Collider}コンポーネントがアタッチされ、@<code>{Rigidbody}コンポーネントがアタッチされていないオブジェクトは、@<kw>{静的コライダー}（Static Collider）と呼ばれます。

このコライダーは@<em>{常に同じ場所に留まる、動くことのないジオメトリにのみ使用することを前提}に最適化が行われます。

そのため、ゲーム中に静的コライダーの@<em>{有効・無効を切り替えたり、移動やスケーリングを行なうべきではありません}。
これらの処理を行うと、@<em>{内部のデータ構造の変更に伴う再計算が行われ、パフォーマンスを著しく低下させる原因となります}。

@<code>{Collider}コンポーネントと@<code>{Rigidbody}コンポーネントの両方がアタッチされているオブジェクトは、@<kw>{動的コライダー}（Dynamic Collider）と呼ばれます。

このコライダーは、物理エンジンによって他のオブジェクトと衝突できます。また、スクリプトから@<code>{Rigidbody}コンポーネントを操作することによって適用される衝突や力に反応できます。

そのため、物理演算が必要なゲームでは、もっともよく利用されるコライダーになります。

@<code>{Collider}コンポーネントと@<code>{Rigidbody}コンポーネントの両方がアタッチして、かつ@<code>{Rigidbody}の@<code>{isKinematic}プロパティを有効にしたコンポーネントは、@<kw>{キネマティックな動的コライダー}（Kinematic Dynamic Collider）として分類されます。

キネマティックな動的コライダーは、@<code>{Transform}コンポーネントを直接操作することで動かせますが、通常の動的コライダーのように@<code>{Rigidbody}コンポーネントを操作することで衝突や力を加えて動かせません。

@<em>{物理演算の実行を切り替えたい場合}や、ドアなどの@<em>{たまに動かしたいが大半は動かない障害物}などにこのコライダーを利用することで、物理演算を最適化できます。

==={practice_physics_rigidbody_and_sleep} Rigidbodyとスリープ状態

物理エンジンでは最適化の一環として、@<code>{Rigidbody}コンポーネントをアタッチしたオブジェクトが一定時間動かない場合、そのオブジェクトは休止中と判断して、そのオブジェクトの内部状態をスリープ状態に変更します。
スリープ状態に移行すると、外力や衝突などのイベントによって動かない限りは、そのオブジェクトにかかる計算コストが最小限に抑えられます。

そのため、@<code>{Rigidbody}コンポーネントがアタッチされたオブジェクトのうち動く必要のないものは、可能な限りスリープ状態に遷移させておくことで物理演算の計算コストを抑えることができます。

@<code>{Rigidbody}コンポーネントがスリープ状態へ移行すべきかを判定する際に利用されるしきい値は@<img>{projectsetting_sleep_threshold}に示すように、Project SettingsのPhysics内部の@<kw>{Sleep Threshold}で設定できます。
また、個別のオブジェクトに対してしきい値を指定したい場合は、@<code>{Rigidbody.sleepThreshold}プロパティから設定できます。

//image[projectsetting_sleep_threshold][Project SettingsのSleep Threshold項目][scale=1.0]

@<kw>{Sleep Threshold}はスリープ状態に移行する際の質量で正規化された運動エネルギーを表します。

この値を大きくすると、オブジェクトはより早くスリープ状態へ移行するため、計算コストを低く抑えられます。しかし、ゆっくり動いている場合にもスリープ状態へ移行する傾向にあるため、オブジェクトが急停止したように見える場合があります。
この値を小さくすると、上記の現象は発生しにくくなりますが、一方でスリープ状態へは移行しづらくなるため、計算コストを抑えられにくい傾向になります。

@<code>{Rigidbody}がスリープ状態かどうかは、@<code>{Rigidbody.IsSleeping}プロパティで確認できます。シーン上でアクティブな@<code>{Rigidbody}コンポーネントの総数は@<img>{profiler_physics}に示すように、プロファイラーのPhysics項目から確認できます。

//image[profiler_physics][ProfilerのPhysics項目。アクティブな@<code>{Rigidbody}の個数だけでなく、物理エンジン上のそれぞれの要素数が確認できる。][scale=1.0]

また、@<kw>{Physics Debugger}を用いると、シーン上のどのオブジェクトがアクティブ状態なのかを確認できます。

//image[physics_debugger][Physics Debugger。シーン上のオブジェクトが物理エンジン上どのような状態か、色分けされて表示される。][scale=1.0]

=={practice_physics_rigidbody_collision_detecion} 衝突検出の最適化

@<code>{Rigidbody}コンポーネントはCollision Detection項目にて、衝突検出で利用するアルゴリズムが選択できます。

Unity 2020.3時点で、衝突判定には下記の4つがあります。

 * Discrete
 * Continuous
 * Continuous Dynamic
 * Continuous Speculative

これらのアルゴリズムは大きく分けて@<kw>{離散的衝突判定}と@<kw>{連続的衝突判定}の2つに分類されます。@<kw>{Discrete}は離散的衝突判定で、それ以外は連続的衝突判定に属します。

離散的衝突判定は名前のとおり、1シミュレーションごとにオブジェクトが離散的にテレポート移動し、すべてのオブジェクトが移動後に衝突判定を行います。
そのため、とくにオブジェクトが@<em>{高速に移動している場合に、衝突を見逃してすり抜けを起こす可能性}があります。

一方で連続的衝突判定は、移動前後のオブジェクトの衝突を考慮するために、@<em>{高速に動くオブジェクトのすり抜けを防ぎます}。その分、計算コストは離散的衝突判定と比べて高くなります。
パフォーマンスを最適化するには、@<em>{可能な限りDiscreteを選択できるようにゲームの挙動を作ります}。

もし不都合がある場合は、連続的衝突判定を検討します。
@<kw>{Continuous}は@<kw>{動的コライダー}と@<kw>{静的コライダー}の組み合わせにのみ連続的衝突判定が有効になり、@<kw>{Conitnuous Dynamic}は動的コライダー同士でも連続的衝突判定が有効になります。
計算コストはContinuous Dynamicのほうが高くなります。

そのためキャラクターがフィールドを走り回る、つまり動的コライダーと静的コライダーの衝突判定のみを考慮する場合はContinuousを選択し、動くコライダー同士のすり抜けも考慮したい場合はContinuous Dynamicを選択します。

@<kw>{Continuous Speculative}は動的コライダー同士で連続的衝突判定が有効にもかかわらずContinuous Dynamicより計算コストが低いですが、複数のコライダーが密接している箇所で誤衝突してしまうゴースト衝突（@<kw>{Ghost Collision}）と呼ばれる現象が発生するため、導入には注意が必要です。

=={practice_physics_settings} その他プロジェクト設定の最適化

これまでに紹介した設定以外で、とくにパフォーマンスの最適化に影響するプロジェクト設定の項目を紹介します。

==={practice_physics_settings_auto_sync_transform} Physics.autoSyncTransforms

Unity 2018.3より前のバージョンでは、@<code>{Physics.Raycast}などの物理演算に関するAPIを呼び出すたびに、@<code>{Transform}と物理エンジンの位置が自動的に同期されていました。

この処理は比較的重たいので、物理演算のAPIを呼び出したときにスパイクの原因になります。

この問題を回避するために、Unity 2018.3以降、@<code>{Physics.autoSyncTransforms}という設定が追加されています。
この値に@<code>{false}を設定すると、上記で説明した、物理演算のAPIを呼び出したときの@<code>{Transform}の同期処理が行われなくなります。

@<code>{Transform}の同期は物理演算のシミュレーション時の、@<code>{FixedUpdate}が呼び出された後になります。
つまり、コライダーを移動してから、そのコライダーの新しい位置に対してレイキャストを実行しても、レイキャストがコライダーに当たらないことを意味します。

==={practice_physics_settings_reuse_collision_callback} Physics.reuseCollisionCallbacks

Unity 2018.3より前のバージョンでは、@<code>{OnCollisionEnter}などの@<code>{Collider}コンポーネントの衝突判定を受け取るイベントが呼び出されるたびに、引数の@<code>{Collision}インスタンスを新たに生成して渡されるため、GC Allocが発生していました。

この挙動は、イベントの呼び出し頻度によってはゲームのパフォーマンスに悪影響を及ぼすため、2018.3以降では新たに@<code>{Physics.reuseCollisionCallbacks}というプロパティが公開されました。

この値に@<code>{true}を設定すると、イベント呼び出し時に渡される@<code>{Collision}インスタンスを内部で使い回すため、GC Allocが抑えられます。

この設定は2018.3以降ではデフォルト値として@<code>{true}が設定されているため、比較的新しいUnityでプロジェクトを作成した場合には問題ないですが、2018.3より前のバージョンでプロジェクトを作成した場合、この値が@<code>{false}になっている場合があります。
もしこの設定が無効になっている場合は、@<em>{この設定を有効にしたうえでゲームが正常に動作するようコードを修正すべきです}。
