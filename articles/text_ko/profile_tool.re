={tool} 프로파일링 도구
프로파일링 도구는 데이터를 수집하고 분석하여 병목현상을 파악하거나 성능 지표를 결정하는 데 사용됩니다. 
이러한 도구는 유니티 엔진에서 제공하는 것만으로도 여러 가지가 있다. 
이 외에도 Xcode나 Android Studio와 같은 네이티브 호환 도구, GPU에 특화된 RenderDoc 등 다양한 도구가 존재한다. 
따라서 각 툴의 특징을 이해하고 적절히 취사선택하는 것이 중요할 것이다. 
이 장에서는 각 도구에 대한 소개와 프로파일링 방법에 대해 알아보고, 적절하게 사용할 수 있는 상태를 목표로 한다. 

==={before_profiling} 측정 시 주의사항
Unity는 에디터에서 애플리케이션을 실행할 수 있기 때문에 '실기'와 '에디터' 모두에서 측정이 가능합니다. 
계측을 할 때 각 환경의 특징을 파악할 필요가 있습니다. 

에디터에서 측정하는 경우, 시행착오를 빠르게 할 수 있다는 것이 가장 큰 장점입니다. 
하지만 에디터 자체의 처리 부하와 에디터가 사용하는 메모리 영역도 측정되기 때문에 측정 결과에 노이즈가 많이 발생한다. 
또한 실기와는 사양이 전혀 다르기 때문에 병목현상을 파악하기 어려워 결과가 다르게 나올 수도 있습니다. 

따라서 프로파일링은 기본적으로 @<em>{실기 계측을 권장하고} 있습니다. 
단, '두 환경 모두에서 발생하는 경우'에 한해 작업 비용이 저렴한 에디터만으로 작업을 완료하는 것이 효율적입니다. 
대부분 두 환경 모두에서 재현되지만, 드물게 두 환경 중 하나에서만 재현되는 경우가 있습니다. 
따라서 먼저 실기기로 현상을 확인합니다. 다음으로 에디터에서도 재현을 확인한 후, 에디터에서 수정하는 흐름이 좋습니다. 
물론 마지막에 실기에서의 수정 확인은 반드시 해야 합니다. 

=={tool_unity_profiler} Unity Profiler
Unity Profiler는 Unity 에디터에 내장된 프로파일링 툴입니다. 
이 툴은 1프레임마다 정보를 수집할 수 있습니다. 
측정할 수 있는 항목은 매우 다양하며, 각 항목을 프로파일러 모듈이라고 부르며, Unity 2020 버전에서는 14개의 항목이 존재합니다. 
이 모듈은 계속 업데이트되고 있으며, Unity 2021.2에서는 Asset 관련 모듈과 File I/O 관련 모듈이 새롭게 추가되었다. 
이처럼 Unity Profiler는 다양한 모듈이 있기 때문에 대략적인 성능의 모습을 파악하기 위한 최적의 도구라고 할 수 있다. 
모듈 목록은 @<img>{profiler_module_list}에서 확인할 수 있습니다. 

//image[profiler_module_list][프로파일러 모듈 목록]

이 모듈들은 프로파일러에 표시 여부를 설정할 수 있습니다. 
단, 표시하지 않은 모듈은 측정되지 않습니다. 반대로 모든 모듈을 표시하면 그만큼 에디터에 부하가 걸립니다. 
//image[profiler_modules][Profiler Modules 표시/숨기기 기능]

또한, 프로파일러 도구 전체에 공통적으로 적용되는 유용한 기능을 소개합니다. 
//image[profiler_often_use_command][Profiler의 기능 설명]

@<img>{profiler_often_use_command}에서 '①'은 각 모듈이 측정하고 있는 항목이 나열되어 있습니다. 
이 항목을 클릭하면 오른쪽 타임라인에 표시/숨기기를 전환할 수 있습니다. 
필요한 항목만 표시하면 뷰가 보기 편해집니다. 
또한 이 항목은 드래그하여 정렬할 수 있으며, 오른쪽의 그래프는 정렬된 순서대로 표시됩니다. 
'②'는 측정한 데이터를 저장, 불러오는 기능입니다. 필요하면 측정 결과를 저장해두면 좋습니다. 
저장되는 데이터는 프로파일러에 표시하고 있는 데이터만 저장됩니다. 

이 책에서는 @<img>{profiler_module_list}중 사용 빈도가 높은 CPU Usage와 Memory 모듈에 대해 설명합니다. 

==={how_to_use_unity_profiler} 측정 방법
여기서는 Unity Profiler에서 실기를 이용한 측정 방법에 대해서 설명합니다. 
빌드 전 작업과 애플리케이션 실행 후 작업 두 가지로 나누어 설명합니다. 
참고로 에디터에서 측정하는 방법은 실행 중에 측정 버튼을 누르기만 하면 되므로 자세한 설명은 생략합니다. 

===={how_to_use_unity_profiler_for_build} 빌드 전 작업
빌드 전 작업은@<kw>{Development Build} 를 활성화하는 것입니다. 
이 설정이 활성화되면 프로파일러와 연결이 가능해진다. 

또한, 보다 상세한 측정을 위한@<kw>{Deep Profile} 라는 옵션도 있습니다. 
이 옵션을 활성화하면 모든 함수 호출의 처리 시간이 기록되어 병목 현상을 일으키는 함수를 쉽게 파악할 수 있습니다. 
단점은 측정 자체에 매우 큰 오버헤드가 발생하기 때문에 동작이 무거워지고, 메모리도 많이 소모된다는 점이다. 
따라서 처리 시간이 매우 오래 걸리는 것처럼 보이지만, 일반 프로파일에서는 그다지 오래 걸리지 않을 수도 있으니 주의해야 합니다. 
기본적으로 일반 프로파일로는 정보가 부족한 경우에만 사용합니다. 
//info{
Deep Profile은 대규모 프로젝트 등으로 메모리를 많이 사용하는 경우, 메모리 부족으로 측정이 불가능할 수도 있습니다. 
이 경우 @<hd>{cpu_module_unity_profiler}절의 @<hd>{cpu_module_sampler}을 참고하여 자체적으로 측정 처리를 추가할 수밖에 없습니다. 
//}

이러한 설정 방법은 스크립트에서 명시적으로 지정하는 방법과 GUI에서 하는 방법이 있습니다. 
먼저 스크립트에서 설정하는 방법을 소개합니다. 

//listnum[setting_development_build_by_script][스크립트에서 Development Build를 설정하는 방법][csharp]{
BuildPlayerOptions buildPlayerOptions = new BuildPlayerOptions();
/* 씬과 빌드 타겟 설정은 생략 */

buildPlayerOptions.options |= BuildOptions.Development;
//  Deep Profile 모드를 활성화하고 싶은 경우에만 추가
buildPlayerOptions.options |= BuildOptions.EnableDeepProfilingSupport;

BuildReport report = BuildPipeline.BuildPlayer(buildPlayerOptions);
//}
@<list>{setting_development_build_by_script}에서 중요한 점은 @<code>{BuildOptions.Development}를 지정하는 것입니다. 

다음으로 GUI에서 설정하는 경우, Build Settings에서 @<img>{development_build_setting}와 같이 Development Build에 체크하여 빌드합니다. 
//image[development_build_setting][Build Settings]

===={how_to_use_unity_profiler_for_connect} 애플리케이션 실행 후 수행해야 할 작업 
애플리케이션 실행 후 Unity Profiler와 연결하는 방법은 '원격 연결'과 '유선(USB) 연결' 두 가지가 있습니다. 
원격 연결은 유선 연결에 비해 환경의 제약이 많아 프로파일링이 원하는 대로 되지 않을 수 있습니다. 
예를 들어, 동일한 와이파이 네트워크에 연결해야 한다거나, 안드로이드만 모바일 통신을 비활성화해야 한다거나, 다른 포트를 해제해야 한다거나 하는 등의 제약이 있습니다. 
따라서 이 절에서는 절차가 간단하고 안정적으로 프로파일링할 수 있는 유선 연결에 대해 설명한다. 
만약 원격 연결을 원한다면 공식 문서를 참고하여 시도해 보시기 바랍니다. 

먼저 iOS의 경우 프로파일러에 접속하는 방법은 다음과 같습니다. 

 1. Build Settings에서 Target Platform을 iOS로 변경
 2. 디바이스를 PC에 연결하고 Development Build 애플리케이션 실행 
 3. Unity Profiler에서 연결 대상 단말 선택 ( @<img>{connect_profiler_by_iphone} ) 
 4. Record 시작

//image[connect_profiler_by_iphone][연결 대상 단말기 선택]

//info{
계측을 하는 Unity 에디터는 빌드한 프로젝트가 아니어도 상관없습니다. 
계측을 위해 신규 프로젝트를 생성하면 동작도 가볍기 때문에 추천합니다. 
//}

다음으로 안드로이드의 경우, iOS에 비해 절차가 조금 더 늘어납니다. 

 1. Build Settings에서 Target Platform을 Android로 변경해 둔다
 2. 기기를 PC에 연결하고 Development Build 애플리케이션을 실행한다
 3. @<code>{adb forward} 명령을 입력한다. (명령어에 대한 자세한 설명은 아래 참조)
 4. Unity Profiler에서 접속할 단말기를 선택한다
 5. Record를 시작한다

@<code>{adb forward} 명령어에는 애플리케이션의 Package Name이 필요하다. 
예를 들어 Package Name이 'jp.co.sample.app'이라면 다음과 같이 입력합니다. 
//listnum[adb_tunnel_command][adb forward 명령어]{
adb forward tcp:34999 localabstract:Unity-jp.co.sample.app
//}

adb가 인식되지 않는 경우, adb의 경로를 설정해야 합니다. 
설정 방법에 대해서는 웹상에 설명이 많이 나와 있으므로 생략합니다. 

간단한 문제 해결로 연결이 되지 않는 경우 아래를 확인해보시기 바랍니다. 

 * 두 장치 공통
 ** 실행한 애플리케이션의 오른쪽 하단에 Development Build라는 표기가 있는지
 * 안드로이드의 경우
 ** 단말기의 USB 디버깅이 활성화되어 있는지
 ** @<code>{adb forward}명령어에 입력한 패키지 이름이 올바른지 여부
 ** @<code>{adb devices}명령어를 입력했을 때 장치가 정상적으로 인식되는지 여부

참고로 Build And Run에서 직접 애플리케이션을 실행한 경우, 위에서 언급한 @<code>{adb forward}명령이 내부적으로 수행됩니다. 
따라서 측정 시 명령어 입력은 필요하지 않습니다. 

====[column]{autoconnect_profiler} Autoconnect Profiler

빌드 설정에는 Autoconnect Profiler라는 옵션이 있습니다. 
이 옵션은 앱 실행 시 에디터의 프로파일러에 자동 연결하기 위한 기능입니다. 
따라서 프로파일에 필수적으로 설정해야 하는 것은 아닙니다. 원격 프로파일링 시에도 마찬가지입니다. 
WebGL만 이 옵션이 없으면 프로파일링을 할 수 없지만, 모바일에 관해서는 그다지 유용한 옵션은 아닐 것입니다. 

