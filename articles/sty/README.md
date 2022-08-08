review-jsbook.cls Users Guide
====================

現時点における最新版 `jsbook.cls  2018/06/23 jsclasses (okumura, texjporg)` をベースに、Re:VIEW 向け review-jsbook.cls を実装しました。

過去の Re:VIEW 2 で jsbook.cls で作っていた資産を、ほとんどそのまま Re:VIEW 3 でも利用できます。

## 特徴

 * クラスオプション `media` により、「印刷用」「電子用」の用途を明示的な意思表示として与えることで、用途に応じた PDF ファイル生成を行えます。
 * （基本的に）クラスオプションを `<key>=<value>` で与えられます。
 * クラスオプション内で、用紙サイズや基本版面を設計できます。

ここで、クラスオプションとは、親 LaTeX 文章ファイルにおいて、以下のような位置にカンマ（,）区切りで記述するオプションです。

```latex
\documentclass[クラスオプションたち（省略可能）]{review-jsbook}
```

## Re:VIEW で利用する

クラスオプションオプションたちは、Re:VIEW 設定ファイル config.yml 内の texdocumentclass において、以下のような位置に記述します。

```yaml
texdocumentclass: ["review-jsbook", "クラスオプションたち（省略可能）"]
```

## 利用可能なクラスオプションたち

### 用途別 PDF データ作成 `media=<用途名>`

印刷用 `print`、電子用 `ebook` のいずれかの用途名を指定します。

 * `print`［デフォルト］：印刷用 PDF ファイルを生成します。
   * トンボあり、デジタルトンボあり、hyperref パッケージを `draft` モードで読み込み、表紙は入れない
 * `ebook`：電子用PDFファイルを生成します。
   * トンボなし、hyperref パッケージを読み込み、表紙を入れる

### 表紙の挿入有無 `cover=<trueまたはfalse>`

`media` の値によって表紙（config.yml の coverimage に指定した画像）の配置の有無は自動で切り替わりますが、`cover=true` とすれば必ず表紙を入れるようになります。

### 表紙画像のサイズの仕上がり紙面合わせ `cover_fit_page=<trueまたはfalse>`

上記の coverimage で指定する画像ファイルは、原寸を想定しているため、サイズが異なる場合にははみ出たり、小さすぎたりすることになります。できるだけ原寸で用意することを推奨しますが、`cover_fit_page=true` とすれば表紙画像を紙面の仕上がりサイズに合わせて拡縮します。

### 特定の用紙サイズ `paper=<用紙サイズ>`

利用可能な特定の用紙サイズを指定できます。

 * `a3`
 * `a4` ［デフォルト］
 * `a5`
 * `a6`
 * `b4`：JIS B4
 * `b5`：JIS B5
 * `b6`：JIS B6
 * `a4var`：210mm x 283mm
 * `b5var`：182mm x 230mm
 * `letter`
 * `legal`
 * `executive`

### トンボ用紙サイズ `tombopaper=<用紙サイズ>` および塗り足し幅 `bleed_margin=<幅>`

`tombopaper` ではトンボ用紙サイズを指定できます。
［デフォルト］値は自動判定します。

`bleed_margin` では塗り足し領域の幅を指定できます。
［デフォルト］3mm になります。

### カスタム用紙サイズ `paperwidth=<用紙横幅>`, `paperheight=<用紙縦幅>`

カスタム用紙サイズ `paperwidth=<用紙横幅>`, `paperheight=<用紙縦幅>` （両方とも与える必要があります）を与えることで、特定の用紙サイズで設定できない用紙サイズを与えられます。

たとえば、B5変形 `paperwidth=182mm`, `paperheight=235mm`。

### 基本版面設計 `fontsize=<文字サイズ>`, `baselineskip=<行送り>`, `line_length=<字詰>`, `number_of_lines=<行数>`, `head_space=<天>`, `gutter=<ノド>`, `linegap=<幅>`, `headheight=<幅>`, `headsep=<幅>`, `footskip=<幅>`

