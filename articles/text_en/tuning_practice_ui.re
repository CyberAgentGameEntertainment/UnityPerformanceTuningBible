={practice_ui} Tuning Practice - UI

 Tuning Practices for uGUI, the Unity standard UI system, and TextMeshPro, the mechanism for drawing text to the screen. 

=={practice_ui_rebuild} Canvas partitioning

In uGUI, when there is a change in an element in @<code>{Canvas}, a process (rebuild) runs to rebuild the entire 
@<code>{Canvas} UI mesh. 
A change is any change, such as active switching, moving or resizing, 
from a major change in appearance to a minor change that is not apparent at first glance. 
The cost of the rebuild process is high, so if it is performed too many times or if the number of UIs in 
@<code>{Canvas} is large, performance will be adversely affected. 

In contrast, the cost of 
rebuilds can be reduced by dividing @<code>{Canvas} by some degree of UI cohesion. 
For example, if you have UIs that animate and UIs that do not animate, you can minimize the 
animation rebuilds by placing them under a separate @<code>{Canvas}
 . 

However, you need to think carefully about how 
to split them, as splitting @<code>{Canvas} will not work for drawing batches. 

//info{

Splitting @<code>{Canvas} is also valid when @<code>{Canvas} is nested under @<code>{Canvas}. 
If the elements contained in the child @<code>{Canvas} change, a rebuild of the child @<code>{Canvas} will only run, not the parent @<code>{Canvas}. 
However, upon closer inspection, it seems that the situation is different when the UI in the child @<code>{Canvas} is switched to the active state by @<code>{SetActive}. 
In this case, if a large number of UIs are placed in the parent @<code>{Canvas}, there seems to be a phenomenon that causes a high load. 
I do not know the details of why this behavior occurs, but it seems that care should be taken when switching the active state of the UI in the nested @<code>{Canvas}. 

//}

=={practice_ui_unity_white} UnityWhite

When developing UIs, it is often the case that we want to display a simple rectangle-shaped object. 
This is where UnityWhite comes in handy. 
UnityWhite is a Unity built-in texture that is used when the @<code>{Image} or @<code>{RawImage} component does not specify the 
image to be used ( @<img>{none_image}). 
You can see how UnityWhite is used in the Frame Debugger ( @<img>{unity_white}). 
This mechanism can be used to draw a white rectangle, so 
a simple rectangle type display can be achieved by combining this with a multiplying color. 

//image[none_image][Using UnityWhite][scale=0.8]
//image[unity_white][UnityWhite in use.][scale=0.8]

 However, since UnityWhite is a different texture than the SpriteAtlas provided in the project, 
draw batches are interrupted. 
This increases draw calls and reduces drawing efficiency. 

Therefore, you should add a small (e.g., 4 × 4 pixel) white square image to SpriteAtlas and use 
that Sprite to draw a simple rectangle. 
This will allow the batch to work, since the same SpriteAtlas will be used for the same material. 

=={practice_ui_layout_component} Layout component

uGUI provides a Layout component that allows you to neatly align objects. 
For example, @<code>{VerticalLayoutGroup} is used for vertical alignment, and @<code>{GridLayoutGroup} is used for alignment on the 
grid ( @<img>{layout_group}). 

//image[layout_group][Example using @<code>{VerticalLayoutGroup} on the left and @<code>{GridLayoutGroup} on the right][scale=0.99]

 When using the Layout component, Layout rebuilds occur when the target object is created or when certain properties are edited. 
Layout rebuilds, like mesh rebuilds, are costly processes. 

To avoid performance degradation due to Layout rebuilds, it is effective to avoid using 
Layout components as much as possible. 

For example, if you do not need dynamic placement, such as text that changes placement based on its content, you do not need to use the 
Layout component. 
If you really need dynamic placement, or if it is used a lot on the screen, it may be better to control it with 
your own scripts. 
Also, if the requirement is to be placed in a specific position relative to the parent even if the parent changes size, this can be accomplished by adjusting the 
@<code>{RectTransform} anchors. 
If you use a Layout component when creating a prefab because it is convenient for placement, 
be sure to delete it and save it. 

=={practice_ui_raycast_target} Raycast Target
@<code>{Graphic} , the base class for @<code>{Image} and @<code>{RawImage}, has a property 
Raycast Target ( @<img>{raycast_target}). 
When this property is enabled, its @<code>{Graphic} becomes the target for clicks and touches. 
When the screen is clicked or touched, objects with this property enabled will be the target of 
processing, so disabling this property as much as possible will improve performance. 

//image[raycast_target][Raycast Target property][scale=0.8]

 This property is enabled by default, but 
actually many @<code>{Graphic} do not require this property to be enabled. 
On the other hand, Unity has a feature called preset @<fn>{preset} that allows you to change the default value in your project. 
Specifically, you can create presets for the @<code>{Image} and @<code>{RawImage} components, respectively, and register 
them as default presets from the Preset Manager in Project Settings. 
You may also use this feature to disable the Raycast Target property by default. 

//footnote[preset][@<href>{https://docs.unity3d.com/ja/current/Manual/Presets.html}]

=={practice_ui_mask} Masks

To represent masks in uGUI, use either the @<code>{Mask} component or the @<code>{RectMask2d} component. 

