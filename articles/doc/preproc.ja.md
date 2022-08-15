# review-preproc ユーザガイド

Re:VIEWではソースコードなどを本文中に埋め込むことができます。

## `#@mapfile`

以下は`scripts/hello.rb`のソースを本文内に埋め込んでいます。

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
「`#@mapfile(ファイル名)`」から「`#@end`」は、指定したファイル名のファイル全体を本文中に埋め込むための記法です。埋め込む際には、`review-preproc`コマンドを実行します。

まず、以下のように、引用したい部分に「`#@mapfile()`」と「`#@end`」だけを書きます。ここでは`//list`ブロック内にこの2行だけを記述しています。

```review
//list[hello.rb.1][hello.re]{
#@mapfile(scripts/hello.rb)
#@end
//}
```
このソースに対して、`review-preproc`コマンドを実行します。すると、変換結果の*.reファイルを標準出力に出力します。

```shell-session
$ review-preproc hello.re
```
`--replace`オプションをつければ、ファイルそのものを置き換えるようになります。

```shell-session
$ review-preproc --replace hello.re
```

なお、rakeを使ってビルドしている場合、Rakefileに以下のような記述を追加すると、このコマンドを実行するtaskが定義されます。

```rake
desc 'preproc all'
task :preproc do
  Dir.glob("*.re").each do |file|
    sh "review-preproc --replace #{file}"
  end
end
```

以下のように実行すれば、コマンドが実行されます。

```shell-session
$ rake preproc
```

## `#@maprange`

対象ファイルの一部だけ抜粋することもできます。こちらは「`#@maprange`」という記法を使います。

対象ファイルの抜粋したい部分の前後に、「`#@range_begin(ID)`」と「`#@range_end`」という記法を埋め込んでおきます。`#@range_begin`の引数には、その断片を抽出する際に使用するIDを指定します。以下の例では「sample」というIDにしています。

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
*.reファイルのほうには、「`#@mapfile(ファイル名)`」ではなく「`#@maprange(ファイル名,ID)`」を記述します。

```review
//list[range.rb][range.rb(抜粋)]{
#@maprange(scripts/range.rb,sample)
#@end
//}
```

あとは、先ほど同様、`review-preproc`コマンドか`rake preproc`コマンドを実行します。そうすると、`scripts/range.rb`の一部が抽出されて埋め込まれます。

```review
//list[range.rb][range.rb(抜粋)]{
#@maprange(scripts/range.rb,sample)
  def hello(name)
    print "hello, #{name}!\n"
  end
#@end
//}
```

## `#@@maprange`

もっとも、Ruby以外では「`#@range_begin`」などがコメントと解釈されないこともあります。そのような場合、その言語のコメントの中に「`#@@range_begin`」と「`#@@range_end`」とを記述します。

Cのソースを例にします。対象となるrange.cは以下です。

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

put_hello関数の定義の前後で「`#@@range_begin`」と「`#@@range_end`」が使われています。

これに対し、*.reファイルでは`#@maprange(scripts/range.c,sample)`と`#@end`を記述します。

```
//list[range.c][range.c(抜粋)]{
#@maprange(scripts/range.c,sample)
#@end
//}
```

変換した結果、以下のようになります。

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
このようにすると、Cなどの言語のファイルに対しても、ソースの一部を抽出して埋め込むことができます。
