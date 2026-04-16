#!/bin/bash
[ ! -z $REVIEW_LANG ] || REVIEW_LANG=jp
[ ! -z $REVIEW_CONFIG_FILE ] || REVIEW_CONFIG_FILE="config-$REVIEW_LANG.yml"

# コマンド手打ちで作業したい時は以下の通り /book に pwd がマウントされます
# docker run -i -t -v $(pwd):/book review-custom:5.11 /bin/bash

DOCKER_IMAGE="review-custom:5.11"

# イメージが存在しない場合はビルドする
if ! docker image inspect "$DOCKER_IMAGE" > /dev/null 2>&1; then
  docker build -t "$DOCKER_IMAGE" .
fi

docker run -t --rm -v $(pwd):/book -v $(pwd)/articles/fonts/bizud:/usr/share/fonts/truetype/bizud "$DOCKER_IMAGE" /bin/bash -ci "cd /book && npm install --ignore-scripts && bundle install && gem pristine --all && REVIEW_CONFIG_FILE=$REVIEW_CONFIG_FILE npm run pdf"
