={practice_graphics} Tuning Practice - Graphics

이 장에서는 유니티의 그래픽 기능을 중심으로 한 튜닝 방법을 소개합니다. 

=={practice_graphics_resolution} 해상도 조정

렌더링 파이프라인 중 프래그먼트 셰이더의 비용은 렌더링하는 해상도에 비례하여 증가합니다. 
특히 요즘의 모바일 디바이스는 디스플레이 해상도가 높아서 렌더링 해상도를 적절한 값으로 조정해야 합니다. 

==={practice_graphics_resolution_dpi} DPI 설정

모바일 플랫폼의 Player Settings의 해상도 관련 항목에 포함된 Resolution Scaling Mode를 @<em>{Fixed DPI로} 설정하면 
특정@<kw>{DPI(dots per inch)} 를 목표로 해상도를 낮출 수 있습니다. 

//image[fixed_dpi][Resolution Scaling Mode]

최종적인 드로잉 해상도는 Target DPI의 값과 Quality Settings에 포함된 Resolution Scaling DPI Scale Factor의 값이 곱해져 결정된다. 

//image[dpi_factor][Resolution Scaling DPI Scale Factor]

==={practice_graphics_resolution_code} 스크립트를 통한 해상도 설정

스크립트에서 드로잉 해상도를 동적으로 변경하려면 @<code>{Screen.SetResolution}를 호출합니다. 

현재 해상도는 @<code>{Screen.width}또는 @<code>{Screen.height}에서 확인할 수 있으며, DPI는 @<code>{Screen.dpi}에서 확인할 수 있다. 

//listnum[screen_setresolution][Screen.SetResolution][csharp]{
public void SetupResolution()
{
    var factor = 0.8f;

    //  Screen.width, Screen.height에서 현재 해상도 가져옵니다.
    var width = (int)(Screen.width * factor);
    var height = (int)(Screen.height * factor);

    //  해상도 설정
    Screen.SetResolution(width, height, true);
}
//}

//info{
 @<code>{Screen.SetResolution} 에서 설정한 해상도는 실기기에만 반영됩니다. 

Editor에서는 변경 사항이 반영되지 않으므로 주의해야 한다. 
//}


=={practice_graphics_overdraw} 반투명과 오버드로우

반투명 머티리얼 사용은@<kw>{오버드로우} 를 증가시키는 주요 원인이 됩니다. 
오버드로우는 화면의 한 픽셀에 대해 여러 번 조각을 그리는 것으로, 조각 셰이더의 부하와 비례하여 성능에 영향을 미칩니다. 

특히 Particle System 등에서 반투명 파티클을 대량으로 생성하는 경우 등에 오버드로가 많이 발생하는 경우가 많습니다. 

오버드로우로 인한 렌더링 부하 증가를 줄이기 위해서는 다음과 같은 방법을 생각해 볼 수 있습니다. 

 * 불필요한 그리기 영역 줄이기
 ** 텍스처가 완전히 투명한 영역도 그리기 대상이 되므로 가급적 줄인다.
 * 오버드로우가 발생할 수 있는 오브젝트에는 가급적 가벼운 셰이더를 사용한다.
 * 반투명 머티리얼은 가급적 사용하지 않는다.
 ** 불투명 머티리얼로 유사하게 반투명해 보이는 효과를 낼 수 있습니다. @<kw>{디더링} 기법도 고려한다.

Built-in Render Pipeline의 Editor에서는 Scene 뷰의 모드를@<kw>{Overdraw} 로 변경하여 오버드로우를 시각화할 수 있으며, 오버드로우 조정의 기준으로 유용하게 사용할 수 있습니다. 

//image[overdraw][Overdraw 모드]


//info{
Universal Render Pipeline에서는 Unity 2021.2부터 구현된@<kw>{Scene Debug View Modes} 를 통해 오버드로우를 시각화할 수 있습니다. 
//}

=={practice_graphics_draw_call} 드로 콜 감소

드로우 콜의 증가는 종종 CPU 부하에 영향을 미치는데, 
유니티에는 드로우 콜 수를 줄일 수 있는 몇 가지 기능이 있습니다. 

==={practice_graphics_dynamic_batching} 동적 배칭

