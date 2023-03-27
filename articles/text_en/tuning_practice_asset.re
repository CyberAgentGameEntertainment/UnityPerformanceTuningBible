={practice_asset} Tuning Practice - Asset
Game production involves handling a large number of different types of assets such as textures, meshes, animations, and sounds. 
This chapter provides practical knowledge about these assets, including settings to keep in mind when tuning performance. 

=={practice_asset_texture} Texture
Image data, which is the source of textures, is an indispensable part of game production. 
On the other hand, it consumes a relatively large amount of memory, so it must be configured appropriately. 

==={practice_asset_texture_import} Import Settings
@<img>{texture_settings} is the import settings for textures in Unity. 
//image[texture_settings][Texture Settings][scale=0.75]

 Here are some of the most important ones to keep in mind. 

==={practice_asset_texture_read_write} Read/Write
This option is disabled by default. If disabled, textures are only expanded in GPU memory. 
If enabled, it will be copied not only to GPU memory but also to main memory, thus doubling the consumption. 
Therefore, if you do not use APIs such as @<code>{Texture.GetPixel} or @<code>{Texture.SetPixel} and only use Shader to access textures, be sure to disable them. 

Also, for textures generated at runtime, set makeNoLongerReadable to true as shown at @<list>{makeNoLongerReadable} to avoid copying to main memory. 
//listnum[makeNoLongerReadable][Setting makeNoLongerReadable][csharp]{
texture2D.Apply(updateMipmaps, makeNoLongerReadable: true)
//}

//info{
Since transferring textures from GPU memory to main memory is time-consuming, performance is improved by deploying textures to both if they are readable. 
//}

==={practice_asset_texture_mipmap} Generate Mip Maps
Enabling the Mip Map setting increases texture memory usage by about 1.3 times. 
This setting is generally used for 3D objects to reduce jaggies and texture transfer for distant objects. 
It is basically unnecessary for 2D sprites and UI images, so it should be disabled. 

==={practice_asset_texture_aniso} Aniso Level
Aniso Level is a function to render textures without blurring when objects are rendered at shallow angles. 
This function is mainly used for objects that extend far, such as the ground or floor. 
The higher the Aniso Level value, the more benefit it provides, but at a higher processing cost. 
//image[aniso_level_compare][Aniso Level adaptation image][scale=1.0]

 The Aniso Level can be set from 0 to 16, but it has a slightly special specification. 

 * 0: Always disabled regardless of project settings
 * 1: Basically disabled. However, if the project setting is Forced On, the value is clamped to 9~16.
 * Otherwise: Set at that value

When textures are imported, the value is 1 by default. 
Therefore, the Forced On setting is not recommended unless you are targeting a high-spec device. 
Forced On can be set from "Anisotropic Textures" in "Project Settings -> Quality". 

//image[project_settings_aniso][Forced On Settings]

Make sure that the Aniso Level setting is not enabled for objects that have no effect, or that it is not set too high for objects that do have an effect. 

//info{
The effect of Aniso Level is not linear, but rather switches in steps. 
The author verified that it changes in four steps: 0~1, 2-3, 4~7, and 8 or later. 
//}

==={practice_asset_texture_compress} Compression Settings
Textures should be compressed unless there is a specific reason not to. 
If you find uncompressed textures in your project, it may be human error or lack of regulation. Check it out as soon as possible. 
More information on compression settings can be found at @<hd>{basic|basic_asset_data_compression}. 

We recommend using TextureImporter to automate these compression settings to avoid human error. 
//listnum[textureImporter][Example of TextureImporter automation][csharp]{
using UnityEditor;

