={practice_asset} Tuning Practice - Asset
게임 제작에서는 텍스처, 메시, 애니메이션, 사운드 등 다양한 종류의 에셋을 많이 다루게 됩니다. 
이 장에서는 이러한 애셋에 대해 성능 튜닝을 할 때 주의해야 할 설정 항목 등 실무적인 지식을 정리합니다. 

=={practice_asset_texture} 텍스처
텍스처의 원천이 되는 이미지 데이터는 게임 제작에 있어 필수적인 요소입니다. 
반면, 메모리 소비량이 상대적으로 많기 때문에 적절한 세팅이 필요합니다. 

==={practice_asset_texture_import} 임포트 설정
@<img>{texture_settings}은 Unity의 텍스처 임포트 설정입니다. 
//image[texture_settings][텍스처 설정][scale=0.75]

 이 중 특히 주의해야 할 부분을 소개합니다. 

==={practice_asset_texture_read_write} Read/Write
이 옵션은 기본적으로 비활성화되어 있습니다. 비활성화 상태에서는 GPU 메모리에만 텍스처가 전개됩니다. 
활성화하면 GPU 메모리뿐만 아니라 메인 메모리에도 복사되므로, 소비량이 2배로 증가합니다. 
따라서 @<code>{Texture.GetPixel}나 @<code>{Texture.SetPixel}같은 API를 사용하지 않고 Shader에서만 접근하는 경우 반드시 비활성화해야 합니다. 

또한 런타임에 생성한 텍스처는 @<list>{makeNoLongerReadable}과 같이 makeNoLongerReadable을 true로 설정하면 메인 메모리에 복사되는 것을 방지할 수 있습니다. 
//listnum[makeNoLongerReadable][makeNoLongerReadable 설정 방법][csharp]{
texture2D.Apply(updateMipmaps, makeNoLongerReadable: true)
//}

//info{
GPU 메모리에서 메인 메모리로 텍스처를 전송하는 데 시간이 오래 걸리므로, 텍스처를 읽을 수 있는 경우 양쪽 모두에 텍스처를 전개하는 것이 성능 향상에 도움이 됩니다. 
//}

==={practice_asset_texture_mipmap} Generate Mip Maps
밉맵 설정을 활성화하면 텍스처의 메모리 사용량이 약 1.3배 증가합니다. 
이 설정은 일반적으로 3D 상의 오브젝트에 대해 설정하는 것이 일반적이며, 먼 거리에 있는 오브젝트에 대해 흔들림 완화 및 텍스처 전송량 감소를 목적으로 설정합니다. 
2D 스프라이트나 UI 이미지에 대해서는 기본적으로 필요하지 않으므로 비활성화해 두세요. 

==={practice_asset_texture_aniso} Aniso Level
Aniso Level은 오브젝트를 얕은 각도로 그릴 때 텍스처가 흐릿하게 보이지 않도록 하는 기능입니다. 
이 기능은 주로 지면이나 바닥 등 멀리까지 이어지는 오브젝트에 사용됩니다. 
Aniso Level 값이 높을수록 그 혜택을 누릴 수 있지만, 처리 비용은 더 많이 듭니다. 
//image[aniso_level_compare][Aniso Level 적응 이미지][scale=1.0]

 설정값은 0~16까지 있지만 조금 특수한 사양으로 되어 있습니다. 

 * 0: 프로젝트 설정에 관계없이 항상 비활성화
 * 1: 기본적으로 비활성화. 단, 프로젝트 설정이 Forced On인 경우 9~16의 값으로 고정된다.
 * 그 외: 해당 값으로 설정

텍스처를 임포트하면 기본적으로 1로 설정되어 있습니다. 
따라서 고사양 단말기가 아닌 이상 Forced On 설정은 권장하지 않습니다. 
Forced On 설정은 'Project Settings -> Quality'의 Anisotropic Textures에서 설정할 수 있습니다. 

//image[project_settings_aniso][Forced On 설정]

Aniso Level 설정이 효과가 없는 오브젝트에서 활성화되어 있지 않은지, 효과가 있는 오브젝트에 대해 무분별하게 높은 설정값이 설정되어 있지 않은지 확인합니다. 