@<kw>{동적 배치} 은 동적 오브젝트를 런타임에 일괄 처리할 수 있는 기능입니다. 
이 기능을 사용하면 동일한 머티리얼을 사용하는 다이내믹 오브젝트의 드로 콜을 통합하여 줄일 수 있습니다. 

사용 방법은 Player Settings에서@<kw>{Dynamic Batching} 항목을 활성화합니다. 

또한, Universal Render Pipeline에서는 Universal Render Pipeline Asset 내의@<kw>{Dynamic Batching} 항목을 활성화해야 합니다. 
단, Universal Render Pipeline에서는 Dynamic Batching 사용을 권장하지 않습니다. 

//image[dynamic_batching][Dynamic Batching 설정]

Dynamic Batching은 CPU 비용을 사용하는 처리이기 때문에 오브젝트에 적용하기 위해서는 여러 가지 조건을 충족해야 합니다. 
아래에서 주요 조건을 설명합니다. 

 * 동일한 머티리얼을 참조하고 있다.
 * MeshRenderer 또는 Particle System으로 렌더링을 하고 있다.
 ** SkinnedMeshRenderer와 같은 다른 컴포넌트는 동적 배칭의 대상이 아닙니다.
 * 메시의 버텍스 수가 300개 이하인 경우
 * 멀티패스를 사용하지 않음
 * 실시간 그림자의 영향을 받지 않음

//info{

동적 배칭은 CPU의 정적 부하에 영향을 미치므로 권장하지 않을 수 있습니다. 
후술하는@<kw>{SRP Batcher} 를 사용하면 동적 배칭에 가까운 효과를 얻을 수 있습니다. 

//}

==={practice_graphics_static_batching} 정적 배칭

@<kw>{정적 배치} 은 씬에서 움직이지 않는 오브젝트를 배치하는 기능입니다. 
이 기능을 사용하면 동일한 머티리얼을 사용하는 정적 오브젝트의 드로 콜을 줄일 수 있습니다. 

동적 배칭과 마찬가지로 Player Settings에서@<kw>{Static Batching} 를 활성화하여 사용할 수 있습니다. 

//image[static_batching][Static Batching 설정]

오브젝트를 스태틱 배칭 대상으로 설정하려면, 오브젝트의@<kw>{static flag} 를 활성화해야 합니다. 
구체적으로는 static 플래그 중@<kw>{Batching Static} 라는 하위 플래그를 활성화해야 한다. 

//image[batching_static][Batching Static]

정적 배칭은 동적 배칭과 달리 런타임에 버텍스 변환 처리를 하지 않기 때문에 낮은 부하로 실행할 수 있습니다. 
다만, 일괄 처리로 결합된 메시 정보를 저장하기 때문에 메모리를 많이 소모한다는 점에 주의해야 합니다. 


==={practice_graphics_instancing} GPU 인스턴싱

@<kw>{GPU 인스턴싱} 은 동일 메시, 동일 머티리얼의 오브젝트를 효율적으로 그리기 위한 기능입니다. 
풀이나 나무처럼 같은 메시를 여러 번 그리는 경우 드로 콜을 줄일 수 있습니다. 

GPU 인스턴싱을 사용하기 위해서는 머티리얼의 Inspector에서@<kw>{Enable Instancing} 를 활성화합니다. 

//image[enable_instancing][Enable Instancing]

GPU 인스턴싱을 사용할 수 있는 셰이더를 만들기 위해서는 몇 가지 전용 지원이 필요합니다. 
다음은 Built-in Render Pipeline에서 GPU 인스턴싱을 사용할 수 있도록 최소한의 구현을 한 셰이더 코드 예시입니다. 

