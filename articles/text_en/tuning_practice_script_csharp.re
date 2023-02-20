={practice_script_csharp} Tuning Practice - Script (C#)

This chapter mainly introduces performance tuning practices for C# code with examples. 
Basic C# notation is not covered here, but rather the design and implementation that you should be 
aware of when developing games that require performance. 

=={practice_script_csharp_sample} GC.Alloc cases and how to deal with them

As introduced in @<hd>{basic|basic_csharp_gc}, 
in this section, let's first understand what kind of specific processing causes GC.Alloc. 

==={practice_script_csharp_new} New of reference type

First of all, this is a very simple case in which GC.Alloc occurs. 

//listnum[simple_list_alloc][Code that GC.Alloc every frame][csharp]{
private void Update()
{
    const int listCapacity = 100;
    //  GC.Alloc with new of List<int>.
    var list = new List<int>(listCapacity);
    for (var index = 0; index < listCapacity; index++)
    {
        // Packing an index into a List with no particular meaning
        list.Add(index);
    }
    //  Randomly take a value out of the list
    var random = UnityEngine.Random.Range(0, listCapacity);
    var randomValue = list[random];
    //  ... Do something from the random value ...
}
//}
The major problem with this code is that 
@<code>{List<int>} is @<code>{new} in the Update method that is executed every frame. 

To fix this, it is possible to avoid 
GC.Alloc every frame by pre-generating @<code>{List<int>} and using it around. 

//listnum[simple_list_nonalloc][Code that eliminates GC.Alloc in every frame][csharp]{
private static readonly int listCapacity = 100;
//  Generate a List in advance
private readonly List<int> _list = new List<int>(listCapacity);

private void Update()
{
    _list.Clear();
    for (var index = 0; index < listCapacity; index++)
    {
        //  Pack indexes into the list, though it doesn't make sense to do so
        _list.Add(index);
    }
    //  Randomly take a value from the list
    var random = UnityEngine.Random.Range(0, listCapacity);
    var randomValue = _list[random];
    //  ... Do something with the random values ...
}
//}

I don't think you will ever write meaningless code like the sample code here, but 
similar examples can be found in more cases than you might imagine. 

====[column] If you lose GC.Alloc.

As you may have noticed, the sample code from @<list>{simple_list_nonalloc} above is all you need to do. 
//listnum[simple_random][][csharp]{
var randomValue = UnityEngine.Random.Range(0, listCapacity);
//  ... Do something from a random value ...
//}
While it is important to think about eliminating GC.Alloc in performance tuning, 
always thinking about eliminating pointless calculations is a step toward speeding up the process. 

====[/column]


==={practice_script_csharp_lambda} Lambda Expressions
Lambda expressions are also a useful feature, but their use is limited in 
games because they too can cause GC.Alloc depending on how they are used. 
Here we assume that the following code is defined. 
//listnum[lambda_define][Assumed code for the lambda expression sample][csharp]{
//  Member Variables
private int _memberCount = 0;

//  static variables
private static int _staticCount = 0;

//  member method
private void IncrementMemberCount() 
{
    _memberCount++;
}

//  static method
private static void IncrementStaticCount() 
{
    _staticCount++;
}

//  Member method that only invokes the received Action
private void InvokeActionMethod(System.Action action)
{
    action.Invoke();
}
//}

At this time, if a variable is referenced in a lambda expression as follows, GC.Alloc will occur. 

//listnum[lambda_member_var][Case of GC.Alloc by referencing a variable in a lambda expression][csharp]{
//  When a member variable is referenced, Delegate Allocation occurs
InvokeActionMethod(() => { _memberCount++; });

//  When a local variable is referenced, Closure Allocation occurs
int count = 0;
//  The same Delegate Allocation as above also occurs
InvokeActionMethod(() => { count++; });
//}           

However, these GC.Alloc can be avoided by referencing static variables as follows. 

//listnum[lambda_static_var][Cases where a static variable is referenced in a lambda expression and GC.Alloc is not performed][csharp]{
//  When a static variable is referenced, GC Alloc does not occur and
InvokeActionMethod(() => { _staticCount++; });
//}           


GC.Alloc is also performed differently for method references in lambda expressions, depending on how they are written. 

//listnum[lambda_method][Cases of GC.Alloc when a method is referenced in a lambda expression][csharp]{
//  When a member method is referenced, Delegate Allocation occurs.
InvokeActionMethod(() => { IncrementMemberCount(); });

//  If a member method is directly specified, Delegate Allocation occurs.
InvokeActionMethod(IncrementMemberCount);

//  When a static method is directly specified, Delegate Allocation occurs
InvokeActionMethod(IncrementStaticCount);
//}

To avoid these cases, it is necessary to reference static methods in a statement format as follows. 

//listnum[lambda_member_method_ref][Cases where a method is referenced in a lambda expression and GC.Alloc is not performed][csharp]{
// Non Alloc if a static method is referenced in a lambda expression
InvokeActionMethod(() => { IncrementStaticCount(); });
//}           
Alloc will not occur and GC.Alloc will not occur if you refer to a static method in a lambda expression. 

However, making all variables and methods static is not very 
adoptable in terms of code safety and readability. In code that needs to be fast, it is safer to design without using lambda expressions for events that fire at 
every frame or at indefinite times, rather than to use a lot of statics to eliminate GC.Alloc. 

==={practice_script_csharp_boxing_generic} Cases where generics are used and boxed

In the following cases where generics are used, what could cause boxing? 

//listnum[generic_boxing][Examples of possible boxed events using generics][csharp]{
public readonly struct GenericStruct<T> : IEquatable<T>
{
    private readonly T _value;

    public GenericStruct(T value)
    {
        _value = value;
    }

    public bool Equals(T other)
    {
        var result = _value.Equals(other);
        return result;
    }
}
//}
In this case, the programmer implemented the @<code>{IEquatable<T>} interface to @<code>{GenericStruct}, but forgot to place restrictions on 
@<code>{T} . As a result, a type that does not implement the @<code>{IEquatable<T>} interface can be specified for 
@<code>{T} , and there is a case where the following @<code>{Equals} is used by implicitly casting to the @<code>{Object} type. 

//listnum[object_class_equals][Object.cs][csharp]{
public virtual bool Equals(object obj);
//}

For example, if @<code>{struct}, which does not implement the @<code>{IEquatable<T>} interface, is specified to @<code>{T}, it will be cast to 
@<code>{object} with the argument @<code>{Equals}, resulting in boxing. To prevent this from happening in advance, change 
the following 

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
//}

By using the @<code>{where} clause (generic type constraint) to restrict the types that @<code>{T} can accept to those that implement @<code>{IEquatable<T>}, 
such unexpected boxing can be prevented. 

====[column] Never lose sight of the original purpose

As introduced in @<hd>{basic|basic_csharp_gc}, there are many cases where the 
structure is chosen because the intention is to avoid GC.Alloc during runtime in games. However, it is not always possible to speed up the process by making everything a structure in order to reduce GC.Alloc. 

One of the most common failures is that when structs are used to avoid GC.Alloc, the cost related to GC is reduced as expected, but the 
data size is so large that copying the value type becomes expensive, resulting in inefficient processing. 

To avoid this, there are also methods that reduce copying costs by using pass-by-reference for method arguments. 
Although this may result in a speed-up, in this case, you should consider selecting a class from the beginning and implementing it in such a way that instances are pre-generated and used around. 
Remember that the ultimate goal is not to eradicate GC.Alloc, but to reduce the processing time per frame. 

====[/column]

=={practice_script_csharp_loop} About for/foreach

As introduced in @<hd>{basic|basic_algorithm}, loops become time-consuming depending on the number of data. 
Also, loops, which at first glance appear to be the same process, can vary in efficiency depending on how the code is written. 

Let's take a look at the results of decompiling the code from IL to C# using SharpLab @<fn>{performance_tips_sharplab}, using @<code>{foreach/for}
 @<code>{List} and just getting the contents of the array one by one. 

//footnote[performance_tips_sharplab][@<href>{https://sharplab.io/}]

First, let's look at the loop around @<code>{foreach}. @<code>{List} I've omitted adding values to the 

//listnum[simple_foreach_list][Example of looping through a List with foreach][csharp]{
var list = new List<int>(128);        
foreach (var val in list)
{
}
//}

//listnum[simple_foreach_list_decompile][Decompilation result of the example of looping through a List with foreach][csharp]{
List<int>.Enumerator enumerator = new List<int>(128).GetEnumerator();
try
{
    while (enumerator.MoveNext())
    {
        int current = enumerator.Current;
    }
}
finally
{
    ((IDisposable)enumerator).Dispose();
}
//}

In the case of turning with @<code>{foreach}, you can see that the implementation is to get the enumerator, move on with @<code>{MoveNext()}, and refer to the value with @<code>{Current}. 
Furthermore, looking at the implementation of 
@<code>{MoveNext()} in list.cs @<fn>{performance_tips_ms_list}, it appears that the number of various property accesses, such as size checks, are increased, and that 
processing is more frequent than direct access by the indexer. 

//footnote[performance_tips_ms_list][ https://referencesource.microsoft.com/#mscorlib/system/collections/generic/list.cs]


Next, let's look at when we turn in @<code>{for}. 

//listnum[simple_for_list][Example of turning a List with for][csharp]{
var list = new List<int>(128);
for (var i = 0; i < list.Count; i++)
{
    var val = list[i];
}
//}

//listnum[simple_for_list_decompile][Decompiled result when turning List with for][csharp]{
List<int> list = new List<int>(128);
int num = 0;
while (num < list.Count)
{
    int num2 = list[num];
    num++;
}
//}

In C#, the @<code>{for} statement is a sugar-coated syntax for the @<code>{while} statement, and the indexer (@<code>{public T this[int index]} ), and is obtained by reference by the indexer ( 
Also, if you look closely at this @<code>{while} statement, you will see that the conditional expression contains @<code>{list.Count}. This means that the 
access to the @<code>{Count} property is performed each time the loop is repeated. @<code>{Count} The more the number of @<code>{Count} accesses to the property, the more the number of accesses to the property increases proportionally, and depending on the number of 
accesses, the load becomes non-negligible. If the @<code>{Count} does not change within the loop, then the load on property accesses can be reduced by caching them before the loop. 

//listnum[simple_for_improve_list][Example of turning a List with for: Improved version][csharp]{
var count = list.Count;
for (var i = 0; i < count; i++)
{
    var val = list[i];
}
//}

//listnum[simple_for_improve_list_decompile][Example of List in for: Decompiled result of the improved version][csharp]{
List<int> list = new List<int>(128);
int count = list.Count;
int num = 0;
while (num < count)
{
    int num2 = list[num];
    num++;
}
//}

Caching @<code>{Count} reduced the number of property accesses and made it faster. 
Both of the comparisons in this loop are not loaded by GC.Alloc, and the difference is due to the difference in implementation. 

In the case of arrays, @<code>{foreach} has also been optimized and is almost unchanged from that described in @<code>{for}. 

//listnum[simple_foreach_array][Example of turning an array with foreach][csharp]{
var array = new int[128];
foreach (var val in array)
{
}
//}

//listnum[simple_foreach_array_decompile][Decompilation result of the example of turning an array by foreach][csharp]{
int[] array = new int[128];
int num = 0;
while (num < array.Length)
{
    int num2 = array[num];
    num++;
}
//}


For the purpose of verification, the number of data is 10,000,000 and random numbers are assigned in advance. @<code>{List<int>}
 The sum of the data is calculated. The verification environment was Pixel 3a and Unity 2021.3.1f1. 

//table[loop_profile_diff_int][Measurement results for each description method in List<int]{
Type	Time ms
List: foreach	66.43
List: for	62.49
List: for (Count cache)	55.11
Array: for	30.53
Array: foreach	23.75
//}

In the case of @<code>{List<int>}, a comparison with a finer set of conditions shows that @<code>{for} and @<code>{for} with @<code>{Count} optimizations are even faster 
than @<code>{foreach}. @<code>{List} The @<code>{foreach} of can be rewritten to @<code>{for} with @<code>{Count} optimization to reduce the overhead of the @<code>{MoveNext()} and @<code>{Current} properties in the processing of 
@<code>{foreach} , thus making it faster. 

Also, when comparing the respective fastest speeds of @<code>{List} and arrays, arrays are approximately 2.3 times faster than @<code>{List}. 
Even if @<code>{foreach} and @<code>{for} are written to have the same IL result, @<code>{foreach} is the faster result, and 
array's @<code>{foreach} is sufficiently optimized. 

Based on the above results, arrays should be considered instead of @<code>{List<T>} for situations where the number of data is large and processing speed must be fast. 

@<code>{foreach}@<code>{for} However, if the rewriting is insufficient, such as when @<code>{List} defined in a field is referenced without local caching, it may not be possible to speed up the process. 

=={practice_script_csharp_reuse} Object Pooling

As we have mentioned in many places, it is important in game development to pre-generate objects and use them around instead of dynamically generating them. 
This is called @<em>{object pool} ing. For example, objects that are to be used in the game phase can be pooled together in the load phase and handled while only assigning and referencing the pooled objects when they are 
used, thereby avoiding GC.Alloc during the game phase. 

In addition to reducing allocations, object pooling can also be used in a variety of other situations, such as enabling 
screen transitions without having to recreate the objects that make up the screen each time, reducing load times, and avoiding multiple heavy calculations by retaining the results of processes with very high calculation costs. 
It is used in a variety of situations. 

Although the term "object" is used here in a broad sense, it applies not only to the smallest unit of data, but also to @<code>{Coroutine} and @<code>{Action}, 
. For example, consider generating @<code>{Coroutine} more than the expected number of executions in advance, and use it when necessary to exhaust it. 
For example, if a game that takes 2 minutes to complete will be executed a maximum of 20 times, you can reduce the cost of generating 
by generating @<code>{IEnumerator} in advance and only using @<code>{StartCoroutine} when you need to use it. 

=={practice_script_csharp_string} string

The @<code>{string} object is a sequential collection of @<code>{System.Char} objects representing strings. @<code>{string} GC.Alloc can easily occur with one usage. 
For example, concatenating two strings using the character concatenation operator @<code>{+} will result in the creation of a new @<code>{string} object. @<code>{string} The value of 
cannot be changed (immutable) after it is created, so an operation that appears to change the value creates and returns a new @<code>{string} object. 

//listnum[string_create][When string concatenation is used to create a STRING][csharp]{
private string CreatePath()
{
    var path = "root";
    path += "/";
    path += "Hoge";
    path += "/";
    path += "Fuga";
    return path;
}
//}

In the above example, a string is created with each string concatenation, resulting in a total of 164Byte allocation. 

When strings are frequently changed, the use of @<code>{StringBuilder}, whose value can be changed, can prevent the mass generation of @<code>{string} objects. 
By performing operations such as character concatenation and deletion in the @<code>{StringBuilder} object and finally extracting the value and @<code>{ToString()} it to the @<code>{string} object, the memory allocation can be limited to only the time of 
acquisition. Also, when using @<code>{StringBuilder}, be sure to set Capacity. 
When unspecified, the default value is 16, and when the buffer is extended with more characters, such as @<code>{Append}, memory allocation and value copying will run. 
Be sure to set an appropriate Capacity that will not cause inadvertent expansion. 


//listnum[string_create_builder][When creating a string with StringBuilder][csharp]{
private readonly StringBuilder _stringBuilder = new StringBuilder(16);
private string CreatePathFromStringBuilder()
{
    _stringBuilder.Clear();
    _stringBuilder.Append("root");
    _stringBuilder.Append("/");
    _stringBuilder.Append("Hoge");
    _stringBuilder.Append("/");
    _stringBuilder.Append("Fuga");
    return _stringBuilder.ToString();
}
//}

In the example using @<code>{StringBuilder}, if @<code>{StringBuilder} is generated in advance (in the above example, 112Byte allocation is made at the time of generation), then from 
onward, only 50Byte allocation is needed which is taken at @<code>{ToString()} when the generated string is retrieved. 

However, @<code>{StringBuilder} is also not recommended for use when you want to avoid GC.Alloc, since allocation is only less likely to occur during value manipulation, and as mentioned above, @<code>{string} objects will be 
generated when @<code>{ToString()} is executed. Also, since the @<code>{$""} syntax is converted to @<code>{string.Format} and the internal implementation of 
@<code>{string.Format} uses @<code>{StringBuilder}, the cost of @<code>{ToString()} is ultimately unavoidable. 
The use of objects in the previous section should be applied here as well, and strings that may be used in advance should be pre-generated @<code>{string} objects and used. 

However, there are times during the game when string manipulation and the creation of @<code>{string} objects must be performed. 
In such cases, it is necessary to have a pre-generated buffer for strings and extend it so that it can be used as is. 
Consider implementing your own code like @<code>{unsafe} or introducing a library with extensions for Unity like ZString @<fn>{string_zstring}
 (e.g. NonAlloc applicability to @<code>{TextMeshPro}). 

//footnote[string_zstring][@<href>{https://github.com/Cysharp/ZString}]

=={practice_script_csharp_linq} LINQ and Latency Evaluation
This section describes how to reduce GC.Alloc by using LINQ and the key points of lazy evaluation. 

===={practice_script_csharp_linq_gc_cause} Mitigating GC.Alloc by using LINQ 
The use of LINQ causes GC.Alloc in cases like @<list>{linq_heap_allocation_sample_code}. 

//listnum[linq_heap_allocation_sample_code][Example of GC.Alloc occurring][csharp]{
var oneToTen = Enumerable.Range(1, 11).ToArray();
var query = oneToTen.Where(i => i % 2 == 0).Select(i => i * i);
//}

@<list>{linq_heap_allocation_sample_code} The reason why GC.Alloc occurs in  is due to the internal implementation of LINQ. 
In addition, some LINQ methods are optimized for the caller's type, so the size of GC.Alloc changes depending on the caller's type. 

//listnum[linq_type_optimization_query_execute][Execution speed verification for each type][csharp]{
private int[] array;
private List<int> list;
private IEnumerable<int> ienumerable;

public void GlobalSetup()
{
    array = Enumerable.Range(0, 1000).ToArray();
    list = Enumerable.Range(0, 1000).ToList();
    ienumerable = Enumerable.Range(0, 1000);
}

public void RunAsArray()
{
    var query = array.Where(i => i % 2 == 0);
    foreach (var i in query){}
}

public void RunAsList()
{
    var query = list.Where(i => i % 2 == 0);
    foreach (var i in query){}
}

public void RunAsIEnumerable()
{
    var query = ienumerable.Where(i => i % 2 == 0);
    foreach (var i in query){}
}
//}

@<list>{linq_type_optimization_query_execute} We measured the benchmark for each method defined in @<img>{linq_type_optimizaiton_query_execute_result}. 
The results show that the size of heap allocations increases in the order @<code>{T[]} → @<code>{List<T>} → @<code>{IEnumerable<T>}. 

Thus, when using LINQ, the size of GC.Alloc can be reduced by being aware of the runtime type. 

//image[linq_type_optimizaiton_query_execute_result][Comparison of Execution Speed by Type][scale=1.0]

====[column] Causes of GC.Alloc in LINQ

Part of the cause of GC.Alloc with the use of LINQ is the internal implementation of LINQ. 
Many LINQ methods take @<code>{IEnumerable<T>} and return @<code>{IEnumerable<T>}, and this API design allows for intuitive description using method chaining. 
The entity @<code>{IEnumerable<T>} returned by a method is an instance of the class for each function. 
LINQ internally instantiates a class that implements @<code>{IEnumerable<T>}, and furthermore, GC.Alloc occurs internally because calls to @<code>{GetEnumerator()} are made to realize loop processing, etc. 

====[/column]

===={practice_script_csharp_linq_lazy} LINQ Lazy Evaluation 
LINQ methods such as @<code>{Where} and @<code>{Select} are lazy evaluations that delay evaluation until the result is actually needed. 
On the other hand, methods such as @<code>{ToArray} are defined for immediate evaluation. 

Now consider the case of the following @<list>{linq_to_array_sample_code} code. 

//listnum[linq_to_array_sample_code][Methods with immediate evaluation in between][csharp]{
private static void LazyExpression()
{
    var array = Enumerable.Range(0, 5).ToArray();
    var sw = Stopwatch.StartNew();
    var query = array.Where(i => i % 2 == 0).Select(HeavyProcess).ToArray();
    Console.WriteLine($"Query: {sw.ElapsedMilliseconds}");

    foreach (var i in query)
    {
        Console.WriteLine($"diff: {sw.ElapsedMilliseconds}");
    }
}

private static int HeavyProcess(int x)
{
    Thread.Sleep(1000);
    return x;
}
//}

@<list>{linq_to_array_sample_code} The result of the execution of @<list>{linq_to_array_sample_code_result} is the result of . 
By adding @<code>{ToArray} at the end, which is an immediate evaluation, the result of executing the method @<code>{Where} or @<code>{Select} and evaluating the value is returned when the assignment is made to @<code>{query}. 
Therefore, @<code>{HeavyProcess} is also called, so you can see that processing time is taken at the timing when @<code>{query} is generated. 

//listnum[linq_to_array_sample_code_result][Result of Adding a Method for Immediate Evaluation]{
Query: 3013
diff: 3032
diff: 3032
diff: 3032
//}

As you can see, unintentional calls to LINQ's immediate evaluation methods can result in bottlenecks at those points. @<code>{ToArray} Methods that require looking at the entire sequence once, such as @<code>{OrderBy}, @<code>{Count}, and , are immediate evaluation, so be aware of the cost when calling them. 

===={practice_script_csharp_not_use_linq} The Choice to "Avoid Using LINQ" 
This section explained the causes of GC.Alloc when using LINQ, how to reduce it, and the key points of delayed evaluation. 
In this section, we explain the criteria for using LINQ. 
The premise is that LINQ is a useful language feature, but its use will worsen heap allocation and execution speed compared to when it is not used. 
In fact, Microsoft's Unity performance recommendations at @<fn>{performance_tips_from_microsoft} clearly state "Avoid use of LINQ. 
Here is a benchmark comparison of the same logic implemented with and without LINQ at @<list>{linq_vs_pure_benchmark}. 

//footnote[performance_tips_from_microsoft][@<href>{https://docs.microsoft.com/en-us/windows/mixed-reality/develop/unity/performance-recommendations-for-unity#avoid-expensive- operations}]

//listnum[linq_vs_pure_benchmark][Performance comparison with and without LINQ][csharp]{
private int[] array;

public void GlobalSetup()
{
    array = Enumerable.Range(0, 100_000_000).ToArray();
}

public void Pure()
{
    foreach (var i in array)
    {
        if (i % 2 == 0)
        {
            var _ = i * i;
        }
    }
}

public void UseLinq()
{
    var query = array.Where(i => i % 2 == 0).Select(i => i * i);
    foreach (var i in query)
    {
    }
}
//}

The results are available at @<img>{linq_vs_pure_benchmark}. The comparison of execution times shows that the process with LINQ takes 19 times longer than the process without LINQ. 

//image[linq_vs_pure_benchmark][Performance Comparison Results with and without LINQ][scale=1.0]

 While the above results clearly show that the use of LINQ deteriorates performance, there are cases where the coding intent is more easily conveyed by using LINQ. 
After understanding these behaviors, there may be room for discussion within the project as to whether to use LINQ or not, and the rules for using LINQ. 

=={practice_script_csharp_async_await} How to avoid async/await overhead
Async/await is a language feature added in C# 5.0 that allows asynchronous processing to be written as a single synchronous process without callbacks. 

===={practice_script_csharp_async_await_compiler_generated} Avoid async where it is not needed 
Methods defined async will have code generated by the compiler to achieve asynchronous processing. 
And if the async keyword is present, code generation by the compiler is always performed. 
Therefore, even methods that may complete synchronously, such as @<list>{possibly_not_async_sample}, are actually code generated by the compiler. 

//listnum[possibly_not_async_sample][Asynchronous processing that may complete synchronously][csharp]{
using System;
using System.Threading.Tasks;

namespace A {
    public class B {
        public async Task HogeAsync(int i) {
            if (i == 0) {
                Console.WriteLine("i is 0");
                return;
            }
            await Task.Delay(TimeSpan.FromSeconds(1));
        }

        public void Main() {
            int i = int.Parse(Console.ReadLine());
            Task.Run(() => HogeAsync(i));
        }
    }
}
//}

In cases such as @<list>{possibly_not_async_sample}, the cost of generating a state machine structure for @<code>{IAsyncStateMachine} implementation, which is unnecessary in the case of synchronous completion, can be omitted by splitting @<code>{HogeAsync}, which may be completed synchronously, and implementing it as @<list>{complete_async_method_sample}. 


//listnum[complete_async_method_sample][Split implementation of synchronous and asynchronous processing][csharp]{
using System;
using System.Threading.Tasks;

namespace A {
    public class B {
        public async Task HogeAsync(int i) {
            await Task.Delay(TimeSpan.FromSeconds(1));
        }

        public void Main() {
            int i = int.Parse(Console.ReadLine());
            if (i == 0) {
                Console.WriteLine("i is 0");
            } else {
                Task.Run(() => HogeAsync(i));
            }
        }
    }
}
//}

====[column] How async/await works

The async/await syntax is realized using compiler code generation at compile time. 
Methods with the async keyword add a process to generate a structure implementing @<code>{IAsyncStateMachine} at compile time, and the async/await function is realized by managing a state machine that advances state when the process to be awaited completes. 
Also, this @<code>{IAsyncStateMachine} is an interface defined in the @<code>{System.Runtime.CompilerServices} namespace and is available only to the compiler. 

====[/column]

===={practice_script_csharp_async_await_thread_context_capture} Avoid capturing synchronous context 
The mechanism to return to the calling thread from asynchronous processing that has been saved to another thread is synchronous context, and @<code>{await} The previous context can be captured by using 
Since this synchronous context is captured each time @<code>{await} is executed, there is an overhead for each @<code>{await}. 
For this reason, UniTask @<fn>{unitask_reference}, which is widely used in Unity development, is implemented without @<code>{ExecutionContext} and @<code>{SynchronizationContext} to avoid the overhead of synchronous context capture. 
As far as Unity is concerned, implementing such libraries may improve performance. 

//footnote[unitask_reference][@<href>{https://tech.cygames.co.jp/archives/3417/}]

=={practice_script_csharp_stackalloc} Optimization with stackalloc

Allocating arrays as local variables causes GC.Alloc to occur each time, which can lead to spikes. 
In addition, reading and writing to the heap area is a little less efficient than to the stack area. 

Therefore, in C#, the @<kw>{unsafe} code-only syntax for allocating arrays on the stack. 

@<list>{how_to_use_stackalloc} Instead of using the @<code>{new} keyword, as in the following example, an array can be allocated on the stack using the @<code>{stackalloc} keyword. 

//listnum[how_to_use_stackalloc][@<code>{stackalloc} Allocating an array on the stack using the][csharp]{
//  stackalloc is limited to unsafe
unsafe
{
    //  Allocating an array of ints on the stack
    byte* buffer = stackalloc byte[BufferSize];
}
//}

Since C# 7.2, the @<code>{Span<T>} structure can be used to allocate an array of ints on the stack as shown in @<list>{how_to_use_safe_stackalloc}@<kw>{The  structure can now be used without unsafe}@<code>{stackalloc} can be used without unsafe as shown in . 

//listnum[how_to_use_safe_stackalloc][@<code>{Span<T>} Allocating an array on the stack using the struct][csharp]{
Span<byte> buffer = stackalloc byte[BufferSize];
//}

For Unity, this is standard from 2021.2. For earlier versions, @<code>{Span<T>} does not exist, so System.Memory.dll must be installed. 

Arrays allocated with @<code>{stackalloc} are stack-only and cannot be held in class or structure fields. They must be used as local variables. 

Even though the array is allocated on the stack, it takes a certain amount of processing time to allocate an array with a large number of elements. 
If you want to use arrays with a large number of elements in places where heap allocation should be avoided, such as in an update loop, it is better to allocate the array in advance during initialization or to prepare a data structure like an object pool, and implement it in such a way that it can be rented out when used. 

Also, note that the stack area allocated by @<code>{stackalloc} is @<em>{not released until the function exits}. 
For example, the code shown at @<list>{stackalloc_with_loop} may cause a Stack Overflow while looping, since all arrays allocated in the loop are retained and released when exiting the @<code>{Hoge} method. 

//listnum[stackalloc_with_loop][@<code>{stackalloc} Allocating Arrays on the Stack Using][csharp]{
unsafe void Hoge()
{
    for (int i = 0; i < 10000; i++)
    {
        //  Arrays are accumulated for the number of loops
        byte* buffer = stackalloc byte[10000];
    }
}
//}

=={practice_script_csharp_sealed} Optimizing method invocation under IL2CPP backend with sealed

When building with IL2CPP as a backend in Unity, method invocation is performed using a C++ vtable-like mechanism to achieve virtual method invocation of the class @<fn>{il2cpp_internal_method_call}. 

//footnote[il2cpp_internal_method_call][@<href>{https://blog.unity.com/technology/il2cpp-internals-method-calls}]

Specifically, for each method call definition of a class, the code shown at @<list>{il2cpp_method_call} is automatically generated. 

//listnum[il2cpp_method_call][C++ code for method calls generated by IL2CPP][c++]{
struct VirtActionInvoker0
{
    typedef void (*Action)(void*, const RuntimeMethod*);

    static inline void Invoke (
        Il2CppMethodSlot slot, RuntimeObject* obj)
    {
        const VirtualInvokeData& invokeData =
            il2cpp_codegen_get_virtual_invoke_data(slot, obj);
        ((Action)invokeData.methodPtr)(obj, invokeData.method);
    }
};
//}

It generates @<em>{similar} C++ code not only for virutal methods, but also for @<em>{non-virtual methods that do not inherit at compile time}. 
This auto-generated behavior leads @<em>{to bloated code size and increased processing time for method calls.}

This problem can be avoided by adding the @<code>{sealed} modifier to the class definition @<fn>{il2cpp_devirtualization}. 

//footnote[il2cpp_devirtualization][@<href>{https://blog.unity.com/technology/il2cpp-optimizations-devirtualization}]

@<list>{il2cpp_method_call_non_sealed} If you define a class like @<list>{il2cpp_method_call_non_sealed_cpp} and call a method, the C++ code generated by IL2CPP will generate a method call like . 

//listnum[il2cpp_method_call_non_sealed][Class definition and method invocation without sealed][csharp]{
public abstract class Animal
{
    public abstract string Speak();
}

public class Cow : Animal
{
    public override string Speak() {
        return "Moo";
    }
}

var cow = new Cow();
//  Calling the Speak method
Debug.LogFormat("The cow says '{0}'", cow.Speak());
//}

//listnum[il2cpp_method_call_non_sealed_cpp][@<list>{il2cpp_method_call_non_sealed} The C++ code corresponding to the method call in][c++]{
//  var cow = new Cow();
Cow_t1312235562 * L_14 =
    (Cow_t1312235562 *)il2cpp_codegen_object_new(
        Cow_t1312235562_il2cpp_TypeInfo_var);
Cow__ctor_m2285919473(L_14, /*hidden argument*/NULL);
V_4 = L_14;
Cow_t1312235562 * L_16 = V_4;

//  cow.Speak()
String_t* L_17 = VirtFuncInvoker0< String_t* >::Invoke(
    4 /* System.String AssemblyCSharp.Cow::Speak() */, L_16);
//}

@<list>{il2cpp_method_call_non_sealed_cpp} as shown in @<code>{VirtFuncInvoker0< String_t* >::Invoke} even though it is not a virtual method call, confirming that a method call like a virtual method is being made. 

On the other hand, defining the @<code>{Cow} class of @<list>{il2cpp_method_call_non_sealed} with the @<code>{sealed} modifier as shown in @<list>{il2cpp_method_call_sealed} generates C++ code like @<list>{il2cpp_method_call_sealed_cpp}. 

//listnum[il2cpp_method_call_sealed][Class Definition and Method Calls Using the SEALED][csharp]{
public sealed class Cow : Animal
{
    public override string Speak() {
        return "Moo";
    }
}

var cow = new Cow();
//  Calling the Speak method
Debug.LogFormat("The cow says '{0}'", cow.Speak());
//}

//listnum[il2cpp_method_call_sealed_cpp][@<list>{il2cpp_method_call_sealed} C++ code corresponding to a method call of][c++]{
//  var cow = new Cow();
Cow_t1312235562 * L_14 =
    (Cow_t1312235562 *)il2cpp_codegen_object_new(
        Cow_t1312235562_il2cpp_TypeInfo_var);
Cow__ctor_m2285919473(L_14, /*hidden argument*/NULL);
V_4 = L_14;
Cow_t1312235562 * L_16 = V_4;

//  cow.Speak()
String_t* L_17 = Cow_Speak_m1607867742(L_16, /*hidden argument*/NULL);
//}

Thus, we can see that the method call calls @<code>{Cow_Speak_m1607867742}, which directly calls the method. 

However, in relatively recent Unity, the Unity official clarifies that this is done partially automatically in such optimizations @<fn>{improved_devirtualization_forum}. 

//footnote[improved_devirtualization_forum][@<href>{https://forum.unity.com/threads/il2cpp-is-sealed-not-worked-as-said-anymore-in-unity-2018-3.659017/#post-4412785}]

In other words, even if you do not explicitly specify @<code>{sealed}, it is possible that such optimization is done automatically. 

However, the "[il2cpp] Is `sealed` Not Worked As Said Anymore In Unity 2018.3?" 
@<m>{\footnotemark[8]} As mentioned in the forum, this implementation is not complete as of April 2019. 

Because of this current state of affairs, it would be a good idea to check the code generated by IL2CPP and decide on the setting of the sealed modifier for each project. 

For more reliable direct method calls, and in anticipation of future IL2CPP optimizations, it may be a good idea to set the @<code>{sealed} modifier as an optimizable mark. 

=={practice_script_csharp_inline} Optimization through inlining

Method calls have some cost. 
Therefore, as a general optimization, not only for C# but also for other languages, relatively small method calls are optimized by compilers through inlining. 

Specifically, for code such as @<list>{inline_sample_no_inline}, inlining generates code such as @<list>{inline_sample_inline}. 

//listnum[inline_sample_no_inline][Code before inlining][csharp]{
int F(int a, int b, int c)
{
    var d = Add(a, b);
    var e = Add(b, c);
    var f = Add(d, e);

    return f;
}

int Add(int a, int b) => a + b;
//}

//listnum[inline_sample_inline][@<list>{inline_sample_no_inline} Code with inlining for][csharp]{
int F(int a, int b, int c)
{
    var d = a + b;
    var e = b + c;
    var f = d + e;

    return f;
}
//}

Inlining is done by copying and expanding the contents within a method, such as @<list>{inline_sample_inline}, and the call to the @<code>{Add} method within the @<code>{Func} method of @<list>{inline_sample_no_inline}. 

In IL2CPP, no particular inlining optimization is performed during code generation. 

However, starting with Unity 2020.2, by specifying the @<code>{MethodImpl} attribute for a method and @<code>{MethodOptions.AggressiveInlining} for its parameter, the corresponding function in the generated C++ code will be given the @<code>{inline} specifier. 
In other words, inlining at the C++ code level is now possible. 

The advantage of inlining is that it not only reduces the cost of method calls, but also saves copying of arguments specified at the time of method invocation. 

For example, arithmetic methods take multiple relatively large structures as arguments, such as @<code>{Vector3} and @<code>{Matrix}. 
If the structs are passed as arguments as they are, they are all copied and passed to the method as passed by value. If the number of arguments and the size of the passed structs are large, the processing cost may be considerable for method calls and argument copying. 
In addition, method calls may become a case that cannot be overlooked as a processing burden because they are often used in periodic processing, such as in the implementation of physical operations and animations. 

In such cases, optimization through inlining can be effective. In fact, Unity's new mathmatics library @<kw>{Mathmatics} specifies @<code>{MethodOptions.AggressiveInlining} for method calls everywhere @<fn>{unity_mathmatics_inline}. 

On the other hand, inlining has the disadvantage that the code size increases with the expansion of the process within the method. 

Therefore, it is recommended to consider inlining especially for methods that are frequently called in a single frame and are hot-passed. 
It should also be noted that specifying an attribute does not always result in inlining. 

Inlining is limited to methods that are small in content, so methods that you want to inline must be kept small. 

Also, in Unity 2020.2 and earlier, the @<code>{inline} specifier is not attached to attribute specifications, and there is no guarantee that inlining will be performed reliably even if the C++ @<code>{inline} specifier is specified. 

Therefore, if you want to ensure inlining, you may want to consider manual inlining for methods that are hotpaths, although it will reduce readability. 

//footnote[unity_mathmatics_inline][@<href>{https://github.com/Unity-Technologies/Unity.Mathematics/blob/f476dc88954697f71e5615b5f57462495bc973a7/src/Unity.Mathematics/math. cs#L1894}]