基本版面情報を与えます。
天、ノドをそれぞれ与えない場合、それぞれ天地、左右中央になります。

 * `fontsize=10pt`［デフォルト］：標準の文字（normalfontsize）の文字サイズを与えます。pt のほか、Q や mm といった単位も指定可能です。ただし、文字サイズは jsbook の挙動に合わせるために 8pt, 9pt, 10pt, 11pt, 12pt, 14pt, 17pt, 20pt, 21pt, 25pt, 30pt, 36pt, 43pt のいずれか近いサイズに丸められます。
 * `baselineskip=16pt`［デフォルト］：行送りを与えます。
 * `line_length=<字詰め幅>`：1行字詰めを与えます。字詰め幅には単位を付ける必要があります。文字数であれば「zw」を使うとよいでしょう（例：35zw＝35文字）。デフォルトでは jsbook の挙動に従い、紙サイズに基いて決定します。
 * `number_of_lines=<行数>`：行数を与えます。デフォルトでは jsbook の挙動に従い、紙サイズに基いて決定します。
 * `head_space=<幅>`：天を与えます。［デフォルト］は天地中央です。
 * `gutter=<幅>`：ノドを与えます。［デフォルト］は左右中央です。
 * `linegap=<幅>`：行送りを baselineskip で指定する代わりに、通常の文字の高さにこのオプションで指定する幅を加えたものを行送りとします。

例をいくつか挙げます。

 * `paper=a5, fontsize=10pt, line_length=35zw, number_of_lines=32, baselineskip=16pt,`
 * `paper=b5, fontsize=13Q, baselineskip=20.5H, head_space=20mm, gutter=20mm,`

さらに、ヘッダー、フッターに関する位置調整は、TeX のパラメータ `\headheight`, `\headsep`, `\footskip` に対応しており、それぞれ `headheight`, `headsep`, `footskip` を与えられます。

## 開始ページ番号 `startpage=<ページ番号>`

大扉からのページ開始番号を指定します。

［デフォルト］は1です。表紙・表紙裏（表1・表2）のぶんを飛ばしたければ、`startpage=3` とします。

## 通しページ番号（通しノンブル） `serial_pagination=<trueまたはfalse>`

大扉からアラビア数字でページ番号を通すかどうかを指定します。

 * `true`：大扉を開始ページとして、前付（catalog.yml で PREDEF に指定したもの）、さらに本文（catalog.yml で CHAPS に指定したもの）に連続したページ番号をアラビア数字で振ります（通しノンブルと言います）。
 * `false`［デフォルト］：大扉を開始ページとして前付の終わり（通常は目次）までのページ番号をローマ数字で振ります。本文は 1 を開始ページとしてアラビア数字で振り直します（別ノンブルと言います）。

### 隠しノンブル 'hiddenfolio=<プリセット>'

印刷所固有の要件に合わせて、ノドの目立たない位置に小さくノンブルを入れます。
'hiddenfolio` にプリセットを与えることで、特定の印刷所さん対応の隠しノンブルを出力することができます。
利用可能なプリセットは、以下のとおりです。

 * `default`：トンボ左上（塗り足しの外）にページ番号を入れます。
 * `marusho-ink`（丸正インキ）：塗り足し幅を5mmに設定、ノド中央にページ番号を入れます。
 * `nikko-pc`（日光企画）, `shippo`（ねこのしっぽ）：ノド中央にページ番号を入れます。

独自の設定を追加したいときには、review-jsbook.cls の実装を参照してください。

ページ番号は紙面に入れるものと同じものが入ります。アラビア数字で通したいときには、上記の `serial_pagination=true` も指定してください。

## 標準で review-jsbook.cls を実行したときの jsbook.cls との違い

 * jsbook.cls のクラスオプション `uplatex`：これまで texdocumentclass に指定が必要だった `uplatex` オプションは不要となっています。
 * jsbook.cls のクラスオプション `nomag`：用紙サイズや版面設計は、review-jsbook.cls 側で行います。
 * hyperref パッケージ：あらかじめ hyperref パッケージを組み込んでおり、`media` オプションにより用途別で挙動を制御します。

### 既存の jsbook.cls のオプションの扱い

