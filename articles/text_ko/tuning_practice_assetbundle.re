={practice_assetbundle} Tuning Practice - AssetBundle

AssetBundle 설정에 문제가 생기면 사용자의 소중한 통신 용량과 저장 공간을 낭비할 뿐만 아니라 
쾌적한 게임 플레이를 방해하는 등 많은 문제가 발생할 수 있습니다. 
이 장에서는 AssetBundle에 대한 설정 및 구현 방침에 대해 설명한다. 

=={practice_assetbundle_granularity} AssetBundle의 세분화

종속성 문제로 인해 AssetBundle을 얼마나 세분화할 것인지에 대해서는 신중하게 고려해야 합니다. 
극단적으로 모든 자산을 하나의 AssetBundle에 넣는 방법과 각각의 자산을 하나씩 AssetBundle로 만드는 방법이 있습니다. 
둘 다 간단한 방법이지만, 전자의 방법은 치명적인 문제가 있다. 
애셋을 추가하거나 하나의 애셋을 업데이트하는 것만으로도 파일 전체를 다시 만들어 배포해야 하기 때문이다. 
애셋의 총량이 GB 단위가 될 경우 업데이트 부하가 매우 커집니다. 

따라서 AssetBundle을 최대한 분할하는 방법을 선택하게 되는데, 너무 세밀하게 분할하면 여러 부분에서 오버헤드가 발생하게 됩니다. 
그래서 기본적으로 다음과 같은 방침으로 AssetBundle화 하는 것을 추천합니다. 

 * 동시에 사용되는 전제 자산은 하나의 AssetBundle로 묶는다.
 * 여러 애셋에서 참조되는 애셋은 별도의 AssetBundle로 만든다.

완벽하게 제어하기는 어렵지만, 프로젝트 내에서 세분화에 대한 규칙을 정하는 것이 좋습니다. 

=={practice_assetbundle_load} AssetBundle 로드 API

AssetBundle에서 애셋을 로드할 때 사용할 수 있는 API는 세 가지가 있습니다. 


 : @<code>{AssetBundle.LoadFromFile}
 스토리지에 존재하는 파일 경로를 지정하여 로드합니다. 
 가장 빠르고 메모리를 절약할 수 있어 보통 이 방법을 사용합니다. 

 : @<code>{AssetBundle.LoadFromMemory}
 메모리에 이미 로드된 AssetBundle 데이터를 지정하여 로드합니다. 
 AssetBundle을 사용하는 동안 매우 큰 데이터를 메모리에 유지해야 하므로 메모리 부하가 매우 큽니다. 
 따라서 보통은 사용하지 않습니다. 

 : @<code>{AssetBundle.LoadFromStream}
 AssetBundle 데이터를 반환하는 @<code>{Stream}을 지정하여 로드합니다. 
 암호화된 AssetBundle의 복호화 과정을 거치면서 로드하는 경우 메모리 부하를 고려하여 이 API를 사용합니다. 
 단, @<code>{Stream}은 탐색이 가능해야 하므로 탐색에 대응하지 못하는 암호화 알고리즘을 사용하지 않도록 주의해야 합니다. 

=={practice_assetbundle_unload} AssetBundle 언로드 전략

AssetBundle이 필요 없어진 시점에서 언로드하지 않으면 메모리를 압박하게 되는데, 
이때 사용하는 API인 @<code>{AssetBundle.Unload(bool unloadAllLoadedObjects)}의 인수 @<code>{unloadAllLoadedObjects}는 
매우 중요하기 때문에 개발 초기에 어떻게 설정할 것인지 결정해야 합니다. 
이 인수가 true인 경우 AssetBundle을 언로드할 때 해당 AssetBundle에서 로드된 모든 애셋도 함께 언로드한다. 
false인 경우 애셋은 언로드되지 않습니다. 

즉, 애셋을 사용하는 동안 AssetBundle도 계속 로드해야 하는 true는 메모리 부하가 높은 반면, 애셋을 확실히 파기할 수 있기 때문에 안전성이 높습니다. 
반면 false의 경우 애셋을 다 사용한 시점에서 AssetBundle을 언로드할 수 있기 때문에 메모리 부하가 낮지만 
다 사용한 애셋을 언로드하는 것을 잊어버리면 메모리 누수로 이어지거나 같은 애셋이 여러 번 메모리 상에 로드될 수 있기 때문에 적절한 메모리 관리가 필요하다, 적절한 메모리 관리가 필요합니다. 
일반적으로 엄격한 메모리 관리는 엄격해지기 때문에, 메모리 부하가 여유가 있다면 @<code>{AssetBundle.Unload(true)}을 추천한다. 

=={practice_assetbundle_loadcount} 동시 로딩되는 AssetBundle 수 최적화하기

@<code>{AssetBundle.Unload(true)} 의 경우 애셋을 사용하는 동안에는 AssetBundle을 언로드할 수 없습니다. 
따라서 실제 현장에서는 AssetBundle의 입도에 따라 100개 이상의 AssetBundle을 동시에 로드하는 상황도 발생할 수 있습니다. 
이 경우 주의해야 할 것은 파일 디스크립터의 제한과 @<code>{PersistentManager.Remapper}의 메모리 사용량입니다. 

파일 기술자란 파일을 읽고 쓸 때 OS 측에서 할당하는 작업용 ID를 말한다. 
하나의 파일을 읽고 쓰기 위해서는 하나의 파일 기술자가 필요하며, 파일 조작이 완료되면 파일 기술자를 해제한다. 
이 파일 기술자를 하나의 프로세스가 가질 수 있는 상한이 정해져 있기 때문에, 이 상한을 초과하는 파일을 동시에 열 수 없습니다. 
"Too many open files"라는 에러가 발생하면 이 상한선에 걸렸다는 의미입니다. 
따라서 AssetBundle이 동시에 로드할 수 있는 개수가 이 제한에 영향을 받으며, Unity 측에서도 어느 정도 파일을 열어두기 때문에 제한에 대한 여유를 가져야 합니다. 
이 제한 값은 OS나 버전 등에 따라 달라지기 때문에 타깃 플랫폼의 값을 미리 조사해야 하는데, 일례로 iOS나 macOS의 경우 제한 값이 256인 버전도 있습니다. 
또한, 설령 상한에 도달하더라도 OS에 따라서는 일시적으로 상한을 상향 조정할 수 있으므로 @<fn>{setrlimit}필요한 경우 구현을 고려해보자. 


//footnote[setrlimit][ 리눅스/유닉스 환경에서는 setrlimit 함수를 사용하여 실행 시 제한값을 변경할 수 있습니다.]

동시에 로드하는 AssetBundle이 많다는 두 번째 문제는 Unity의 @<code>{PersistentManager.Remapper}의 존재입니다. 
PersistentManager는 간단히 말해서 Unity 내부에서 객체와 데이터의 매핑 관계를 관리하는 기능이다. 
즉, 동시에 로드하는 AssetBundle의 수에 따라 메모리를 사용하는 것을 상상할 수 있는데, 문제는 AssetBundle을 해제해도 사용한 메모리 영역은 해제되지 않고 풀링된다는 것입니다. 
이러한 특성 때문에 동시접속자 수에 비례하여 메모리를 압박하게 되므로, 동시접속자 수를 줄이는 것이 중요해집니다. 

따라서 @<code>{AssetBundle.Unload(true)}정책으로 운영할 경우 동시 로드하는 AssetBundle을 최대 150~200개 정도를 기준으로 하고, 
@<code>{AssetBundle.Unload(false)} 정책으로 운영할 경우 최대 150개 이하를 기준으로 하는 것이 좋습니다. 
