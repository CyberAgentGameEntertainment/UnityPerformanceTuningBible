={practice_third_party} Tuning Practice - Third Party

This chapter introduces some things to keep in mind from a performance perspective when implementing third-party libraries that are often used when developing games in Unity. 


=={practice_third_party_dotween} DOTween
DOTween @<fn>{dotween} is a library that allows scripts to create smooth animations. 
For example, an animation that zooms in and out can be easily written as the following code 

//listnum[dotween_example][Example of DOTween usage][csharp]{
public class Example : MonoBehaviour {
    public void Play() {
        DOTween.Sequence()
            .Append(transform.DOScale(Vector3.one * 1.5f, 0.25f))
            .Append(transform.DOScale(Vector3.one, 0.125f));
    }
}
//}

==={practice_third_party_dotween_set_auto_kill} SetAutoKill
Since the process of creating a tween, such as @<code>{DOTween.Sequence()} or @<code>{transform.DOScale(...)}, basically involves memory allocation, 
consider reusing instances for animations that are frequently replayed. 

By default, the tween is automatically discarded when the animation completes, so @<code>{SetAutoKill(false)} suppresses this. 
The first use case can be replaced with the following code 


//listnum[dotween_reuse_tween][Reusing Tween Instances][csharp]{
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

Note that a tween that calls @<code>{SetAutoKill(false)} will leak if it is not explicitly destroyed. 
Call @<code>{Kill()} when it is no longer needed, or use the @<kw>{SetLink} described below. 

//listnum[dotween_kill_tween][Explicitly destroying a tween][csharp]{
    private void OnDestroy() {
        _tween.Kill();
    }
//}

==={practice_third_party_dotween_set_link} SetLink
Tweens that call @<code>{SetAutoKill(false)} or that are made to repeat indefinitely with @<code>{SetLoops(-1)} will not be automatically destroyed, so you will need to manage their lifetime on your own. 
It is recommended that such a tween be associated with an associated GameObject at @<code>{SetLink(gameObject)} so that when the GameObject is Destroyed, the tween is also destroyed. 

//listnum[dotween_restart_tween][Tethering a Tween to the Lifetime of a GameObject][csharp]{
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
During playback in the Unity Editor, a GameObject named @<kw>{[DOTween]} You can check the state and settings of the DOTween from the Inspector by selecting the GameObject named 

//image[dotween_inspector_game_object][[DOTween\] GameObject][scale=0.5]
//image[dotween_inspector][DOTween Inspector][scale=0.5]

 It is also useful to check for tween objects that continue to move even though their associated GameObjects have been discarded 
and for tween objects that are in a Pause state and leaking without being discarded. 

//footnote[dotween][@<href>{http://dotween.demigiant.com/index.php}]


=={practice_third_party_unirx} UniRx
UniRx @<fn>{unirx} is a library implementing Reactive Extensions optimized for Unity. 
With a rich set of operators and helpers for Unity, event handling of complex conditions can be written in a concise manner. 

==={practice_third_party_unirx_add_to} Unsubscribe
UniRx allows you to subscribe ( @<code>{Subscribe}) to the stream publisher @<code>{IObservable} to receive notifications of its messages. 

When subscribing, instances of objects to receive notifications, callbacks to process messages, etc. are created. 
To avoid these instances remaining in memory beyond the lifetime of the @<code>{Subscribe} party, it is basically the @<code>{Subscribe} party's responsibility to unsubscribe when it no longer needs to receive 
notifications. 

There are several ways to unsubscribe, but for performance considerations, it is better to explicitly @<code>{Dispose} retain the @<code>{IDisposable} return value of @<code>{Subscribe}. 

//listnum[unirx_unsbscribe][][csharp]{
public class Example : MonoBehaviour {
    private IDisposable _disposable;

    private void Awake() {
        _disposable = Observable.EveryUpdate()
            .Subscribe(_ => {
                //  Processes to be executed every frame
            });
    }

    private void OnDestroy() {
        _disposable.Dispose();
    }
}
//}