//info{
Aniso Level에 의한 효과는 선형적이지 않고 단계적으로 전환되는 동작을 하고 있습니다. 
필자가 확인한 결과, 0~1, 2~3, 4~7, 8 이상 등 4단계로 변화하는 것을 확인할 수 있었다. 
//}

==={practice_asset_texture_compress} 압축 설정
텍스처는 특별한 이유가 없는 한 압축하는 것이 좋습니다. 
만약 프로젝트 내에 비압축 텍스처가 존재한다면, 휴먼 에러이거나 규정이 없는 것일 수 있습니다. 빨리 확인해야 합니다. 
압축 설정에 대한 자세한 내용은 @<hd>{basic|basic_asset_data_compression}에서 확인할 수 있습니다. 

이러한 압축 설정은 휴먼에러를 방지하기 위해 TextureImporter를 이용하여 자동화하는 것을 권장합니다. 
//listnum[textureImporter][TextureImporter 자동화 예시][csharp]{
using UnityEditor;

public class ImporterExample : AssetPostprocessor
{
    private void OnPreprocessTexture()
    {
        var importer = assetImporter as TextureImporter;
        //  Read/Write 설정 등도 가능
        importer.isReadable = false;

        var settings = new TextureImporterPlatformSettings();
        //  Android = "Android", PC = "Standalone" 지정
        settings.name = "iPhone";
        settings.overridden = true;
        settings.textureCompression = TextureImporterCompression.Compressed;
        //  압축 형식 지정
        settings.format = TextureImporterFormat.ASTC_6x6;
        importer.SetPlatformTextureSettings(settings);
    }
}
//}

또한 모든 텍스처가 동일한 압축 형식일 필요는 없습니다. 
예를 들어, UI 이미지 중 전체적으로 그라데이션이 적용된 이미지는 압축으로 인한 품질 저하가 눈에 띄기 쉽다. 
이 경우 대상 이미지 중 일부 이미지만 압축률을 낮게 설정하는 것이 좋습니다. 
반대로 3D 모델과 같은 텍스처는 품질 저하가 눈에 잘 띄지 않으므로 압축률을 높이는 등 적절한 설정을 찾는 것이 좋습니다. 

=={practice_asset_mesh} Mesh
Unity로 임포트한 메쉬(모델)를 다룰 때 주의해야 할 사항을 소개합니다. 임포트한 모델 데이터는 설정에 따라 성능이 향상됩니다. 주의해야 할 점은 다음 4가지입니다. 

 * Read/Write Enabled
 * Vertex Compression
 * Mesh Compression
 * 메시 데이터 최적화

==={practice_asset_mesh_read_write_enabled} Read/Write Enabled
Mesh의 첫 번째 주의 사항은 Read/Write Enabled입니다. 
모델 인스펙터에 있는 이 옵션은 기본적으로 비활성화되어 있습니다. 
//image[practice_asset_mesh_inspector][Read/Write 설정]
런타임 동안 메시를 사용할 필요가 없다면 비활성화하는 것이 좋습니다. 구체적으로 모델을 Unity에 배치하고 AnimationClip을 재생하는 정도라면 Read/Write Enabled는 비활성화해도 문제없습니다. 

활성화하면 CPU에서 접근할 수 있는 정보를 메모리에 보관하기 때문에 메모리를 두 배로 소모합니다. 비활성화하는 것만으로도 메모리를 절약할 수 있으니 꼭 확인해보시기 바랍니다. 

==={practice_asset_vertex_compression} Vertex Compression
Vertex Compression은 메시의 버텍스 정보 정확도를 float에서 half로 변경하는 옵션입니다. 
이를 통해 @<em>{런타임 시 메모리 사용량과 파일 크기를} 줄일 수 있습니다. 
'Project Settings -> Player'의 Other에서 설정할 수 있으며, 기본 설정은 다음과 같습니다. 
//image[practice_asset_vertex_compression_settings][Vertex Compression 기본 설정]