//listnum[instancing_shader][GPU 인스턴싱을 지원하는 셰이더들]{
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
        //  프래그먼트 셰이더에서 INSTANCED_PROP에 접근하는 경우에만 필요
        UNITY_VERTEX_INPUT_INSTANCE_ID
     };

    UNITY_INSTANCING_BUFFER_START(Props)
        UNITY_DEFINE_INSTANCED_PROP(float4, _Color)
    UNITY_INSTANCING_BUFFER_END(Props)

    v2f vert(appdata v)
    {
        v2f o;

        UNITY_SETUP_INSTANCE_ID(v);

        //  프래그먼트 셰이더에서 INSTANCED_PROP에 접근하는 경우에만 필요
        UNITY_TRANSFER_INSTANCE_ID(v, o);

         o.vertex = UnityObjectToClipPos(v.vertex);
        return o;
    }

    fixed4 frag(v2f i) : SV_Target
    {
        //  프래그먼트 셰이더에서 INSTANCED_PROP에 접근하는 경우에만 필요
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

GPU 인스턴싱은 동일한 머티리얼을 참조하는 오브젝트에만 작용하지만, 각 인스턴스별로 프로퍼티를 설정할 수 있습니다. 
위의 셰이더 코드처럼 대상 프로퍼티를 @<code>{UNITY_INSTANCING_BUFFER_START(Props)}과 @<code>{UNITY_INSTANCING_BUFFER_END(Props)}으로 둘러싸서 개별적으로 변경하는 프로퍼티로 설정할 수 있습니다. 

이 프로퍼티를 C#에서@<kw>{MaterialPropertyBlock} 의 API를 사용하여 변경하여 개별 색상 등의 프로퍼티를 설정할 수 있습니다. 
단, 너무 많은 인스턴스에 대해 MaterialPropertyBlock을 사용하면 MaterialPropertyBlock에서의 접근이 CPU 성능에 영향을 줄 수 있으므로 주의해야 합니다. 

==={practice_graphics_srp_batcher} SRP Batcher

@<kw>{SRP Batcher} 란?@<kw>{Scriptable Render Pipeline} 에서만 사용할 수 있는 렌더링의 CPU 비용 절감을 위한 기능입니다. 
이 기능을 사용하면 동일한 셰이더 변형을 사용하는 여러 셰이더의 세트 패스 콜을 일괄적으로 처리할 수 있습니다. 

SRP Batcher를 사용하려면,@<kw>{Scriptable Render Pipeline Asset} 의 인스펙터에서@<kw>{SRP Batcher} 항목을 활성화합니다. 

//image[enable_srp_batcher][SRP Batcher 활성화]

또한, 런타임에는 아래의 C# 코드로 SRP Batcher의 활성화/비활성화를 변경할 수 있다. 

//listnum[srp_batcher_enable][SRP Batcher 활성화][csharp]{
GraphicsSettings.useScriptableRenderPipelineBatching = true;
//}

셰이더를 SRP Batcher에 대응시키기 위해서는 다음 두 가지 조건을 충족해야 합니다. 

 1. 오브젝트별로 정의되는 빌트인 프로퍼티를 @<kw>{UnityPerDraw라는} 하나의 CBUFFER에 정의할 것
머티리얼별  2. 프로퍼티를 @<kw>{UnityPerMaterial이라는} 하나의 CBUFFER에 정의할 것

UnityPerDraw의 경우, Universal Render Pipeline 등의 셰이더에서는 기본적으로 기본적으로 지원되지만 
UnityPerMaterial의 CBUFFER 설정은 직접 해야 합니다. 

아래와 같이 머티리얼별 프로퍼티를 @<code>{CBUFFER_START(UnityPerMaterial)}과 @<code>{CBUFFER_END}으로 둘러싸면 됩니다. 

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

위의 대응으로 SRP Batcher를 지원하는 셰이더를 만들 수 있지만, 
Inspector에서 해당 셰이더가 SRP Batcher를 지원하는지 확인할 수 있습니다. 

셰이더의 Inspector의@<kw>{SRP Batcher} 항목이@<kw>{compatible} 로 되어 있으면 SRP Batcher를 지원하며, 
@<kw>{not compatible} 로 되어 있으면 지원하지 않는 것을 알 수 있다. 

//image[srp_batcher_compatible][SRP Batcher를 지원하는 셰이더]

=={practice_graphis_atlas} SpriteAtlas

2D 게임이나 UI에서는 많은 스프라이트들을 사용하여 화면을 구성하는 경우가 많습니다. 
이러한 경우, 많은 양의 드로우 콜을 발생시키지 않기 위한 기능이 바로@<kw>{SpriteAtlas} 입니다. 

SpriteAtlas를 사용하면 여러 개의 스프라이트를 하나의 텍스처로 묶어 드로 콜을 줄일 수 있습니다. 

SpriteAtlas를 만들려면 먼저 Package Manager에서@<kw>{2D Sprite} 를 프로젝트에 설치해야 합니다. 

//image[2d_sprite][2D Sprite]

설치 후 Project 뷰에서 우클릭하여 'Create -> 2D -> Sprite Atlas'를 선택하여 SpriteAtlas 애셋을 생성합니다. 

//image[create_sprite_atlas][SpriteAtlas 생성]

아틀라스화할 스프라이트들을 지정하려면 SpriteAtlas의 Inspector에서@<kw>{Objects for Packing} 항목에 @<em>{스프라이트 또는 스프라이트가 포함된 폴더} 를 설정합니다. 

//image[atlas_packing][Objects for Packing 설정]

위와 같이 설정하면 빌드 시 또는 Unity Editor 재생 시 아틀라스화 처리가 이루어지며, 대상 스프라이트를 그릴 때 SpriteAtlas에서 통합된 텍스처를 참조하게 됩니다. 

아래와 같은 코드로 SpriteAtlas에서 직접 스프라이트를 가져오는 것도 가능합니다. 

//listnum[load_atlas_sprite][SpriteAtlas에서 Sprite 로드하기][csharp]{
[SerializeField]
private SpriteAtlas atlas;

public Sprite LoadSprite(string spriteName)
{
    //  SpriteAtlas에서 Sprite 이름을 인수로 SpriteAtlas에서 Sprite 가져 오기
    var sprite = atlas.GetSprite(spriteName);
    return sprite;
}
//}

SpriteAtlas에 포함된 Sprite를 하나 로드하면 아틀라스 전체 텍스처를 불러오기 때문에 하나만 로드하는 것보다 더 많은 메모리를 소모합니다. 
따라서 SpriteAtlas를 적절히 분할하는 등 주의해서 사용해야 합니다. 

//info{
이 절은 SpriteAtlas V1을 대상으로 설명합니다. 
SpriteAtlas V2에서는 아틀라스화할 대상 스프라이트의 폴더 지정이 불가능해지는 등 동작에 큰 변화가 있을 수 있습니다. 
//}

=={practice_graphics_culling} 컬링

Unity에서는 최종적으로 화면에 표시되지 않는 부분의 처리를 미리 생략하기 위한@<kw>{컬링} 라는 처리가 이루어집니다. 

==={practice_graphics_frustum_culling} 원추형 컬링

@<kw>{시스루 컬링} 은 카메라의 렌더링 범위가 되는 시준대 바깥에 있는 오브젝트를 렌더링 대상에서 제외하는 처리입니다. 
이를 통해 카메라 범위 밖에 있는 오브젝트는 렌더링 계산이 이루어지지 않게 됩니다. 

시준대 컬링은 별도의 설정을 하지 않아도 기본적으로 수행됩니다. 
버텍스 셰이더 부하가 높은 오브젝트 등의 경우, 메시를 적절히 분할하여 컬링 대상으로 삼아 렌더링 비용을 낮추는 방법도 효과적입니다. 

==={practice_graphics_shader_culling} 후면 컬링

@<kw>{백 컬링} 은 카메라에서 보이지 않는(있어야 하는) 폴리곤의 뒷면을 렌더링에서 제외하는 처리입니다. 
대부분의 메시는 닫혀있기 때문에(카메라에 보이는 것은 앞쪽의 폴리곤뿐) 뒷면은 그리지 않아도 됩니다. 

Unity에서는 셰이더에 명시하지 않으면 폴리곤의 뒷면이 컬링의 대상이 되지만, 명시하면 컬링 설정을 바꿀 수 있습니다. 
SubShader 내에 다음과 같이 작성합니다. 

//listnum[shader_cull][컬링 설정]{
SubShader
{
    Tags { "RenderType"="Opaque" }
    LOD 100

    Cull Back //  Front, Off

    Pass
    {
        CGPROGRAM
        #pragma vertex vert
        #pragma fragment frag
        ENDCG
    }
}
//}

@<code>{Back} , @<code>{Front}, @<code>{Off}세 가지 설정이 있는데, 각각의 효과는 아래와 같습니다. 

 * @<kw>{Back} - 시점과 반대쪽의 폴리곤을 그리지 않는다.
 * @<kw>{Front} - 시점과 같은 방향의 폴리곤을 그리지 않는다.
 * @<kw>{Off} - 뒷면 컬링을 비활성화하고 모든 면을 그립니다.

==={practice_graphics_occlusion_culling} 오클루전 컬링

@<kw>{오클루전 컬링} 은 오브젝트에 가려져 카메라에 보이지 않는 오브젝트를 그리기 대상에서 제외하는 처리입니다. 
이 기능은 미리 구운 차폐 판정용 데이터를 바탕으로 런타임에 오브젝트가 차폐되어 있는지 판단하여 차폐된 오브젝트를 그리기 대상에서 제외하는 기능입니다. 

오브젝트를 오클루전 컬링 대상으로 설정하려면, Inspector의 static 플래그에서@<kw>{Occluder Static} 또는@<kw>{Occludee Static} 를 활성화합니다. 
Occluder Static을 비활성화하고 Occludee Static을 활성화하면 오브젝트를 오클루딩하는 측으로 판단하지 않고 오클루딩을 @<em>{받는 측으로만} 처리하게 됩니다. 
그 반대의 경우, 오클루더가 가려지는 것으로 판단되지 않고 오클루더가 가려지는 @<em>{것으로만} 처리됩니다. 

//image[occlusion_static][오클루전 컬링의 static 플래그]

오클루전 컬링을 위한 사전 베이킹을 위해 사용합니다,@<kw>{Occlusion Culling 창} 를 표시합니다. 
이 창에서는 각 오브젝트의 스태틱 플래그 변경, 베이크 설정 변경 등을 할 수 있다,@<kw>{Bake 버튼} 을 눌러 베이킹을 할 수 있습니다. 

//image[occlusion_window][Occlusion Culling 창]

오클루전 컬링은 렌더링 비용을 줄이는 대신 컬링 처리를 위해 CPU에 부하를 주기 때문에 각각의 부하 밸런스를 보고 적절히 설정해야 합니다. 

//info{
오클루전 컬링으로 절감되는 것은 오브젝트 드로잉 처리만 감소하고, 실시간 그림자 그리기 등의 처리는 그대로 진행됩니다. 
//}


=={practice_graphics_shader} 셰이더
셰이더는 그래픽 표현에 매우 효과적이지만, 종종 성능 문제를 야기합니다. 

==={practice_graphics_shader_float} 부동소수점 유형 정확도 저하
GPU(특히 모바일 플랫폼)의 계산 속도는 큰 데이터 유형보다 작은 데이터 유형이 더 빠르다. 
따라서 대체 가능한 경우, 부동소수점 유형을@<kw>{float형(32bit)} 에서@<kw>{half형(16bit)} 로 대체하는 것이 효과적입니다. 

깊이 계산 등 정밀도가 필요한 경우 float 타입을 사용해야 하지만, Color 계산 등에서는 정밀도를 떨어뜨려도 결과물의 외형에 큰 차이가 나지 않습니다. 

==={practice_graphics_shader_vertex} 버텍스 셰이더로 계산하기
버텍스 셰이더의 처리는 메시의 버텍스 수만큼만 실행되고, 프래그먼트 셰이더의 처리는 최종적으로 쓰여지는 픽셀 수만큼만 실행됩니다. 
일반적으로 버텍스 셰이더의 실행 횟수는 프래그먼트 셰이더보다 적은 경우가 많기 때문에 복잡한 계산은 가급적 버텍스 셰이더로 하는 것이 좋다. 

버텍스 셰이더의 계산 결과는 셰이더 시맨틱스를 통해 프래그먼트 셰이더에 전달되지만, 여기서 전달되는 값은 보간된 값이므로 프래그먼트 셰이더로 계산했을 때와 다르게 보일 수 있다는 점에 유의해야 합니다. 

//listnum[shader_vertex_factor][버텍스 셰이더를 통한 사전 계산]{
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

    //  복잡한 사전 계산하기
    o.factor = CalculateFactor();

    return o;
}

