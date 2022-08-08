# A collection of prh rules [![CircleCI](https://circleci.com/gh/prh/rules.svg?style=svg)](https://circleci.com/gh/prh/rules)

## フォルダ構成

* languages
    * 各自然言語固有のルール
* terms
    * 技術用語のルール
    * 商標やツールの正式名称などのルール
    * 各種ツール固有のルール
* media
    * 各媒体固有のルール

## ルール

* yaml内でimportを使ってはならない
    * rootディレクトリのprh.ymlとmediumの中は除く
* 原則として、media内のルールを他のファイルから参照しない（組み合わせて運用する前提のものではないため）
* mediaには原則として、各団体・出版社別ルールを置く
    * 作品別ルールは作品のリポジトリにて個別に管理するのを推奨する
* 団体内部で複数のファイルを置きたい場合、 media/techbooster/ のように団体名のディレクトリを切ってその中で自由にすること
