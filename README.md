<h1 align="center">Unity Performance Tuning Bible</h1>

Unityのパフォーマンスチューニングに関する書籍を作成するためのプロジェクトです。  
作成にはRe:VIEW 5.1を使用しています。

## セットアップ

#### 手元でビルド（PDF出力）する環境の構築
まず手元で出力を確認できる環境を作ります。
Dockerを使うのが手っ取り早いのでオススメです。

```
$ docker pull vvakame/review:5.1
```

Macの場合、Ruby・TeXLive・Re:VIEWをそれぞれインストールすれば自力で環境構築できますが、  
いろいろと面倒なので特別な理由がなければDockerを使うのがいいと思います。

#### クローン
本リポジトリをクローンします。

## 記事を編集する

#### 記事を新たに追加する
articles/textに新規で「.reファイル」を作成します。  
  
つぎに章扉の画像をimages/chapter_titleに追加します。  
章扉の画像名は「chapter_章番号.png」となります。

#### 編集するファイル
articles/text内の「.reファイル」を編集します。  

#### Re:VIEWの書き方
Re:VIEWの一般的な記法は以下を参照してください。
https://github.com/kmuto/review/blob/master/doc/format.ja.md

また、執筆者向けの[README](/README_CONTRIBUTER.md)にて、よく使う記法をまとめています。  
他にも、ワークフローや推奨するエディターについても記載してあるので、一度ご確認ください。

## ビルド方法

#### 手元でビルドする
上述のDockerイメージをpullして環境構築が完了している状態を想定しています。  
Dockerを使って手元でビルドするには以下のシェルを実行します。  
  
書籍用
```
$ ./build-in-docker.sh
```

電子書籍用
```
$ ./build-in-docker-epub.sh
```

(C) 2022 CyberAgent, Inc.