좀 더 자세히 설명하면, 이 옵션을 활성화하면 빌드 시 에디터의 IP 주소가 바이너리에 기록되어 시작 시 해당 주소로 접속을 시도하게 됩니다. 
전용 빌드 머신 등에서 빌드하는 경우, 해당 머신에서 프로파일을 설정하지 않는 한 이 옵션은 필요하지 않습니다. 
오히려 애플리케이션 시작 시 자동 연결이 타임아웃(8초 정도)되는 대기 시간이 늘어날 뿐입니다. 

스크립트에서는 @<code>{BuildOptions.ConnectWithProfiler}라는 옵션 이름으로 되어 있어 필수인 것처럼 착각하기 쉬우므로 주의해야 합니다. 

====[/column]

==={cpu_module_unity_profiler} CPU Usage
CPU Usage는 @<img>{profiler_cpu_usage}와 같이 표시됩니다. 
//image[profiler_cpu_usage][CPU Usage 모듈 (Timeline 표시)]

이 모듈을 확인하는 방법은 크게 두 가지로 나뉜다. 

 * Hierarchy (Raw Hierarchy)
 * Timeline

먼저 Hierarchy 뷰와 관련하여 표시 내용과 사용법에 대해 설명합니다. 

===={cpu_module_hierarchy} 1. Hierarchy 뷰
Hierarchy 뷰는 @<img>{profiler_hierarchy_view}와 같이 표시됩니다. 
//image[profiler_hierarchy_view][Hierarchy 뷰]

이 뷰의 특징은 측정 결과가 목록 형태로 나열되어 있고, 헤더의 항목별로 정렬이 가능하다는 점입니다. 
조사를 할 때 목록에서 관심 있는 항목을 열어서 병목현상을 파악할 수 있습니다. 
단, 표시되는 정보는 '선택된 스레드'에서 소요된 시간을 표시하는 것이다. 
예를 들어, Job System이나 멀티스레드 렌더링을 사용하는 경우, 다른 스레드에서의 처리 시간은 포함되지 않습니다. 
확인하려면 @<img>{profiler_hieracrhy_thread}와 같이 스레드를 선택하면 확인할 수 있습니다. 
//image[profiler_hieracrhy_thread][스레드 선택]


다음으로 헤더의 항목에 대해 설명하겠습니다. 
//table[hierarchy_header][Hierarchy 헤더 정보]{
헤더 이름	설명
-------------------- 
Overview	샘플 이름.
Total	이 함수를 처리하는 데 걸린 총 시간. (% 표시)
Self	이 함수 자체의 처리 시간. @<br>{}하위 함수의 시간은 포함되지 않습니다. (% 표시)
Calls	1프레임에서 호출된 횟수.
GC Alloc	이 함수로 할당된 스크립트의 힙 메모리.
Time ms	Total을 ms 단위로 표시한 것.
Self ms	Self를 ms 단위로 표시한 것.
//}

Calls는 여러 번의 함수 호출을 하나의 항목으로 묶어서 보기 편한 뷰가 됩니다. 
하지만 모두 동일한 처리 시간인지, 아니면 그 중 하나만 처리 시간이 긴지는 알 수 없습니다. 
이런 경우에@<kw>{Raw Hierarchy 뷰} 를 이용합니다. 
Raw Hierarchy 뷰는 반드시 Calls가 1로 고정된다는 점에서 Hierarchy 뷰와 다릅니다. 
@<img>{profile_raw_hierarchy}에서는 Raw Hierarchy 뷰로 설정했기 때문에 동일한 함수 호출이 여러 개 표시되어 있습니다. 
//image[profile_raw_hierarchy][Raw Hierarchy 뷰]

지금까지의 내용을 정리하면 Hierarchy 뷰는 다음과 같은 용도로 사용합니다. 

 * 처리 지연되는 병목 현상 파악, 최적화 (Time ms, Self ms)
 * GC 할당 파악, 최적화 (GC Allocation)

이러한 작업을 할 때는 원하는 항목별로 내림차순으로 정렬한 후 확인하는 것을 권장합니다. 

//info{
항목을 열어보면 깊은 계층 구조로 되어 있는 경우가 많다. 
이때 Mac의 경우 Option 키(Windows의 경우 Alt 키)를 누른 상태에서 열면 모든 계층이 열립니다. 
반대로 키를 누른 상태에서 항목을 닫으면 해당 계층 이하가 닫힌 상태가 됩니다. 
//}

===={cpu_module_timeline} 2. 타임라인 보기
또 다른 확인 방법인 타임라인 뷰는 다음과 같이 표시됩니다. 
//image[profiler_timeline][Timeline 뷰]

타임라인 뷰에서는 Hierarchy 뷰의 항목이 박스로 시각화되어 있기 때문에 한눈에 전체를 볼 때 부하가 걸리는 부분을 직관적으로 파악할 수 있습니다. 
그리고 마우스 조작이 가능하기 때문에 계층 구조가 깊은 경우에도 드래그만으로 전체 내용을 파악할 수 있습니다. 
또한, 타임라인에서는 굳이 스레드를 전환할 필요 없이 모두 표시되어 있습니다. 
따라서 각 스레드에서 언제 어떤 처리가 이루어지고 있는지도 쉽게 파악할 수 있습니다. 
이러한 특징으로 인해 주로 다음과 같은 용도로 사용합니다. 

 * 처리 부하를 전체적으로 조망하고 싶다.
 * 각 스레드의 처리 부하를 파악하여 튜닝하고 싶다.

Timeline이 적합하지 않은 것은 무거운 처리 순서를 파악하기 위한 정렬 작업이나 할당 총량을 확인하는 경우 등입니다. 
따라서 할당량 튜닝을 위해서는 Hierarchy 뷰가 더 적합할 것입니다. 

===={cpu_module_sampler} 보충: Sampler에 대하여
함수 단위로 처리 시간을 측정하는 경우 두 가지 방법이 있습니다. 
하나는 앞서 소개한 Deep Profile 모드입니다. 다른 하나는 스크립트에 직접 삽입하는 방법입니다. 

직접 삽입하는 경우 다음과 같이 작성합니다. 
//listnum[profile_begin_samle][Begin/EndSample을 이용한 방법]{
using UnityEngine.Profiling;
/* ... 생략...*/
private void TestMethod()
{
	for (int i = 0; i < 10000; i++)
	{
		Debug.Log("Test");
	}
}

private void OnClickedButton()
{
	Profiler.BeginSample("Test Method")
	TestMethod();
	Profiler.EndSample()
}
//}

임베디드 샘플은 Hierarchy 뷰, Timeline 뷰 모두에 표시됩니다. 
//image[sampler_display_example][Sampler 표시]

한 가지 더 주목할 만한 특징이 있습니다. 프로파일링 코드는 Development Build가 아닌 경우 호출 측이 비활성화되어 오버헤드가 0이 된다는 점입니다. 
향후 처리 부하가 늘어날 것 같은 부분은 미리미리 준비해 두는 것도 좋은 방법일 수 있습니다. 

BeginSample 메서드는 정적 함수이기 때문에 쉽게 사용할 수 있지만, 비슷한 기능을 가진 CustomSampler라는 것도 있습니다. 
이는 Unity 2017 이후에 추가된 것으로, BeginSample보다 측정 오버헤드가 적어 보다 정확한 시간을 측정할 수 있다는 특징이 있습니다. 


//listnum[profile_custom_sampler][CustomSampler를 이용한 방법]{
using UnityEngine.Profiling;
/* ... 생략...*/
private CustomSampler _samplerTest = CustomSampler.Create("Test");

private void TestMethod()
{
	for (int i = 0; i < 10000; i++)
	{
		Debug.Log("Test");
	}
}

private void OnClickedButton()
{
	_samplerTest.Begin();
	TestMethod();
	_samplerTest.End();
}
//}

차이점으로는 사전에 인스턴스를 생성해 두어야 한다는 점입니다. 
CustomSampler는 측정 후 스크립트 내에서 측정 시간을 얻을 수 있는 것도 특징입니다. 
좀 더 정확도가 필요하거나, 처리 시간에 따라 경고 등을 주고 싶다면 CustomSampler를 사용하는 것이 좋습니다. 

==={memory_module_unity_prfoiler} Memory
메모리 모듈은 @<img>{profiler_memory}와 같이 표시됩니다. 
//image[profiler_memory][Memory 모듈]

이 모듈을 확인하는 방법은 다음 두 가지가 있습니다. 

 * Simple 보기
 * Detailed 뷰

먼저 Simple 뷰에 대해 표시 내용과 사용법에 대해 설명합니다. 

===={memory_module_simple_view} 1. Simple 뷰
Simple 뷰는 @<img>{profiler_memory_simple}와 같이 표시됩니다. 
//image[profiler_memory_simple][Simple 뷰]

뷰에 기재된 항목에 대해 설명합니다. 


 : Total Used Memory
 Unity가 할당받은(사용 중인) 총 메모리 양입니다. 

 : Total Reserved Memory
 Unity가 현재 확보한 메모리의 총량입니다. 
 OS 측에서 미리 일정량의 연속 메모리 영역을 풀로 확보해 두었다가 필요한 시점에 할당합니다. 
 풀 영역이 부족해지면 다시 OS 측에 요청하여 확장합니다. 

 : System Used Memory
 애플리케이션에서 사용하고 있는 메모리의 총량입니다. 
 이 항목에서는 Total Reserved에서 측정되지 않은 항목(플러그인 등)도 측정됩니다. 
 단, 이 항목도 모든 메모리 할당을 추적할 수는 없습니다. 
 정확한 파악을 위해서는 Xcode와 같은 네이티브 호환 프로파일링 툴을 사용해야 한다. 

@<img>{profiler_memory_simple}의 Total Used Memory 오른쪽에 기재된 항목의 의미는 다음과 같습니다. 

//table[simple_view_total_detail][Simple View 용어 설명]{
용어명	설명
-------------------- 
GC	힙 영역에서 사용하고 있는 메모리 양. @<br>{} GC Alloc 등의 요인으로 증가한다.
Gfx	Texture, Shader, Mesh 등에서 확보하고 있는 메모리 양입니다.
Audio	오디오 재생에 사용되는 메모리 양입니다.
Video	비디오 재생을 위해 사용하고 있는 메모리 용량입니다.
Profiler	프로파일링에 사용되는 메모리의 양입니다.
//}
용어에 대한 보충 설명으로, Unity 2019.2부터 'Mono'는 'GC'로, 'FMOD'는 'Audio'로 표기 변경되었습니다. 

@<img>{profiler_memory_simple}에는 그 외에도 다음과 같은 애셋의 사용 개수와 메모리 확보량도 기재되어 있습니다. 

 * Texture
 * Mesh
 * Material
 * 애니메이션 클립
 * Audio Clip