단, 이 Vertex Compression은 다음과 같은 조건에 해당하면 비활성화되므로 주의해야 합니다. 

 * Read/Write가 활성화
 * Mesh Compression이 활성화된 경우
 * Dynamic Batching이 활성화되어 있고 적응 가능한 메시(버텍스 수 300개 이하, 버텍스 속성 900개 이하)

==={practice_asset_mesh_compression} Mesh Compression
Mesh Compression은 메시의 압축률을 변경할 수 있습니다. 압축률이 높을수록 파일 크기가 작아져 저장공간을 차지하는 비율이 줄어듭니다. 
압축된 데이터는 런타임에 압축이 풀립니다. 따라서 @<em>{런타임 메모리 사용량에는 영향을 미치지 않습니다}. 

Mesh Compresssion의 압축 설정에는 4가지 옵션이 있습니다. 

 * Off: 비압축
 * Low: 저압축
 * Medium: 중간 압축
 * High: 고압축

//image[practice_asset_mesh_compression][Mesh Compression]

@<hd>{practice_asset_vertex_compression} 에서도 언급했지만, 이 옵션을 활성화하면 @<em>{Vertex Compression이 비} 활성화됩니다. 
특히 메모리 사용량 제한이 심한 프로젝트에서는 이 단점을 잘 파악한 후 설정하시기 바랍니다. 

==={practice_asset_optimize_mesh_data} Optimize Mesh Data
Optimize Mesh Data는 메시에 불필요한 버텍스 데이터를 자동으로 삭제하는 기능입니다. 
불필요한 버텍스 데이터 판단은 사용 중인 Shader에서 자동으로 판단합니다. 
이는 런타임 시 메모리, 스토리지 모두 절감 효과가 있습니다. 
'Project Settings -> Player'의 Other에서 설정이 가능합니다. 
//image[practice_asset_optimize_mesh_data][Optimize Mesh Data 설정][scale=0.8]

 이 옵션은 버텍스 데이터가 자동 삭제되어 편리하지만, 예기치 못한 문제를 일으킬 수 있으므로 주의해야 합니다. 
예를 들어 런타임에 머티리얼이나 셰이더를 전환할 때, 접근한 프로퍼티가 삭제되어 렌더링 결과가 이상해질 수 있습니다. 
또한, Mesh만 애셋 번들링할 때 머티리얼 설정이 잘못되었을 때 불필요한 버텍스 데이터로 판단되는 경우도 있습니다. 
이는 Particle System과 같이 Mesh에 대한 레퍼런스만 갖게 하는 경우가 많습니다. 

=={practice_asset_material} 머티리얼
머티리얼은 오브젝트를 어떻게 그릴지 결정하는 중요한 기능입니다. 
익숙한 기능이지만, 잘못 사용하면 쉽게 메모리 누수가 발생할 수 있습니다. 
이번 절에서는 안전한 머티리얼을 사용하는 방법을 소개합니다. 

===={practice_asset_material_clone} 파라미터에 접근하는 것만으로 복제된다
머티리얼의 가장 큰 주의점은 파라미터에 접근하는 것만으로도 머티리얼이 복제된다는 점입니다. 그리고 복제된 것을 알아차리기 어렵다는 점입니다. 

다음 코드를 살펴보겠습니다. 
//listnum[material_clone][Material이 복제되는 예시][csharp]{
Material material;

void Awake()
{
    material = renderer.material;
    material.color = Color.green;
}
//}

머티리얼의 color 프로퍼티에 Color.green을 설정한 간단한 처리입니다. 이 렌더러의 머티리얼은 복제되어 있습니다. 그리고 복제된 오브젝트는 명시적으로 Destroy해야 합니다. 
//listnum[material_destroy][복제된 Material 삭제 예시][csharp]{
Material material;

void Awake()
{
    material = renderer.material;
    material.color = Color.green;
}

void OnDestroy()
{
    if (material != null)
    {
        Destroy(material)
    }
}
//}
이렇게 복제된 머티리얼을 Destroy하면 메모리 누수를 방지할 수 있습니다. 

===={practice_asset_material_instantiate} 생성한 머티리얼을 철저하게 정리하자
동적으로 생성한 머티리얼도 메모리 누수의 원인이 되기 쉽습니다. 생성한 머티리얼도 다 사용한 후에는 반드시 Destroy를 해야 합니다. 