fixed4 frag (v2f i) : SV_Target
{
    fixed4 col = tex2D(_MainTex, i.uv);

    //  버텍스 셰이더로 계산한 값을 프래그먼트 셰이더에서 사용하기
    col *= i.factor;

    return col;
}
ENDCG
//}

==={practice_graphis_shader_tex} 텍스처에 정보 미리 넣기 
셰이더 내의 복잡한 계산 결과가 외부의 값에 의해 변하지 않는다면, 텍스처의 요소로 미리 계산한 결과를 저장해 두는 것도 효과적인 방법이다. 

방법으로는 Unity에서 전용 텍스처를 생성하는 툴을 구현하거나 각종 DCC 툴의 확장 기능으로 구현하는 방법 등을 생각해 볼 수 있습니다. 
이미 사용하고 있는 텍스처의 알파 채널이 있다면 거기에 써넣거나 전용 텍스처를 준비하는 것이 좋습니다. 

예를 들어, 컬러 그레이딩에 사용되는@<kw>{LUT(색상 대응표)} 는 각 픽셀의 좌표가 각 색에 해당하는 텍스처에 대해 미리 색조 보정을 해줍니다. 
그 텍스처를 셰이더에서 원본 컬러를 기준으로 샘플링하면, 미리 색조 보정을 한 텍스처를 원본 컬러에 적용했을 때와 거의 동일한 결과를 얻을 수 있습니다. 

