# LaTeX and review-pdfmaker

The command `review-pdfmaker` in Re:VIEW use [LaTeX](https://en.wikipedia.org/wiki/LaTeX) to generate a PDF file.

To use the command, you need to set up LaTeX system.

## About new layout file and review-jsbook.cls

Since Re:VIEW 3.0, review-jsbook.cls becomes the default class file.

If `texdocumentclass` isn't specified, the layout file is switched based on the value of review_version in config.yml. If `review_version` is 2.0, older layout files and jsbook.cls are used for compatibility.

### Changes in layout file `layout.tex.erb`

The new layout file `layout.tex.erb` is incompatible with the Re:VIEW 2 version's.

The values of config.yml are converted to a TeX macro, and they are used for class files and style files. This makes it easier (although knowledge of TeX is necessary) to use another compiler other than upLaTeX, or to create your own class files and style files.

Conversion from config.yml to TeX macro is executed in `templates/latex/config.erb` of Re:VIEW. As a result, the embedded code of Ruby in `layout.tex.erb` exists only in the following preamble.

```
\documentclass[<%= @documentclassoption %>]{<%= @documentclass %>}
<%= latex_config %>
<%- if @config['texstyle'] -%>
<%-   [@config['texstyle']].flatten.each do |x| -%>
\usepackage{<%= x %>}
<%-   end -%>
<%- end -%>

\begin{document}

```

In the new `layout.tex.erb`, content is constructed by defining or changing the following macros.

* `\reviewbegindocumenthook` : hook begining the document
* `\reviewcoverpagecont` : cover contents
* `\reviewfrontmatterhook` : hook before frontmatter
* `\reviewtitlepagecont` : titlepage contents
* `\revieworiginaltitlepagecont` : original titlepage contents (in the case of a translation)
* `\reviewcreditfilecont` : credit contents
* `\reviewprefacefiles` : frontmatter contents (PREDEF)
* `\reviewtableofcontents` : table of contents
* `\reviewmainmatterhook` : hook before mainmatter
* `\reviewchapterfiles` : mainmatter contents (CHAPS)
* `\reviewappendixhook` : hook before appendix
* `\reviewappendixfiles` : appendix contents (APPENDIX)
* `\reviewbackmatterhook` : hook before backmatter
* `\reviewpostdeffiles` : backmatter contents (POSTDEF)
* `\reviewprintindex` : index
* `\reviewprofilepagecont` : profile contents
* `\reviewadvfilepagecont` : advertisement contents
* `\reviewcolophonpagecont` : colophon contents
* `\reviewbackcovercont` : backcover contents
* `\reviewenddocumenthook` : hook before ending the document

It is also possible to continue using your `layouts/layout.tex.erb`.

### structure of review-jsbook.cls set

When creating a new document folder with the `review-init` command, the following files are created in `sty` folder.

* `review-jsbook.cls` : provides a basic design of paper representation.
* `jsbook.cls` : is a copy of the upstream jsbook. To avoid differences in version on TeX distribution, Re:VIEW provides a snapshot of `jsbook.cls`.
* `plistings.sty` : provides highlight expressions of program code.
* `jumoline.sty` : expresses underlining.
* `gentombow.sty` : makes bleed box.
* `reviewmacro.sty` : imports the following style file. By specifying this file with `texstyle` parameter of config.yml, all style files are imported.
* `review-base.sty` : associates Re:VIEW macro name and TeX macro. Also defines the entity of each macros of `layout.tex.erb`.
* `review-style.sty` :defines visual decorations.
* `review-custom.sty` : is empty file. It is assumed that the user adds macros or overwrites existing macros arbitrarily.

Default value of texdocumentclass are as follows.

```
texdocumentclass: ["review-jsbook", "media=print,paper=a4,cover=false"]

texdocumentclass: ["review-jsbook", ""] (same effort)
```

### About review-jlreq.cls

Since Re:VIEW 3.0, review-jlreq.cls is also provided. This class file extends jlreq.cls which makes paper design based on "Requirements for Japanese Text Layout" ( https://www.w3.org/TR/2012/NOTE-jlreq-20120403/ ).

`review-init --latex-template=review-jlreq` command copies the review-jlreq.cls set to `sty` folder.

### Handing over custom parameters

For technical reasons, only a small part of the YAML parameters in config.yml is converted to TeX macros. Re:VIEW's internal `config.erb` ( https://github.com/ kmuto/review/blob/master/templates/latex/ config.erb ) ERB script manages this. You cannot change this script.

If you want to give YAML parameters to TeX, you can use your own ERB script that do the YAML to TeX conversion. This is done by creating `layouts/config-local.tex.erb` in the project folder.

After evaluation and embedding of `config.erb`, `config-local.tex. erb` will be evaluated and embedded as well. For example,

`config.yml`:

```
mycustom:
  mystring: HELLO_#1
  mybool: true
```

`layouts/config-local.tex.erb`:

```
\def\mystring{<%= escape(@config['mycustom']['mystring']) %>}
<%- if @config['mycustom']['mybool'] -%>
\def\mybool{true}
<%- end -%>
```

will be parsed:

```
 …
\makeatother
%% BEGIN: config-local.tex.erb
\def\mystring{HELLO\textunderscore{}\#1}
\def\mybool{true}
%% END: config-local.tex.erb

\usepackage{reviewmacro}
 …
```

Refer to these macros in your sty file.

## Important Changes about LaTeX in Re:VIEW 2.0

* Default LaTeX compiler is upLaTeX, not pLaTeX.
* The meaning of `scale` option in `@<image>` command is changed and configurable.
* `prt` is printer, not publisher. You can use `pbl` for publisher.

## about upLaTeX

In Re:VIEW 2.0 released at April 2016, default LaTeX compiler became upLaTeX from pLaTeX.  The upLaTeX support Unicode and you can use unicode characters and other characters without otf package.

Almost packages in pLaTeX can be supported in upLaTeX, but some package (such as jsbook class and otf package) need `uplatex` option.

Default settings of Re:VIEW is below:

```yaml
texcommand: uplatex
texoptions: "-interaction=nonstopmode -file-line-error -halt-on-error"
texdocumentclass: ["review-jsbook", "uplatex,twoside"]
dvicommand: dvipdfmx
dvioptions: "-d 5"
```

## How to use old pLaTeX

You also use pLaTeX with Re:VIEW 2.0.

To use pLaTeX, You can add these configuration in config.yml.

```yaml
texcommand: platex
texoptions: "-kanji=utf-8"
texdocumentclass: ["jsbook", "oneside"]
dvicommand: dvipdfmx
dvioptions: "-d 5"
```

You can use a variable `@texcompiler` to compare latex command in layout ERB files (lib/review/layout.tex.erb in default).
The value of `@texcompiler` is `platex` (when using pLaTeX) and `uplatex` (when using upLaTeX).
The usage is below:

```
<% if @texcompiler == "uplatex" %>
\usepackage[deluxe,uplatex]{otf}
<% else %>
\usepackage[deluxe]{otf}
<% end %>
```

## about `scale` option

In Re:VIEW 2.0, the meaning of `scale=..` in the third option of ``//image`` command.
The meaning in 1.0 is "scale for image file" (`1.0` is same as original image size), but the one in 2.0 is "scale for paper width" (`1.0` is same as paper widdth.)

If you need the same behavior in Re:VIEW 1.x, you should add ``image_scale2width: false`` in config.yml (default value is `true`).

```yaml
image_scale2width: false
```
