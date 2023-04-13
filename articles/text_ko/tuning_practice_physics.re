={practice_physics} Tuning Practice - Physics

이 장에서는 Physics(물리 연산) 최적화에 대해 소개합니다. 

여기서 Physics는 PhysX에 의한 물리 연산을 의미하며, ECS의 Unity Physics는 다루지 않습니다. 

또한 본 장에서는 3D Physics를 주로 다루고 있지만, 2D Physics에서도 참고할 수 있는 부분이 많을 것입니다. 

=={practice_physics_you_need_physics} 물리 연산 켜기/끄기

Unity 표준에서는 물리 연산 관련 컴포넌트가 씬에 하나도 배치되어 있지 않더라도 물리 엔진에 의한 물리 연산 처리는 매 프레임마다 반드시 실행됩니다. 
따라서 게임 내에서 물리 연산이 필요하지 않은 경우 물리 엔진을 꺼두는 것이 좋습니다. 

물리 엔진의 처리는 @<code>{Physics.autoSimulation}에 값을 설정하여 켜고 끌 수 있습니다. 
예를 들어, 게임 내에서만 물리 연산을 사용하고 그 외에는 물리 연산을 사용하지 않는다면, 게임 내에서만 이 값을 @<code>{true}로 설정해두면 됩니다. 

=={practice_physics_fixed_timestep} Fixed Timestep과 Fixed Update의 빈도 최적화

MonoBehaviour의 @<code>{FixedUpdate}은 @<code>{Update}과 달리 고정된 시간으로 실행됩니다. 

물리 엔진은 이전 프레임의 경과 시간에 대해 1프레임 내에 Fixed Update를 여러 번 호출하여 게임 세계의 경과 시간과 물리 엔진 세계의 시간을 일치시킵니다. 
따라서 Fixed Timestep의 값이 작으면 @<em>{Fixed Update가 더 많이 호출되어 부하가 발생} 하게 됩니다. 

이 시간은 @<img>{projectsetting_time_fixed_timestep}에서 보듯이 Project Settings의@<kw>{Fixed Timestep} 에서 설정할 수 있습니다. 이 값의 단위는 초입니다. 기본값은 0.02, 즉 20밀리초가 지정되어 있습니다. 

//image[projectsetting_time_fixed_timestep][Project Settings의 Fixed Timestep 항목][scale=1.0]

 또한, 스크립트 내에서 @<code>{Time.fixedDeltaTime}을 조작하여 변경할 수 있습니다. 

@<kw>{Fixed Timestep} 는 일반적으로 작을수록 물리 연산의 정확도가 높아져 콜리전 누락과 같은 문제가 발생하기 어렵습니다. 
따라서 정확도와 부하를 절충해야 하지만, @<em>{게임 동작에 문제가 생기지 않는 범위 내에서 목표 FPS에 가까운 값을 설정하는 것이} 바람직합니다. 

==={practice_physics_maximum_allowed_timestep} Maximum Allowed Timestep

앞서 설명한 바와 같이, Fixed Update는 이전 프레임으로부터의 경과 시간을 기준으로 여러 번 호출됩니다. 

'특정 프레임에서 렌더링 처리가 많이 진행되었다'는 등의 이유로 이전 프레임의 경과 시간이 길어지면 해당 프레임에서 Fixed Update가 평소보다 더 많이 호출될 수 있습니다. 

예를 들어@<kw>{Fixed Timestep} 가 20밀리초이고 이전 프레임에 200밀리초가 걸렸다면, Fixed Update는 10번 호출될 것입니다. 

즉, 한 프레임에서 처리 실패가 발생하면 다음 프레임의 물리 연산 비용이 높아진다. 
이로 인해 그 프레임도 처리 중단될 위험이 높아져 다음 프레임의 물리 연산이 더 무거워지는 등 부정적인 나선형에 빠지는 현상이 물리 엔진의 세계에서는 잘 알려져 있습니다. 

이 문제를 해결하기 위해 유니티에서는 @<img>{projectsetting_time_maximum_allowed_timestep}에 나와 있는 것처럼 Project Settings에서@<kw>{Maximum Allowed Timestep} 라는 1프레임 내에서 물리 연산이 사용할 수 있는 최대 시간을 설정할 수 있습니다. 
이 값은 기본적으로 0.33초로 설정되어 있지만, 목표 FPS에 가까운 값으로 설정하여 Fixed Update 호출 횟수를 제한하고 프레임 속도를 안정화시키는 것이 좋습니다. 

//image[projectsetting_time_maximum_allowed_timestep][Project Settings의 Maximum Allowed Timestep 항목][scale=1.0]