또한, 다음과 같은 오브젝트 수와 GC Allocation에 대한 정보도 있습니다. 


 : Asset Count
 로드된 애셋의 총 개수입니다. 

 : Game Object Count
 씬에 존재하는 게임 오브젝트 수입니다. 

 : Scene Object Count
 씬에 존재하는 컴포넌트, 게임 오브젝트 등의 총 개수입니다. 

 : Object Count
 애플리케이션에서 생성하고 로드한 모든 오브젝트의 총 개수입니다. 
 이 값이 증가하면 어떤 오브젝트가 유출되었을 가능성이 높습니다. 

 : GC Allocation in Frame
 1프레임 내에서 Allocation이 발생한 횟수와 총량입니다. 

 마지막으로 이 정보들을 통해 Simple 뷰의 사용 사례를 정리해 보겠습니다. 

 * 힙 영역 및 Reserved의 확장 타이밍 파악 및 모니터링
 * 각종 에셋 및 오브젝트 누수 여부 확인
 * GC Allocation 모니터링

//info{
Unity 2021 이후의 Simple view는 UI가 크게 개선되어 표시 항목이 보기 편해졌습니다. 
내용 자체에는 큰 변화가 없으므로 여기서 소개한 지식은 그대로 사용할 수 있습니다. 
다만 명칭이 일부 변경되었으니 참고하세요. 
예를 들어 GC가 Managed Heap이라는 이름으로 변경되었습니다. 
//image[profile_memory_2021][2021년 이후의 심플 뷰]
//}

===={memory_module_detailed_view}2. Detailed 뷰 
Detailed 뷰는 @<img>{profiler_memory_detailed}와 같이 표시됩니다. 
//image[profiler_memory_detailed][Detailed 뷰]
이 표시 결과는 'Take Sample' 버튼을 눌러 해당 시점의 메모리 스냅샷을 얻을 수 있다. 
Simple 뷰와 달리 실시간 업데이트가 되지 않으므로, 표시를 갱신하려면 Take Sample을 다시 해야 합니다. 

@<img>{profiler_memory_detailed}의 오른쪽에 'Referenced By'라는 항목이 있습니다. 
현재 선택 중인 오브젝트를 참조하고 있는 오브젝트가 표시됩니다. 
만약 유출된 애셋이 존재한다면, 오브젝트의 참조처 정보가 해결의 실마리가 될 수 있습니다. 
이 표시는 'Gather object references'를 활성화한 경우에만 표시됩니다. 
이 기능을 활성화하면 Take Sample 시 처리 시간이 길어지지만, 기본적으로 활성화해 두는 것이 좋습니다. 
//info{
Referenced By에 @<code>{ManagedStaticReferences()}라는 표기를 볼 수 있습니다. 
이것은 어떤 정적 오브젝트에서 참조되었다는 의미입니다. 
프로젝트에 익숙한 분이라면 이 정보만으로도 어느 정도 감을 잡을 수 있을 것입니다. 
그렇지 않다면 @<hd>{tool_heap_explorer}를 사용하는 것이 좋습니다. 
//}

Detailed 뷰의 헤더 항목은 본 그대로의 의미이므로 여기서는 설명하지 않겠습니다. 
조작 방법은 @<hd>{cpu_module_unity_profiler}의 @<hd>{cpu_module_hierarchy}과 동일하며, 
헤더별 정렬 기능이 있고, 항목이 계층적으로 표시되어 있습니다. 
여기서는 Name 항목에 표시되는 최상위 노드에 대해 설명합니다. 

//table[memory_detailed_top_node][Detailed의 최상위 노드]{
이름	설명
-------------------- 
Assets	씬에 포함되지 않은 로드한 애셋.
Not Saved	코드에 의해 런타임에 생성된 애셋입니다. @<br>{}예를 들어, new Materiala()와 같이 코드에서 생성된 오브젝트입니다.
Scene Memory	로드한 씬에 포함된 애셋입니다.
기타	위의 항목이 아닌 오브젝트. @<br>{}유니티가 다양한 시스템에서 사용하는 항목에 할당합니다.
//}

상위 노드 중 기타에 기재된 항목은 평소에는 익숙하지 않은 것들이 많을 것입니다. 
그 중에서도 알아두면 좋은 것들을 소개합니다. 



 : System.ExecutableAndDlls
 바이너리, DLL 등에 사용된 할당량을 나타냅니다. 
 플랫폼이나 단말기에 따라 획득하지 못할 수 있으며, 이 경우 0B로 처리됩니다. 
 공통 프레임워크를 사용하는 다른 애플리케이션과 공유하는 경우도 있으며, 
 프로젝트에 대한 메모리 사용량은 기재된 수치만큼 크지 않을 수 있습니다. 
 이 항목을 줄이는 데 급급하기보다는 Asset을 개선하는 것이 좋습니다. 
 줄이는 방법은 DLL이나 불필요한 Script를 줄이는 것이 효과적입니다. 가장 간단한 방법은 Stripping Level을 변경하는 것입니다. 
 단, 실행 시 타입이나 메소드가 누락될 위험이 있으므로 디버깅을 꼼꼼히 해야 합니다. 

 : SerializedFile
 AssetBundle 내 객체의 테이블과 타입 정보인 Type Tree 등의 메타 정보를 나타냅니다. 
 AssetBundle.Unload(true or false)로 해제할 수 있습니다. 
 가장 효율적인 방법은 애셋 로드 후 이 메타 정보만 해제하는 Unload(false)를 하는 것이지만, 
 해제 타이밍이나 리소스 참조 관리를 철저히 하지 않으면 리소스를 이중으로 로드하거나 쉽게 메모리 누수를 유발할 수 있으므로 주의해야 한다. 

 : PersistentManager.Remapper
 Remapper는 메모리 풀을 이용하며, 메모리 풀이 부족하면 두 배로 확장되어 줄어들지 않습니다. 
 너무 많이 확장되지 않도록 주의해야 합니다. 특히 AssetBundle을 많이 로드하면 매핑 영역이 부족하여 확장됩니다. 
 따라서 동시에 로드되는 파일 수를 줄이기 위해 불필요한 AssetBundle을 언로드하는 것이 좋습니다. 
 또한, 하나의 AssetBundle에 불필요한 애셋이 많이 포함되어 있다면, 그 자리에서 분할하는 것이 좋습니다. 

 마지막으로 지금까지 소개한 내용을 바탕으로 Detailed 뷰의 활용 사례를 정리해 보겠습니다. 

 * 특정 타이밍의 메모리 상세 파악 및 튜닝
 ** 불필요한 애셋이 있는지, 예상치 못한 애셋은 없는지
 * 메모리 누수 조사

=={tool_profile_analyzer} Profile Analyzer
Profile Analyzer는 Profiler의 CPU Usage에서 얻은 데이터를 보다 세밀하게 분석하기 위한 툴입니다. 
Unity Profiler에서는 1프레임 단위의 데이터만 볼 수 있었던 반면, Profile Analyzer는 지정한 프레임 구간을 기준으로 평균값, 중앙값, 최소값, 최대값을 얻을 수 있습니다. 
프레임별로 편차가 있는 데이터를 적절히 다룰 수 있기 때문에 최적화를 할 때 개선 효과를 보다 명확하게 보여줄 수 있을 것입니다. 
또한, CPU Usage에서 할 수 없었던 측정 데이터 간 비교 기능도 있어 최적화 결과를 비교, 시각화하는 데 매우 유용한 툴입니다. 

//image[profile_analyzer][Profile Analyzer]

==={profile_analyzer_install} 도입 방법
이 도구는 Package Manager에서 설치가 가능합니다. 
Unity에서 공식적으로 지원하므로 Packages를 Unity Registry로 변경하고 검색창에 'Profile'을 입력하면 나타납니다. 
설치 후 'Window -> Analysis -> Profile Analyzer'에서 툴을 실행할 수 있습니다. 

//image[profile_analyzer_install][PackageManager에서 설치하기]

==={profile_analyzer_top} 사용 방법
Profile Analyzer는 실행 직후에는 @<img>{profile_analyzer_top_view}와 같이 표시됩니다. 
//image[profile_analyzer_top_view][실행 직후]

기능으로는 'Single'과 'Compare' 두 가지 모드가 있습니다. 
Single 모드는 하나의 계측 데이터를 분석할 때 사용하고, Compare 모드는 두 개의 계측 데이터를 비교할 때 이용한다. 

'Pull Data'는 Unity Profiler에서 측정한 데이터를 분석하게 하여 결과를 표시할 수 있다. 
미리 Unity Profiler에서 계측을 해 두어야 한다. 

'Save', 'Load'는 Profile Analyzer에서 분석한 데이터를 저장하고 불러올 수 있습니다. 
물론 Unity Profiler의 데이터만 보관해 두어도 문제가 없습니다. 
이 경우 Unity Profiler에서 데이터를 로드하고, Profile Analyzer에서 매번 Pull Data를 수행해야 합니다. 
그 절차가 번거롭다면 전용 데이터로 저장해 두는 것이 좋습니다. 

==={profile_analyzer_display_single} 분석 결과 (Single 모드)
분석 결과 화면은 다음과 같은 구성으로 되어 있습니다. 
여기서 마커라는 단어가 나오는데, 이는 처리의 명칭(메소드명)을 의미합니다. 

 * 분석 구간 설정 화면
 * 표시 항목의 필터 입력 화면
 * 마커의 중앙값 Top10
 * 마커 분석 결과
 * 프레임 요약
 * 스레드 요약
 * 선택 중 마커 요약

각각의 표시 화면에 대해 살펴보자. 

===={prfoile_analyzer_frame_segment} 1. 분석 구간 설정 화면
각 프레임의 처리 시간이 표시되며, 초기 상태에서는 모든 프레임이 선택된 상태입니다. 
프레임 구간을 @<img>{profile_anlyzer_frame_segment}과 같이 드래그하여 변경할 수 있으므로, 필요에 따라 조정할 수 있습니다. 
//image[profile_anlyzer_frame_segment][프레임 구간 지정]

===={prfoile_analyzer_filters} 2. 필터 입력 화면
필터 입력 화면은 분석 결과를 필터링할 수 있는 화면입니다. 
//image[profile_analyzer_filter][필터 입력 화면]
각 항목은 다음과 같습니다. 

//table[analyzer_filter_explain][Filters 항목]{
항목명	설명
-------------------- 
Name Filter	검색하고자 하는 프로세스 이름으로 필터링합니다.
Exclude Filter	검색에서 제외할 프로세스 이름으로 필터링합니다.
Thread	선택 대상 스레드가 분석 결과에 표시됩니다. @<br>{}다른 스레드의 정보가 필요하다면 추가할 수 있습니다.
Depth Slice	CPU Usage에서 Hierarchy의 계층 수입니다. @<br>{}예를 들어 Depth가 3인 경우, 세 번째 계층이 표시됩니다.
Analysis Type	Total과 Self를 전환할 수 있습니다. @<br>{} CPU Usage에서 소개한 헤더 항목과 동일한 내용입니다.
Units	시간 표시를 밀리초 또는 마이크로초로 변경할 수 있습니다.
Marker Columns	분석 결과의 헤더 표시를 변경할 수 있습니다.
//}

Depth Slice가 All인 경우, PlayerLoop이라는 최상위 노드가 표시되거나 동일한 처리의 계층적 차이가 표시되어 보기 어려울 때가 있습니다. 
이 경우 Depth를 2~3 정도로 고정하고, 렌더링, 애니메이션, 물리 연산 등의 서브시스템이 표시될 정도로 설정하는 것이 좋습니다. 