다음 샘플 코드를 살펴봅시다. 
//listnum[material_dynamic][동적으로 생성한 머티리얼 삭제 예시][csharp]{
Material material;

void Awake()
{
    material = new Material();  //  머티리얼을 동적으로 생성
}

void OnDestroy()
{
    if (material != null)
    {
        Destroy(material);  //  사용한 머티리얼을 Destroy
    }
}
//}
생성한 머티리얼은 다 사용한 시점(OnDestroy)에 Destroy합시다. 프로젝트의 규칙이나 사양에 따라 적절한 타이밍에 머티리얼을 Destroy하세요. 

=={practice_asset_animation} Animation
애니메이션은 2D, 3D를 막론하고 많이 사용되는 에셋입니다. 
이번 절에서는 Animation Clip과 Animator에 대한 실습을 소개합니다. 

==={practice_asset_animation_influence} 스킨 웨이트 수 조정하기
모션은 내부적으로 각 버텍스가 어떤 본의 영향을 얼마나 받는지 계산하여 위치를 업데이트하고 있습니다. 
이 위치 계산에 고려하는 뼈의 수를 스킨웨이트 수 또는 인플루언스 수라고 합니다. 
따라서 스킨웨이트 수를 조정하면 부하를 줄일 수 있습니다. 
단, 스킨 웨이트 수를 줄이면 외형이 이상하게 보일 수 있으므로 조정할 때는 검증을 거쳐야 합니다. 

스킨 웨이트 수는 'Project Settings -> Quality'의 Other에서 설정할 수 있습니다. 
//image[practice_asset_animation_influence][스킨 웨이트 조정]
이 설정은 스크립트에서 동적으로 조정할 수도 있습니다. 
따라서 저사양 단말기는 Skin Weights를 2로 설정하고, 고사양 단말기는 4로 설정하는 등의 미세 조정이 가능합니다. 
//listnum[skinweight_settings][SkinWeight 설정 변경][csharp]{
//  QualitySettings를 통째로 바꾸는 방법 
//  인수의 번호는 QualitySettings의 순서대로 0부터 시작합니다.
QualitySettings.SetQualityLevel(0);

//  SkinWeights만 변경하는 방법
QualitySettings.skinWeights = SkinWeights.TwoBones;
//}

==={practice_asset_animation_key_frame_reduction} 키 줄이기
애니메이션 파일은 키 수에 의존하여 저장공간과 런타임 메모리를 압박합니다. 
키 수를 줄이는 방법 중 하나로 Anim. Compression이라는 기능이 있습니다. 
이 옵션은 모델 임포트 설정에서 애니메이션 탭을 선택하면 나타납니다. 
Anim. Compression을 활성화하면 애셋 임포트 시 불필요한 키가 자동으로 삭제됩니다. 
//image[practice_asset_key_frame_reduction][Anim. Compression 설정 화면]
Keyframe Reduction은 값의 변화가 적은 경우 키를 줄입니다. 
구체적으로는 직전 커브와 비교하여 오차(Error) 범위 안에 들어갔을 때 삭제됩니다. 
이 오차 범위는 조정이 가능합니다. 
//image[practice_asset_animation_error][Error 설정]
조금 복잡하지만 Error 설정은 항목에 따라 값의 단위가 다릅니다. 
Rotation은 각도, Position과 Scale은 퍼센트입니다. 
캡처한 이미지의 허용 오차는 Rotation이 0.5도, Position과 Scale은 0.5%가 됩니다. 
자세한 알고리즘은 유니티 문서 @<fn>{animation_error_tolerance}에 있으니 궁금하신 분은 한번 살펴보시기 바랍니다. 
//footnote[animation_error_tolerance][@<href>{https://docs.unity3d.com/Manual/class-AnimationClip.html#tolerance}]

Optimal은 좀 더 이해하기 어려운데, Dense Curve라는 형식과 Keyframe Reduction이라는 두 가지 감소 방법을 비교하여 데이터가 작아지는 쪽을 채택합니다. 
여기서 주의해야 할 점은 Dense는 Keyframe Reduction에 비해 크기가 작아진다는 것입니다. 
하지만 노이즈가 발생하기 쉬워 애니메이션 품질이 떨어질 수 있습니다. 
이 특성을 파악한 후, 실제 애니메이션을 눈으로 확인하여 허용 가능한지 확인해야 합니다. 