=={practice_physics_collision} 콜리전 형상 선정

충돌 판정 처리 비용은 콜리전 모양과 그 상황에 따라 달라집니다. 
그 비용을 일률적으로 말할 수는 없지만, 대략적으로 판단 비용이 낮은 순서대로 구면 콜리데이션, 캡슐 콜리데이션, 박스 콜리데이션, 메쉬 콜리데이션 순으로 기억해두면 좋을 것입니다. 

예를 들어, 사람 모양의 캐릭터 형상을 근사화하기 위해 캡슐 콜리더를 많이 사용하지만, 게임상 키가 사양에 영향을 주지 않는다면, 스피어 콜리더로 대체하는 것이 판정 비용이 더 적게 듭니다. 

또한, 이들 형상 중 특히 메쉬 콜리더는 부하가 높다는 점에 유의해야 합니다. 

우선은 구형 콜리더 또는 캡슐 콜리더, 박스 콜리더와 그 조합으로 콜리전을 준비할 수 없는지 검토해 보십시오. 
그래도 불편하다면 메쉬 콜리더를 사용해야 합니다. 

=={practice_physics_collision_mesh} 충돌 매트릭스 및 레이어 최적화

Physics에는 어떤 게임 오브젝트의 레이어끼리 충돌할 수 있는지를 정의하는 '충돌 매트릭스'라는 설정이 있습니다. 
이 설정은 @<img>{projectsetting_layer_collision_matrix}에서 볼 수 있듯이 Project Settings의 Physics > Layer Collision Matrix에서 변경할 수 있습니다. 

//image[projectsetting_layer_collision_matrix][Project Settings의 Layer Collision Matrix 항목]

충돌 매트릭스는 두 개의 레이어가 교차하는 위치의 체크박스에 체크가 되어 있으면 해당 레이어가 충돌한다는 것을 나타냅니다. 

충돌하지 않는 레이어 간에는 브로드 @<em>{페이즈라고 하는 오브젝트의 대략적인 맞물림 판단을 하는 전 계산에서도 제외되므로}, 이 설정을 적절히 설정하는 것이 @<em>{충돌할 필요가 없는 오브젝트 간 계산을 생략하는 데 가장 효율적이다}. 

성능을 고려하면 @<em>{물리 연산은 전용 레이어를 마련하고}, @<em>{충돌할 필요가 없는 레이어 간의 체크박스는 모두} 해제하는 것이 바람직합니다. 

=={practice_physics_raycast} 레이캐스트 최적화

레이캐스트는 날아간 레이와 충돌한 콜리더의 충돌 정보를 얻을 수 있는 편리한 기능이지만, 반대로 부하를 유발하는 원인이 되기도 하므로 주의해야 합니다. 

==={practice_physics_raycast_type} 레이캐스트의 종류

레이캐스트는 선분으로 충돌 판정을 하는 @<code>{Physics.Raycast}외에도 @<code>{Physics.SphereCast}등 다른 모양으로 판정을 하는 메소드가 준비되어 있습니다. 

다만 판정을 내리는 형상이 복잡할수록 그 부하가 높아집니다. 성능을 고려하면 가급적 @<code>{Physics.Raycast}만 사용하는 것이 바람직합니다. 

==={practice_physics_raycast_parameter} 레이캐스트 파라미터 최적화

@<code>{Physics.Raycast} 은 레이캐스트의 시작점과 방향이라는 두 가지 파라미터 외에 성능 최적화와 관련된 파라미터로 @<code>{maxDistance}과 @<code>{layerMask}이 있다. 

@<code>{maxDistance} 은 레이캐스트를 판단하는 최대 길이, 즉 레이의 길이를 지정합니다. 

이 파라미터를 생략하면 @<code>{Mathf.Infinity}가 기본값으로 전달되어 매우 긴 레이로 판단을 시도합니다. 
이러한 레이는 브로드 페이즈에 악영향을 미치거나, 애초에 맞을 필요가 없는 오브젝트와 맞을 수 있으므로 필요 이상의 거리를 지정하지 않는 것이 좋습니다. 

또한 @<code>{layerMask}도 부딪힐 필요가 없는 레이어의 비트를 세우지 않도록 합니다. 

충돌 매트릭스와 마찬가지로 비트가 서 있지 않은 레이어는 브로드 페이즈에서도 제외되므로 계산 비용을 줄일 수 있습니다. 
이 파라미터를 생략하면 기본값으로 @<code>{Physics.DefaultRaycastLayers}이라는 Ignore Raycast를 제외한 모든 레이어와 충돌하는 값이 지정되므로 반드시 이 파라미터도 지정한다. 