Since @<code>{Mask} uses stencils to realize masks, the drawing cost increases with each additional component. 
On the other hand, @<code>{RectMask2d} uses shader parameters to realize masks, so the increase in drawing cost is suppressed. 
However, @<code>{Mask} can be hollowed out in any shape, while @<code>{RectMask2d} can only be hollowed out as a rectangle. 

It is a common belief that @<code>{RectMask2d} should be selected if available, but 
recent Unity users should also be careful about using @<code>{RectMask2d}. 

Specifically, when @<code>{RectMask2d} is enabled, the CPU load for culling every frame is proportional to 
as its masking target increases. 
This phenomenon, which generates load every frame even when the UI is not moving anything, seems to be a 
side effect of a fix to issue @<fn>{mask_issue} that was included in Unity 2019.3, according to comments in the uGUI's internal implementation. 
//footnote[mask_issue][@<href>{https://issuetracker.unity3d.com/issues/rectmask2d-diffrently-masks-image-in-the-play-mode-when-animating-rect-transform-pivot- property}]

Therefore, it is useful to take measures to avoid using @<code>{RectMask2d} as much as possible, to use @<code>{enabled} as @<code>{false} when it is not needed even if 
is used, and to keep 
masked targets to the minimum necessary. 

=={practice_ui_text_mesh_pro} TextMeshPro
The common way to set text in TextMeshPro is to assign text to the @<code>{text} property, 
but there is another method, @<code>{SetText}. 

There are many overloads to @<code>{SetText}, 
for example, that take a string and a value of type @<code>{float} as arguments. 
If you use this method like @<list>{setText}, you can print the value of the second argument. 
However, assume that @<code>{label} is a variable of type @<code>{TMP_Text}(or inherited from it) and 
@<code>{number} is of type @<code>{float}. 

//listnum[setText][Example of using SetText][csharp]{
label.SetText("{0}", number);
//}

The advantage of this method is that it reduces the cost of generating strings. 

//listnum[noSetText][Example of not using SetText][csharp]{
label.text = number.ToString();
//}

@<list>{noSetText} In the method using the @<code>{text} property, as in the following example, @<code>{ToString()} of type 
@<code>{float} is executed, so the string generation cost is incurred each time this process is executed. 
In contrast, the method using @<code>{SetText} is designed to generate as few strings as possible, which is a performance advantage when the text to be displayed changes frequently, as is the case with 
. 

This feature of TextMeshPro is also very powerful when combined with ZString @<fn>{zstring}. 
ZString is a library that reduces memory allocation in string generation. 
ZString provides many extension methods for the @<code>{TMP_Text} type, 
and by using those methods, flexible text display can be achieved while reducing the cost of string generation. 

//footnote[zstring][@<href>{https://github.com/Cysharp/ZString}]

=={practice_ui_active} UI Display Switching

uGUI components are characterized by the high cost of active switching of objects by @<code>{SetActive}. 
This is due to the fact that @<code>{OnEnable} sets the Dirty flag for various rebuilds and performs initialization related to masks. 
Therefore, it is important to consider alternatives to the @<code>{SetActive} method for switching the display and non-display of the UI. 

The first method is to change @<code>{enabled} of @<code>{Canvas} to @<code>{false}( @<img>{canvas_disable}). 
This will prevent all objects under @<code>{Canvas} from being rendered. 
Therefore, this method has the disadvantage that it can only be used if you want to hide all the objects under @<code>{Canvas}. 

//image[canvas_disable][@<code>{Canvas} Disabling][scale=0.8]

 Another method is to use @<code>{CanvasGroup}. 
@<code>{CanvasGroup} has a function that allows you to adjust the transparency of all objects under it at once. 
If you use this function and set the transparency to 0, you can hide all the objects under 
its @<code>{CanvasGroup}( @<img>{canvas_group}). 

//image[canvas_group][@<code>{CanvasGroup} Set the transparency of the  to 0.][scale=0.8]

 While these methods are expected to avoid the load caused by @<code>{SetActive}, you may need to be careful because 
@<code>{GameObject} will remain in the active state. 
For example, if @<code>{Update} methods are defined, be aware that they will continue to run even in the hidden state, which may 
lead to an unexpected increase in load. 

For reference, we measured the processing time for 1280 @<code>{GameObject} with @<code>{Image} components attached, when switching between visible and hidden states using 
each method ( @<table>{activation_time}). 
The processing time was measured using the Unity editor, and Deep Profile was not used. 
The processing time of the method 
is the sum of the execution time of the actual switching @<fn>{activation_profile} and the execution time of @<code>{UIEvents.WillRenderCanvases} in the frame 
. 
The execution time of @<code>{UIEvents.WillRenderCanvases} is added together because the UI rebuild is performed in 
. 

//footnote[activation_profile][ For example, if @<code>{SetActive}, the @<code>{SetActive} method call is enclosed in @<code>{Profiler.BeginSample} and @<code>{Profiler.EndSample} to measure the time.]


//tsize[|latex||l|r|r|]
//table[activation_time][Processing time for display switching]{
Method	Processing time (display)	Processing time (hidden)
------------------------------------------------------------- 
@<code>{SetActive}	323.79ms	209.93ms
@<code>{Canvas} of @<code>{enabled}	61.25ms	61.23ms
@<code>{CanvasGroup} of @<code>{alpha}	3.64ms	3.40ms
//}

@<table>{activation_time} From the results of 
, we found that the method using CanvasGroup has by far the shortest processing time in the situation we tried this time. 