public class ImporterExample : AssetPostprocessor
{
    private void OnPreprocessTexture()
    {
        var importer = assetImporter as TextureImporter;
        //  Read/Write settings, etc. are also possible.
        importer.isReadable = false;

        var settings = new TextureImporterPlatformSettings();
        //  Specify Android = "Android", PC = "Standalone
        settings.name = "iPhone";
        settings.overridden = true;
        settings.textureCompression = TextureImporterCompression.Compressed;
        //  Specify compression format
        settings.format = TextureImporterFormat.ASTC_6x6;
        importer.SetPlatformTextureSettings(settings);
    }
}
//}

Not all textures need to be in the same compression format. 
For example, among UI images, images with overall gradations tend to show a noticeable quality loss due to compression. 
In such cases, it is recommended to set a lower compression ratio for only some of the target images. 
On the other hand, for textures such as 3D models, it is difficult to see the quality loss, so it is best to find an appropriate setting such as a high compression ratio. 

=={practice_asset_mesh} Mesh
The following are points to keep in mind when dealing with mesh (models) imported into Unity. Performance of imported model data can be improved depending on the settings. The following four points should be noted. 

 * Read/Write Enabled
 * Vertex Compression
 * Mesh Compression
 * Optimize Mesh Data

==={practice_asset_mesh_read_write_enabled} Read/Write Enabled
The first mesh note is Read/Write Enabled. 
This option in the model inspector is disabled by default. 
//image[practice_asset_mesh_inspector][Read/Write Settings]
If you do not need to access the mesh during runtime, you should disable it. Specifically, if the model is placed on Unity and used only to play an AnimationClip, Read/Write Enabled is fine to disable. 

Enabling Read/Write Enabled will consume twice as much memory because information accessible by the CPU is stored in memory. Please check it out, as simply disabling it will save memory. 

==={practice_asset_vertex_compression} Vertex Compression
Vertex Compression is an option that changes the precision of mesh vertex information from float to half. 
This @<em>{can reduce memory usage and file size at runtime}. 
The default setting is as follows. 
//image[practice_asset_vertex_compression_settings][Default settings for Vertex Compression]

However, please note that Vertex Compression is disabled under the following conditions 

 * Read/Write is enabled
 * Mesh Compression is enabled
 * Mesh with Dynamic Batching enabled and adaptable (less than 300 vertices and less than 900 vertex attributes)

==={practice_asset_mesh_compression} Mesh Compression
Mesh Compression allows you to change the compression ratio of the mesh. The higher the compression ratio, the smaller the file size and the less storage space is required. 
Compressed data is decompressed at runtime. Therefore, @<em>{memory usage at runtime is not affected}. 

Mesh Compresssion offers four compression settings. 

 * Off: Uncompressed
 * Low: Low compression
 * Medium: Medium compression
 * High: High compression

//image[practice_asset_mesh_compression][Mesh Compression]

As mentioned in @<hd>{practice_asset_vertex_compression}, enabling this option @<em>{disables Vertex Compression}. 
Especially for projects with strict memory usage limitations, please be aware of this disadvantage before setting this option. 

==={practice_asset_optimize_mesh_data} Optimize Mesh Data
Optimize Mesh Data is a function that automatically deletes unnecessary vertex data from the mesh. 
Unnecessary vertex data is automatically determined based on the shader used. 
This has the effect of reducing both memory and storage at runtime. 
The setting can be configured in "Other" under "Project Settings -> Player". 
//image[practice_asset_optimize_mesh_data][Optimize Mesh Data setting][scale=0.8]

 This option is useful because it automatically deletes vertex data, but be aware that it may cause unexpected problems. 
For example, when switching between Material and Shader at runtime, the properties accessed may be deleted, resulting in incorrect rendering results. 
When bundling only Mesh assets, the incorrect Material settings may result in unnecessary vertex data. 
This is common in cases where only a mesh reference is provided, such as in the Particle System. 

=={practice_asset_material} Material
Material is an important function that determines how an object is rendered. 
Although it is a familiar feature, it can easily cause memory leaks if used incorrectly. 
In this section, we will show you how to use materials safely. 