review-jsbook.cls は jsbook.cls を包んでおり、一部の jsbook.cls のクラスオプションはそのまま指定可能です。

 * `oneside`: 奇数ページ・偶数ページで柱やページ番号などを同じ体裁にします。review-jsbook.cls にも有効ですが、review-style.sty でこれを打ち消し奇数・偶数で別の見た目にするデザイン (fancyhdr) が定義されているので、review-style.sty も調整する必要があります。
 * `twoside`: 奇数ページ・偶数ページで柱やページ番号などを別の体裁にします (デフォルト)。
 * `vartwoside`: twoside とおおむね同じですが、傍注が小口ではなく常に右側になります。Re:VIEW のデフォルトでは傍注は使用していないので、効果は通常表れません。
 * `onecolumn`: 1段組の体裁にします (デフォルト)。
 * `twocolumn`: 2段組の体裁にします。
 * `openright`: 章の始まりを右ページ (奇数ページ) にします (デフォルト)。前の章が右ページで終わった場合には、白紙のページが1ページ挿入されます。
 * `openleft`: 章の始まりを左ページ (偶数ページ) にします。前の章が左ページで終わった場合には、白紙のページが1ページ挿入されます。
 * `openany`: 章の始まりを左右どちらのページからでも始めます。
 * `draft`: 確認作業のために、overfull box が起きた箇所の行末に罫線を引き、画像は実際に貼り付けずにボックスとファイル名だけを表記するようにします。
 * `final`: 上記の draft の処理を行いません (デフォルト)。
 * `leqno`: 数式の番号を右ではなく左側に置きます。ただし Re:VIEW では LaTeX のやり方での採番付き数式を作っていないので、効果は通常表れません。
 * `fleqn`: 数式をデフォルトの左右中央ではなく、左側に配置します。
 * `english`: 英語ドキュメント向けに、字下げをなくしたり、「章」や「目次」などの定型の文字列を英語化します。しかし、Re:VIEW では定型文字列はロケールファイルで処理しており、ほとんどは無視されます。
 * `jslogo`: 「LaTeX」等のロゴを置き換えます (デフォルト)。
 * `nojslogo`: ロゴを置き換えません。
 * `report`: oneside と openany の両方と同じ効果を持ちます。
 * `landscape`: 用紙を横長で使います。review-jsbook.cls のクラスオプションで基本版面設計をやり直す必要があることに注意してください。

jsbook.cls の以下のクラスオプションは挙動が異なります。代わりに review-jsbook.cls のクラスオプションを利用してください。

 * `8pt`・`9pt`・`10pt`・`11pt`・`12pt`・`14pt`・`17pt`・`20pt`・`21pt`・`25pt`・`30pt`・`36pt`・`43pt`・`12Q`・`14Q`・`10ptj`・`10.5ptj`・`11ptj`・`12ptj`: 基本文字のサイズを指定します。そのまま review-jsbook.cls の fontsize に渡されますが、上記の fontsize クラスオプションの説明にあるとおり丸められます。
 * `tombow`・`tombo`・`mentuke`: トンボや塗り足しを作成しますが、これらは PDF 入稿に求められる正しいデジタルトンボ情報を入れないので使用してはなりません。review-jsbook.cls の `media=print` を使ってください。
 * `a4paper`・`a5paper`・`b5paper`・`b4paper`・`letterpaper`: 紙サイズを指定します。誤りではありませんが、review-jsbook.cls の paper クラスオプションを使うほうが妥当です。

jsbook.cls の以下のクラスオプションは無視またはエラーになります。

 * `uplatex`: 暗黙に指定されるので無視されます。
 * `autodetect-engine`: pLaTeX/upLaTeX を自動判別するオプションですが、Re:VIEW では review-jsbook 利用時に upLaTeX を暗黙に前提としているので無視されます。
 * `papersize`: dvips などに紙サイズ情報を与えるオプションですが、Re:VIEW ではこれを利用しないので、結果的に無視されます。
 * `titlepage`・`notitlepage`: 表題の独立ページ化の有無ですが、Re:VIEW では表題を利用していないため、結果的に無視されます。
 * `usemag`・`nomag`・`nomag*`: 用紙サイズと版面設計は review-jsbook.cls のクラスオプションを使うため、無視されます。
 * `a4j`・`a5j`・`b4j`・`b5j`・`winjis`・`mingoth`: これらは無効としており、エラーになります。review-jsbook.cls のクラスオプションを利用してください。
 * `jis`: jis フォントメトリックスを使う指定ですが、通常の環境ではコンパイルエラーになります。
 * `disablejfam`: 数式内の利用フォント数を増やすために、数式内の日本語文字を使わないようにする指定ですが、Re:VIEW を利用する上では単にエラーを誘発するだけでしょう。
