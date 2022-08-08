#!/bin/sh
npm install --no-save node-sass
$(npm bin)/node-sass ./articles/ --output ./articles/