===={practice_asset_material_clone} Simply accessing a parameter will duplicate it.
The most important thing to remember about materials is that they can be duplicated simply by accessing their parameters. And it is hard to notice that it is being duplicated. 

Take a look at the following code 
//listnum[material_clone][Example of Material being duplicated][csharp]{
Material material;

void Awake()
{
    material = renderer.material;
    material.color = Color.green;
}
//}

This is a simple process of setting the material's color property to Color.green. The renderer's material is duplicated. And the duplicated object must be explicitly Destroyed. 
//listnum[material_destroy][Example of deleting a duplicated Material][csharp]{
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
Destroying a duplicated material in this way avoids memory leaks. 

===={practice_asset_material_instantiate} Let's thoroughly clean up generated materials.
Dynamically generated materials are another common cause of memory leaks. Make sure to Destroy generated materials when you are done using them. 

Take a look at the following sample code. 
//listnum[material_dynamic][Example of deleting a dynamically generated material][csharp]{
Material material;

void Awake()
{
    material = new Material();  //  Dynamically generated material
}

void OnDestroy()
{
    if (material != null)
    {
        Destroy(material);  //  Destroying a material when you have finished using it
    }
}
//}
Materials should be destroyed when they are finished being used (OnDestroy). Destroy materials at the appropriate timing according to the rules and specifications of the project. 

=={practice_asset_animation} Animation
Animation is a widely used asset in both 2D and 3D. 
This section introduces practices related to animation clips and animators. 

==={practice_asset_animation_influence} Adjusting the number of skin weights
Internally, motion updates the position of each vertex by calculating how much of each bone is affected by each vertex. 
The number of bones taken into account in the position calculation is called the skinweight or influence count. 
Therefore, the load can be reduced by adjusting the number of skin weights. 
However, reducing the number of skin weights may result in a strange appearance, so be sure to verify this when adjusting the number of skin weights. 

The number of skin weights can be set from "Other" under "Project Settings -> Quality. 
//image[practice_asset_animation_influence][Adjusting Skin Weight]
This setting can also be adjusted dynamically from a script. 
Therefore, it is possible to set Skin Weights to 2 for low-spec devices and 4 for high-spec devices, and so on, for fine-tuning. 
//listnum[skinweight_settings][Changing SkinWeight settings][csharp]{
//  How to switch QualitySettings entirely 
//  The argument number is the order of the QualitySettings, starting with 0.
QualitySettings.SetQualityLevel(0);

//  How to change only SkinWeights
QualitySettings.skinWeights = SkinWeights.TwoBones;
//}

==={practice_asset_animation_key_frame_reduction} Reducing Keys
Animation files are dependent on the number of keys, which can be a drain on storage and memory at run-time. 
One way to reduce the number of keys is to use the Anim. Compression feature. 
This option can be found by selecting the Animation tab from the model import settings. 
When Anim. Compression is enabled, unnecessary keys are automatically removed during asset import. 
//image[practice_asset_key_frame_reduction][Anim. Compression settings screen]
Keyframe Reduction reduces keys when there is little change in value. 
Specifically, keys are removed when they are within the Error range compared to the previous curve. 
This error range can be adjusted. 
//image[practice_asset_animation_error][Error Settings]
The Error settings are a bit complicated, but the units of the Error settings differ depending on the item. 
Rotation is in degrees, while Position and Scale are in percent. 
The tolerance for a captured image is 0.5 degrees for Rotation, and 0.5% for Position and Scale. 
The detailed algorithm can be found in the Unity documentation at @<fn>{animation_error_tolerance}, so please take a peek if you are interested. 
//footnote[animation_error_tolerance][@<href>{https://docs.unity3d.com/Manual/class-AnimationClip.html#tolerance}]

Optimal is even more confusing, but it compares two reduction methods, the Dense Curve format and Keyframe Reduction, and uses the one with the smaller data. 
The key point to keep in mind is that Dense Curve is smaller in size than Keyframe Reduction. 
However, it tends to be noisy, which may degrade the animation quality. 
After understanding this characteristic, let's visually check the actual animation to see if it is acceptable. 

