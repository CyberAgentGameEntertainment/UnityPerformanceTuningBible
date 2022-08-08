# `review-preproc` User Guide

Re:VIEW has a preprocessor, `review-preproc`.  It can embed a (part of) file into *.re files.

## `#@mapfile`

The code below is a sample of embedding code from `scripts/hello.rb`.

```review
//list[hello.rb][hello.rb]{
#@mapfile(scripts/hello.rb)
#!/usr/bin/env ruby

class Hello
  def hello(name)
    print "hello, #{name}!\n"
  end
end

if __FILE__ == $0
  Hello.new.hello("world")
end
#@end
 //}
```

`#@mapfile(filename)` and `#@end` are markers for preprocessor.  You can embed and update the content of the file `filename` into Re:VIEW file with `review-preproc` command.

First, add `#@mapfile()` and `#@end` on the document.

```review
//list[hello.rb.1][hello.re]{
#@mapfile(scripts/hello.rb)
#@end
//}
```

Then execute `review-preproc` command.  The command display the result on stdout.

```shell-session
$ review-preproc hello.re
```

With `--replace` option, `review-preproc` replaces the Re:VIEW file.

```shell-session
$ review-preproc --replace hello.re
```

If you use `rake` to build Re:VIEW document, you can add the code to Rakefile to define a task of `review-preproc`.

```rake
desc 'preproc all'
task :preproc do
  Dir.glob("*.re").each do |file|
    sh "review-preproc --replace #{file}"
  end
end
```

Then, you can execute preprocessor with `rake`.

```shell-session
$ rake preproc
```

## `#@maprange`

You also can embed a part of the target file with `#@maprange` marker.

Add `#@range_begin(ID)` and `#@range_end` in the target file you want to embed.
In `#@range_begin(ID)`, you must add ID as parameger to identify the part.

In the code below, `sample` is the ID of the range to embed.

```ruby
#!/usr/bin/env ruby

class Hello
#@range_begin(sample)
  def hello(name)
    print "hello, #{name}!\n"
  end
#@range_end(sample)
end

if __FILE__ == $0
  Hello.new.hello("world")
end
```

In *.re file, add `#@maprange(filename,ID)`, instead of `#@mapfile(filename)`, and `#@end`.

```review
//list[range.rb][range.rb(partial)]{
#@maprange(scripts/range.rb,sample)
#@end
//}
```

After that, execute `review-preproc` or `rake preproc`, and you get the *.re files embedded the `hello` method in `scripts/range.rb`.

```review
//list[range.rb][range.rb(partial)]{
#@maprange(scripts/range.rb,sample)
  def hello(name)
    print "hello, #{name}!\n"
  end
#@end
 //}
```

## `#@@maprange`

But in some language, a line `#@range_begin` is not a comment.  If you want to embed some code of such languages, you can use new markers `#@@range_begin` and `#@@range_end` into a comment in the target document.

Let's explain how to do.  A target file `range.c` is below:

```c
#include <stdio.h>

/* #@@range_begin(sample)  */
void
put_hello(char *name)
{
  printf("hello, %s!\n", name);
}
/* #@@range_end(sample) */

int main()
{
  put_hello("world");
}
```

There are markers `#@@range_begin` and `#@@range_end` around `put_hello` method.

In `*.re`, add `#@maprange(scripts/range.c,sample)` and `#@end`.


```
//list[range.c][range.c(抜粋)]{
#@maprange(scripts/range.c,sample)
#@end
//}
```

With `review-preproc`, we get the result of conversion like below:

```
//list[range.c][range.c(抜粋)]{
#@maprange(scripts/range.c,sample)
void
put_hello(char *name)
{
  printf("hello, %s!\n", name);
}
#@end
//}
```
