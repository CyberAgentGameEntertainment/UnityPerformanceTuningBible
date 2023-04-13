={practice_third_party} Tuning Practice - Third Party

이 장에서는 유니티로 게임을 개발할 때 자주 사용되는 서드파티 라이브러리를 도입할 때 성능 측면에서 주의해야 할 사항을 소개합니다. 


=={practice_third_party_dotween} DOTween
DOTween @<fn>{dotween}은 스크립트에서 부드러운 애니메이션을 구현할 수 있는 라이브러리입니다. 
예를 들어, 확대/축소하는 애니메이션은 아래 코드와 같이 쉽게 작성할 수 있습니다. 

//listnum[dotween_example][DOTween 사용 예시][csharp]{
public class Example : MonoBehaviour {
    public void Play() {
        DOTween.Sequence()
            .Append(transform.DOScale(Vector3.one * 1.5f, 0.25f))
            .Append(transform.DOScale(Vector3.one, 0.125f));
    }
}
//}

==={practice_third_party_dotween_set_auto_kill} SetAutoKill
@<code>{DOTween.Sequence()} 나 @<code>{transform.DOScale(...)}등 Tween을 생성하는 과정은 기본적으로 메모리 할당을 수반하므로 
자주 재생되는 애니메이션은 인스턴스 재사용을 고려해보자. 

기본적으로 애니메이션이 완료되면 자동으로 Tween이 파기되므로 @<code>{SetAutoKill(false)}에서 이를 억제합니다. 
첫 번째 사용 예제는 다음 코드로 대체할 수 있습니다. 


//listnum[dotween_reuse_tween][Tween 인스턴스 재사용하기][csharp]{
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

@<code>{SetAutoKill(false)} 를 호출한 Tween은 명시적으로 파기하지 않으면 유출될 수 있으므로 주의해야 합니다. 
더 이상 필요하지 않은 시점에 @<code>{Kill()}를 호출하거나, 아래에서 설명할@<kw>{SetLink} 를 사용하는 것이 좋다. 

//listnum[dotween_kill_tween][Tween을 명시적으로 파기하기][csharp]{
    private void OnDestroy() {
        _tween.Kill();
    }
//}

==={practice_third_party_dotween_set_link} SetLink
@<code>{SetAutoKill(false)} 를 호출한 Tween이나 @<code>{SetLoops(-1)}에서 무한 반복 재생되도록 설정한 Tween은 자동으로 파기되지 않으므로, 그 수명을 직접 관리해야 합니다. 
이러한 Tween에는 @<code>{SetLink(gameObject)}에서 관련 GameObject와 연결하여 GameObject가 Destroy되는 동시에 Tween도 파기되도록 하는 것이 좋습니다. 

//listnum[dotween_restart_tween][Tween을 GameObject의 라이프타임에 연결하기.][csharp]{
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
유니티 에디터에서 재생 중입니다,@<kw>{[DOTween]} 라는 이름의 GameObject를 선택하면 Inspector에서 DOTween의 상태와 설정을 확인할 수 있습니다. 

//image[dotween_inspector_game_object][[DOTween\] GameObject][scale=0.5]
//image[dotween_inspector][DOTween Inspector][scale=0.5]

 관련 GameObject가 폐기되었음에도 불구하고 계속 움직이는 Tween이 있는지, 
또한 Pause 상태로 폐기되지 않고 유출되는 Tween이 있는지 등을 조사할 때 유용하게 사용할 수 있다. 

//footnote[dotween][@<href>{http://dotween.demigiant.com/index.php}]


=={practice_third_party_unirx} UniRx
UniRx @<fn>{unirx}는 Unity에 최적화된 Reactive Extensions를 구현한 라이브러리입니다. 
풍부한 오퍼레이터와 Unity용 헬퍼를 통해 복잡한 조건의 이벤트 처리를 간결하게 작성할 수 있습니다. 

==={practice_third_party_unirx_add_to} 구독 취소
UniRx에서는 스트림 게시자( @<code>{IObservable})를 구독( @<code>{Subscribe})하여 해당 메시지에 대한 알림을 받을 수 있습니다. 

이 구독 시 알림을 받기 위한 객체와 메시지를 처리하는 콜백 등의 인스턴스가 생성된다. 
이러한 인스턴스가 @<code>{Subscribe}한 쪽의 수명을 넘어 메모리에 계속 남아 있는 것을 피하기 위해 
알림을 더 이상 받을 필요가 없다면, 기본적으로 @<code>{Subscribe}한 쪽의 책임으로 구독을 해제해야 합니다. 

구독을 해제하는 방법에는 여러 가지가 있지만, 성능을 고려한다면 @<code>{Subscribe}의 반환값 @<code>{IDisposable}을 유지한 채 명시적으로 @<code>{Dispose}하는 것이 좋다. 

//listnum[unirx_unsbscribe][][csharp]{
public class Example : MonoBehaviour {
    private IDisposable _disposable;

    private void Awake() {
        _disposable = Observable.EveryUpdate()
            .Subscribe(_ => {
                //  매 프레임마다 실행하는 처리
            });
    }