==={practice_asset_animation_update_reduction} Reduction of update frequency
By default, Animator updates every frame even if the animation is not on screen. 
There is an option called Culling Mode that allows you to change this update method. 
//image[practice_asset_animator_cull][Culling Mode]

The meaning of each option is as follows 
//table[animator_culling_mode][Description of culling mode]{
Type	Meaning
-------------------- 
Always Animate	Always update even when off-screen. (Default setting)
Cull Update Transform	Do not write IK or Transform when off-screen. @<br>{} State machine updates are performed.
Cull Completely	No state machine updates are performed when off-screen. @<br>{} Animation stops completely.
//}
There are a few things to note about each option. 
First, be careful when using Root motion when setting Cull Completely. 
For example, if you have an animation that frames in from off-screen, the animation will stop immediately because it is off-screen. 
As a result, the animation will not frame in forever. 

Next is Cull Update Transform. 
This seems like a very useful option, since it only skips updating the transform. 
However, be careful if you have a shaking or other Transform-dependent process. 
For example, if a character goes out of frame, no updates will be made from the pose at that time. 
When the character enters the frame again, it will be updated to a new pose, which may cause the shaking object to move significantly. 
It is a good idea to understand the pros and cons of each option before changing the settings. 

Also, even with these settings, it is not possible to dynamically change the frequency of animation updates in detail. 
For example, you can optimize the frequency of animation updates by halving the frequency of animation updates for objects that are farther away from the camera. 
In this case, you need to use AnimationClipPlayable or deactivate Animator and call Animator.Update yourself. 
Both require writing your own scripts, but the latter is easier to implement than the former. 

=={practice_asset_particle} Particle System
Game effects are essential for game presentation, and Unity often uses the Particle System. 
In this chapter, we will introduce how to use the Particle System from the perspective of performance tuning and what to keep in mind to avoid mistakes. 

The following two points are important. 

 * Keep the number of particles low.
 * Be aware that noise is heavy.

==={practice_asset_emit_count} Reduce the number of particles
The number of particles is related to the load, and since the Particle System is CPU-powered (CPU particles), the more particles there are, the higher the CPU load. 
As a basic policy, set the number of particles to the minimum necessary. Adjust the number of particles as needed. 

There are two ways to limit the number of particles. 

 * Limit the number of particles emitted by the Emission module
 * Limit the maximum number of particles in the Max Particles main module

//image[practice_asset_emit_count][Limit on the number of emissions in the Emission module][scale=1.0]

  * Rate over Time: Number of parts emitted per second
 * Bursts > Count: Number of particles to be emitted at burst timing

Adjust these settings to achieve the minimum number of particles required. 

//image[practice_asset_max_particles][Limiting the number of particles emitted with Max Particles][scale=1.0]
 Another way is to use Max Particles in the main module. In the example above, particles over 1000 will not be emitted. 

===={practice_asset_subemitters} Be careful with Sub Emitters
The Sub Emitters module should also be considered when reducing the number of particles. 
//image[practice_asset_subemitters][Sub Emitters Module][scale=1.0]
 The Sub Emitters module generates arbitrary particle systems at specific times (at creation, at the end of life, etc.) Depending on the Sub Emitters settings, the number of particles may reach the peak number at once, so be careful when using this module. 

==={practice_asset_noise} Note that noise is heavy.
//image[practice_asset_noise][Noise Module][scale=1.0]

 The Noise module's Quality is easily overloaded. 
Noise can express organic particles and is often used to easily increase the quality of effects. 
Because it is a frequently used function, you should be careful about its performance. 

//image[practice_asset_noise_quality][Quality of the Noise module][scale=1.0]

  * Low (1D)
 * Midium (2D)
 * High (3D)

