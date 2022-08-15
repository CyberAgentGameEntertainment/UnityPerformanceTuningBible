# Re:VIEWクイックスタートガイド

Re:VIEW は、EWB や RD あるいは Wiki に似た簡易フォーマットで記述したテキストファイルを、目的に応じて各種の形式に変換するツールセットです。

平易な文法ながらも、コンピュータ関係のドキュメント作成のための多くの機能を備えており、テキスト、LaTeX、HTML、XML といった形式に変換できます。独自のカスタマイズも簡単です。

Re:VIEW は GNU Lesser General Public License Version 2.1 に基づいて配布されており、自由に利用、改変、再配布できます。このライセンスは、Re:VIEW を使ってあなたが作成しようとする文書とは無関係であり、あなたの文書はこのライセンスに強制されることはありません。Re:VIEW のツールセットあるいは Re:VIEW を組み込んだシステムを配布あるいは販売しようとしているときには、ライセンスファイル COPYING をよく確認してください。

このドキュメントでは、Re:VIEW のセットアップから変換の例までを簡単に説明します。

このドキュメントは、Re:VIEW 5.1 に基づいています。

## セットアップ

Re:VIEW は Ruby 言語で記述されており、Linux/Unix 互換システムで動作します。macOS および Windows でも動作可能です。Ruby gem あるいは Git のいずれかを使ってダウンロード・展開します。

なお、Re:VIEW フォーマット自体は文字で表現されたタグが付いている以外は単なるテキストファイルなので、エディタ、OS については UTF-8 文字エンコーディングさえ使用できれば制限はありません。

### RubyGems を使う場合

機能セットがまとまった区切りごとに、定期的に Re:VIEW の開発チームが Re:VIEW の gem を更新しています。

次のように Re:VIEW の gem をインストールします。

```bash
$ gem install review
```

Ruby gem の bin ディレクトリにパスを通すようにしておいてください。

インストール後、最新の gem に追従するには次のようにします。

```bash
$ gem update review
```

Gemfile を使っている場合は、以下の行を Gemfile に追加して、`bundle` コマンドを実行します。

```Gemfile
gem 'review'
```

### Gitを使う場合

Re:VIEW は GitHub で開発されており、バージョン管理ツールの Git を使って最新の Re:VIEW コードを入手できます。Git は分岐が容易なので、独自のカスタマイズを施すのにも向いています。

初めて取得するときには、次のようにします (コピーを作っています)。

```bash
$ git clone https://github.com/kmuto/review.git
```

review というディレクトリに展開されるので、review/bin にパスを通すようにしておいてください。

最新の開発に追従するには次のようにします。

```bash
$ git pull
```

## Re:VIEW プロジェクトの作成

### 雛型の作成
review-init コマンドを使って、雛型となるプロジェクトフォルダを作成できます。

```
$ review-init プロジェクト名
```

TeX を利用した PDF 作成を目的としているならば、雛型の作成時に、Web ブラウザによるウィザードモードで基本版面設計を行うことができます。これには `-w` オプションを付けます。

```
$ review-init -w プロジェクト名
```

「http://localhost:18000」に Web ブラウザでアクセスし、文字数×行数および余白を調整できます。

これで指定のプロジェクト名のフォルダが用意され、中に次のようなファイルが置かれます。

 * プロジェクト名を冠した「.re」拡張子を持つファイル（Re:VIEW フォーマットテキストファイル）
 * config.yml : 設定ファイル
 * config-ebook.yml : 電子ブック用設定ファイル（ウィザードモード使用時）
 * catalog.yml : カタログファイル（目次構成）
 * Rakefile : rake コマンドのルールファイル
 * images : 画像の配置フォルダ
 * style.css : サンプルスタイルシート
 * sty : スタイルファイル配置フォルダ（TeX 用）
 * doc : 本ドキュメントを含む使い方のドキュメント
 * lib : 主に rake コマンドの詳細ルールファイル

review-init コマンドによらず、独自にプロジェクトフォルダを作成してもかまいません。

### Re:VIEW テキストの作成と変換