==={practice_asset_animation_update_reduction} 업데이트 빈도 줄이기
애니메이터는 기본 설정으로 화면에 보이지 않아도 매 프레임마다 업데이트가 이루어집니다. 
이 업데이트 방식을 변경할 수 있는 컬링 모드라는 옵션이 있습니다. 
//image[practice_asset_animator_cull][컬링 모드]

각 옵션의 의미는 다음과 같습니다. 
//table[animator_culling_mode][컬링 모드 설명]{
종류	의미
-------------------- 
Always Animate	화면 밖에서도 항상 업데이트합니다. (기본 설정)
Cull Update Transform	화면 밖에 있을 때 IK나 Transform을 쓰지 않습니다. @<br>{}스테이트 머신의 업데이트는 수행합니다.
Cull Completely	화면 밖에 있을 때 스테이트 머신을 업데이트하지 않습니다. @<br>{}애니메이션이 완전히 멈춥니다.
//}
각 옵션에 대한 주의 사항이 있습니다. 
먼저 Cull Completely를 설정하는 경우, Root 모션을 사용할 때는 주의가 필요합니다. 
예를 들어 화면 밖에서 프레임 인하는 애니메이션의 경우, 화면 밖에 있기 때문에 애니메이션이 즉시 멈춰버립니다. 
그 결과 언제까지나 프레임 인을 하지 않게 됩니다. 

다음은 Cull Update Transform입니다. 
이 옵션은 Transform의 업데이트를 건너뛰기만 하면 되므로 매우 편리한 옵션으로 느껴집니다. 
하지만 흔들림과 같이 Transform에 의존하는 처리가 있는 경우 주의해야 합니다. 
예를 들어, 캐릭터가 프레임 아웃되면 해당 타이밍의 포즈에서 업데이트가 되지 않습니다. 
그리고 다시 프레임 인할 때 새로운 포즈로 갱신되기 때문에 그 탄력으로 흔들림이 크게 움직일 수 있습니다. 
각 옵션의 장단점을 파악한 후 설정을 변경하는 것이 좋습니다. 

또한, 이러한 설정을 사용하더라도 애니메이션의 업데이트 빈도를 동적으로 세밀하게 변경하는 것은 불가능합니다. 
예를 들어 카메라와 거리가 먼 오브젝트의 애니메이션 업데이트 빈도를 절반으로 줄이는 등의 최적화입니다. 
이 경우 AnimationClipPlayable을 이용하거나 Animator를 비활성화한 후 직접 Animator.Update를 호출해야 합니다. 
둘 다 자체적으로 스크립트를 작성해야 하지만, 전자에 비해 후자가 더 쉽게 도입할 수 있을 것입니다. 

=={practice_asset_particle} Particle System
게임 이펙트는 게임 연출에 필수적인 요소로, 유니티에서는 Particle System을 많이 사용합니다. 
이 장에서는 퍼포먼스 튜닝 관점에서 Particle System의 실패하지 않는 사용법과 주의점에 대해 소개합니다. 

중요한 것은 다음 두 가지입니다. 

 * 파티클 개수 줄이기
 * 노이즈가 많으므로 주의한다.

==={practice_asset_emit_count} 파티클 개수 줄이기
Particle System은 CPU에서 동작하는 파티클(CPU 파티클)이기 때문에 파티클 수가 많을수록 CPU 부하가 높아집니다. 
기본 방침으로 필요한 최소한의 파티클 수로 설정하는 것이 좋습니다. 필요에 따라 개수를 조정해 보세요. 

파티클 개수를 제한하는 방법은 두 가지가 있습니다. 

 * Emission 모듈의 방출 개수 제한
 * 메인 모듈의 Max Particles에서 최대 방출 개수 제한

//image[practice_asset_emit_count][Emission 모듈에서 방출 개수 제한][scale=1.0]

  * Rate over Time: 초당 방출하는 개수
 * Bursts > Count: 버스트 타이밍에 방출하는 개수

