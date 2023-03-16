import unittest
import xml2review

class TestFunc(unittest.TestCase):
    def test_convert_review(self):
        input = """For example, specifying <code>struct</code>, for which the <code>IEquatable&lt;T&gt;</code>interface is not implemented, to <code>T</code>would result in a cast to <code>object</code> with the argument <code>Equals</code>, which would cause boxing. To prevent this from happening in advance, change the following 
<listnum generic_non_boxing> Example with restrictions to prevent boxing</listnum><parameter csharp/><block>
<code>public readonly struct GenericOnlyStruct&lt;T&gt; : IEquatable&lt;T&gt;</code>
<code>    where T : IEquatable&lt;T&gt;</code>
<code>{</code>
<code>    private readonly T _value;</code>
<code></code>
<code>    public GenericOnlyStruct(T value)</code>
<code>    {</code>
<code>        _value = value;</code>
<code>    }</code>
<code></code>
<code>    public bool Equals(T other)</code>
<code>    {</code>
<code>        var result = _value.Equals(other);</code>
<code>        return result;</code>
<code>    }</code>
<code>}</code>
</block>""".splitlines()
        expected = """For example, specifying @<code>{struct}, for which the @<code>{IEquatable<T>} interface is not implemented, to @<code>{T} would result in a cast to @<code>{object} with the argument @<code>{Equals}, which would cause boxing. To prevent this from happening in advance, change the following 
//listnum[generic_non_boxing][Example with restrictions to prevent boxing][csharp]{
public readonly struct GenericOnlyStruct<T> : IEquatable<T>
    where T : IEquatable<T>
{
    private readonly T _value;

    public GenericOnlyStruct(T value)
    {
        _value = value;
    }

    public bool Equals(T other)
    {
        var result = _value.Equals(other);
        return result;
    }
}
//}""".splitlines()
        self._test(input, expected)

    def test_definition_list(self):
        input = """<dt> System.ExecutableAndDlls</dt><br/>
Indicates the amount of allocations used for binaries, DLLs, and so on. <br/>
Depending on the platform or terminal, it may not be obtainable, in which case it is treated as 0B. <br/>
The memory load for the <br/>
project is not as large as the listed values, as it may be shared with other applications using a common framework. <br/>
It is better to improve Asset than to rush to reduce this item. <br/>
The most effective way to do this is to reduce DLLs and unnecessary scripts. The easiest way is to change the Stripping Level. <br/>
However, there is a risk of missing types and methods at runtime, so debug carefully. <br/>
<dt> SerializedFile</dt><br/>
Indicates meta-information such as the table of objects in the AssetBundle and the Type Tree that serves as type information. <br/>
This can be released by AssetBundle.Unload(true or false). <br/>
Unload(false), which releases only this meta-information after the asset is loaded, is the most efficient way. <br/>
Note that if the release timing and resource reference management are not done carefully, resources can be double-loaded and memory leaks can easily occur. <br/>
<dt> PersistentManager.Remapper</dt><br/>
Remapper manages the relationship between objects in memory and on disk. <br/>
Be careful not to over-expand. Specifically, if a large number of AssetBundles are loaded, the mapping area will not be sufficient and will be expanded. <br/>
Therefore, it is a good idea to unload unnecessary AssetBundles to reduce the number of files loaded at the same time. <br/>
Also, if a single AssetBundle contains a large number of assets that are not needed on the fly, it is a good idea to split it up. <br/>""".splitlines()
        expected = """
 : System.ExecutableAndDlls
 Indicates the amount of allocations used for binaries, DLLs, and so on. 
 Depending on the platform or terminal, it may not be obtainable, in which case it is treated as 0B. 
 The memory load for the 
 project is not as large as the listed values, as it may be shared with other applications using a common framework. 
 It is better to improve Asset than to rush to reduce this item. 
 The most effective way to do this is to reduce DLLs and unnecessary scripts. The easiest way is to change the Stripping Level. 
 However, there is a risk of missing types and methods at runtime, so debug carefully. 

 : SerializedFile
 Indicates meta-information such as the table of objects in the AssetBundle and the Type Tree that serves as type information. 
 This can be released by AssetBundle.Unload(true or false). 
 Unload(false), which releases only this meta-information after the asset is loaded, is the most efficient way. 
 Note that if the release timing and resource reference management are not done carefully, resources can be double-loaded and memory leaks can easily occur. 

 : PersistentManager.Remapper
 Remapper manages the relationship between objects in memory and on disk. 
 Be careful not to over-expand. Specifically, if a large number of AssetBundles are loaded, the mapping area will not be sufficient and will be expanded. 
 Therefore, it is a good idea to unload unnecessary AssetBundles to reduce the number of files loaded at the same time. 
 Also, if a single AssetBundle contains a large number of assets that are not needed on the fly, it is a good idea to split it up. """.splitlines()
        self._test(input, expected)
        
    def _test(self, input: str, expected: str):
        actual = xml2review.convert_review(input)
        actual = "".join(actual).splitlines()
        self.assertEqual(expected, actual)

if __name__ == '__main__':
    unittest.main()