===={prfoile_analyzer_median_top10} 3. 마커의 중앙값 Top10
이 화면은 각 마커의 처리 시간을 중앙값으로 정렬하여 상위 10개 마커만 표시한 화면입니다. 
상위 10개 마커가 각각 얼마나 많은 처리 시간을 차지하고 있는지 한눈에 알 수 있습니다. 
//image[profile_analyzer_median_top10][마커의 중앙값 Top10]

===={prfoile_analyzer_marker} 4. 마커 분석 결과
각 마커의 분석 결과를 확인할 수 있습니다. 
Marker Name에 기재된 처리명과 Median(중앙값), Mean(평균값) 값을 통해 개선해야 할 처리를 분석하는 것이 좋습니다. 
헤더 항목에 마우스 포인터를 올리면 해당 항목에 대한 설명이 표시되므로, 내용을 잘 모르는 경우 참고해 보시기 바랍니다. 

//image[profile_analyzer_marker_detail][각 처리의 분석 결과]

====[column]{median_mean} 평균값과 중앙값

평균값은 모든 값을 더한 후 데이터 개수로 나눈 값입니다. 
반면, 중앙값은 정렬된 데이터의 중앙에 위치한 값을 말합니다. 데이터가 짝수일 경우, 중앙에 있는 앞뒤의 데이터에서 평균값을 구합니다. 

평균값은 극단적으로 값이 떨어져 있는 데이터가 있으면 영향을 받기 쉬운 특성이 있습니다. 
스파이크가 자주 발생하거나 샘플링 수가 충분하지 않은 경우, 중앙값을 참고하는 것이 더 나은 경우가 있을 수 있습니다. 

@<img>{mean_median}는 중앙값과 평균값의 차이가 큰 예입니다. 
//image[mean_median][중앙값과 평균값]
이 두 값의 특징을 알고 데이터를 분석해 봅시다. 

====[/column]

===={prfoile_analyzer_frame_summary} 5. 프레임 요약
이 화면에는 측정한 데이터의 프레임 통계 정보가 표시됩니다. 
//image[profile_analyzer_frame_summary][프레임 요약 화면][scale=0.5]

 분석 중인 프레임의 구간 정보 및 값의 변동 정도를 박스그래프(Boxplot)와 히스토그램을 이용하여 표시합니다. 
박스그래프는 사분위수(quartile number)를 이해해야 합니다. 
사분위수란 데이터를 정렬한 상태에서 @<table>{interquatile}와 같이 값을 정한 것이다. 

//table[interquatile][사분위수]{
이름	설명
-------------------- 
최소값 (Min)	최소값
1사분위수 (Lower Quartile)	최소값에서 25% 위치에 있는 값
중앙값 (Median)	최소값에서 50% 위치에 있는 값
상위 사분위수 (Upper Quartile)	최소값에서 75% 위치에 있는 값
최대값 (Max)	최대값
//}

25%에서 75% 구간을 박스로 묶은 것을 박스형 그래프라고 한다. 
//image[box_plot][박스비어드 그래프]

히스토그램은 가로축에 처리 시간, 세로축에 데이터 수를 나타내며, 이 역시 데이터의 분포를 파악하는 데 유용합니다. 
프레임 요약에서는 커서를 가져가면 구간과 프레임 수를 확인할 수 있습니다. 
//image[histogram][히스토그램]

이러한 도표의 보기 방법을 이해한 후, 특징을 분석하면 좋습니다. 

===={prfoile_analyzer_thread_summary} 6. 스레드 요약
이 화면은 선택한 스레드의 통계를 보여주는 화면입니다. 
각 스레드에 대한 박스히스토그램을 볼 수 있습니다. 
//image[profile_analyzer_thread_summary][프레임 요약 화면]

===={prfoile_analyzer_marker_summary} 7. 선택 중인 마커 요약
@<hd>{prfoile_analyzer_marker} 화면에서 선택 중인 마커에 대한 요약입니다. 
선택 중인 마커에 대한 처리 시간이 박스 수염 그림과 히스토그램으로 표현됩니다. 
//image[profile_analyzer_marker_summary][선택 중인 마커 요약]

==={profile_analyzer_display_compare} 분석 결과 (Compare 모드)
이 모드에서는 두 개의 데이터를 비교할 수 있습니다. 
상하 각 데이터별로 분석할 구간을 설정할 수 있습니다. 
//image[profile_analyzer_compare_frame_segment][비교 데이터 설정]

화면 사용법은 Single 모드와 크게 다르지 않지만, @<img>{profile_analyzer_comapre_marker}과 같이 'Left'와 'Right'라는 단어가 다양한 화면으로 나타납니다. 
//image[profile_analyzer_comapre_marker][마커 비교]
이것은 어느 쪽의 데이터인지 표시하는 것으로, @<img>{profile_analyzer_compare_frame_segment}에 표시된 색상과 일치합니다. 
Left는 상단의 데이터, Right는 하단의 데이터입니다. 
이 모드를 이용하면 튜닝 결과의 좋고 나쁨을 쉽게 분석할 수 있을 것이다. 

=={tool_frame_debugger} Frame Debugger
Frame Debugger는 현재 표시되고 있는 화면이 어떤 흐름으로 렌더링되고 있는지 분석할 수 있는 도구입니다. 
이 도구는 기본적으로 에디터에 설치되어 있으며 'Window -> Analysis -> Frame Debugger'에서 열 수 있습니다. 

에디터와 실제 기기에서 사용할 수 있습니다. 
실제 기기에서 사용할 때는 Unity Profiler와 마찬가지로 'Development Build'로 빌드된 바이너리가 필요합니다. 
애플리케이션을 실행하고 장치 연결 대상을 선택한 후 'Enable'을 누르면 드로잉 명령어가 표시됩니다. 
//image[frame_debugger_connect][FrameDebugger 연결 화면]

==={frame_debugger_enable} 분석 화면
'Enable'을 누르면 다음과 같은 화면이 나타납니다. 
//image[frame_debugger_enable][FrameDebugger 캡처]

왼쪽 프레임은 1개의 항목이 하나의 드로잉 명령어이며, 위에서부터 순서대로 발행된 명령어들이 나열되어 있습니다. 
오른쪽 프레임은 드로잉 명령어에 대한 상세 정보입니다. 어떤 Shader가 어떤 프로퍼티와 함께 처리되었는지 알 수 있습니다. 
이 화면을 보면서 다음과 같은 것을 염두에 두고 분석을 진행합니다. 

 * 불필요한 명령어가 없는지
 * 드로잉 배칭이 제대로 작동하고 있는지, 정리할 수 없는지
 * 드로잉 대상의 해상도가 너무 높지 않은지
 * 의도하지 않은 Shader를 사용하고 있지 않은지

==={frame_debugger_detailed} 상세 화면
전 절에서 소개한 @<img>{frame_debugger_enable}의 오른쪽 테두리 내용을 자세히 설명합니다. 

==== 조작 패널
먼저 상단 부분에 있는 조작 패널에 대해 알아보겠습니다. 
//image[frame_debugger_header_tool][상단 조작 패널]
RT0이라고 표시된 부분은 여러 개의 렌더링 타겟이 존재할 때 변경할 수 있습니다. 
특히 멀티 렌더 타겟을 이용하고 있을 때 각각의 렌더링 상태를 확인할 때 유용하게 사용할 수 있습니다. 
Channels는 RGBA 모두 또는 하나의 채널만 표시할 것인지 변경할 수 있습니다. 
Levels는 슬라이더로 되어 있으며, 렌더링 결과의 밝기를 조절할 수 있습니다. 
예를 들어 앰비언트나 간접조명 등 렌더링 결과가 어두울 때 밝기를 조절하여 보기 좋게 만드는 데 유용합니다. 

==== 그리기 개요
이 영역에서는 드로잉 대상의 해상도와 형식에 대한 정보를 확인할 수 있습니다. 분명 높은 해상도의 렌더링 대상이 있다면 바로 알아차릴 수 있습니다. 
이외에도 사용 중인 Shader 이름, Cull 등의 Pass 설정, 사용 중인 키워드 등을 알 수 있습니다. 
하단에 기재된 'Why this~'라는 문장은 왜 드로잉을 일괄 처리하지 못했는지를 알려주는 문구입니다. 
@<img>{frame_debugger_shader_syntax}의 경우, 첫 번째 드로잉 콜을 선택했기 때문에 배치할 수 없다고 나와 있습니다. 
이렇게 세세하게 원인이 기재되어 있기 때문에, 배치에 대한 고민을 하고 싶을 때 이 정보를 참고하여 조정하면 좋을 것 같습니다. 
//image[frame_debugger_shader_syntax][중간 부분의 도면 개요]

==== Shader 속성 상세 정보
이 영역은 그리는 Shader의 프로퍼티 정보가 기재되어 있습니다. 
디버깅 시 유용합니다. 
//image[frame_debugger_shader_property][하단의 Shader 속성 상세 정보]

//info{
프로퍼티 정보에 표시된 Texture2D가 어떤 상태인지 자세히 확인하고 싶을 때가 있습니다. 
이때 Mac의 경우 Command 키(Windows의 경우 Control 키)를 누른 상태에서 이미지를 클릭하면 확대할 수 있습니다. 
//image[frame_debugger_preview][Texture2D 미리보기 확대하기]
//}

=={tool_memory_profiler} Memory Profiler
Memory Profiler는 Preview Package로 유니티가 공식적으로 제공하는 툴입니다. 
Unity Profiler의 Memory 모듈과 비교했을 때, 주로 다음과 같은 장점이 있습니다. 

 * 캡처한 데이터가 스크린샷과 함께 로컬에 저장된다.
 * 각 카테고리의 메모리 점유량이 시각화되어 알기 쉽다.
 * 데이터 비교 가능

Memory Profiler는 v0.4 이후와 그 이전 버전에서 UI가 많이 바뀌었다. 
이 책에서는 작성 시점의 최신 버전인 v0.5를 사용합니다. 
또한 v0.4 이상 버전에서 모든 기능을 이용하려면 Unity 2020.3.12f1 이상의 버전이 필요합니다. 
또한 v0.4와 v0.5는 언뜻 보면 비슷해 보이지만, v0.5에서 기능이 대폭 업데이트되었습니다. 
특히 오브젝트의 참조 관계 추적이 매우 쉬워졌기 때문에 기본적으로 v0.5 이상 사용을 권장한다. 

//image[memory_profiler_top][Memory Profiler]

==={memory_profiler_install} 도입 방법
Unity 2020에서 Preview 버전의 패키지는 'Project Settings -> Package Manager'에서 'Enable Preview Packages'를 활성화해야 합니다. 
//image[enable_preview_package][Preview Packages 활성화]

이후 Unity Registry의 Package에서 Memory Profiler를 설치합니다. 
설치 후 'Window -> Analysis -> Memory Profiler'에서 툴을 실행할 수 있습니다. 
//image[memory_profiler_install][PackageManager에서 설치]

