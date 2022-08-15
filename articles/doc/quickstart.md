# Re:VIEW Quick Start Guide

Re:VIEW is a toolset to convert from Re:VIEW format documents into various formats.

Re:VIEW uses original lightweight markup language like EWB, RD or Wiki.  Its syntax is simple but powerful for writing IT documents.
When you write your documents in Re:VIEW format, you can convert them with Re:VIEW tools into LaTeX, HTML, EPUB, InDesign, Text, and so on.

Re:VIEW is free software under the terms of the GNU Lesser General Public License Version 2.1, so you can use, modify and redistribute it. This license has no relations with your documents using Re:VIEW, so your documents are not forced to use this license. When you want to distribute Re:VIEW software itself or the system including Re:VIEW software, you should read COPYING file carefully.

This article describes how to setup Re:VIEW and use it.

The supported version of the article is Re:VIEW 5.1.

## Set up Re:VIEW

Re:VIEW is a software in Ruby and worked in Linux/Unix, macOS, and Windows. You can install Re:VIEW with RubyGems or Git.

Note that Re:VIEW format is plain texts with simple markups in UTF-8, so you can write it on any editors and OSes.

### using RubyGems

Re:VIEW is released as RubyGems.

* https://rubygems.org/gems/review

Add this line to your application's Gemfile:

```Gemfile
gem 'review'
```

And then execute:

```
$ bundle
```

Or install it yourself as:

```bash
$ gem install review
```

### using Git

You can get latest Re:VIEW sources from GitHub.

```bash
$ git clone https://github.com/kmuto/review.git
```

You can use Re:VIEW to add `review/bin` directory to `$PATH` variable.

You can update the sources as follows:

```bash
$ git pull
```

## Writing Re:VIEW Document

### Generating template files

After setup, you can use `review-init` command to generate Re:VIEW project directory.

To generate `hello` project:

```bash
$ review-init hello
$ cd hello
$ ls hello
Rakefile     catalog.yml  config.yml   hello.re     images/      layouts/     sty/         style.css
```

If your purpose is to create a PDF using TeX, it is a good idea to use the wizard mode for basic layouting in a web browser. Add `-w` option to review-init.

(notice: currently wizard UI supports only Japanese language)

```bash
$ review-init -w hello
(access http://localhost:18000 on Web browser)
```

In `hello` directory, many files are generated.

* `*.re` : Re:VIEW format file.
* config.yml : configuration file
* config-ebook.yml : configuration file for ebook (when using the wizard mode)
* catalog.yml : catalog file (for TOC)
* Rakefile : rule file for `rake` command
* images : folder for images
* style.css : sample stylesheet
* sty : style files for LaTeX
* doc : various documents (including this document)
* lib : detailed rule files of rake command and etc.

### writing Re:VIEW documents and converting them

```review
= My First Re:VIEW

//lead{
"Hello, Re:VIEW."
//}

== What's Re:VIEW

@<b>{Re:VIEW} is a converter from simple markup documents into various formats.

You can generate documents as follows:

 * text with tagging
 * LaTeX
 * HTML
 * XML

You can install Re:VIEW with:

 1. RubyGems
 2. Git
 3. Download from GitHub

The web site of Re:VIEW is @<tt>{https://reviewml.org/}.
```

You should use UTF-8 as encodings in text files.

### generating PDF, EPUB, plain-text, and InDesign XML

To convert files in the project, use review-*maker command.

- review-pdfmaker: generate PDF
- review-epubmaker: generate EPUB
- review-textmaker: genrate plaintext
- review-idgxmlmaker: genrate InDesign XML

