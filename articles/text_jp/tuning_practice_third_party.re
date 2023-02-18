={practice_third_party} Tuning Practice - Third Party

この章では、Unityでゲームを開発する際によく使用されるサードパーティライブラリを導入する上で、パフォーマンスの観点から気をつけるべきことを紹介します。


=={practice_third_party_dotween} DOTween
DOTween@<fn>{dotween}は、スクリプトで滑らかなアニメーションを実現できるライブラリです。
たとえば、拡大して縮小するアニメーションは以下のコードのように簡単に記述できます。

//listnum[dotween_example][DOTween使用例][csharp]{
public class Example : MonoBehaviour {
    public void Play() {
        DOTween.Sequence()
            .Append(transform.DOScale(Vector3.one * 1.5f, 0.25f))
            .Append(transform.DOScale(Vector3.one, 0.125f));
    }
}
//}

==={practice_third_party_dotween_set_auto_kill} SetAutoKill
@<code>{DOTween.Sequence()}や@<code>{transform.DOScale(...)}など、Tweenを生成する処理は基本的にメモリアロケーションを伴うため、
頻繁に再生されるアニメーションはインスタンスの再利用を検討しましょう。

デフォルトではアニメーション完了時に自動的にTweenが破棄されてしまうので、@<code>{SetAutoKill(false)}でこれを抑制します。
最初の使用例は、次のコードに置き換えることができます。

//embed[latex]{
\clearpage
//}

//listnum[dotween_reuse_tween][Tweenインスタンスを再利用する][csharp]{
    private Tween _tween;

    private void Awake() {
        _tween = DOTween.Sequence()
            .Append(transform.DOScale(Vector3.one * 1.5f, 0.25f))
            .Append(transform.DOScale(Vector3.one, 0.125f))
            .SetAutoKill(false)
            .Pause();
    }

    public void Play() {
        _tween.Restart();
    }
//}

@<code>{SetAutoKill(false)}を呼び出したTweenは、明示的に破棄しなければリークしてしまうため注意が必要です。
不要になったタイミングで@<code>{Kill()}を呼び出すか、後述する@<kw>{SetLink}を使用するとよいでしょう。

//listnum[dotween_kill_tween][Tweenを明示的に破棄する][csharp]{
    private void OnDestroy() {
        _tween.Kill();
    }
//}

==={practice_third_party_dotween_set_link} SetLink
@<code>{SetAutoKill(false)}を呼び出したTweenや、@<code>{SetLoops(-1)}で無限に繰り返し再生されるようにしたTweenは自動的には破棄されなくなるため、そのライフタイムを自前で管理する必要があります。
そのようなTweenには@<code>{SetLink(gameObject)}で関連するGameObjectと紐付け、GameObjectがDestroyされると同時にTweenも破棄されるようにするとよいでしょう。

//listnum[dotween_restart_tween][TweenをGameObjectのライフタイムに紐付ける][csharp]{
    private void Awake() {
        _tween = DOTween.Sequence()
            .Append(transform.DOScale(Vector3.one * 1.5f, 0.25f))
            .Append(transform.DOScale(Vector3.one, 0.125f))
            .SetAutoKill(false)
            .SetLink(gameObject)
            .Pause();
    }
//}

==={practice_third_party_dotween_inspector} DOTween Inspector
Unity Editorで再生中に、@<kw>{[DOTween]}という名前のGameObjectを選択することで、Inspector上からDOTweenの状態や設定を確認できます。

//image[dotween_inspector_game_object][[DOTween\] GameObject][scale=0.5]
//image[dotween_inspector][DOTween Inspector][scale=0.5]

関連するGameObjectが破棄されているにもかかわらず動き続けているTweenがないか、
また、Pause状態で破棄されずにリークしているTweenがないかなど調査するときに役立ちます。

//footnote[dotween][@<href>{http://dotween.demigiant.com/index.php}]


=={practice_third_party_unirx} UniRx
UniRx@<fn>{unirx}はUnityに最適化されたReactive Extensionsを実装したライブラリです。
豊富なオペレーター群やUnity向けのヘルパーにより、複雑な条件のイベントハンドリングを簡潔に記述できます。

==={practice_third_party_unirx_add_to} 購読の解除
UniRxでは、ストリーム発行元の@<code>{IObservable}に対して購読 (@<code>{Subscribe}) することで、そのメッセージの通知を受け取ることができます。

この購読時に、通知を受け取るためのオブジェクトや、メッセージを処理するコールバックなどのインスタンスが生成されます。
これらのインスタンスが@<code>{Subscribe}した側の寿命を超えてメモリに残り続けるのを避けるため、
通知を受け取る必要がなくなった場合は、基本的に@<code>{Subscribe}した側の責任で購読を解除しましょう。

購読を解除する方法はいくつかありますが、パフォーマンスを考慮する場合@<code>{Subscribe}の戻り値の@<code>{IDisposable}を保持して明示的に@<code>{Dispose}するのがよいでしょう。

//listnum[unirx_unsbscribe][][csharp]{
public class Example : MonoBehaviour {
    private IDisposable _disposable;