또한 Unity 2021 이상에서는 패키지 추가 방법이 변경되었습니다. 
추가하려면 'Add Package by Name'을 누르고 'com.unity.memoryprofiler'를 입력해야 합니다. 
//image[memory_profiler_add_package_2021][2021년 이후 추가 방법]

==={memory_profiler_control} 사용 방법
Memory Profiler는 크게 4가지 요소로 구성되어 있습니다. 

 * 툴바
 * Snapshot Panel
 * 측정 결과
 * Detail Panel

각 영역에 대한 설명을 제공합니다. 

===={memory_profiler_tool_bar} 1. 툴바
//image[memory_profiler_tool_bar][툴바 영역]
@<img>{memory_profiler_tool_bar}은 Header의 캡쳐를 나타냅니다. 
의 버튼은 측정 대상을 선택할 수 있습니다. 
의 버튼은 누르면 해당 시점의 메모리를 측정합니다. 옵션으로 측정 대상을 Native Object로만 설정하거나, 스크린샷을 비활성화할 수 있습니다. 
기본 기본 설정으로 문제 없을 것입니다. 
3번 버튼은 측정된 데이터를 불러올 수 있는 버튼입니다. 
그 외 'Snapshot Panel'과 'Detail Panel'을 누르면 좌우에 있는 정보 패널을 표시/숨길 수 있습니다. 
Tree Map의 표시만 보고 싶을 때는 숨기는 것이 좋습니다. 
또한 '? 를 누르면 공식 문서를 열 수 있습니다. 

측정과 관련하여 한 가지 주의할 점이 있습니다. 
계측 시 필요한 메모리가 새로 확보되고 이후 해제되지 않는다는 점입니다. 
하지만 무한정 늘어나는 것은 아니며, 몇 번 측정하면 어느 정도 안정화됩니다. 측정 시 메모리 확보량은 프로젝트의 복잡도에 따라 달라질 수 있습니다. 
이 전제를 모르면, 부풀어 오른 메모리 사용량을 보고 누수가 있을 거라고 착각할 수 있으니 주의해야 합니다. 

===={memory_profiler_snapshot_panel} 2. Snapshot Panel
//image[memory_profiler_snapshot_panel_single][Snapshot Panel (Single)]

Snapshot Panel에는 측정한 데이터가 표시되며, 어떤 데이터를 볼지 선택할 수 있다. 
이 데이터는 앱 시작부터 종료까지를 1세션으로 간주하여 세션별로 정리되어 있습니다. 
또한, 측정 데이터를 마우스 오른쪽 버튼으로 클릭하면 삭제 및 이름 변경이 가능합니다. 

상단에 'Single Snapshot', 'Compare Snapshots'가 있다. 
Compare Snapshots를 누르면 측정 데이터를 비교하는 UI로 표시가 바뀝니다. 
//image[memory_profiler_snapshot_panel_compare][Snapshot Panel (Comapre)]

'A'는 Single Snapshot에서 선택한 데이터, 'B'는 Comapre Snapshots에서 선택한 데이터입니다. 
교체 버튼을 눌러 'A'와 'B'를 교체할 수 있으므로, 굳이 Single Snapshot 화면으로 돌아가지 않고도 전환할 수 있습니다. 

===={memory_profiler_snapshot_result} 3. 측정 결과
측정 결과는 'Summary', 'Objects and Allocations', 'Fragmentation'의 세 가지 탭이 있습니다. 
본 절에서는 자주 사용하는 Summary를 설명하고, 나머지는 보충 설명으로 간략히 다룰 것이다. 
Summary의 화면 상단은 Memory Usage Overview라는 영역으로, 현재 메모리의 개요가 표시되어 있다. 
항목을 누르면 Detail Panel에 설명이 표시되므로, 모르는 항목은 확인하면 좋습니다. 
//image[memory_profiler_usage_overview][Memory Usage Overview]

다음으로 화면 중앙은 Tree Map이라고 불리며, 오브젝트의 카테고리별로 메모리 사용량을 그래픽으로 표시합니다. 
각 카테고리를 선택하면 카테고리 내 오브젝트를 확인할 수 있습니다. 
@<img>{memory_profiler_tree_map}여기서는 Texture2D 카테고리를 선택한 상태입니다. 
//image[memory_profiler_tree_map][Tree Map]

그리고 화면 하단은 Tree Map Table이라고 합니다. 여기에는 오브젝트 목록이 테이블 형태로 나열되어 있습니다. 
표시 항목은 Tree Map Table의 헤더를 눌러 그룹화, 정렬, 필터링을 할 수 있습니다. 
//image[memory_profiler_tree_map_table_option][헤더 조작]

특히 Type을 그룹화하면 분석이 쉬워지므로 적극적으로 활용해보자. 
//image[memory_profiler_tree_map_table_group][Type으로 그룹화하기]

또한 Tree Map에서 카테고리를 선택하면 해당 카테고리 내의 객체만 표시하는 필터가 자동으로 설정됩니다. 
//image[memory_profiler_tree_map_table][필터 자동 설정]

마지막으로 Compare Snapshots를 사용했을 때의 UI 변화에 대해 설명한다. 
Memory Usage Overview에는 각 오브젝트의 차이점이 표시됩니다. 
//image[memory_profiler_compare_snaphost_overview][Compare Snapshots의 Memory Usage Overview]

또한 Tree Map Table에는 Header에 Diff라는 항목이 추가됩니다. 
Diff에는 다음과 같은 유형이 있습니다. 

//table[compare_tree_map_view][Tree Map Table (Compare)]{
Diff	설명
-------------------- 
Same	A, B 동일 객체
Not in A (Deleted)	A에 있지만 B에는 없는 객체
Not in B (New)	A에는 없지만 B에 있는 객체
//}

이러한 정보를 보면서 메모리가 증감하고 있는지 확인할 수 있습니다. 

===={memory_profiler_snapshot_detail} 4. Detail Panel
이 패널은 선택한 오브젝트의 참조 관계를 추적하고 싶을 때 사용합니다. 
이 Referenced By를 확인하면 참조를 계속 잡고 있는 원인을 파악할 수 있습니다. 
//image[memory_profiler_detail_references][Referenced By]

하단의 Selection Details라는 부분에는 오브젝트에 대한 자세한 정보를 확인할 수 있습니다. 
그 중 'Help'라는 항목에는 해제하는 방법에 대한 조언 등이 나와 있습니다. 
어떻게 해야 할지 모르겠다면 한 번 읽어보시면 좋을 것 같습니다. 
//image[memory_profiler_detail_selection][Selection Details]


===={memory_profiler_others} 추가: Summary 이외의 측정 결과에 대해 

'Objects and Allocations'는 Summary와 달리 할당 등 좀 더 자세한 내용을 테이블 형태로 확인할 수 있습니다. 
//image[memory_profiler_objects_allocations][Table View 지정]
'Fragmentation'은 가상 메모리의 상황을 시각화해 주기 때문에 조각화 조사에 활용할 수 있다. 
메모리 주소 등 직관적이지 않은 정보가 많기 때문에 이용 난이도가 높을 수 있습니다. 
//image[memory_profiler_fragmentation][Fragmentation]

또한 Memory Profiler v0.6부터 'Memory Breakdowns'라는 새로운 기능이 추가되었습니다. 
Unity 2022.1 이상부터 필수적으로 적용되며, TreeMap을 목록으로 볼 수 있고, Unity Subsystems와 같은 오브젝트 정보도 확인할 수 있게 되었다. 
이 외에도 중복 가능성이 있는 오브젝트를 확인할 수 있게 되었습니다. 
//image[memory_profiler_memory_break_down][Memory Breakdowns]

=={tool_heap_explorer} Heap Explorer
Heap Explorer는 개인 개발자 Peter77( @<fn>{author_of_heap_explore})의 오픈소스 툴이다. 
이 도구는 Memory Profiler와 마찬가지로 메모리 조사를 할 때 많이 사용하는 도구입니다. 
Memory Profiler는 0.4 이전 버전에서는 레퍼런스가 목록 형태로 표시되지 않아 추적하는 데 많은 노력이 필요했다. 
0.5 이후 버전에서 개선됐지만, 지원하지 않는 Unity 버전을 사용하는 분들도 있을 것이다. 
그 때 대체할 수 있는 도구로 여전히 활용가치가 높아서 소개해드리고자 합니다. 
//footnote[author_of_heap_explore][@<href>{https://github.com/pschraut}]

//image[heap_explorer_top][Heap Explorer]

==={heap_explorer_install} 도입 방법
본 도구는 GitHub 리포지토리 @<fn>{url_of_heap_explorer}에 기재된 
Package URL's를 복사하여 Package Manager의 Add Package from Git url에서 추가합니다. 
설치 후 'Window -> Analysis -> Memory Profiler'에서 툴을 실행할 수 있습니다. 
//footnote[url_of_heap_explorer][@<href>{https://github.com/pschraut/UnityHeapExplorer}]

==={heap_explorer_control} 사용 방법
Heap Explorer의 툴바는 다음과 같습니다. 
//image[heap_explorer_header][Heap Explorer 툴바]


 : 좌우 화살표
 조작의 뒤로, 앞으로 이동을 할 수 있습니다. 특히 참조를 추적할 때 유용합니다. 

 : File
 측정 파일을 저장, 불러올 수 있습니다. .heap이라는 확장자로 저장됩니다. 

 : View
 표시 화면을 전환할 수 있습니다. 다양한 종류가 있으니 관심이 있으시면 문서를 참고하세요. 

 : Capture
 계측을 수행합니다. 단, @<em>{측정 대상은 Heap Explorer에서 변경할 수 없습니다}. 
 대상 전환은 Unity Profiler 등 Unity에서 제공하는 툴에서 변경해야 합니다. 
 또한 Save는 파일에 저장한 후 결과를 표시하고, Analyze는 저장하지 않고 표시합니다. 
 또한, Memory Profiler와 마찬가지로 측정할 때 확보된 메모리는 해제되지 않으므로 주의해야 합니다. 

//image[heap_explorer_target][측정 대상 전환]

측정 결과 화면은 다음과 같습니다. 이 화면을 Overview라고 합니다. 
//image[heap_explorer_capture_top][Heap Explorer의 측정 결과 (Overview)]
Overview에서 특히 신경 써야 할 카테고리는 초록색 선으로 표시된 Native Memory Usage와 Managed Memory Usage이다. 
Investigate 버튼을 누르면 각 카테고리의 세부 정보를 확인할 수 있습니다. 

아래에서는 카테고리 세부 정보 표시 중 중요한 부분을 중심으로 설명하겠습니다. 

===={heap_explorer_objects} 1. Object
Native Memory를 Investigate한 경우, C++ Objects가 이 영역에 표시됩니다. 
Managed Memory의 경우 C# Objects가 이 영역에 표시됩니다. 
//image[heap_explorer_objects_view][Object 표시 영역]

헤더에 생소한 항목이 몇 개 있어서 보충 설명을 해두겠습니다. 


 : DDoL
 Don't Destroy On Load의 약자입니다. 씬을 전환해도 파괴하지 않는 오브젝트로 지정되어 있는지를 알 수 있습니다. 

 : Persistent
 영구적인 오브젝트인지 여부입니다. 시작 시 Unity가 자동 생성하는 것이 이에 해당합니다. 

