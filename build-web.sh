#!/bin/bash
set -e

WEBROOT="articles/webroot"
OUTDIR="articles/webroot-dist"

rm -rf "$OUTDIR"
mkdir -p "$OUTDIR/ja" "$OUTDIR/en"

# Build Japanese
echo "=== Building Japanese ==="
REVIEW_CONFIG_FILE=config-epub-jp.yml npx grunt web
cp -r "$WEBROOT/"* "$OUTDIR/ja/"

# Build English
echo "=== Building English ==="
REVIEW_CONFIG_FILE=config-epub-en.yml npx grunt web
cp -r "$WEBROOT/"* "$OUTDIR/en/"

# Generate search indices
echo "=== Building search indices ==="
node build-search-index.js "$OUTDIR/ja"
node build-search-index.js "$OUTDIR/en"

# Create root redirect
cat > "$OUTDIR/index.html" << 'REDIRECT'
<!DOCTYPE html>
<html>
<head><meta http-equiv="refresh" content="0; url=ja/index.html" /></head>
<body><a href="ja/index.html">日本語版へ / Go to Japanese version</a></body>
</html>
REDIRECT

# Replace webroot with final output
rm -rf "$WEBROOT"
mv "$OUTDIR" "$WEBROOT"

echo "=== Done: $WEBROOT/ja/ and $WEBROOT/en/ ==="