//image[lut-texture-1024][색조 보정 전 LUT 텍스처(1024x32)]

==={practice_graphics_variant_collection} ShaderVariantCollection
@<kw>{ShaderVariantCollection} 를 사용하면 셰이더를 사용하기 전에 컴파일하여 스파이크를 방지할 수 있습니다. 

ShaderVariantColletion은 게임 내에서 사용할 셰이더 변형 목록을 애셋으로 보관할 수 있습니다. 
Project 뷰에서 'Create -> Shader -> Shader Variant Collection'을 선택하여 생성합니다. 

//image[create-shadervariant][ShaderVariantCollection 만들기]

생성한 ShaderVariantCollection의 Inspector 뷰에서 Add Shader를 눌러 대상 셰이더를 추가하고, 해당 셰이더에 대해 어떤 변형을 추가할 것인지 선택합니다. 

//image[shadervariant-inspector][ShaderVariantCollection의 Inspector 보기]

ShaderVariantCollection을 Graphics Settings의@<kw>{Shader preloading} 항목의@<kw>{Preloaded Shaders} 에 추가하여 
애플리케이션 시작 시 컴파일할 셰이더 변형을 설정할 수 있습니다. 

//image[preloaded-shaders][Preloaded Shaders]

스크립트에서@<kw>{ShaderVariantCollection.WarmUp()} 를 호출하여 해당 ShaderVariantCollection에 포함된 셰이더 변형을 명시적으로 미리 컴파일할 수도 있습니다. 