The higher the dimension of Quality, the higher the load. If you do not need Noise, turn off the Noise module. If you need to use noise, set the Quality setting to Low first, and then increase the Quality according to your requirements. 

=={practice_asset_audio} Audio
The default state with sound files imported has some improvement points in terms of performance. The following three settings are available. 

 * Load Type
 * Compression Format
 * Force To Mono

Set these settings appropriately for background music, sound effects, and voices that are often used in game development. 

==={practice_asset_audio_load_type} Load Type
There are three types of ways to load a sound file (AudioClip). 
//image[practice_asset_audio_load_type][AudioClip LoadType][scale=1.0]

  * Decompress On Load
 * Compressed In Memory
 * Streaming

===={practice_asset_audio_decomplress_on_load} Decompress On Load
Decompress On Load loads uncompressed video into memory. 
It is less CPU-intensive, so playback is performed with less wait time. 
On the other hand, it uses a lot of memory. 

It is recommended for short sound effects that require immediate playback. 
BGM and long voice files use a lot of memory, so care should be taken when using this function. 

===={practice_asset_audio_complress_in_memory} Compressed In Memory
Compressed In Memory loads an AudioClip into memory in a compressed state. 
This means that it is decompressed at the time of playback. 
This means that the CPU load is high and playback delays are likely to occur. 

It is suitable for sounds with large file sizes that you do not want to decompress directly into memory, or for sounds that do not suffer from a slight playback delay. 
It is often used for voice. 

===={practice_asset_audiostreaming} Streaming
Streaming, as the name implies, is a method of loading and playing back sounds. 
It uses less memory, but is more CPU-intensive. 
It is recommended for use with long BGM. 


//table[loadtype][Summary of Loading Methods and Main Uses]{
Type	Usage
-------------------- 
Decompress On Load	Sound Effects
Compressed In Memory	Voice
Streaming	BGM
//}

==={practice_asset_audio_compression_format} Compression Format
Compression format is the compression format of the AudioClip itself. 

//image[practice_asset_audio_compression_format][AudioClip Compression Format][scale=1.0]

===={practice_asset_pcm} PCM
Uncompressed and consumes a large amount of memory. Do not set this unless you want the best sound quality. 

===={practice_asset_adpcm} ADPCM
Uses 70% less memory than PCM, but the quality is lower, and the CPU load is much smaller than with Vorbis. 
The CPU load is much lower than Vorbis, which means that the speed of decompression is faster, making it suitable for immediate playback and for sounds that are played back in large quantities. 
This is especially true for noisy sounds such as footsteps, collisions, weapons, etc., that need to be played back quickly and in large quantities. 

===={practice_asset_vorbis} Vorbis
As a lossy compression format, the quality is lower than PCM, but the file size is smaller. It is the only format that allows for fine-tuning of the sound quality. 
It is the most used compression format for all sounds (background music, sound effects, voices). 

//table[compressionformat][Summary of Compression Methods and Main Uses]{
Type	Usage
-------------------- 
PCM	Not used
ADPCM	Sound Effects
Vorbis	BGM, sound effects, voice
//}

==={practice_asset_sample_rate} Sample Rate
Quality can be adjusted by specifying the sample rate. All compressed formats are supported. 
Three methods can be selected from Sample Rate Setting. 

//image[practice_asset_audio_sample_rate_setting][Sample Rate Settings][scale=1.0]

===={practice_asset_preserve_sample_rate} Preserve Sample Rate
Default setting. The sample rate of the original sound source is used. 

===={practice_asset_optimize_sample_rate} Optimize Sample Rate
Analyzed by Unity and automatically optimized based on the highest frequency component. 

===={practice_asset_override_sample_rate} Override Sample Rate
Override the sample rate of the original sound source. 8,000 to 192,000 Hz can be specified. The quality will not be improved even if the sample rate is higher than the original source. Use this option when you want to lower the sample rate than the original sound source. 