    private void Awake() {
        _disposable = Observable.EveryUpdate()
            .Subscribe(_ => {
                // 毎フレーム実行する処理
            });
    }

    private void OnDestroy() {
        _disposable.Dispose();
    }
}
//}

またMonoBehaviourを継承したクラスであれば@<code>{AddTo(this)}を呼ぶことで、自身がDestroyされるタイミングで自動的に解除することもできます。
Destroyを監視するため内部的に@<code>{AddComponent}が呼ばれるオーバーヘッドがありますが、記述が簡潔になるこちらを利用するのもよいでしょう。

//listnum[unirx_monobehaviour_add_to_this][][csharp]{
    private void Awake() {
        Observable.EveryUpdate()
            .Subscribe(_ => {
                // 毎フレーム実行する処理
            })
            .AddTo(this);
    }
//}

//footnote[unirx][@<href>{https://github.com/neuecc/UniRx}]


=={practice_third_party_unitask} UniTask
UniTaskはUnityでハイパフォーマンスな非同期処理を実現するための強力なライブラリで、
値型ベースの@<code>{UniTask}型によりゼロアロケーションで非同期処理を行えることが特徴です。
またUnityのPlayerLoopに沿った実行タイミングの制御も可能なため、従来のコルーチンを完全に置き換えることができます。

==={practice_third_party_unitask_v2} UniTask v2
UniTaskは2020年6月、メジャーバージョンアップになるUniTask v2がリリースされました。
UniTask v2はasyncメソッド全体のゼロアロケーション化など大幅な性能改善と、
非同期LINQ対応や外部アセットのawaitサポートなどの機能追加が行われています。@<fn>{unitask_v2}

一方で、@<code>{UniTask.Delay(...)}などFactoryで返すタスクが呼び出し時に起動されたり、
通常の@<code>{UniTask}インスタンスへの複数回awaitが禁止@<fn>{unitask_v2_multiple_await}されるなど、
破壊的な変更も含まれるためUniTask v1からアップデートする際は注意が必要です。
しかしながらアグレッシブな最適化によりさらにパフォーマンスが向上しているため、基本的にはUniTask v2を使用するとよいでしょう。

//footnote[unitask_v2][@<href>{https://tech.cygames.co.jp/archives/3417/}]
//footnote[unitask_v2_multiple_await][@<code>{UniTask.Preserve}を使用することで複数回awaitできるUniTaskに変換することが可能です。]

==={practice_third_party_unitask_unitask_tracker} UniTask Tracker
@<kw>{UniTask Tracker}を使用することで待機中のUniTaskと、その生成時のスタックトレースを可視化できます。
//image[unitask_tracker][UniTask Tracker][scale=0.75]

たとえば、何かに衝突したら@<code>{_hp}が1ずつ減っていくMonoBehaviourがあるとします。

//listnum[unitask_leak_task][][csharp]{
public class Example : MonoBehaviour {
    private int _hp = 10;

    public UniTask WaitForDeadAsync() {
        return UniTask.WaitUntil(() => _hp <= 0);
    }

    private void OnCollisionEnter(Collision collision) {
        _hp -= 1;
    }
}
//}

このMonoBehaviourの@<code>{_hp}が減り切る前にDestroyされた場合、
それ以上@<code>{_hp}が減ることはないため@<code>{WaitForDeadAsync}の戻り値の@<code>{UniTask}は完了する機会を失い、
そのままずっと待機し続けてしまいます。

このように終了条件の設定ミスなどでリークしている@<code>{UniTask}がないか、このツールを用いて確認するとよいでしょう。

====[column] タスクのリークを防ぐ

例示したコードでタスクがリークするのは、終了条件を満たす前に自身がDestroyされるケースを考慮できていないのが原因でした。

これには、シンプルに自身がDestroyされていないかチェックする。
または自身への@<code>{this.GetCancellationTokenOnDestroy()}で得られる@<code>{CancellationToken}を@<code>{WaitForDeadAsync}へ渡し、
Destroy時にタスクがキャンセルされるようにする、といった対応が考えられます。

//listnum[unitask_fix_leak_task][][csharp]{
    // 自身がDestroyされているかチェックするパターン
    public UniTask WaitForDeadAsync() {
        return UniTask.WaitUntil(() => this == null || _hp <= 0);
    }

    // CancellationTokenを渡すパターン
    public UniTask WaitForDeadAsync(CancellationToken token) {
        return UniTask.WaitUntil(
            () => _hp <= 0,
            cancellationToken: token);
    }
//}

//listnum[unitask_fix_leak_task_caller][WaitForDeadAsync(CancellationToken) 呼び出し例][csharp]{
    Example example = ...
    var token = example.GetCancellationTokenOnDestroy();
    await example.WaitForDeadAsync(token);
//}

Destroy時に前者の@<code>{UniTask}は何事もなく完了しますが、後者は@<code>{OperationCanceledException}が投げられます。
どちらの挙動が望ましいかは状況によって異なりますので、適切な実装を選択するとよいでしょう。