デフォルトの re 拡張子のファイルは、「`=`」とあるだけのほぼ空っぽのファイルです。次に Re:VIEW フォーマットで記述を追加した簡単な例を示します。

```review
= はじめてのRe:VIEW

//lead{
「Hello, Re:VIEW.」
//}

== Re:VIEWとは

@<b>{Re:VIEW}は、EWBやRDあるいはWikiに似た簡易フォーマットで記述したテキストファイルを、目的に応じて各種の形式に変換するツールセットです。

平易な文法ながらも、コンピュータ関係のドキュメント作成のための多くの機能を備えており、次のような形式に変換できます。

 * テキスト（指示タグ付き）
 * LaTeX
 * HTML
 * XML

現在入手手段としては次の3つがあります。

 1. Ruby gem
 2. Git
 3. Download from GitHub

ホームページは@<tt>{https://reviewml.org/}です。
```

テキストファイルの文字エンコーディングには、UTF-8 を使用してください。

### PDF 化と EPUB 化、プレインテキスト化、XML 化

review-*maker コマンドで一括して変換作成ができます。

- review-pdfmaker コマンド：PDF ブックの作成
- review-epubmaker コマンド：EPUB ファイルの作成
- review-textmaker コマンド：プレインテキストの作成
- review-idgxmlmaker コマンド：InDesign XML の作成

