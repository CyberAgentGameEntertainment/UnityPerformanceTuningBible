# LaTeX と review-pdfmaker について
Re:VIEW の review-pdfmaker は、フリーソフトウェアの簡易 DTP システム「LaTeX」を呼び出して PDF を作成しています。

そのため、利用にあたっては TeX の環境を別途セットアップしておく必要があります。OS に応じたセットアップについては、以下の TeX Wiki サイトなどを参照してください。

#### TeX Wiki - TeX入手法
* https://texwiki.texjp.org/?TeX入手法

## Re:VIEW バージョンによる変化についての注意
* Re:VIEW 3.0 より、使用するデフォルトのクラスファイルを jsbook.cls から review-jsbook.cls に変更しました。また、別のクラスファイルとして review-jlreq.cls も提供しています。以下の「新しいレイアウトファイルおよび review-jsbook.cls について」を参照してください。
* Re:VIEW 2.0 より、LaTeX コンパイラのデフォルトが pLaTeX から upLaTeX になりました。以下の「upLaTeX について」を参照してください。
* Re:VIEW 2.0 より、image タグに `scale` オプションを使って倍率数値を定義していた場合の挙動が変わりました。以下の「scale オプションの挙動について」を参照してください。
* Re:VIEW 2.0 より、config.yml 等の設定ファイルで使われる `prt` のデフォルトが「発行所」ではなく「印刷所」になりました。「発行所」には `pbl` のほうをお使いください。

## 新しいレイアウトファイルおよび review-jsbook.cls について

Re:VIEW 3.0 より、デフォルトのクラスファイルを jsbook.cls から review-jsbook.cls に切り替えました。

`texdocumentclass` で明示的な設定をしていない場合は、config.yml の `review_version` の値に基づいて切り替えられます。`review_version` が 2.0 の場合は、互換性保持のために従来のレイアウトファイルおよび jsbook.cls が使われるようになっています。

### レイアウトファイル `layout.tex.erb` の変更

レイアウトファイル  `layout.tex.erb` は Re:VIEW 2 バージョンのものから一新され、互換性はなくなりました。

これまでは `layout.tex.erb` の中で config.yml の値などの判定を Ruby の埋め込みコードで処理していましたが、config.yml の値はまず TeX のマクロに文字列として格納され、そのマクロをクラスファイルやスタイルファイルで参照して使用します。これにより、（TeX の知識はより必要になりますが）upLaTeX 以外の別のコンパイラを使用したり、独自のクラスファイルやスタイルファイルを作ったりすることが容易になります。

config.yml から TeX マクロへの転換は、Re:VIEW プログラムの `templates/latex/config.erb` で実装されています。この結果、新しい `layout.tex.erb` における Ruby の埋め込みコードは以下のプリアンブルの箇所のみとなります。

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

新しい `layout.tex.erb` では、以下のようなマクロを定義あるいは変更することで書籍を構成します。

* `\reviewbegindocumenthook` : ドキュメント開始直後のフック
* `\reviewcoverpagecont` : 表紙
* `\reviewfrontmatterhook` : 前付の前のフック
* `\reviewtitlepagecont` : 大扉
* `\revieworiginaltitlepagecont` : 翻訳書における原書大扉
* `\reviewcreditfilecont` : クレジット情報
* `\reviewprefacefiles` : 前付（PREDEF）
* `\reviewtableofcontents` : 目次
* `\reviewmainmatterhook` : 本文の前のフック
* `\reviewchapterfiles` : 本文（CHAPS）
* `\reviewappendixhook` : 付録の前のフック
* `\reviewappendixfiles` : 付録（APPENDIX）
* `\reviewbackmatterhook` : 後付の前のフック
* `\reviewpostdeffiles` : 後付（POSTDEF）
* `\reviewprintindex` : 索引
* `\reviewprofilepagecont` : 著者紹介
* `\reviewadvfilepagecont` : 広告
* `\reviewcolophonpagecont` : 奥付
* `\reviewbackcovercont` : 裏表紙
* `\reviewenddocumenthook` : ドキュメント終了直前のフック

過去のバージョンのように全面的に Ruby の埋め込みコードで各種の処理をしたいときには、任意の `layouts/layout.tex.erb` を使用し続けることも可能です。