이후 소개하는 표시 영역은 @<img>{heap_explorer_objects_view}의 오브젝트를 선택하면 업데이트됩니다. 

===={heap_explorer_referenced_by} 2. Referenced by
대상 오브젝트의 참조 대상이 되는 오브젝트가 표시됩니다. 
//image[heap_explorer_referenced_by][Referenced by]

===={heap_explorer_references_to} 3. References to
대상 오브젝트의 참조 대상 오브젝트를 표시합니다. 
//image[heap_explorer_references_to][References to]

===={heap_explorer_path_to_root} 4. Path to Root
대상 오브젝트를 참조하는 루트 오브젝트가 표시됩니다. 
메모리 누수를 조사할 때 어떤 참조를 가지고 있는지 파악할 수 있어 유용하다. 
//image[heap_explorer_path_to_root][Path to Root]

지금까지의 내용을 정리하면 다음과 같은 그림이 됩니다. 
//image[heap_explorer_reference_explain][참조 이미지]

지금까지 소개한 것처럼 Heap Explorer는 메모리 누수 및 메모리 조사에 필요한 기능을 모두 갖추고 있습니다. 
동작도 가볍기 때문에 이 도구의 활용을 고려해 보시기 바랍니다. 
마음에 든다면 감사의 의미로 Star를 달아주면 더욱 좋을 것 같습니다. 

=={tool_xcode} Xcode
Xcode는 애플이 제공하는 통합 개발 환경 도구입니다. 
Unity에서 타깃 플랫폼을 iOS로 설정하면, 그 빌드 결과물이 Xcode 프로젝트가 됩니다. 
Unity에서 측정하는 것보다 더 정확한 수치를 얻을 수 있기 때문에 엄격한 검증을 할 때는 Xcode를 사용하는 것을 권장합니다. 
이번 절에서는 Debug Navigator, GPU Frame Capture, Memory Graph의 세 가지 프로파일링 도구에 대해 알아보겠습니다. 

==={tool_xcode_debug_start} 프로파일링 방법
Xcode에서 프로파일링하는 방법은 두 가지가 있습니다. 
첫 번째는 Xcode에서 직접 단말기에 빌드하여 애플리케이션을 실행하는 방법입니다. 
@<img>{xcode_run}그림과 같이 실행 버튼을 누르기만 하면 프로파일링이 시작됩니다. 
빌드할 때 인증서 등의 설정은 본 문서에서는 생략합니다. 
//image[xcode_run][Xcode의 실행 버튼]

두 번째는 실행 중인 애플리케이션을 Xcode의 디버거에 첨부하는 방법입니다. 
이는 앱을 실행한 후 Xcode 메뉴의 'Debug -> Attach to Process'에서 실행 중인 프로세스를 선택하면 프로파일을 생성할 수 있습니다. 
단, 빌드 시 인증서가 개발자용(Apple Development)이어야 한다. 
Ad Hoc이나 Enterprise 인증서로는 첨부할 수 없으니 주의해야 합니다. 
//image[xcode_debugger_attach][Xcode의 디버거 첨부]

==={tool_xcode_debug_navi} Debug Navigator
Debug Navigator는 Xcode에서 애플리케이션을 실행하기만 하면 CPU, Memory 등의 디버그 게이지를 확인할 수 있습니다. 
애플리케이션 실행 후 @<img>{xcode_debug_navi_tab}의 스프레이 마크를 누르면 6가지 항목이 표시됩니다. 
또는 Xcode 메뉴의 'View -> Navigators -> Debug'에서 열 수도 있습니다. 
아래에서는 각 항목에 대해 설명하겠습니다. 
//image[xcode_debug_navi_tab][Debug Navigator 선택]

===={tool_xcode_debug_navi_cpu} 1. CPU 게이지
CPU를 얼마나 사용하고 있는지 확인할 수 있습니다. 
또한 각 스레드별 사용률도 확인할 수 있습니다. 
//image[xcode_debug_navi_cpu][CPU 게이지]


===={tool_xcode_debug_navi_memory} 2. Memory 게이지
메모리 사용량의 개요를 볼 수 있습니다. 
내역 등 세부적인 분석은 불가능합니다. 
//image[xcode_debug_navi_memory][Memory 게이지]

===={tool_xcode_debug_navi_enery} 3. Energy 게이지
전력 소비에 대한 개요를 볼 수 있습니다. 
CPU, GPU, Network 등의 사용량 현황을 파악할 수 있습니다. 
//image[xcode_debug_navi_energy][Energy 게이지]

===={tool_xcode_debug_navi_disk} 4. Disk 게이지
File I/O의 개요를 확인할 수 있습니다. 
예기치 않은 타이밍에 파일 읽기/쓰기가 이루어지고 있지 않은지 확인하는 데 도움이 될 것입니다. 
//image[xcode_debug_navi_disk][Disk 게이지]

===={tool_xcode_debug_navi_network} 5. Network 게이지
네트워크 통신의 개요를 확인할 수 있습니다. 
Disk와 마찬가지로 예기치 않은 통신을 하고 있지는 않은지 등을 체크하는 데 도움이 될 것입니다. 
//image[xcode_debug_navi_network][Network 게이지]

===={tool_xcode_debug_navi_fps} 6. FPS 게이지
이 게이지는 기본적으로 표시되지 않습니다. 
@<hd>{tool_xcode_frame_debug} 에서 설명하는 GPU Frame Capture를 활성화하면 표시됩니다. 
FPS는 물론 셰이더 스테이지의 사용률, 각 CPU와 GPU의 처리 시간을 확인할 수 있습니다. 
//image[xcode_debug_navi_fps][FPS 게이지]

==={tool_xcode_frame_debug} GPU Frame Capture
GPU Frame Capture는 Xcode에서 프레임 디버깅을 할 수 있는 툴입니다. 
Unity의 Frame Debugger와 마찬가지로 렌더링이 완료되기까지의 과정을 확인할 수 있습니다. 
유니티에 비해 셰이더의 각 단계별 정보량이 많기 때문에 병목현상 분석 및 개선에 도움이 될 수 있습니다. 
아래에서는 사용법을 설명합니다. 

===={tool_xcode_frame_debug_ready} 1. 준비
Xcode에서 GPU Frame Capture를 활성화하기 위해서는 스키마 편집이 필요합니다. 
먼저 'Product -> Scheme -> Edit Scheme'에서 스키마 편집 화면을 엽니다. 
//image[xcode_frame_debugger_scheme][스키마 편집 화면]

다음으로 'Options' 탭에서 GPU Frame Capture를 'Metal'로 변경합니다. 
//image[xcode_frame_debugger_gpu_enable][GPU Frame Capture 활성화]

마지막으로 'Diagnostics' 탭에서 Metal의 'Api Validation'을 활성화한다. 
//image[xcode_frame_debugger_api][Api Validation 활성화]

===={tool_xcode_frame_debug_capture} 2. 캡처
실행 중 디버그 바에서 카메라 마크를 누르면 캡처가 이루어집니다. 
장면의 복잡도에 따라 처음 캡처하는 데 시간이 걸리므로 조금만 기다리자. 
참고로 Xcode13 이후 버전에서는 Metal 아이콘으로 변경되었습니다. 
//image[xcode_frame_debugger_capture_icon][GPU Frame Capture 버튼]

캡처가 완료되면 다음과 같은 요약 화면이 표시됩니다. 
//image[xcode_frame_debugger_summary][요약 화면]

이 요약 화면에서는 드로잉의 종속성, 메모리 등의 세부 정보를 확인할 수 있는 화면으로 전환할 수 있습니다. 
또한 Navigator 영역에는 드로잉에 관한 명령이 표시되어 있습니다. 
이 표시 방법은 'View Frame By Call'과 'View Frame By Pipeline State'가 있습니다. 
//image[xcode_frame_debugger_navigator_view][표시 방법 변경]

By Call 표시에서는 모든 드로잉 명령이 호출된 순서대로 정렬됩니다. 
이는 드로잉의 사전 준비가 되는 버퍼의 설정 등도 포함되기 때문에 매우 많은 명령이 나열됩니다. 
반면 By Pipeline State는 각 셰이더에서 그려진 지오메트리에 대한 드로잉 명령만 나열됩니다. 
어떤 것을 조사할 것인지에 따라 표시를 전환하는 것이 좋습니다. 
//image[xcode_frame_debugger_view_frame_diff][표시의 차이점]

Navigator 영역에 나열된 드로잉 명령어를 누르면 해당 명령어에 사용된 프로퍼티 정보를 확인할 수 있습니다. 
프로퍼티 정보에는 텍스처, 버퍼, 샘플러, 셰이더 함수, 지오메트리 등이 있습니다. 
각 프로퍼티는 더블클릭을 통해 세부 정보를 확인할 수 있습니다. 
예를 들어, 셰이더 코드 자체나 샘플러가 Repeat인지 Clamp인지까지 파악할 수 있습니다. 
//image[xcode_frame_debuggeer_command][드로잉 커맨드 디테일]

지오메트리 프로퍼티는 버텍스 정보가 테이블 형태로 표시될 뿐만 아니라, 카메라를 움직여 형상을 확인할 수도 있습니다. 
//image[xcode_frame_debugger_geometry][지오메트리 뷰어]


다음으로 요약 화면의 Performance란에 있는 'Profile'에 대해 설명합니다. 
이 버튼을 누르면 보다 세밀한 분석을 시작합니다. 
분석이 끝나면 도면에 소요된 시간이 Navigator 영역에 표시됩니다. 
//image[xcode_frame_debugger_navigator_profile][프로파일 후 표시]

뿐만 아니라 분석 결과를 Counters라는 화면에서 보다 상세하게 확인할 수 있습니다. 
이 화면에서는 각 도면의 Vertex, Rasterized, Fragment 등의 처리 시간을 그래픽으로 확인할 수 있습니다. 
//image[xocde_frame_debugger_counter][Counters 화면]

다음으로 요약 화면의 Memory 항목에 있는 'Show Memory'에 대해 설명하겠습니다. 
이 버튼을 누르면 GPU에서 사용하고 있는 리소스를 확인할 수 있는 화면으로 전환됩니다. 
표시되는 정보는 주로 텍스처와 버퍼입니다. 불필요한 것이 없는지 확인하는 것이 좋습니다. 
//image[xcode_frame_debugger_memory][GPU 리소스 확인 화면]

마지막으로 요약 화면의 Overview에 있는 'Show Dependencies'에 대해 설명합니다. 
이 버튼을 누르면 각 렌더패스의 종속성을 확인할 수 있습니다. 
Dependency를 살펴볼 때 '화살표가 바깥쪽을 향하고 있는 버튼을 누르면 해당 계층 이하의 종속성을 더 열어볼 수 있습니다. 
//image[xcode_frame_debugger_dependency_open][Dependency 열기 버튼]
이 화면은 어떤 드로잉이 무엇에 의존하고 있는지 확인하고 싶을 때 사용합니다. 
//image[xcode_frame_debugger_dependency][계층구조를 연 상태]