이 설정들을 조정하여 필요한 최소한의 파티클 수가 되도록 설정합니다. 

//image[practice_asset_max_particles][Max Particles로 방출 개수 제한][scale=1.0]
 또 다른 방법은 메인 모듈의 Max Particles입니다. 위 예시에서는 1000개 이상의 파티클은 방출되지 않습니다. 

===={practice_asset_subemitters} Sub Emitters에도 주의
파티클 수를 줄이는 데 있어 서브 이미터(Sub Emitters) 모듈도 주의해야 합니다. 
//image[practice_asset_subemitters][Sub Emitters 모듈][scale=1.0]
 Sub Emitters 모듈은 특정 타이밍(생성 시, 수명이 다했을 때 등)에 임의의 Particle System을 생성하는데, Sub Emitters의 설정에 따라서는 한꺼번에 피크 수에 도달할 수 있으므로 사용 시 주의해야 합니다. 

==={practice_asset_noise} 노이즈가 심하므로 주의
//image[practice_asset_noise][Noise 모듈][scale=1.0]

 Noise(노이즈) 모듈의 Quality는 부하가 걸리기 쉽습니다. 
노이즈는 유기적인 파티클을 표현할 수 있고, 간편하게 이펙트의 퀄리티를 높일 수 있어 많이 사용됩니다. 
자주 사용하는 기능인 만큼 성능에 신경을 써야 하는 부분입니다. 

//image[practice_asset_noise_quality][Noise 모듈의 Quality][scale=1.0]

  * Low(1D)
 * Midium(2D)
 * High(3D)

Quality는 차원이 올라갈수록 부하는 올라갑니다. 만약 노이즈가 필요하지 않다면 노이즈 모듈을 끄면 됩니다. 또한 노이즈를 사용해야 한다면 Quality의 설정은 Low를 우선으로 하고, 요구사항에 따라 Quality를 높여가면 됩니다. 

=={practice_asset_audio} Audio
사운드 파일을 임포트한 기본 상태는 성능적으로 개선할 점이 있습니다. 설정 항목은 다음 세 가지입니다. 

 * Load Type
 * Compression Format
 * Force To Mono

게임 개발에서 자주 사용하는 BGM, 효과음, 보이스에 맞게 설정해 보세요. 

==={practice_asset_audio_load_type} Load Type
사운드 파일(AudioClip)을 로드하는 방법은 3가지가 있습니다. 
//image[practice_asset_audio_load_type][AudioClip LoadType][scale=1.0]

  * Decompress On Load
 * Compressed In Memory
 * 스트리밍

===={practice_asset_audio_decomplress_on_load} Decompress On Load
Decompress On Load는 비압축으로 메모리에 로드합니다. 
CPU 부하가 낮기 때문에 대기 시간이 짧게 재생됩니다. 
반면 메모리를 많이 사용합니다. 

음계가 짧고 빠르게 재생되는 효과음에 추천합니다. 
BGM이나 음계가 긴 음성 파일에서 사용하면 메모리를 많이 사용하게 되므로 주의가 필요합니다. 

===={practice_asset_audio_complress_in_memory} Compressed In Memory
Compressed In Memory는 AudioClip을 압축된 상태로 메모리에 로드합니다. 
재생할 때 압축을 풀어서 재생하는 방식입니다. 
즉, CPU 부하가 높고 재생 지연이 발생하기 쉽습니다. 

파일 사이즈가 크고, 메모리에 그대로 펼쳐놓고 싶지 않은 사운드나 약간의 재생 지연이 문제가 되지 않는 사운드에 적합합니다. 
음성으로 사용하는 경우가 많습니다. 

===={practice_asset_audiostreaming} Streaming
Streaming은 이름에서 알 수 있듯이 로드하면서 재생하는 방식입니다. 
메모리 사용량은 적은 대신 CPU 부하가 높습니다. 
장시간의 배경음악에 사용하는 것을 추천합니다. 


//table[loadtype][로딩 방법 및 주요 사용 용도 정리]{
종류	용도
-------------------- 
Decompress On Load	효과음
Compressed In Memory	음성
Streaming	BGM
//}

