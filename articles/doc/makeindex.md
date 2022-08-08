# How to use index
You can embed an index with using inline tags `@<idx>` and `@<hidx>`. In order to sort the index containing Kanji, "syllabary" of the Kanji is necessary. LaTeX builder can sort and layout them using a dictionary or morphological analysis.

## Notice
* Since it depends on mendex of LaTeX, it may work only in English or Japanese at present.
* Morphological analysis relies on external software MeCab (http://taku910.github.io/mecab/), IPA dictionary, and mecab Ruby binding.
* Other builders don't provide a method to use indexes. (see the hint.)
* The index generation is disabled by default. You have to explicitly enable in the onfiguration YAML file.

## Setting up MeCab
When using morphological analytics, it is necessary to install MeCab (http://taku910.github.io/mecab/), its dictionary, and mecab of Ruby binding library.

On Debian GNU/Linux or Ubuntu:

```
apt install mecab mecab-ipadic-utf8 ruby-mecab
```

There are 2 standard dictionaries for MeCab, "IPA" and "Juman", but use "IPA". Because only IPA supports "-Oyomi" option.

## Configuration
To enable index generation in review-pdfmaker, add a configuration to config.yml.

```
pdfmaker:
  makeindex: true
```

Embed the indexes in Re:VIEW re files using inline tags `@<idx>` or `@<hidx>`.
`@<idx>` outputs the word and sets it as an index. `@<hidx>` (hidden index) only indexes the word (no output). By separating strings with `<<>>` in `@<hidx>`, like `parentindex<<>>childindex` or `parentindex<<>>childindex<<>>grandchildindex`, you can define a leveled index.

```
I @<idx>{display} it.
  ↓
I display it.

[Index]
D
display....1
```

```
I @<hidx>{display} it.@<hidx>{Country<<>>Japan}
  ↓
I it.

[Index]
C
 Country
   Japan....1


D
 display....1
```

## Dictionary for Kanji
Morphological analysis is not a silver bullet. It frequently misreads.
By providing a dictionary, mendex program will try to match the word with priority. Also even in environments where MeCab can't be used, you can use the index generation by using a dictionary to resolve Kanji words.

Dictionary file is a text file in which Kanji and Kana pairs are separated by tabs or spaces.

```
漢字  かんじ
読み  よみ
α    あるふぁ
```

Specify the above file (e.g. my.dic) in config.yml.

```
pdfmaker:
  makeindex: true
  makeindex_dic: my.dic
```

Due to mendex's algorithm, automatic Kanji sorting will not always succeed.
In complicated case, you will need to modify a LaTeX source using review-pdfmaker's hook feature. See online manual of mendex for details.

## Customize
The following is an index setting that can be described under `pdfmaker:` in config.yml.

* `makeindex`: if `true`, generate index page (default: `null` = don't generate)
* `makeindex_command`: command path of the index tool (default: `mendex`)
* `makeindex_options`: options of the index tool (default: `-f -r -I utf8`)
* `makeindex_sty`: style file path of the index tool (default: `null`)
* `makeindex_dic`: dictionary file path of the index tool (default: `null`)
* `makeindex_mecab`: use MeCab (default: `true` = yes, I use)
* `makeindex_mecab_opts`: options of MeCab (default: `-Oyomi`)

For example, if you want to change the appearance of the index page, specify the style file to `makeindex_sty`. See online manual of mendex for details.

## Hint
The builders other than LaTeX builder don't provide a method to use indexes. Because the targets of other builders don't have a general way of sorting or outputting indexes.

In EPUB (HTML builder), the index is embeded as `<!-- IDX:indexword -->` comment. In IDGXML builder, it is embeded as `<index value="indexword">` XML element.
You can probably pick up them and prepare your own tool to sort (by using LaTeX mendex command, e.g.) and output.