### review-jsbook.cls の構成

`review-init` コマンドで新たなドキュメントフォルダを作成すると sty フォルダに以下のファイルが展開されます。

* `review-jsbook.cls` : 紙面表現の基本的な設計を提供するクラスファイルです。
* `jsbook.cls` : jsbook オリジナルのコピーです。TeX 配布物でのバージョン違いの差異を避けるため、Re:VIEW 本体でスナップショットを保持しています。
* `plistings.sty` : プログラムコードのハイライト表現を行う支援パッケージです。
* `jumoline.sty` : 下線を表現する支援パッケージです。
* `gentombow.sty` : トンボを表現する支援パッケージです。
* `reviewmacro.sty` : 以下のスタイルファイルを取り込むスタイルファイルです。このファイルを config.yml の texstyle パラメータで指定すれば、すべてのスタイルファイルが取り込まれます。
* `review-base.sty` : Re:VIEW固有のマクロ名と TeX のマクロとを結び付けるスタイルファイルです。また、`layout.tex.erb` の各マクロの実体を定義します。
* `review-style.sty` : 基本的な見映えに加えて見た目の調整を定義するスタイルファイルです。review-jsbook.cls が提供するこのファイルは、従来のバージョンとの互換性を持たせたもので、華やかさを追求したものではありません。
* `review-custom.sty` : このファイルは空の内容で、ユーザーが任意でマクロを追加したり既存のマクロを上書きしたりすることを想定したスタイルファイルです。

review-jsbook.cls は従来の jsbook.cls を包み、以下の機能を提供します。

* ドキュメントクラスオプションでほとんどの紙面設計を設定できます。
* クラスオプション `media=print` （デフォルト）で印刷用に適した紙面、`media=ebook` で電子PDF媒体に適した紙面を作成します。印刷用ではデジタルトンボおよび紙面上トンボを配置し、表紙は付けません。電子用ではトンボなしの仕上がりサイズで、表紙を付けます。明示的に表紙の有無を指定することも可能です（`cover=trueまたはfalse`）。
* クラスオプション `paper=紙サイズ` で用紙サイズを指定できます。
* クラスオプション `fontsize=基本文字サイズ`、`baselineskip=基本行送り`、`line_length=1行の字詰め`、`number_of_lines=行数`、`head_space=天`、`guter=ノド` による基本版面設計が可能です。
* クラスオプション `startpage=ページ番号` で大扉のページ開始番号を指定できます。
* クラスオプション `serial_pagination=trueまたはfalse` でページ番号を大扉からアラビア数字で通すことができます。
* クラスオプション `hiddenfolio=プリセット` でノドに隠しノンブルを入れることができます（プリセットは `nikko-pc` など）。

texdocumentclass パラメータに何も指定をしなければ、以下のようになります。

* 印刷用（`media=print`）。トンボあり
* 表紙なし
* 用紙はA4（`paper=a4`）
* 表紙なし（`cover=false`）
* 基本版面の基本文字サイズは 10pt、ほかは jsbook を継承
* 隠しノンブルなし

その他の詳細な設定については、sty フォルダにある README.md を参照してください。

既存のドキュメントフォルダを Re:VIEW 3.0 （およびより新しいバージョン）に更新するには、ドキュメントフォルダ内で review-update コマンドを実行します。これで、スタイルファイルのコピーや、config.yml 等の既存のパラメータを解析して妥当なオプション指定に移行するといった処理が行われます。

### review-jlreq.cls の構成

