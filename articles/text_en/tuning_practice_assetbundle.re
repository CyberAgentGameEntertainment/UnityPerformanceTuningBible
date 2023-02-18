={practice_assetbundle} Tuning Practice - AssetBundle

Problems in AssetBundle configuration can cause many problems, such as wasting valuable communication and storage space for the user, as well as hindering the 
comfortable game play. 
This chapter describes the configuration and implementation policies for AssetBundle. 

=={practice_assetbundle_granularity} Granularity of AssetBundle

The granularity of the AssetBundle should be carefully considered due to dependency issues. 
At the extreme, there are two ways to do this: put all assets in one AssetBundle, or put each asset in one AssetBundle. 
Both methods are simple, but the former method has a critical problem. 
The former method is simple, but the former method has a fatal problem: even if you only add assets or update one asset, you have to recreate the entire file and distribute it. 
If the total amount of assets is in GB, the update load is very high. 

Therefore, the method of dividing the AssetBundle as much as possible is chosen, but if it is too detailed, it will cause overhead in various areas. 
Therefore, we basically recommend the following policy for making AssetBundle. 

 * Assets that are supposed to be used at the same time should be combined into a single AssetBundle.
 * Assets that are referenced by multiple assets should be in separate AssetBundles.

It is difficult to control perfectly, but it is a good idea to set some rules regarding granularity within the project. 

=={practice_assetbundle_load} Load API for AssetBundle

There are three types of APIs for loading assets from AssetBundle. 


 : @<code>{AssetBundle.LoadFromFile}
 Load by specifying the file path that exists in storage. 
 This is usually used because it is the fastest and most memory-efficient. 

 : @<code>{AssetBundle.LoadFromMemory}
 Load by specifying the AssetBundle data already loaded in memory. 
 While using AssetBundle, a very large amount of data needs to be maintained in memory, and the memory load is very large. 
 For this reason, it is not normally used. 

 : @<code>{AssetBundle.LoadFromStream}
 Load by specifying @<code>{Stream} which returns the AssetBundle data. 
 When loading an encrypted AssetBundle while decrypting it, use this API in consideration of the memory load. 
 However, since @<code>{Stream} must be seekable, be careful not to use a cipher algorithm that cannot handle seek. 

=={practice_assetbundle_unload} AssetBundle unloading strategy

If AssetBundle is not unloaded when it is no longer needed, it will overwhelm memory. 
The argument @<code>{unloadAllLoadedObjects} of @<code>{AssetBundle.Unload(bool unloadAllLoadedObjects)}, which is the API to be used in this case, 
is very important and should be decided how to set it up at the beginning of development. 
If this argument is true, when unloading an AssetBundle, all assets loaded from that AssetBundle will also be unloaded. 
If false, no assets are unloaded. 

In other words, the true case, which requires the AssetBundle to be loaded continuously while the assets are being used, is more memory-intensive, but it is also safer because it ensures that the assets are destroyed. 
On the other hand, the false case has a low memory load because the AssetBundle can be unloaded when the asset is finished loading. 
However, forgetting to unload the used assets can lead to memory leaks or cause the same asset to be loaded multiple times in memory, so proper memory management is required. Therefore, proper memory management is required. 
In general, strict memory management is severe, so @<code>{AssetBundle.Unload(true)} is recommended if memory load is sufficient. 

=={practice_assetbundle_loadcount} Optimization of the number of simultaneously loaded AssetBundles

In the case of @<code>{AssetBundle.Unload(true)}, AssetBundle cannot be unloaded while assets are in use. 
Therefore, depending on the granularity of the AssetBundle, there may be situations where more than 100 AssetBundles are loaded at the same time. 
In this case, you need to be careful about the file descriptor limit and the memory usage of @<code>{PersistentManager.Remapper}. 

A file descriptor is an operation ID assigned by the OS when reading or writing a file. 
One file descriptor is required to read or write one file, and the file descriptor is released when the file operation is completed. 
Since there is a limit to the number of file descriptors a process can have, it is not possible to have more than this number of files open at the same time. 
If you see the error message "Too many open files", it means that the maximum number of files cannot be opened at the same time. 
Therefore, the number of simultaneous loads in the AssetBundle is affected by this limit, and Unity also has to maintain a certain amount of margin for the limit, since it is opening some files at the same time. 
This limit varies depending on the OS and version, so it is necessary to investigate the value for the target platform in advance. 
Even if the limit is hit, it is possible to temporarily raise the limit depending on the OS @<fn>{setrlimit}, so consider implementing this if necessary. 


//footnote[setrlimit][ In Linux/Unix environments, the limit can be changed at runtime using the setrlimit function]

A second problem with having many AssetBundles to load at the same time is the presence of @<code>{PersistentManager.Remapper} in Unity. 
Simply put, the PersistentManager is a function that manages the mapping relationship between objects and data within Unity. 
In other words, you can imagine that it uses memory in proportion to the number of AssetBundles loaded at the same time, but the problem is that even if you release an AssetBundle, the memory space used is not released, but pooled. 
Because of this nature, memory will be squeezed in proportion to the number of concurrent loads, so it is important to reduce the number of concurrent loads. 

Based on the above, when operating under the @<code>{AssetBundle.Unload(true)} policy, it is recommended that the maximum number of concurrently loaded AssetBundles be 150 to 200, and when operating under the 
@<code>{AssetBundle.Unload(false)} policy, the maximum number should be 150 or less. 