==={practice_physics_raycastall} RaycastAll과 RaycastNonAlloc

@<code>{Physics.Raycast} 에서는 충돌한 콜리더 중 하나의 충돌 정보를 반환하지만, @<code>{Physics.RaycastAll}메서드를 이용하면 여러 개의 충돌 정보를 얻을 수 있습니다. 

@<code>{Physics.RaycastAll} 은 충돌 정보를 @<code>{RaycastHit}구조체의 배열을 동적으로 확보하여 반환합니다. 
따라서 이 메서드를 호출할 때마다 GC Alloc이 발생하여 GC에 의한 스파이크의 원인이 됩니다. 

이 문제를 해결하기 위해 이미 확보된 배열을 인수로 전달하면 결과를 해당 배열에 쓰고 반환하는 @<code>{Physics.RaycastNonAlloc}메서드가 존재합니다. 

성능을 고려하면 @<code>{FixedUpdate}내에서는 가급적 GC Alloc을 발생시키지 않아야 합니다. 

@<list>{how_to_use_raycast_all_nonalloc}에서와 같이 결과를 기록할 배열을 클래스의 필드나 풀링 등의 메커니즘으로 보관하고 그 배열을 @<code>{Physics.RaycastNonAlloc}에 전달하면 배열 초기화 시 외에는 GC.Alloc을 피할 수 있습니다. 

//listnum[how_to_use_raycast_all_nonalloc][@<code>{Physics.RaycastAllNonAlloc}의 활용 방법][csharp]{
//  레이를 날릴 시작점
var origin = transform.origin;
//  레이를 날릴 방향
var direction = Vector3.forward;
//  레이의 길이
var maxDistance = 3.0f;
//  레이가 충돌할 대상 레이어
var layerMask = 1 << LayerMask.NameToLayer("Player");

//  레이캐스트의 충돌 결과를 저장하는 배열
//  이 배열을 초기화할 때 미리 확보하거나,
//  풀에 확보되어 있는 것을 이용한다.
//  레이캐스트 결과의 최대 개수를 미리 결정해야 한다.
//  private const int kMaxResultCount = 100; private const int kMaxResultCount = 100;
//  private readonly RaycastHit[] _results = new RaycastHit[kMaxResultCount]; private

//  모든 충돌 정보가 배열로 반환된다.
//  반환값은 충돌 개수
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

    //  _results 배열에는 충돌 정보가 순서대로 저장된다.
    var firstHit = _results[0];

    //  개수를 초과하는 인덱스는 유효하지 않은 정보이므로 주의해야 한다.
}
//}

=={practice_physics_collider_and_rigidbody} 콜리더와 Rigidbody

Unity의 Physics에는 Sphere Collider, Mesh Collider 등의 충돌을 다루는 @<code>{Collider}컴포넌트와 강체 기반의 물리 시뮬레이션을 위한 @<code>{Rigidbody}컴포넌트가 있습니다. 
이 컴포넌트들의 조합과 설정에 따라 세 가지 콜리더로 분류됩니다. 

@<code>{Collider} 컴포넌트가 부착된 오브젝트는 , @<code>{Rigidbody}컴포넌트가 부착되지 않은 오브젝트는 ,@<kw>{정적 콜리더} (Static Collider)라고 합니다. 

이 콜리더는 @<em>{항상 같은 위치에 머무는, 움직이지 않는 지오메트리에만 사용한다는 전제로} 최적화가 이루어집니다. 

따라서 게임 중에 스태틱 콜리더를 @<em>{활성화/비활성화하거나, 이동 및 스케일링을} 해서는 안 됩니다. 
이러한 작업을 수행하면 @<em>{내부 데이터 구조 변경에 따른 재계산이 발생하여 성능이 크게 저하될} 수 있습니다. 

@<code>{Collider} 컴포넌트와 @<code>{Rigidbody}컴포넌트가 모두 부착되어 있는 객체는,@<kw>{동적 콜리더} (Dynamic Collider)라고 한다. 

이 콜리더는 물리 엔진에 의해 다른 오브젝트와 충돌할 수 있습니다. 또한, 스크립트에서 @<code>{Rigidbody}컴포넌트를 조작하여 가해지는 충돌이나 힘에 반응할 수 있다. 

따라서 물리 연산이 필요한 게임에서 가장 많이 사용되는 콜리더가 된다. 

@<code>{Collider} 컴포넌트와 @<code>{Rigidbody}컴포넌트를 모두 부착하고 @<code>{Rigidbody}의 @<code>{isKinematic}속성을 활성화한 컴포넌트,@<kw>{키네마틱 다이내믹 콜리더} (Kinematic Dynamic Collider)로 분류됩니다. 