==={tool_xcode_memory_graph} Memory Graph
이 도구는 캡처 타이밍의 메모리 상황을 분석할 수 있습니다. 
왼쪽의 Navigator 영역에는 인스턴스가 표시되며, 해당 인스턴스를 선택하면 참조 관계가 그래프로 표시됩니다. 
오른쪽의 Inspector 영역에는 해당 인스턴스의 상세 정보가 표시된다. 
//image[xcode_memory_graph_top][MemoryGraph 캡처 화면]

이 도구는 플러그인 등 Unity에서 측정할 수 없는 오브젝트의 메모리 사용량을 조사할 때 사용하면 좋습니다. 
아래에서는 사용 방법을 설명합니다. 

===={tool_xcode_memory_graph_ready} 1. 사전 준비
Memory Graph에서 유용한 정보를 얻기 위해서는 Scheme을 편집해야 합니다. 
'Product -> Scheme -> Edit Scheme'에서 Scheme 편집 화면을 엽니다. 
그리고 'Diagnostics' 탭에서 'Malloc Stack Logging'을 활성화한다. 
//image[xcode_memory_graph_malloc_stack_logging][Malloc Stack Logging 활성화]

이를 활성화하면 Inspector에 Backtrace가 표시되어 어떤 흐름으로 할당되었는지 알 수 있습니다. 
//image[xcode_memory_graph_backtrace][Backtrace 표시]

===={tool_xcode_memory_graph_capture} 2. 캡처하기
애플리케이션 실행 중 디버그 바에서 가지 모양의 아이콘을 누르면 캡처가 이루어집니다. 
//image[xcode_memory_graph_icon][Memory Graph Capture 버튼]

또한 Memory Graph는 'File -> Export MemoryGraph'에서 파일로 저장할 수 있다. 
이 파일에 대해 vmmap 명령이나 heap 명령, malloc_history 명령어를 활용하여 더 깊이 있는 조사를 할 수 있습니다. 
관심이 있다면 꼭 한번 살펴보시기 바랍니다. 
vmmap 명령의 Summary 표시를 예로 들어보면, MemoryGraph에서는 파악하기 어려웠던 전체 그림을 파악할 수 있습니다. 
//listnum[vmmap_summary][vmmap summary 명령어]{
vmmap --summary hoge.memgraph
//}

//image[xcode_vmmap_summary][MemoryGraph Summary 표시]

=={tool_instruments} Instruments
Xcode에는 Instruments(인스트루먼츠)라는 세부적인 측정 및 분석에 특화된 도구가 있습니다. 
Instruments는 'Product -> Analyze'를 선택하면 빌드가 시작됩니다. 
완료되면 다음과 같은 계측 항목의 템플릿을 선택할 수 있는 화면이 열립니다. 
//image[instruments_menu][Instruments 템플릿 선택 화면]

많은 템플릿에서 알 수 있듯이 Instruments는 다양한 내용을 분석할 수 있습니다. 
본 절에서는 이 중에서 특히 활용 빈도가 높은 'Time Profiler'와 'Allocations'에 대해 알아보겠습니다. 

==={tool_instruments_time_profiler} Time Profiler
Time Profiler는 코드의 실행 시간을 측정하는 도구입니다. 
Unity Profiler에 있던 CPU 모듈과 마찬가지로 처리 시간을 개선할 때 사용합니다. 

측정을 시작하려면 Time Profiler의 툴바에 있는 빨간색 동그라미 모양의 레코드 버튼을 눌러야 합니다. 
//image[insturments_record_start][레코드 시작 버튼]


계측을 시작하면 @<img>{instruments_time_profile_result}와 같은 화면이 나타납니다. 
//image[instruments_time_profile_result][측정 결과]
Unity Profiler와 달리 프레임 단위가 아닌 구간 단위로 분석을 진행합니다. 
하단의 Tree View에는 구간 내 처리 시간이 표시됩니다. 
게임 로직의 처리 시간을 최적화할 경우, Tree View의 PlayerLoop 이하의 처리를 분석하는 것이 좋습니다. 

Tree View의 표시를 보기 쉽게 하기 위해 Xcode 하단에 있는 Call Trees 설정을 @<img>{instruments_call_tress}와 같이 설정해두면 좋습니다. 
특히 Hide System Libraries에 체크하면, 손이 닿지 않는 시스템 코드가 숨겨져 조사를 쉽게 할 수 있습니다. 
//image[instruments_call_tress][Call Trees 설정]

이렇게 처리 시간을 분석하고 최적화할 수 있습니다. 

//info{
Time Profiler에서는 유니티 프로파일러와 기호 이름이 다릅니다. 
크게 다르지는 않지만 '클래스명_함수명_임의의 문자열'이라는 표기가 됩니다. 
//image[xcode_time_profiler_il2cpp][Time Profiler의 심볼 이름]
//}

==={tool_instruments_allocations} Allocations
Allocations는 메모리 사용량을 측정하기 위한 도구입니다. 메모리 누수나 사용량을 개선할 때 사용합니다. 
//image[instruments_allocations_view][Allocations 측정 화면]

측정을 하기 전에 'File -> Recording Options'를 열고 'Discard events for freed memory'를 체크합니다. 
//image[instruments_recording_options][옵션 설정 화면]

이 옵션을 활성화하면 메모리가 해제되었을 때 기록을 파기합니다. 
//image[instruments_recodring_options_diff][옵션 설정에 따른 차이점]
@<img>{instruments_recodring_options_diff}를 보면 알 수 있듯이 옵션 유무에 따라 외형이 크게 달라집니다. 
옵션이 있는 경우, 메모리가 할당된 시점에만 선이 기록됩니다. 
또한 기록된 라인은 그 확보된 영역이 해제되면 파기됩니다. 
즉, 이 옵션을 설정하면 선이 계속 남아 있으면 메모리에서 해제되지 않는다는 뜻입니다. 
예를 들어, 장면 전환을 통해 메모리를 해제하는 설계의 경우, 전환 전 장면 구간에 선이 많이 남아있다면 메모리 누수를 의심해 볼 수 있습니다. 
이 경우 Tree View로 세부 사항을 추적해 봅시다. 

화면 하단의 Tree View는 Time Profiler와 마찬가지로 지정한 범위의 세부 정보가 표시됩니다. 
이 Tree View의 표시 방법은 4가지가 있습니다. 
//image[instruments_allocations_view_type][표시 방법 선택]
가장 추천하는 표시 방법은 Call Trees입니다. 
이를 통해 어떤 코드에 의해 할당이 발생했는지 추적할 수 있습니다. 
화면 하단에 Call Trees 표시 옵션이 있으므로, Time Profiler에서 소개한 
@<img>{instruments_call_tress}처럼 Hide System Libraries 등의 옵션을 설정하면 됩니다. 
@<img>{instruments_allocations_call_tree}에서는 Call Trees 표시를 캡쳐해 보았습니다. 
12.05MB의 할당이 SampleScript의 OnClicked에서 발생하고 있음을 알 수 있습니다. 
//image[instruments_allocations_call_tree][Call Tree 표시]

마지막으로 Generations라는 기능을 소개합니다. 
Xcode 하단에 'Mark Generations'라는 버튼이 있습니다. 
//image[instruments_allocations_mark_generation][Mark Generation 버튼]
이 버튼을 누르면 해당 타이밍의 메모리가 기억됩니다. 
이후 다시 Mark Generations를 누르면 이전 데이터와 비교하여 새롭게 확보된 메모리 양이 기록됩니다. 
//image[instruments_allocations_generations][Generations]
@<img>{instruments_allocations_generations}의 각 Generations는 
세부 내용을 보면 Call Tree 형태로 표시되므로, 메모리 증가가 무엇에 의해 발생했는지 추적할 수 있습니다. 

=={tool_android_studio} 안드로이드 스튜디오
안드로이드 스튜디오는 안드로이드의 통합 개발 환경 도구입니다. 
이 도구를 이용해 애플리케이션의 상태를 측정할 수 있다. 
프로파일링 가능한 항목은 CPU, Memory, Network, Energy의 4가지입니다. 
본 절에서는 먼저 프로파일링 방법을 소개하고, 이후 CPU와 Memory의 측정 항목에 대해 설명한다. 
//image[android_studio_profile][프로파일 화면]

==={tool_android_studio_debug_start} 프로파일 방법
프로파일링 방법은 두 가지가 있습니다. 
첫 번째는 Android Studio를 통해 빌드하고 프로파일링하는 방법입니다. 
이 방법은 먼저 Android Studio의 프로젝트를 Unity에서 내보내기 합니다. 
Build Settings에서 'Export Project'를 체크하여 빌드합니다. 
//image[android_studio_export][프로젝트 Export]

다음으로 내보낸 프로젝트를 Android Studio에서 엽니다. 
그리고 안드로이드 단말기를 연결한 상태에서 오른쪽 상단에 있는 게이지 모양의 아이콘을 누르면 빌드가 시작됩니다. 
빌드가 완료되면 앱이 실행되고 프로파일이 시작됩니다. 
//image[android_studio_tool_bar][Profile 시작 아이콘]

두 번째는 실행 중인 프로세스를 디버거에 연결하여 측정하는 방법입니다. 
먼저 Android Studio 메뉴의 'View -> Tool Windows -> Profiler'에서 Android Profiler를 엽니다. 
//image[android_studio_profiler_menu][Android Profiler 열기]

다음으로 열린 Profiler의@<kw>{SESSIONS} 에서 측정할 세션을 선택합니다. 
세션을 연결하기 위해서는 측정하고자 하는 애플리케이션이 실행되어 있어야 합니다. 
또한 @<em>{Development Build를 적용한 바이너리여야} 합니다. 
세션 연결이 완료되면 프로파일이 시작됩니다. 
//image[android_studio_profiler_attach][프로파일링할 SESSION 선택하기]

두 번째 디버거에 연결하는 방법은 프로젝트를 내보낼 필요 없이 간편하게 이용할 수 있으므로 기억해두면 좋습니다. 

//info{
엄밀히 말하면 Unity의 Development Build가 아닌 AndroidManifest.xml에서 debuggable과 profileable을 설정해야 한다. 
Unity에서는 Development Build를 하면 자동으로 debuggable이 true로 설정됩니다. 
//}

==={tool_android_studio_cpu} CPU 측정
CPU 계측 화면은 @<img>{android_studio_cpu_thread_select}과 같습니다. 
이 화면만으로는 무엇이 얼마나 많은 처리 시간을 소비하고 있는지 알 수 없습니다. 
좀 더 자세히 보기 위해서는 자세히 보고 싶은 스레드를 선택해야 합니다. 
//image[android_studio_cpu_thread_select][CPU 계측 상단 화면, 스레드 선택]

스레드 선택 후 Record 버튼을 누르면 해당 스레드의 콜스택이 측정됩니다. 
@<img>{android_studio_record_button}와 같이 몇 가지 측정 유형이 있지만 'Callstack Sample Recording'으로도 문제 없을 것입니다. 
//image[android_studio_record_button][Record 시작 버튼]