==={practice_asset_audio_force_to_mono} Set Force To Mono for sound effects.
By default, Unity plays stereo, but by enabling Force To Mono, mono playback is enabled. Enabling mono playback will cut the file size and memory size in half, since there is no need to have separate data for left and right channels. 
//image[practice_asset_audio_force_to_mono][AudioClip Force To Mono][scale=1.0]

 Mono playback is often fine for sound effects. In some cases, mono playback is also better for 3D sound. It is recommended to enable Force To Mono after careful consideration. 
The performance tuning effect is a mountain out of a molehill. If you have no problem with monaural playback, you should actively use Force To Mono. 

//info{
Although this is not the same as performance tuning, uncompressed audio files should be imported into Unity. 
If you import compressed audio files, they will be decoded and recompressed on the Unity side, resulting in a loss of quality. 
//}

=={practice_asset_special_folder} Resources / StreamingAssets
There are special folders in the project. The following two in particular require attention from a performance standpoint. 

 * Resources folder
 * StreamingAssets folder

Normally, Unity only includes objects referenced by scenes, materials, scripts, etc. in a build. 

//listnum[practice_asset_special_folder_script_reference][Example of an object referenced by a script][csharp]{
//  Referenced objects are included in the build
[SerializeField] GameObject sample;
//}

The rules are different for the special folders mentioned above. Stored files are included in the build. This means that even files that are not actually needed are included in the build if they are stored, leading to an expansion of the build size. 

The problem is that it is not possible to check from the program. You have to visually check for unnecessary files, which is time consuming. Be careful adding files to these folders. 

However, the number of stored files will inevitably increase as the project progresses. Some of these files may be mixed in with unnecessary files that are no longer in use. In conclusion, we recommend that you review your stored files on a regular basis. 

==={practice_asset_resources} Resources folder slows down startup time
Storing a large number of objects in the Resources folder will increase application startup time. 
The Resources folder is an old-fashioned convenience feature that allows you to load objects by string reference. 

//listnum[practice_asset_special_folder_resources][Example of an object referenced in a script][csharp]{
var object = Resources.Load("aa/bb/cc/obj");
//}
You can load objects with code like this 
It is easy to overuse the Resources folder because you can access objects from scripts by storing them in the Resources folder. 
However, overloading the Resources folder will increase the startup time of the application, as mentioned above. 
The reason for this is that when Unity starts up, it analyzes the structure in all Resources folders and creates a lookup table. 
It is best to minimize the use of the Resources folder as much as possible. 

=={practice_asset_scriptable_object} ScriptableObject

ScriptableObjects are YAML assets, and many projects are likely to manage their files as text files. 
By explicitly specifying an @<code>{[PreferBinarySerialization]} Attribute to change the storage format to binary. 
For assets that are mainly large amounts of data, binary format improves the performance of write and read operations. 

However, binary format is naturally more difficult to use with merge tools. For assets that need to be updated only by overwriting the asset, such as those for which there is no need to check the text for changes ( 
), or for assets whose data is no longer being changed after game development is complete, it is recommended to use 
. @<code>{[PreferBinarySerialization]} ScriptableObjects are not required to be used with ScriptableObjects. 

//info{
A common mistake when using ScriptableObjects is mismatching class names and source code file names. 
The class and file must have the same name. 
Be careful with naming when creating classes and make sure that the @<code>{.asset} file is correctly serialized and saved in the 
binary format. 
//}

//listnum[scriptable_object_sample][Example Implementation of ScriptableObject][csharp]{
/*
*  When the source code file is named ScriptableObjectSample.cs
*/

//  Serialization succeeded
[PreferBinarySerialization]
public sealed class ScriptableObjectSample : ScriptableObject
{
    ...
}

//  Serialization Failure
[PreferBinarySerialization]
public sealed class MyScriptableObject : ScriptableObject
{
    ...
}
//}