If your class inherits from MonoBehaviour, you can also call @<code>{AddTo(this)} to automatically unsubscribe at the timing of your own Destroy. 
Although there is an overhead of calling @<code>{AddComponent} internally to monitor the Destroy, it is a good idea to use this method, which is simpler to write. 

//listnum[unirx_monobehaviour_add_to_this][][csharp]{
    private void Awake() {
        Observable.EveryUpdate()
            .Subscribe(_ => {
                //  Processing to be executed every frame
            })
            .AddTo(this);
    }
//}

//footnote[unirx][@<href>{https://github.com/neuecc/UniRx}]


=={practice_third_party_unitask} UniTask
UniTask is a powerful library for high-performance asynchronous processing in Unity, featuring zero-allocation asynchronous processing with the 
value-based @<code>{UniTask} type. 
It can also control the execution timing according to Unity's PlayerLoop, thus completely replacing conventional coroutines. 

==={practice_third_party_unitask_v2} UniTask v2
UniTask v2, a major upgrade of UniTask, was released in June 2020. 
UniTask v2 features significant performance improvements, such as zero-allocation of the entire async method, and added features such as 
asynchronous LINQ support and await support for external assets. @<fn>{unitask_v2}

On the other hand, be careful when updating from UniTask v1, as it includes 
destructive changes, such as @<code>{UniTask.Delay(...)} and other tasks returned by Factory being invoked at invocation time, 
prohibiting multiple await to normal @<code>{UniTask} instances, @<fn>{unitask_v2_multiple_await} and so on. 
However, aggressive optimizations have further improved performance, so basically UniTask v2 is the way to go. 

//footnote[unitask_v2][@<href>{https://tech.cygames.co.jp/archives/3417/}]
//footnote[unitask_v2_multiple_await][@<code>{UniTask.Preserve} UniTask v2 can be converted to a UniTask that can be awaited multiple times by using]

==={practice_third_party_unitask_unitask_tracker} UniTask Tracker
@<kw>{UniTask Tracker} can be used to visualize waiting UniTasks and the stack trace of their creation. 
//image[unitask_tracker][UniTask Tracker][scale=0.75]

 For example, suppose you have a MonoBehaviour whose @<code>{_hp} is decremented by 1 when it collides with something. 

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

If @<code>{_hp} of this MonoBehaviour is Destroyed before 
is fully depleted, @<code>{_hp} will not be depleted any further, so @<code>{UniTask}, the return value of @<code>{WaitForDeadAsync}, will lose the opportunity to complete, and 
will continue to wait. 

It is recommended that you use this tool to check for @<code>{UniTask} leaking due to a misconfiguration of termination conditions. 

====[column] Preventing Task Leaks

The reason why the example code leaks a task is that it does not take into account the case where the task itself is destroyed before the termination condition is met. 

To do this, simply check to see if the task itself has been destroyed. 
Or, the @<code>{CancellationToken} obtained by @<code>{this.GetCancellationTokenOnDestroy()} to itself can be passed to @<code>{WaitForDeadAsync} so that the task is canceled when 
is Destroyed. 

//listnum[unitask_fix_leak_task][][csharp]{
    //  Pattern for checking whether the user is Destroyed or not
    public UniTask WaitForDeadAsync() {
        return UniTask.WaitUntil(() => this == null || _hp <= 0);
    }

    //  Pattern for passing a CancellationToken
    public UniTask WaitForDeadAsync(CancellationToken token) {
        return UniTask.WaitUntil(
            () => _hp <= 0,
            cancellationToken: token);
    }
//}

//listnum[unitask_fix_leak_task_caller][Example of WaitForDeadAsync(CancellationToken) call][csharp]{
    Example example = ...
    var token = example.GetCancellationTokenOnDestroy();
    await example.WaitForDeadAsync(token);
//}

At Destroy time, the former @<code>{UniTask} completes without incident, while the latter @<code>{OperationCanceledException} is thrown. 
Which behavior is preferable depends on the situation, and the appropriate implementation should be chosen. 