PDF を作成するには、TeXLive2012 以上の環境が必要です。EPUB を作成するには、rubyzip gem あるいは zip コマンドが必要です（MathML も使いたいときには、 [MathML ライブラリ](http://www.hinet.mydns.jp/?mathml.rb)も必要です）。

いずれのコマンドも、必要な設定情報を記した YAML 形式ファイルを引数に指定して実行します。review-init コマンドで作成した環境には、デフォルトで config.yml として用意されているので、これを利用します。

```bash
$ review-pdfmaker config.yml       ←PDFの作成
$ review-pdfmaker config-ebook.yml ←電子ブック設定でのPDFの作成
$ review-epubmaker config.yml      ←EPUBの作成
$ review-textmaker config.yml      ←テキストの作成（装飾情報あり）
$ review-textmaker -n config.yml   ←テキストの作成（装飾情報なし）
$ review-idgxmlmaker config.yml    ←InDesign XMLの作成
```

rake コマンドを利用できるなら、次のように実行することもできます。

```bash
$ rake pdf  ←PDFの作成
$ REVIEW_CONFIG_FILE=config-ebook.yml rake pdf ←電子ブック設定でのPDFの作成
$ rake epub ←EPUBの作成
$ rake text  ←テキストの作成（装飾情報あり）
$ rake plaintext ←テキストの作成（装飾情報なし）
$ rake idgxml ←InDesign XMLの作成
```

config.yml のサンプルについては以下を参照してください。

* [config.yml.sample](https://github.com/kmuto/review/blob/master/doc/config.yml.sample)

#### Vivliostyle CLI を使った PDF 化

TeX (`review-pdfmaker`、`rake pdf`) を利用する代わりに、[Vivliostyle CLI](https://github.com/vivliostyle/vivliostyle-cli) を使って PDF を作成することもできます。Re:VIEW が EPUB を作成したあと、VivliostyleCLI がそれを PDF に変換します。

```bash
$ rake vivliostyle:build    ← Vivliostyle を使って PDF を作成
$ rake vivliostyle:preview  ← Chrome/Chromium ブラウザでプレビュー
$ rake vivliostyle          ← vivliostyle:buildのショートカット
```

### 章を増やす、カスタマイズする
作成した PDF あるいは EPUB を見ると、先に作成した RE:VIEW フォーマットテキストファイルが「第1章」となっていることがわかります。

章を増やすには、同様に .re 拡張子の Re:VIEW フォーマットテキストファイルを用意し、catalog.yml にそれを登録します。

```
PREDEF:

CHAPS:
  - 1章のファイル
  - 2章のファイル
   ……

APPENDIX:

POSTDEF:
```

catalog.yml の PREDEF は前付、CHAPS は章、APPENDIX は付録、POSTDEF は後付です。詳細は [catalog.ja.md](https://github.com/kmuto/review/blob/master/doc/catalog.ja.md) を参照してください。

### 情報
Re:VIEW フォーマットについての詳細は、 [format.ja.md](https://github.com/kmuto/review/blob/master/doc/format.ja.md) を参照してください。

Re:VIEW の使用実例については、https://github.com/reviewml/review-samples にもまとめています。

## ファイル単位の変換
通常の用途では review-pdfmaker や review-epubmaker で事足りるはずですが、それ以外の形式に変換したり、PDF や EPUB の変換にあたってどのような変換が行われているのかを確認したりしたいときには、review-compile コマンドを使用します。

たとえば sample.re ファイルを変換するには次のようになります。

```bash
$ review-compile --target text sample.re > sample.txt    ←テキストにする
$ review-compile --target html sample.re > sample.html   ←HTMLにする
$ review-compile --target latex sample.re > sample.tex   ←LaTeXにする
$ review-compile --target idgxml sample.re > sample.xml  ←InDesign XMLにする
$ review-compile --target markdown sample.re > sample.md ←Markdownにする
```

review-compile を含め、ほとんどのコマンドは `--help` オプションを付けるとオプションについてのヘルプが表示されます。

## プリプロセッサ、ボリューム表示

`#@mapfile`、`#@maprange`、`#@mapoutput` のタグを使って、指定のファイルの内容あるいはコマンドの実行結果を挿入できます。挿入・更新を行うには、プリプロセッサとなる review-preproc コマンドを使います。

```bash
$ review-preproc ファイル > 結果ファイル ←標準出力をリダイレクト

## または
$ review-preproc --replace ファイル ←ファイルを更新したもので上書き
```

各章の分量などを表示するには、review-vol コマンドを使います。

```bash
$ review-vol
```

より細かな見出し一覧などを出したいときには、review-index コマンドを使うとよいでしょう。

```bash
$ review-index -l 掘り下げる見出しレベル数
$ review-index -l 掘り下げる見出しレベル数 -d ←分量の詳細を表示
```

review-vol と review-index では、文字数や行数、見込みページなど各値に大きな差が出ることがあります。これは、review-vol がごく簡単にファイルから概算しているのに対し、review-index は実際に内部でコンパイルして比較的精密に計量しているからです。

## プロジェクトフォルダを新しい Re:VIEW バージョンに追従する方法

Re:VIEW は定期的に更新されています。おおむね後方互換性を保持していますが、新しいバージョン固有の機能を利用したいときには、プロジェクトフォルダを新しいバージョンに更新する review-update コマンドを利用します。

```bash
$ review-update
** review-update はプロジェクトを 5.0.0 に更新します **
config.yml: 'review_version' を '5.0' に更新しますか? [y]/n ←Enterで実行
Rakefile は Re:VIEW バージョンのもの (/.../review/samples/sample-book/src/Rakefile) で置き換えられます。本当に進めますか? [y]/n
lib/tasks/review.rake は Re:VIEW バージョンのもの (/.../review/samples/sample-book/src/lib/tasks/review.rake) で置き換えられます。本当に進めますか? [y]/n
INFO: 新しいファイル /.../sty/plistings.sty が作成されました。
/.../sty/reviewmacro.sty は Re:VIEW バージョンのもの (/.../review/templates/latex/review-jsbook/reviewmacro.sty) で置き換えられます。本当に進めますか? [y]/n
INFO: 新しいファイル /.../sty/README.md が作成されました。
INFO: 新しいファイル /.../sty/review-custom.sty が作成されました。
INFO: 新しいファイル /.../sty/review-jsbook.cls が作成されました。
INFO: 新しいファイル /.../sty/review-style.sty が作成されました。
INFO: 新しいファイル /.../sty/review-base.sty が作成されました。
INFO: 新しいファイル /.../sty/gentombow.sty が作成されました。
完了しました。
```

## クレジット

Re:VIEW は、青木峰郎によって最初に作成されました。武藤健志がこの開発・保守を引き継ぎ、2020年10月時点では、武藤健志、高橋征義、角征典が開発・保守を継続しています。

バグ・パッチの報告、その他の情報は、

* http://reviewml.org/
* https://github.com/kmuto/review/wiki

を参照してください。