Re:VIEW 3.0 から review-jlreq.cls という別のクラスファイルも用意しています。review-jlreq.cls は「日本語組版処理の要件」( https://www.w3.org/TR/jlreq/ja/ ) に基づく紙面設計を採用している jlreq.cls を包んだクラスファイルです。

`review-init --latex-template=review-jlreq` コマンドで、このクラスファイルを使うように sty フォルダへファイルが展開されます。

その他の詳細な設定については、sty フォルダにある README.md を参照してください。

### カスタムパラメータの引き渡し

技術的な都合で、config.yml の YAML 設定から TeX のマクロへの変換はすべての設定について行われるわけではなく、ごく限られた値のみに制限しています。これは Re:VIEW 内部の `config.erb` ( https://github.com/kmuto/review/blob/master/templates/latex/config.erb ) の ERB スクリプトによって処理されており、この挙動を変更することは許容していません。

任意の YAML 設定を TeX に引き渡すには、`config.erb` と同様に YAML から TeX への変換を行う固有の ERB スクリプトを作成して配置します。このためには、プロジェクトフォルダに `layouts` フォルダを作成し、そこに固有の ERB スクリプトを `config-local.tex.erb` という名前で置きます。

`config-local.tex.erb` は `config.erb` の評価・埋め込みの後、評価・埋め込みされます。

config.yml を次のようにして、これを TeX に渡したいとします。

```
mycustom:
  mystring: HELLO_#1
  mybool: true
```

`layouts/config-local.tex.erb` はたとえば次のようになります。

```
\def\mystring{<%= escape(@config['mycustom']['mystring']) %>}
<%- if @config['mycustom']['mybool'] -%>
\def\mybool{true}
<%- end -%>
```

次のように展開されます。
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

こうして定義されたマクロを sty ファイルなどで参照します。

なお、ERB での YAML 解析、および TeX マクロの記述において誤りがあると、極めてわかりにくいエラーになることがあります。`--debug` オプション付きで実行して展開された `__REVIEW_BOOK__.tex` を確認して原因を調査するのがよいでしょう。

## Re:VIEW 2.0 以前の情報

### upLaTeX について

2016年4月リリースの Re:VIEW 2.0 より、LaTeX のコンパイラのデフォルトが、「pLaTeX」から「upLaTeX」に切り替わりました。upLaTeX は pLaTeX の内部文字処理を Unicode 対応にしたもので、丸数字（①②…）のように pLaTeXでは otf パッケージが必要だった文字、あるいは韓国語や中国語との混植などを、直接扱うことができます。

ほとんどの pLaTeX 向けのパッケージはそのまま動作しますが、jsbook クラスや otf パッケージなどでは uplatex オプションが必要です。

LaTeX コンパイラコマンドおよびオプションについて、Re:VIEW の設定のデフォルトは次のとおりです。

```yaml
texcommand: uplatex
texoptions: "-interaction=nonstopmode -file-line-error -halt-on-error"
texdocumentclass: ["jsbook", "uplatex,oneside"]
dvicommand: dvipdfmx
dvioptions: "-d 5"
```

### 旧来の pLaTeX を使用するには（Re:VIEW 1.0）

既存のドキュメントについて、利用しているマクロやパッケージが upLaTeX でうまく動かない、あるいはこれまでと異なる紙面ができてしまう場合は、pLaTeX に戻したいと思うかもしれません。

Re:VIEW 2.0 よりも前のバージョンと同じコンパイラ設定に戻すには、config.yml に次のように記述します。

```yaml
texcommand: platex
texoptions: "-kanji=utf-8"
texdocumentclass: ["jsbook", "oneside"]
dvicommand: dvipdfmx
dvioptions: "-d 5"
```

レイアウト erb ファイル（デフォルトは lib/review/layout.tex.erb）において、upLaTeX と pLaTeX の区別は内部変数 texcompiler で行えます。変数 texcompiler には、パラメータ texcommand の値からフォルダパスとファイル拡張子を除いたものが入っており、pLaTeX の場合は "platex"、upLaTeX の場合は "uplatex" となります。使用例は次のとおりです。

```
<% if texcompiler == "uplatex" %>
\usepackage[deluxe,uplatex]{otf}
<% else %>
\usepackage[deluxe]{otf}
<% end %>
```

### scale オプションの挙動について
Re:VIEW 2.0 より、``//image`` タグの第3オプションに ``scale=倍率`` で数値のみで倍率を指定していたときの挙動が変わりました。以前は「画像ファイルに対する倍率」でしたが、「紙面横幅に対する倍率」となります（もともと数値以外の文字も scale の値に含めていた場合には、変化はありません）。

旧来の「画像ファイルに対する倍率」に戻したいときには、config.yml にパラメータ ``image_scale2width: false`` を指定してください（デフォルトは true）。

```yaml
image_scale2width: false
```