키네마틱 다이내믹 콜리더는 @<code>{Transform}컴포넌트를 직접 조작하여 움직일 수 있지만, 일반 다이내믹 콜리더처럼 @<code>{Rigidbody}컴포넌트를 조작하여 충돌이나 힘을 가하여 움직일 수 없습니다. 

@<em>{물리 연산의 실행을 전환하고 싶을 때나,} 문과 같이 @<em>{가끔 움직이고 싶지만 대부분 움직이지 않는 장애물} 등에 이 콜리더를 이용하면 물리 연산을 최적화할 수 있습니다. 

==={practice_physics_rigidbody_and_sleep} Rigidbody와 슬립 상태

물리 엔진에서는 최적화의 일환으로 @<code>{Rigidbody}컴포넌트를 부착한 오브젝트가 일정 시간 동안 움직이지 않을 경우, 해당 오브젝트는 휴면 상태라고 판단하여 해당 오브젝트의 내부 상태를 슬립 상태로 변경합니다. 
슬립 상태로 전환되면 외력이나 충돌 등의 이벤트에 의해 움직이지 않는 한 해당 오브젝트에 대한 계산 비용을 최소화할 수 있다. 

따라서 @<code>{Rigidbody}컴포넌트가 부착된 오브젝트 중 움직일 필요가 없는 오브젝트는 가급적 슬립 상태로 전환하여 물리 연산의 계산 비용을 줄일 수 있다. 

@<code>{Rigidbody} 컴포넌트가 슬립 상태로 전환해야 하는지 판단할 때 사용되는 임계값은 @<img>{projectsetting_sleep_threshold}에서 볼 수 있듯이 Project Settings의 Physics 내부의@<kw>{Sleep Threshold} 에서 설정할 수 있습니다. 
또한, 개별 오브젝트에 대해 임계값을 지정하고 싶다면 @<code>{Rigidbody.sleepThreshold}속성에서 설정할 수 있습니다. 

//image[projectsetting_sleep_threshold][Project Settings의 Sleep Threshold 항목][scale=1.0]

 @<kw>{Sleep Threshold} 항목은 슬립 상태로 전환할 때 질량으로 정규화된 운동 에너지를 나타냅니다. 

이 값을 높이면 오브젝트가 더 빨리 슬립 상태로 전환되어 계산 비용을 낮출 수 있습니다. 하지만 천천히 움직이고 있는 경우에도 슬립 상태로 전환하는 경향이 있기 때문에 오브젝트가 갑자기 정지한 것처럼 보일 수 있다. 
이 값을 낮추면 위와 같은 현상이 발생하기 어려워지지만, 한편으로는 절전 상태로 전환하기 어려워지므로 계산 비용을 낮추기 어려워지는 경향이 있습니다. 

@<code>{Rigidbody} 가 슬립 상태인지 여부는 @<code>{Rigidbody.IsSleeping}속성에서 확인할 수 있습니다. 씬에서 활성화된 @<code>{Rigidbody}컴포넌트의 총 개수는 @<img>{profiler_physics}과 같이 프로파일러의 Physics 항목에서 확인할 수 있습니다. 

//image[profiler_physics][Profiler의 Physics 항목. 활성화된 @<code>{Rigidbody}의 개수뿐만 아니라 물리 엔진에 있는 각 요소의 개수를 확인할 수 있다.][scale=1.0]

 또한,@<kw>{Physics Debugger} 를 사용하면 씬에서 어떤 오브젝트가 활성 상태인지 확인할 수 있다. 

//image[physics_debugger][Physics Debugger, 씬의 오브젝트가 물리엔진 상에서 어떤 상태인지 색상으로 구분되어 표시된다.][scale=1.0]

=={practice_physics_rigidbody_collision_detecion} 충돌 감지 최적화

@<code>{Rigidbody} 컴포넌트는 Collision Detection 항목에서 충돌 감지에 사용할 알고리즘을 선택할 수 있다. 

Unity 2020.3 기준, 충돌 판정은 아래 4가지가 있다. 

 * Discrete
 * Continuous
 * Continuous Dynamic
 * Continuous Speculative

이러한 알고리즘은 크게 두 가지로 나뉜다. @<kw>{이산적 충돌 판단} 과@<kw>{연속적 충돌 판단} 두 가지로 나뉩니다. @<kw>{Discrete} 는 이산적 충돌 판정, 나머지는 연속적 충돌 판정에 속합니다. 

