# Re:VIEW フォーマット InDesign XML 形式拡張

Re:VIEW フォーマットから、Adobe 社の DTP ソフトウェア「InDesign」で読み込んで利用しやすい XML 形式に変換できます (通常の XML とほぼ同じですが、文書構造ではなく見た目を指向した形態になっています)。実際には出力された XML を InDesign のスタイルに割り当てるフィルタをさらに作成・適用する必要があります。

基本のフォーマットのほかにいくつかの拡張命令を追加しています。

このドキュメントは、Re:VIEW 3.0 に基づいています。

## 追加したブロック
これらのブロックは基本的に特定の書籍向けのものであり、将来廃棄する可能性があります。

* `//insn[タイトル]{ 〜 //}` または `//box[タイトル]{ 〜 //}` : 書式
* `//planning{ 〜 //}` または `//planning[タイトル]{ 〜 //}` : プランニング
* `//best{ 〜 //}` または `//best[タイトル]{ 〜 //}` : ベストプラクティス
* `//security{ 〜 //}` または `//security[タイトル]{ 〜 //}` : セキュリティ
* `//expert{ 〜 //}`  : エキスパートに訊く
* `//point{ 〜 //}` または `//point[タイトル]{ 〜 //}` : ワンポイント
* `//shoot{ 〜 //}` または `//shoot[タイトル]{ 〜 //}` : トラブルシューティング
* `//term{ 〜 //}` : 用語解説
* `//link{ 〜 //}` または `//link[タイトル]{ 〜 //}` : 他の章やファイルなどへの参照説明
* `//practice{ 〜 //}` : 練習問題
* `//reference{ 〜 //}` : 参考情報

## 相互参照

`//label[〜]` でラベルを定義し、`@<labelref>{〜}` で参照します。XML としては `<label id='〜' />` と `<labelref idref='〜' />` というタグに置き換えられます。

実際にどのような相互参照関係にするかは、処理プログラムに依存します。想定の用途では、章や節の番号およびタイトルを記憶し、labelref の出現箇所に `「節（あるいは章）番号　タイトル」` という文字列を配置します。

## 丸数字

`@<maru>{数値}` で丸数字を出力します。UTF-8 の丸数字を直接使うほうが適切です。

## キートップ

`@<keytop>{キー文字}` でキーボードマークを出力します（実際のフォントとの割り当て処理はフィルタや review-ext.rb で調整します）。

## DTP 命令指定

`@<dtp>{ 〜 }` で「`<? dtp 〜 ?>`」という XML インストラクションを埋め込みます。処理系に渡す任意の文字列を指定できますが、次のような文字列を特殊文字指定できます。

* maru : 番号リストの各段落先頭で使い、このリスト段落の番号は丸数字であることを示す
* return : 改行記号文字
* autopagenumber : 現ページ番号
* nextpageunmber : 次ページ番号
* previouspagenumber : 前ページ番号
* sectionmarker : セクションマーカー
* bulletcharacter : ビュレット (ナカグロ)
* copyrightsymbol : 著作権記号
* degreesymbol : 度記号
* ellipsischaracter : 省略記号
* forcedlinebreak : 強制改行
* discretionarylinebreak : 任意の改行
* paragraphsymbol : 段落記号
* registeredtrademark : 登録商標記号
* sectionsymbol : セクション記号
* trademarksymbol : 商標記号
* rightindenttab : 右インデントタブ
* indentheretab : 「ここまでインデント」タブ
* zerowidthnonjoiner : 結合なし
* emdash : EMダッシュ
* endash : ENダッシュ
* discretionaryhyphen : 任意ハイフン
* nonbreakinghyphen : 分散禁止ハイフン
* endnestedstyle : 先頭文字スタイルの終了文字
* doubleleftquote : 左二重引用符
* doublerightquote : 右二重引用符
* singleleftquote : 左用符
* singlerightquote : 右引用符
* singlestraightquote : 半角一重左用符
* doublestraightquote : 半角二重引用符
* emspace : EMスペース
* enspace : ENスペース
* flushspace : フラッシュスペース
* hairspace : 極細スペース
* nonbreakingspace : 分散禁止スペース
* fixedwidthnonbreakingspace : 分散禁止スペース（固定幅）
* textvariable : 全角スペース
* thinspace : 細いスペース
* figurespace : 数字の間隔
* punctuationspace : 句読点等の間隔
* sixthspace : 1/6スペース
* quarterspace : 1/4スペース
* thirdspace : 1/3スペース
* columnbreak : 改段
* framebreak : 改フレーム
* pagebreak : 改ページ
* oddpagebreak : 奇数改ページ
* evenpagebreak : 偶数改ページ
* footnotesymbol : 脚注記号

## 表セル幅の指定

`//tsize[|idgxml|1列目の幅,2列目の幅,...]` で、続く `//table` の表の列幅を指定します（単位mm）。これを利用するときには、`review compile --target=idgxml` を実行する際、オプション`--table=表幅` を付ける必要があります（表幅の単位は mm）。列幅指定の個数が実際の列数に満たない場合、残りの列は均等分割したものとなります。列幅の合計が表幅を超えると警告が表示されます。

## 表セルの結合やスタイル指定
`@<dtp>{table 表オプション}` を各セルに入れることで、そのセルの挙動を制御します。

表オプションには次のものがあります。

* `colspan=列数` : 右方向に結合
* `rowspan=行数` : 下方向に結合
* `type=名前` : セルの種類の指定
* `align=位置` : セル内の内容の位置。通常の用途では left|center|right のいずれか

実際には、スクリプトで判断して後処理することを想定しています。

* 例: https://github.com/kmuto/reviewindesignbook/blob/master/book-IDGXML/book-IDGXML-02modifyTable.jsx