==={practice_asset_audio_compression_format} Compression Format
Compression format은 AudioClip 자체의 압축 포맷입니다. 

//image[practice_asset_audio_compression_format][AudioClip Compression Format][scale=1.0]

===={practice_asset_pcm} PCM
비압축이며, 메모리를 많이 소모합니다. 음질에 대한 요구가 매우 높은 경우가 아니라면 설정하지 않는 것이 좋습니다. 

===={practice_asset_adpcm} ADPCM
PCM에 비해 메모리 사용량은 70% 감소하지만 음질은 떨어지며, Vorbis에 비해 CPU 부하가 현저히 적은 것이 특징입니다. 
즉, 전개 속도가 빠르기 때문에 즉각적인 재생이나 대량으로 재생하는 사운드에 적합하다. 
구체적으로 발소리, 충돌음, 무기 등 노이즈가 많으면서도 대량으로 재생해야 하는 사운드에 적합하다. 

===={practice_asset_vorbis} Vorbis
비가역 압축 포맷이기 때문에 PCM보다 음질은 떨어지지만, 파일 크기는 작아집니다. 유일하게 Quality를 설정할 수 있어 미세 조정이 가능합니다. 
모든 사운드(BGM, 효과음, 음성)에서 가장 많이 사용되는 압축 형식입니다. 

//table[compressionformat][압축 방법 및 주요 사용 용도 정리]{
종류	용도
-------------------- 
PCM	사용하지 않음
ADPCM	효과음
Vorbis	BGM, 효과음, 음성
//}

==={practice_asset_sample_rate} 샘플 레이트 지정
샘플 레이트를 지정하여 품질을 조절할 수 있습니다. 모든 압축 포맷을 지원합니다. 
Sample Rate Setting에서 3가지 방법을 선택할 수 있습니다. 

//image[practice_asset_audio_sample_rate_setting][Sample Rate Settings][scale=1.0]

===={practice_asset_preserve_sample_rate} Preserve Sample Rate
기본 설정입니다. 원본 음원의 샘플 레이트가 채택됩니다. 

===={practice_asset_optimize_sample_rate} Optimize Sample Rate
Unity 측에서 분석하여 가장 높은 주파수 성분을 기준으로 자동 최적화합니다. 

===={practice_asset_override_sample_rate} Override Sample Rate
원본 음원의 샘플 레이트를 덮어씁니다. 8,000~192,000Hz까지 지정할 수 있습니다. 원본 음원보다 높게 설정해도 음질은 올라가지 않습니다. 원본 음원보다 샘플 레이트를 낮추고 싶을 때 사용합니다. 


==={practice_asset_audio_force_to_mono} 효과음은 Force To Mono 설정
Unity는 기본 상태에서 스테레오로 재생되지만, Force To Mono를 활성화하면 모노로 재생됩니다. 모노 재생을 강제하면 좌우 각각의 데이터를 보유하지 않아도 되므로 파일 크기와 메모리 크기가 절반으로 줄어듭니다. 
//image[practice_asset_audio_force_to_mono][AudioClip Force To Mono][scale=1.0]

 효과음은 모노로 재생해도 문제가 없는 경우가 많습니다. 또한 3D 사운드도 모노 재생이 더 좋은 경우도 있습니다. 고려해서 Force To Mono를 활성화하는 것이 좋습니다. 
퍼포먼스 튜닝 효과도 먼지가 쌓이면 산더미처럼 쌓입니다. 모노 재생에 문제가 없다면 적극적으로 Force To Mono를 활용하세요. 

//info{
성능 튜닝과는 조금 다른 이야기지만, 음성 파일은 무압축으로 유니티로 가져오는 것이 좋습니다. 
압축된 것을 가져오면 Unity 측에서 디코딩 & 재압축을 하기 때문에 품질 저하가 발생합니다. 
//}

=={practice_asset_special_folder} Resources / StreamingAssets
프로젝트에는 특별한 폴더가 존재합니다. 특히 다음 두 가지는 성능 측면에서 주의가 필요합니다. 

 * Resources 폴더
 * StreamingAssets 폴더