이산적 충돌 판정은 이름 그대로 시뮬레이션마다 오브젝트가 이산적으로 텔레포트 이동하고, 모든 오브젝트가 이동한 후 충돌 판정을 한다. 
따라서 특히 오브젝트가 @<em>{빠르게 이동하는 경우, 충돌을 놓} 쳐서 빠져나갈 가능성이 있습니다. 

반면 연속 충돌 판정은 이동 전후의 오브젝트 충돌을 고려하기 때문에 @<em>{고속으로 움직이는 오브젝트의 미끄러짐을 방지한다}. 그만큼 이산적 충돌 판단에 비해 계산 비용이 높아진다. 
성능을 최적화하려면 가능한 @<em>{한 Discrete를 선택할 수 있도록 게임 동작을 만들어야 한다}. 

만약 불편하다면 연속적인 충돌 판정을 고려합니다. 
@<kw>{Continuous} 은@<kw>{동적 콜리더} 와@<kw>{정적 콜리더} 의 조합에 대해서만 연속적인 충돌 판단이 활성화된다,@<kw>{Conitnuous Dynamic} 은 동적 콜리더끼리도 연속적 충돌 판정이 활성화됩니다. 
계산 비용은 Continuous Dynamic이 더 높습니다. 

따라서 캐릭터가 필드를 돌아다니는, 즉 동적 콜리더와 정적 콜리더의 충돌 판정만 고려한다면 Continuous를 선택하고, 움직이는 콜리더 간의 스플라이스도 고려하고 싶다면 Continuous Dynamic을 선택하면 됩니다. 

@<kw>{Continuous Speculative} 은 동적 콜리더 간 연속적인 충돌 판단이 유효함에도 불구하고 Continuous Dynamic보다 계산 비용이 낮지만, 여러 콜리더가 근접한 곳에서 오충돌하는 고스트 콜리더(@<kw>{Ghost Collision} )라는 현상이 발생하므로 도입 시 주의가 필요합니다. 

=={practice_physics_settings} 기타 프로젝트 설정 최적화

지금까지 소개한 설정 외에 특히 성능 최적화에 영향을 미치는 프로젝트 설정 항목을 소개합니다. 

==={practice_physics_settings_auto_sync_transform} Physics.autoSyncTransforms

Unity 2018.3 이전 버전에서는 @<code>{Physics.Raycast}등의 물리 연산 관련 API를 호출할 때마다 @<code>{Transform}과 물리 엔진의 위치가 자동으로 동기화되었습니다. 

이 처리는 비교적 무거운 작업이기 때문에 물리 연산 API를 호출할 때 스파이크가 발생할 수 있습니다. 

이 문제를 해결하기 위해 Unity 2018.3부터 @<code>{Physics.autoSyncTransforms}라는 설정이 추가되었습니다. 
이 값에 @<code>{false}를 설정하면 위에서 설명한 물리 연산 API 호출 시 @<code>{Transform}의 동기화 처리가 이루어지지 않습니다. 

@<code>{Transform} 의 동기화는 물리 연산 시뮬레이션 시 @<code>{FixedUpdate}이 호출된 이후에 이루어집니다. 
즉, 콜리더를 이동한 후 해당 콜리더의 새로운 위치에 대해 레이캐스트를 실행해도 레이캐스트가 콜리더에 닿지 않는다는 의미입니다. 

==={practice_physics_settings_reuse_collision_callback} Physics.reuseCollisionCallbacks

Unity 2018.3 이전 버전에서는 @<code>{OnCollisionEnter}등 @<code>{Collider}컴포넌트의 충돌 판정을 받는 이벤트가 호출될 때마다 인수의 @<code>{Collision}인스턴스를 새로 생성하여 전달하기 때문에 GC 할당(GC Alloc)이 발생했습니다. 

이 동작은 이벤트 호출 빈도에 따라 게임 성능에 악영향을 끼칠 수 있어, 2018.3 이후부터는 @<code>{Physics.reuseCollisionCallbacks}라는 속성이 새롭게 공개되었습니다. 

이 값에 @<code>{true}를 설정하면 이벤트 호출 시 전달되는 @<code>{Collision}인스턴스를 내부에서 순환 사용하므로 GC Alloc을 억제할 수 있습니다. 

이 설정은 2018.3 이상에서는 기본값으로 @<code>{true}가 설정되어 있기 때문에 비교적 새로운 Unity에서 프로젝트를 생성한 경우에는 문제가 없지만, 2018.3 이전 버전에서 프로젝트를 생성한 경우에는 이 값이 @<code>{false}으로 설정되어 있을 수 있습니다. 
만약 이 설정이 비활성화되어 있다면, @<em>{이 설정을 활성화한 후 게임이 정상적으로 작동하도록 코드를 수정해야 합니다}. 
