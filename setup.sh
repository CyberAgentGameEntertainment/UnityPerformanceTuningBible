#!/bin/bash

set -eux

# 真にクリーンビルドを有効にする場合は下記を有効に
#rm -rf node_modules

# --unsafe-perm はrootでの実行時(= docker環境)で必要 非root時の挙動に影響なし
npm install --unsafe-perm