//listnum[warmup_shader_variant][ShaderVariantCollection.WarmUp]{
public void PreloadShaderVariants(ShaderVariantCollection collection)
{
    //  명시적으로 셰이더 바리에이션 사전 컴파일하기
    if (!collection.isWarmedUp)
    {
        collection.WarmUp();
    }
}
//}

=={practice_graphics_lighting} 라이팅
라이팅은 게임 아트 표현에 있어 매우 중요한 요소 중 하나이지만, 퍼포먼스에 큰 영향을 미치는 경우가 많습니다. 

==={practice_graphics_shadow} 실시간 그림자
실시간 그림자 생성은 드로우 콜과 필레이트에 큰 영향을 미칩니다. 
따라서 리얼타임 섀도우를 사용할 때는 세팅을 신중하게 고려해야 합니다. 

===={practice_graphics_shadow_call} 드로 콜 감소
그림자 생성의 드로 콜을 줄이기 위해서는 다음과 같은 정책을 고려할 수 있습니다. 

 * 그림자를 드리우는 오브젝트 수 줄이기
 * 일괄 처리로 드로우 콜을 통합한다.

그림자를 드리우는 오브젝트를 줄이는 방법은 여러 가지가 있지만, 가장 간단한 방법은 MeshRenderer의@<kw>{Cast Shadows} 설정을 해제하는 것입니다. 
이렇게 하면 오브젝트를 그림자 그리기 대상에서 제외할 수 있습니다. 
이 설정은 일반적으로 Unity에서 켜져 있으므로 그림자를 사용하는 프로젝트에서는 주의해야 합니다. 

