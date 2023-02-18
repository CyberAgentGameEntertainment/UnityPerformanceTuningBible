#!/bin/bash
[ ! -z $REVIEW_LANG ] || REVIEW_LANG=jp
[ ! -z $REVIEW_CONFIG_FILE ] || REVIEW_CONFIG_FILE="config-epub-$REVIEW_LANG.yml"

# コマンド手打ちで作業したい時は以下の通り /book に pwd がマウントされます
# docker run -i -t -v $(pwd):/book vvakame/review:5.1 /bin/bash

# reduce size of images
REVIEW_IMAGE_DIR="articles/images_$REVIEW_LANG"
FIND_COMMAND="find $REVIEW_IMAGE_DIR"' -type d -name cover -prune -o -type d -name chapter_title -prune -o -name "*.png" -print' # coverフォルダとchapter_titleフォルダを除いたpngファイルを抽出する
IMAGEMAGICK_COMMAND='mogrify -format jpg -quality 75 -background white -alpha remove -alpha off' # JPG化してサイズを減らす
docker run -v $(pwd)/articles:/articles --entrypoint=sh dpokidov/imagemagick:7.1.0-47-buster -c "${FIND_COMMAND} | xargs ${IMAGEMAGICK_COMMAND}"
eval "${FIND_COMMAND}" | xargs rm -fr # PNGファイルを一時的に削除

docker run -t --rm -v $(pwd):/book -v $(pwd)/articles/fonts/bizud:/usr/share/fonts/truetype/bizud vvakame/review:5.1 /bin/bash -ci "cd /book && ./setup.sh && REVIEW_CONFIG_FILE=$REVIEW_CONFIG_FILE npm run pdf"

# restore images
git restore "$REVIEW_IMAGE_DIR"
git clean -fdx "$REVIEW_IMAGE_DIR"