Stop 버튼을 누르면 측정이 종료되고 결과가 표시됩니다. 
결과 화면은 Unity Profiler의 CPU 모듈과 같은 형태로 표시됩니다. 
//image[android_studio_sampling_result][콜스택 측정 결과 화면]

==={tool_android_studio_memory} Memory 측정
Memory 계측 화면은 @<img>{android_studio_memory}과 같습니다. 
이 화면에서는 메모리 내역을 확인할 수 없습니다. 
//image[android_studio_memory][Memory 측정 화면]

메모리 내역을 확인하려면 추가로 계측을 해야 합니다. 
측정 방법은 3가지가 있습니다. 'Capture heap dump'는 누른 타이밍의 메모리 정보를 얻을 수 있습니다. 
그 외의 버튼은 측정 구간 중 할당량을 분석하기 위한 버튼입니다. 
//image[android_studio_memory_record_options][Memory 측정 옵션]

예시로 @<img>{android_studio_memory_heap_dump}에 Heap Dump의 측정 결과를 캡쳐했습니다. 
세부적으로 분석하기에는 조금 입도가 거칠기 때문에 난이도가 높은 편입니다. 
//image[android_studio_memory_heap_dump][Heap Dump 결과]

=={tool_graphics_render_doc} RenderDoc
RenderDoc은 오픈소스로 개발되어 무료로 사용할 수 있는 고품질 그래픽 디버거 도구입니다. 
이 도구는 현재 Windows와 Linux를 지원하지만, Mac은 지원하지 않는다. 
또한 지원하는 Graphics API는 Vulkan, OpenGL(ES), D3D11, D3D12 등입니다. 
따라서 안드로이드에서는 사용할 수 있지만 iOS에서는 사용할 수 없습니다. 

이번 절에서는 안드로이드 애플리케이션을 실제로 프로파일링해 보겠습니다. 
단, 안드로이드 프로파일링에는 몇 가지 제약이 있으니 주의해야 한다. 
우선 안드로이드 OS 버전은 6.0 이상이 필수입니다. 그리고 측정 대상 애플리케이션은 Debuggable을 활성화해야 합니다. 
이는 빌드 시 Development Build를 선택하면 문제가 없습니다. 
참고로 프로파일에 사용한 RenderDoc의 버전은 v1.18입니다. 

==={render_doc_ready} 측정 방법
먼저 RenderDoc을 준비합니다. 
공식 사이트( @<fn>{render_doc_url})에서 설치 프로그램을 다운로드하여 툴을 설치한다. 
설치 후 RenderDoc 툴을 엽니다. 
//footnote[render_doc_url][@<href>{https://renderdoc.org/}]

//image[render_doc_top][RenderDoc 실행 후 화면]

다음으로 안드로이드 기기를 RenderDoc과 연결합니다. 
왼쪽 하단에 있는 집 표시를 누르면 PC와 연결된 기기 목록이 표시됩니다. 
목록에서 측정하고자 하는 단말기를 선택합니다. 
//image[render_doc_connection][장치와의 연결]

다음으로 연결된 기기에서 실행할 애플리케이션을 선택합니다. 
오른쪽에 있는 탭에서 Launch Application을 선택하고, Executable Path에서 실행할 앱을 선택합니다. 
//image[render_doc_launch_application_tab][Launch Application 탭 실행할 앱 선택]

File Browser 창이 열리면, 이번에 측정할 Pacakge Name을 찾아 Activity를 선택합니다. 
//image[render_doc_launch_application_select][측정할 애플리케이션 선택]

마지막으로 Launch Application에서 Launch 버튼을 누르면 디바이스에서 애플리케이션이 실행됩니다. 
또한 RenderDoc 상에서 계측을 위한 탭이 새롭게 추가됩니다. 
//image[render_doc_capture_menu][측정용 탭]

試しにCapture Frame(s) Immediatelyを押すと、フレームデータがキャプチャされ、Capture collectedに並びます。 
이 데이터를 더블 클릭하면 캡처 데이터를 열 수 있습니다. 

==={render_doc_capture_dislay} 캡처 데이터 보는 방법
RenderDoc에는 다양한 기능이 있지만, 본 절에서는 중요한 부분만 추려서 기능을 설명한다. 
먼저 화면 상단에는 캡처한 프레임의 타임라인이 표시됩니다. 
각각의 렌더링 명령이 어떤 순서로 이루어졌는지 시각적으로 파악할 수 있습니다. 
//image[render_doc_timeline][타임라인]

다음으로 Event Browser입니다. 여기에는 각 명령이 위에서부터 순서대로 나열되어 있습니다. 
//image[render_doc_event_browser][Event Browser]

Event Browser 내 상단에 있는 '시계 표시'를 누르면 각 명령의 처리 시간이 'Duration'에 표시됩니다. 
측정 타이밍에 따라 처리 시간이 달라지므로, 참고만 하시면 됩니다. 
또한 DrawOpaqueObjects의 명령어 내역을 보면 3개가 Batch로 처리되고 있고, 1개만 Batch에서 벗어난 드로잉을 하고 있는 것을 확인할 수 있습니다. 

다음으로 오른쪽에 탭으로 나열된 각 창에 대해 설명하겠습니다. 
이 탭 안에는 Event Browser에서 선택한 명령의 상세 정보를 확인할 수 있는 창이 있습니다. 
특히 중요한 것은 Mesh Viewer, Texture Viewer, Pipeline State 세 가지입니다. 
//image[render_doc_window_tab][각 창]

먼저 Pipeline State에 대해 알아보겠습니다. 
Pipeline State는 해당 오브젝트가 화면에 그려지기까지 각 셰이더 단계에서 어떤 파라미터를 사용했는지 확인할 수 있습니다. 
또한 사용된 셰이더와 그 내용도 확인할 수 있습니다. 
//image[render_doc_pipeline_state][Pipieline State]
파이프라인 스테이트에 표시되는 스테이지 이름은 생략되어 있으므로 정식 명칭을 @<table>{pipeline_state_name}으로 정리합니다. 
//table[pipeline_state_name][PipielineState 정식 명칭]{
스테이지 이름	정식명
-------------------- 
VTX	Vertex Input
VS	Vertex Shader
TCS	Tessellation Control Shader
TES	테셀레이션 평가 셰이더
GS	지오메트리 셰이더
RS	Rasterizer
FS	Fragment Shader
FB	프레임 버퍼
CS	Compute Shader
//}

@<img>{render_doc_pipeline_state}에서는 VTX 스테이지가 선택되어 있으며, 토폴로지 및 버텍스 입력 데이터를 확인할 수 있습니다. 
이 외에도 @<img>{render_doc_pipeline_state_framebuffer}의 FB 스테이지에서는 출력되는 텍스처의 상태, Blend State 등의 세부 정보를 확인할 수 있다. 
//image[render_doc_pipeline_state_framebuffer][FB(Frame Buffer)의 State]

또한 @<img>{render_doc_pipeline_state_fragment}의 FS 스테이지에서는 프래그먼트 셰이더 내에서 사용된 텍스처와 파라미터를 확인할 수 있다. 
//image[render_doc_pipeline_state_fragment][FS(Fragment Shader)의 State]
FS 스테이지 중앙의 Resources에는 사용된 텍스처와 샘플러가 표기되어 있습니다. 
또한 FS 스테이지 하단에 있는 Uniform Buffers에는 CBuffer가 표시되어 있습니다. 이 CBuffer에는 float, color 등 수치 정보 속성이 저장되어 있습니다. 
각 항목의 오른쪽에는 'Go'라는 화살표 아이콘이 있으며, 이를 누르면 데이터 세부 정보를 확인할 수 있습니다. 

FS 스테이지 상단에는 사용한 셰이더가 표시되며, View를 누르면 해당 셰이더 코드를 확인할 수 있습니다. 
표시를 쉽게 하기 위해 Disassembly type은 GLSL을 추천합니다. 
//image[render_doc_shader_code][Shader 코드 확인]

다음으로 Mesh Viewer입니다. 이것은 Mesh의 정보를 시각적으로 볼 수 있어 최적화나 디버깅에 유용한 기능입니다. 
//image[render_doc_mesh_viewer][Mesh Viewer]
Mesh Viewer의 상단에는 메시의 버텍스 정보가 테이블 형태로 표기되어 있습니다. 
하단에는 카메라를 움직여 형상을 확인할 수 있는 미리보기 화면이 있습니다. 
둘 다 In과 Out으로 탭이 나뉘어져 있어 변환 전후의 값과 모양이 어떻게 달라졌는지 파악할 수 있습니다. 
//image[render_doc_mesh_view_in_out][Mesh Viewer의 In, Out Preview 표시]

마지막으로 Texture Viewer입니다. 
이 화면에서는 Event Browser에서 선택한 명령의 '입력에 사용한 텍스처'와 '출력 결과'를 확인할 수 있습니다. 
//image[texture_viewer_input_output][Texture Viewer 텍스처 확인 화면]
화면 오른쪽 영역에서는 입출력의 텍스처를 확인할 수 있습니다. 표시된 텍스처를 누르면 화면 왼쪽 영역에 반영됩니다. 
왼쪽의 화면은 단순히 표시만 하는 것이 아니라, 컬러 채널을 필터링하거나 툴바의 설정을 적용할 수 있습니다. 
//image[texture_viewer_toolbar][텍스처 뷰어의 툴바]
@<img>{texture_viewer_input_output}에서는 Overlay에 'Wireframe Mesh'를 선택했기 때문에 
이 명령으로 그려진 오브젝트에 노란색 와이어프레임 표시가 되어 있어 시각적으로 알기 쉽게 되어 있습니다. 

또한 Texture Viewer에는 Pixel Context라는 기능이 있습니다. 
이것은 선택한 픽셀의 그리기 히스토리를 볼 수 있는 기능입니다. 
히스토리를 알 수 있다는 것은 어떤 픽셀에 대해 얼마나 자주 채우기가 이루어졌는지 파악할 수 있습니다. 
이는 오버드로우 조사나 최적화를 할 때 유용한 기능입니다. 
단, 픽셀 단위이기 때문에 전체적인 오버드로우를 조사하기에는 적합하지 않습니다. 
조사 방법은 @<img>{texture_viewer_input_output}의 왼쪽 화면에서 조사하고자 하는 부분을 마우스 오른쪽 버튼으로 클릭하면 Pixel Context에 그 위치가 반영됩니다. 
//image[texture_viewer_pixel_context][Pixel Context에 반영]
다음으로 Pixel Context의 History 버튼을 누르면 해당 픽셀의 그리기 이력을 확인할 수 있습니다. 
//image[texture_viewer_pixel_history][픽셀의 그리기 이력]
@<img>{texture_viewer_pixel_history}에는 4개의 히스토리가 있습니다. 
초록색 라인은 심도 테스트 등 파이프라인의 테스트를 모두 통과하여 렌더링되었음을 나타냅니다. 일부 테스트에 실패하여 그려지지 않은 경우 빨간색으로 표시됩니다. 
캡처한 이미지에서는 화면 지우기 처리와 캡슐 그리기가 성공했고, Plane과 Skybox가 깊이 테스트에 실패한 것을 확인할 수 있습니다. 