//image[mesh-castshadow][Cast Shadows]

오브젝트가 섀도맵에 그려지는 최대 거리를 줄이는 것도 도움이 될 수 있습니다. 
Quality Settings의@<kw>{Shadow Distance} 항목에서 섀도맵의 최대 그리기 거리를 변경할 수 있으며, 이 설정을 통해 그림자를 드리우는 오브젝트 수를 필요한 최소값으로 줄일 수 있습니다. 
이 설정을 조정하면 섀도맵의 해상도에 대해 최소한의 범위에서 그림자를 그릴 수 있기 때문에 그림자의 해상도 저하를 억제할 수 있습니다. 

//image[shadow-distance][Shadow Distance]

일반 드로잉과 마찬가지로 그림자 드로잉도 일괄 처리 대상으로 설정하면 드로우 콜을 줄일 수 있습니다. 
배치 방법에 대한 자세한 내용은 @<hd>{practice_graphics_draw_call}을 참고하시기 바랍니다. 


===={practice_graphics_shadow_fill} 필레이트 절약
그림자에 의한 필레이트는 그림자 맵의 드로잉과 그림자의 영향을 받는 오브젝트의 드로잉 모두에 영향을 받습니다. 

Quality Settings의 Shadows 항목에 있는 몇 가지 설정을 조정하여 각각의 채우기 속도를 절약할 수 있습니다. 

//image[quality-shadow][Quality Settings -> Shadows]

@<kw>{Shadows} 항목에서는 그림자 형식을 변경할 수 있습니다,@<kw>{Hard Shadows} 는 그림자 경계가 뚜렷하게 나타나지만 상대적으로 부하가 적다,@<kw>{Soft Shadows} 는 그림자 경계를 흐리게 표현할 수 있지만 부하가 높습니다. 

@<kw>{Shadow Resolution} 와@<kw>{Shadow Cascades} 항목은 섀도맵의 해상도에 영향을 주는 항목으로, 크게 설정하면 섀도맵의 해상도가 높아져 필레이트 소모가 커집니다. 
하지만 이 설정은 그림자의 퀄리티와도 관련이 깊기 때문에, 퍼포먼스와 퀄리티의 균형을 잘 맞춰서 조정해야 합니다. 

일부 설정은@<kw>{Light} 컴포넌트 인스펙터에서 일부 설정을 변경할 수 있으므로, 개별 라이트마다 설정을 변경할 수도 있습니다. 

//image[light-component][Light 컴포넌트의 그림자 설정]

===={practice_graphics_fake_shadow} 의사 그림자
게임 장르나 아트 스타일에 따라 판다각형 등으로 오브젝트의 그림자를 유사하게 표현하는 방법도 유효합니다. 
이 기법은 사용상의 제약이 많아 자유도가 높지는 않지만, 일반적인 실시간 그림자 그리기 기법에 비해 압도적으로 가벼운 그림자를 그릴 수 있습니다. 

//image[fake-shadow][판다각형에 의한 의사 그림자]

==={practice_graphics_lightmap} 라이트 매핑
라이팅 효과와 그림자를 미리 텍스처에 베이킹해 두면, 실시간 생성보다 훨씬 적은 부하로 고품질의 라이팅 표현을 구현할 수 있습니다. 

라이트맵을 굽기 위해서는 먼저 씬에 배치한 Light 컴포넌트의@<kw>{Mode} 항목을@<kw>{Mixed} 또는@<kw>{Baked} 로 변경합니다. 

//image[light-mixed][Light의 Mode 설정]

또한, 베이크 대상 오브젝트의 스태틱 플래그를 활성화합니다. 

//image[object-static][static 활성화]

이 상태에서 메뉴에서 Window -> Rendering -> Lighting을 선택하여 Lighting 뷰를 표시합니다. 