일반적으로 Unity는 씬, 머티리얼, 스크립트 등에서 참조된 오브젝트만 빌드에 포함한다. 

//listnum[practice_asset_special_folder_script_reference][스크립트에서 참조된 오브젝트 예시][csharp]{
//  참조된 오브젝트는 빌드에 포함된다.
[SerializeField] GameObject sample;
//}

앞의 특수 폴더는 규칙이 다릅니다. 저장한 파일은 빌드에 포함된다. 즉, 실제로 필요하지 않은 파일도 저장되어 있으면 빌드에 포함되어 빌드 크기가 커질 수 있다. 

문제는 프로그램에서 확인할 수 없다는 것이다. 불필요한 파일을 육안으로 확인해야 하므로 시간이 오래 걸린다. 이러한 폴더에는 주의해서 파일을 추가해야 한다. 

하지만 프로젝트가 진행되면서 저장 파일은 계속 늘어날 수밖에 없다. 그 중에는 더 이상 사용하지 않는 불필요한 파일이 섞여 있을 수도 있습니다. 결론적으로 정기적인 저장 파일 검토를 추천한다. 

==={practice_asset_resources} 시작 시간을 느리게 하는 Resources 폴더
Resources 폴더에 많은 양의 오브젝트를 저장하면 앱 실행 시간이 길어집니다. 
Resources 폴더는 문자열 참조로 오브젝트를 로드할 수 있는 구식 편의 기능입니다. 

//listnum[practice_asset_special_folder_resources][스크립트에서 참조된 오브젝트 예시][csharp]{
var object = Resources.Load("aa/bb/cc/obj");
//}
다음과 같은 코드로 오브젝트를 로드할 수 있습니다. 
Resources 폴더에 저장해두면 스크립트에서 오브젝트에 접근할 수 있기 때문에 자주 사용하게 된다. 
하지만 Resources 폴더에 너무 많이 넣으면 앞서 언급했듯이 앱 실행 시간이 길어집니다. 
Unity 시작 시 전체 Resources 폴더의 구조를 분석하여 조회 테이블을 생성하기 때문입니다. 
가급적 Resources 폴더의 사용을 최소화하는 것이 좋습니다. 

=={practice_asset_scriptable_object} ScriptableObject

ScriptableObject는 YAML의 에셋으로, 텍스트 형식으로 파일 관리하고 있는 프로젝트가 많을 것입니다. 
명시적으로@<code>{[PreferBinarySerialization]} Attribute를 지정하여 저장 형식을 바이너리 형식으로 변경할 수 있습니다. 
주로 데이터 양이 많은 애셋의 경우, 바이너리 형식으로 저장하면 쓰기/읽기 성능이 향상됩니다. 

하지만 당연히 바이너리 포맷의 경우 병합 툴 등에서 다루기 어려워집니다. 애셋 업데이트가 덮어쓰기만 하면 되는 
변경 차이를 텍스트로 확인할 필요가 없는 애셋이나, 게임 개발이 완료되어 더 이상 데이터 변경이 이루어지지 않는 애셋의 경우 
을 적극적으로 지정하는 것이 좋습니다. @<code>{[PreferBinarySerialization]} 를 적극적으로 지정하는 것이 좋다. 

//info{
ScriptableObject를 사용할 때 흔히 범하기 쉬운 실수는 클래스 이름과 소스 코드의 파일 이름 불일치입니다. 
클래스와 파일은 반드시 같은 이름을 사용해야 합니다. 
클래스 생성 시 이름 지정에 주의하면서 @<code>{.asset}파일이 올바르게 직렬화되어 
바이너리 형식으로 저장되어 있는지 확인하자. 
//}

//listnum[scriptable_object_sample][ScriptableObject 구현 예시][csharp]{
/*
*  소스코드 파일명이 ScriptableObjectSample.cs인 경우
*/

//  시리얼라이즈 성공
[PreferBinarySerialization]
public sealed class ScriptableObjectSample : ScriptableObject
{
    ...
}

//  직렬화 실패
[PreferBinarySerialization]
public sealed class MyScriptableObject : ScriptableObject
{
    ...
}
//}
