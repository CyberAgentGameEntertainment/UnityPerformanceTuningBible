# 縦書きのサポート（実験的）

Re:VIEW 2.0 より、縦書きの実験的なサポートを含めています。このドキュメントでその利用方法を説明します。

このドキュメントは、Re:VIEW 3.0 に基づいています。

## Re:VIEW フォーマットテキストファイルでの利用
縦書きのドキュメントをサポートするため、次のような機能を提供します。

### 縦中横文字
縦書き中の1〜2文字を横置きに回転させる目的で、インライン命令 `@<tcy>` を用意しました。

```
@<tcy>{縦中横化する文字}
```

図表などの大きなブロックを回転させる用途ではありません。2桁の半角アラビア数字などのごく小さな箇所の回転に対応するものです。

### 採番の漢数字や全角対応
locale.yml の番号付けに縦書き向けの以下のカスタムフォーマットを用意しています。

* `%pAW` : アルファベット（大文字・いわゆる全角）Ａ, Ｂ, Ｃ, ...
* `%paW` : アルファベット（小文字・いわゆる全角）ａ, ｂ, ｃ, ...
* `%pJ` : 漢数字 一, 二, 三, ...
* `%pdW' : アラビア数字（0〜9まではいわゆる全角、10以降半角）１, ２, ... 10, ...
* `%pDW' : アラビア数字（すべて全角）１, ２, ... １０, ...

## EPUB（review-epubmaker）
HTML のボディ部が縦書きになるよう、CSS ファイル（review-init コマンドで作成した雛型を利用しているのであれば style.css）にスタイルを追加します。

```css
body {
    （…既存の設定…）
    -webkit-writing-mode: vertical-rl;
    -epub-writing-mode:   vertical-rl;
    writing-mode: tb-rl;
}
```

また、縦書きの書籍は通常、「左開き」（右ページから左ページへ進む）となるので、config.yml の direction パラメータを設定します。

```yaml
direction: "rtl"
```

なお、仕様としては正しくても、正しく表示されるかは EPUB リーダーおよび使用フォントに強く依存します（たとえばフォントの選択によっては、句読点が横書き位置のままで崩れた表示になることがあります）。

## TeX PDF（review-pdfmaker）
review-pdfmaker のバックエンドとなっている TeX で縦書きを利用するには、ドキュメントクラスファイルを縦書き対応のものに変更する必要があります。config.yml に次のように追加します（`"oneside"` の箇所は必要に応じて変更してください）。

```yaml
texdocumentclass: ["utbook", "oneside"]
```

utbook クラスは upLaTeX に合わせた縦書きの紙面デザインを提供します。マクロや追加パッケージが縦書きに対応していない場合、エラーあるいは予想外の結果になることがあります。

また、表紙・大扉・奥付のように主題とまったく異なる紙面表現の箇所は、TeX のマクロで表現しようとするよりも、Adobe InDesign や Microsoft Word などの別のツールを使って作成した原寸の PDF を用意し、それを貼り付けたほうが思いどおりの結果になるでしょう。`review-custom.sty` に記述する例を以下に示します（各 PDF は `images` フォルダに置いているものとします）。

```
% 縦書きのため各貼り付けPDFは90度回転させる
\def\reviewcoverpagecont{% 表紙
\includefullpagegraphics[angle=90]{images/cover.pdf}
}

\def\reviewtitlepagecont{% 大扉
  \includefullpagegraphics[angle=90]{images/titlepage.pdf}
}

\def\reviewcolophonpagecont{% 奥付
  \includefullpagegraphics[angle=90]{images/colophon.pdf}
}
```
