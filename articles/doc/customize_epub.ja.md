# EPUB ローカルルールへの対応方法
Re:VIEW の review-epubmaker が生成する EPUB ファイルは IDPF 標準に従っており、EpubCheck を通過する正規のものです。

しかし、ストアによってはこれに固有のローカルルールを設けていることがあり、それに合わせるためには別途 EPUB ファイルに手を入れる必要があります。幸い、ほとんどのルールは EPUB 内のメタ情報ファイルである OPF ファイルにいくつかの情報を加えることで対処できます。

Re:VIEW の設定ファイルは config.yml を使うものとします。

## 電書協ガイドライン
* http://ebpaj.jp/counsel/guide

電書協ガイドラインの必須属性を満たすには、次の設定を config.yml に加えます。

```yaml
opf_prefix: {ebpaj: "http://www.ebpaj.jp/"}
opf_meta: {"ebpaj:guide-version": "1.1.3"}
```

これは次のように展開されます。

```xml
<package …… prefix="ebpaj: http://www.ebpaj.jp/">
 ……
    <meta property="ebpaj:guide-version">1.1.3</meta>
```

ただし、Re:VIEW の生成する EPUB は、ファイルやフォルダの構成、スタイルシートの使い方などにおいて電書協ガイドラインには準拠していません。

## iBooks ストア
デフォルトでは、iBooks で EPUB を見開きで開くと、左右ページの間に影が入ります。
これを消すには、次のように指定します。

```yaml
opf_prefix: {ibooks: "http://vocabulary.itunes.apple.com/rdf/ibooks/vocabulary-extensions-1.0/"}
opf_meta: {"ibooks:binding": "false"}
```

すでにほかの定義があるときには、たとえば次のように追加してください。

```yaml
opf_prefix: {ebpaj: "http://www.ebpaj.jp/", ibooks: "http://vocabulary.itunes.apple.com/rdf/ibooks/vocabulary-extensions-1.0/"}
opf_meta: {"ebpaj:guide-version": "1.1.3", "ibooks:binding": "false"}
```

## Amazon Kindle

EPUB を作成したあと、mobi ファイルにする必要があります。これには Amazon が無料で配布している KindleGen を使用します。

- https://www.amazon.com/gp/feature.html?ie=UTF8&docId=1000765211

OS に合わせたインストーラでインストールした後、`kindlegen EPUBファイル` で mobi ファイルに変換できます。

Kindle Previewer にも内包されています。

- https://kdp.amazon.co.jp/ja_JP/help/topic/G202131170

注意点として、KindleGen は論理目次だけだとエラーを報告します。物理目次ページを付けるために、次のように config.yml に設定します。

```yaml
epubmaker:
  toc: true
```

CSS によっては、Kindle では表現できないことについての警告が表示されることがあります。「Amazon Kindle パブリッシング・ガイドライン」では、使用可能な文字・外部ハイパーリンクの制約・色の使い方・画像サイズなどが詳細に説明されています。

- http://kindlegen.s3.amazonaws.com/AmazonKindlePublishingGuidelines_JP.pdf