기본 상태에서는@<kw>{Lighting Settings} 에셋이 지정되어 있지 않습니다,@<kw>{New Lighting Settings} 버튼을 눌러 새로 생성합니다. 

//image[new-lighting-settings][New Lighting Settings]

라이트맵에 대한 설정은 주로@<kw>{Lightmapping Settings} 탭에서 설정합니다. 

//image[lightmapping-settings][Lightmapping Settings]

많은 설정 항목이 있는데, 이 값을 조정하면 라이트맵의 베이킹 속도와 품질이 달라집니다. 
따라서 원하는 속도와 품질에 맞게 적절히 설정해야 합니다. 

이 설정 중 성능에 가장 큰 영향을 미치는 것은@<kw>{Lightmap Resolution} 입니다. 
이 설정은 유니티에서 1unit당 라이트맵의 픽셀을 얼마나 할당할 것인지에 대한 설정으로, 이 값에 따라 최종 라이트맵의 크기가 달라지기 때문에 스토리지와 메모리 용량, 텍스처에 대한 접근 속도 등에 큰 영향을 미칩니다. 

//image[lightmap-resolution][Lightmap Resolution]

마지막으로 Inspector 뷰 하단의@<kw>{Generate Lighting} 버튼을 눌러 라이트맵을 구울 수 있습니다. 
베이킹이 완료되면 씬과 같은 이름의 폴더에 베이크된 라이트맵이 저장되어 있는 것을 확인할 수 있습니다. 

//image[generate-lighting][Generate Lighting]

//image[baked-lightmaps][구워진 라이트맵]

=={practice_graphics_lod} Level of Detail
카메라에서 먼 거리에 있는 오브젝트를 하이 폴리곤, 고화질로 그리는 것은 비효율적입니다. 
@<kw>{레벨 오브 디테일(LOD, Level of Detail)} 라는 기법을 이용하여 카메라로부터의 거리에 따라 오브젝트의 디테일을 줄일 수 있습니다. 

Unity에서는 오브젝트에@<kw>{LOD Group} 컴포넌트를 추가하여 LOD를 제어할 수 있습니다. 

//image[lod-group][LOD Group]

LOD Group이 부착된 GameObject의 자손에 각 LOD 레벨의 메시를 가진 Renderer를 배치하고, LOD Group의 각 LOD 레벨로 설정하면 카메라에 따라 LOD 레벨을 전환할 수 있게 됩니다. 
카메라의 거리에 대해 어떤 LOD 레벨을 할당할 것인지를 LOD Group별로 설정할 수도 있습니다. 

LOD를 사용하면 일반적으로 렌더링 부하를 줄일 수 있지만, 각 LOD 레벨의 메시가 모두 로드되기 때문에 메모리와 스토리지에 부담을 줄 수 있으므로 주의해야 합니다. 

=={practice_graphics_texture_sreaming} 텍스처 스트리밍
Unity의@<kw>{텍스처 스트리밍} 를 사용하면 텍스처에 필요한 메모리 용량과 로딩 시간을 줄일 수 있습니다. 
텍스처 스트리밍은 씬의 카메라 위치에 따라 밉맵을 로드하여 GPU 메모리를 절약하는 기능입니다. 

이 기능을 활성화하려면 Quality Settings의@<kw>{Texture Streaming} 를 활성화합니다. 

//image[texture-streaming][Texture Streaming]

또한 텍스처 밉맵을 스트리밍할 수 있도록 텍스처 임포트 세팅을 변경해야 합니다. 
텍스처의 Inspector를 엽니다,@<kw>{Advanced} 세팅에서@<kw>{Streaming Mipmaps} 를 활성화합니다. 

//image[streaming-mip][Streaming Mipmaps]

이 설정을 통해 지정된 텍스처의 밉맵을 스트리밍할 수 있습니다. 
또한 Quality Settings의@<kw>{Memory Budget} 항목을 조정하여 로드하는 텍스처의 총 메모리 사용량을 제한할 수 있습니다. 
텍스처 스트리밍 시스템은 여기서 설정한 메모리 양을 초과하지 않도록 밉맵을 로드합니다. 