To generate PDF, you should install TeXLive 2012 or later. To generate EPUB, you should install zip command.
When you want to use MathML, you should install [MathML library](http://www.hinet.mydns.jp/?mathml.rb)

Each maker need `config.yml`, configuration YAML files. `review-init` command generates `config.yml` in default.

```bash
$ review-pdfmaker config.yml       ## generate PDF
$ review-pdfmaker config-ebook.yml ## generate PDF (for ebook)
$ review-epubmaker config.yml      ## generate EPUB
$ review-textmaker config.yml      ## generate text with decoration
$ review-textmaker -n config.yml   ## generate text without decoration
$ review-idgxmlmaker config.yml    ## generate InDesign XML
```

You also can generate them with Rake.

```bash
$ rake pdf       ## generate PDF
$ REVIEW_CONFIG_FILE=config-ebook.yml rake pdf       ## generate PDF (for ebook)
$ rake epub      ## generate EPUB
$ rake text      ## generate text with decoration
$ rake plaintext ## generate text without decoration
$ rake idgxml    ## generate InDesign XML
```

There is a sample YAML file [config.yml.sample](https://github.com/kmuto/review/blob/master/doc/config.yml.sample) in the same directory of this document.

#### generate PDF using Vivliostyle CLI

Instead of using TeX (`review-pdfmaker` or `rake pdf`), you can also create a PDF use [Vivliostyle CLI](https://github.com/vivliostyle/vivliostyle-cli). Re:VIEW creates EPUB first and then converts it to PDF with Vivliostyle CLI.

```bash
$ rake vivliostyle:build    ## build PDF using Viliostyle
$ rake vivliostyle:preview  ## preview pages in Chrome/Chromium browser
$ rake vivliostyle          ## shortcut of vivliostyle:build
```

### add chapters and modify them

`catalog.yml` file is a catalog of Re:VIEW format files.

If you create new `*.re` files as new chapters, you should add the name of files into `catalog.yml`.

```bash
$ cat catalog.yml
PREDEF:

CHAPS:
  - first-chapter.re
  - second-chapter.re

APPENDIX:

POSTDEF:
```

The first item in CHAPS is the first chapter, and the second item (if you add) is the second chapter. PREDEF is for front matter, APPENDIX is for appendix, and POSTDEF is for back matter.  You can see in detail with [catalog.md](https://github.com/kmuto/review/blob/master/doc/catalog.ja.md).

### more information

For more information about Re:VIEW format, see [format.md](https://github.com/kmuto/review/blob/master/doc/format.md).

## convert file by file

You can convert a *.re file with `review-compile` command.

When you want to convert sample.re, you can do as belows:

```bash
$ review-compile --target text sample.re > sample.txt    ## for text
$ review-compile --target html sample.re > sample.html   ## for HTML
$ review-compile --target latex sample.re > sample.tex   ## for LaTeX
$ review-compile --target idgxml sample.re > sample.xml  ## for XML
$ review-compile --target markdown sample.re > sample.md ## for Markdown
```

`review-compile` and other commands in Re:VIEW has `--help` option to output help.  `review-compile` has many options, so you may see them.

## preprocessor and other commands

With Re:VIEW tags such as `#@mapfile`, `#@maprange` and `#@mapoutput`, you can include content of files or result of command execution. To do so, you use `review-preproc` command.

```bash
$ review-preproc hello_orig.re > hello.re   ## redirect stdout into file

## also:
$ review-preproc --replace hello.re   ## update files overwriting
```

You can use `review-vol` command to know the volume of the document.

```bash
$ review-vol
```

You can also use `review-index` command to generate header list.

```bash
$ review-index --level <heading level>
$ review-index --level <heading level> -d  ## show volumes also
```

review-vol and review-index can have large differences in the number of characters, lines, and pages. While review-vol only estimates from the file, review-index actually compiles and calculates.

## how to update the document folder to the new Re:VIEW version

Re:VIEW is updated regularly. We Re:VIEW team watch the backward compatibility carefully, but if you want to use the features in the new version, you can use review-update command to update the project folder.

```bash
$ review-update
** review-update updates your project to 5.0.0 **
config.yml: Update 'review_version' to '5.0'? [y]/n
Rakefile will be overridden with Re:VIEW version (/.../review/samples/sample-book/src/Rakefile). Do you really proceed? [y]/n
lib/tasks/review.rake will be overridden with Re:VIEW version (/.../review/samples/sample-book/src/lib/tasks/review.rake). Do you really proceed? [y]/n
INFO: new file /.../sty/plistings.sty is created.
/.../sty/reviewmacro.sty will be overridden with Re:VIEW version (/.../review/templates/latex/review-jsbook/reviewmacro.sty). Do you really proceed? [y]/n
INFO: new file /.../sty/README.md is created.
INFO: new file /.../sty/review-custom.sty is created.
INFO: new file /.../sty/review-jsbook.cls is created.
INFO: new file /.../sty/review-style.sty is created.
INFO: new file /.../sty/review-base.sty is created.
INFO: new file /.../sty/gentombow.sty is created.
Finished.
```

## Copyright

The original author of Re:VIEW is Minero Aoki. The current maintainer is Kenshi Muto(@kmuto), and committers are Masayoshi Takahashi and Masanori Kado (March 2020).

If you want to report bugs and patches, or to get more information, see:

* http://reviewml.org/
* https://github.com/kmuto/review/wiki