    private void OnDestroy() {
        _disposable.Dispose();
    }
}
//}

또한, MonoBehaviour를 상속받은 클래스라면 @<code>{AddTo(this)}를 호출하여 자신이 Destroy되는 시점에 자동으로 해제할 수도 있습니다. 
Destroy를 모니터링하기 위해 내부적으로 @<code>{AddComponent}가 호출되는 오버헤드가 있지만, 설명이 간결한 이쪽을 이용하는 것도 좋습니다. 

//listnum[unirx_monobehaviour_add_to_this][][csharp]{
    private void Awake() {
        Observable.EveryUpdate()
            .Subscribe(_ => {
                //  매 프레임마다 실행하는 처리
            })
            .AddTo(this);
    }
//}

//footnote[unirx][@<href>{https://github.com/neuecc/UniRx}]


=={practice_third_party_unitask} UniTask
UniTask는 Unity에서 고성능 비동기 처리를 구현하기 위한 강력한 라이브러리로, 
값형 기반의 @<code>{UniTask}타입을 통해 제로할당으로 비동기 처리를 할 수 있는 것이 특징이다. 
또한 Unity의 PlayerLoop에 따라 실행 타이밍을 제어할 수 있어 기존 코루틴을 완전히 대체할 수 있습니다. 

==={practice_third_party_unitask_v2} UniTask v2
UniTask는 2020년 6월, 대대적인 버전업인 UniTask v2가 출시됐다. 
UniTask v2는 async 메소드 전체의 제로할당화 등 대폭적인 성능 개선과 
비동기 LINQ 지원, 외부 애셋의 wait 지원 등의 기능 추가가 이루어졌다. @<fn>{unitask_v2}

한편, @<code>{UniTask.Delay(...)}등 Factory에서 반환하는 태스크가 호출 시 실행되거나 
정상적인 @<code>{UniTask}인스턴스에 대한 다중 wait가 금지 @<fn>{unitask_v2_multiple_await}되는 등 
파괴적인 변경 사항도 포함되어 있어 UniTask v1에서 업데이트할 때는 주의가 필요합니다. 
하지만 공격적인 최적화를 통해 성능이 더욱 향상되었기 때문에 기본적으로 UniTask v2를 사용하는 것이 좋습니다. 

//footnote[unitask_v2][@<href>{https://tech.cygames.co.jp/archives/3417/}]
//footnote[unitask_v2_multiple_await][@<code>{UniTask.Preserve}를 사용하여 다중 대기 가능한 UniTask로 변환할 수 있습니다.]

==={practice_third_party_unitask_unitask_tracker} UniTask Tracker
@<kw>{UniTask Tracker} 를 사용하여 대기 중인 UniTask와 생성 시 스택 트레이스를 시각화할 수 있습니다. 
//image[unitask_tracker][UniTask Tracker][scale=0.75]

 예를 들어, 무언가에 충돌하면 @<code>{_hp}가 1씩 줄어드는 MonoBehaviour가 있다고 가정해 봅시다. 

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

이 MonoBehaviour의 @<code>{_hp}이 줄어들기 전에 Destroy되면 
더 이상 @<code>{_hp}이 줄어들지 않기 때문에 @<code>{WaitForDeadAsync}의 반환값 @<code>{UniTask}은 완료할 기회를 잃고 
그대로 계속 대기하게 됩니다. 

이처럼 종료 조건 설정 오류 등으로 인해 @<code>{UniTask}가 누출되고 있는 것은 아닌지 이 툴을 통해 확인해보는 것이 좋다. 

====[column] 태스크 누수 방지

예시한 코드에서 태스크가 유출되는 것은 종료 조건을 만족하기 전에 자신이 Destroy되는 경우를 고려하지 못했기 때문입니다. 

이를 위해서는 간단하게 자신이 Destroy되지 않았는지 확인한다. 
또는 자신에게 @<code>{this.GetCancellationTokenOnDestroy()}에서 얻은 @<code>{CancellationToken}을 @<code>{WaitForDeadAsync}에 전달하여 
Destroy 시 태스크가 취소되도록 하는 등의 대응을 생각해 볼 수 있다. 

//listnum[unitask_fix_leak_task][][csharp]{
    //  자신이 Destroy되었는지 확인하는 패턴
    public UniTask WaitForDeadAsync() {
        return UniTask.WaitUntil(() => this == null || _hp <= 0);
    }

    //  CancellationToken을 전달하는 패턴
    public UniTask WaitForDeadAsync(CancellationToken token) {
        return UniTask.WaitUntil(
            () => _hp <= 0,
            cancellationToken: token);
    }
//}

//listnum[unitask_fix_leak_task_caller][WaitForDeadAsync(CancellationToken) 호출 예제][csharp]{
    Example example = ...
    var token = example.GetCancellationTokenOnDestroy();
    await example.WaitForDeadAsync(token);
//}

Destroy 시 전자의 @<code>{UniTask}은 아무 일 없이 완료되지만, 후자는 @<code>{OperationCanceledException}이 던져집니다. 
어떤 동작이 바람직한지는 상황에 따라 달라지므로 적절한 구현을 선택하면 된다. 
