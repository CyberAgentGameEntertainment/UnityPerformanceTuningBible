"use strict";

// Generates search-index.json from webroot HTML files.
// Usage: node build-search-index.js [directory]

const fs = require("fs");
const path = require("path");

const webroot = process.argv[2] || path.join(__dirname, "articles", "webroot");
const outFile = path.join(webroot, "search-index.json");

const htmlFiles = fs.readdirSync(webroot).filter(f => f.endsWith(".html") && f !== "index.html" && f !== "titlepage.html");

const index = [];

htmlFiles.forEach(file => {
  const html = fs.readFileSync(path.join(webroot, file), "utf8");

  // Extract page title from <title>
  const titleMatch = html.match(/<title>([^|]*)/);
  const pageTitle = titleMatch ? titleMatch[1].trim() : file;

  // Extract sections: split by h1/h2 headings
  const sectionRegex = /<h[12][^>]*id="([^"]*)"[^>]*>([\s\S]*?)<\/h[12]>/g;
  const sections = [];
  let match;
  const headings = [];

  while ((match = sectionRegex.exec(html)) !== null) {
    headings.push({ id: match[1], pos: match.index, title: stripTags(match[2]) });
  }

  if (headings.length === 0) {
    // No headings — index the whole page
    const bodyMatch = html.match(/<div class="book-page">([\s\S]*?)<\/div>\s*<nav/);
    const text = bodyMatch ? stripTags(bodyMatch[1]).slice(0, 500) : "";
    index.push({ file, title: pageTitle, id: "", text });
  } else {
    // Index each section
    const bodyMatch = html.match(/<div class="book-page">([\s\S]*)/);
    const body = bodyMatch ? bodyMatch[1] : html;

    for (let i = 0; i < headings.length; i++) {
      const start = headings[i].pos;
      const end = i + 1 < headings.length ? headings[i + 1].pos : body.length;
      const sectionHtml = body.slice(start, end);
      const text = stripTags(sectionHtml).slice(0, 500);
      sections.push({
        file,
        title: headings[i].title,
        id: headings[i].id,
        text
      });
    }
    index.push(...sections);
  }
});

function stripTags(html) {
  return html
    .replace(/<[^>]+>/g, "")
    .replace(/&lt;/g, "<")
    .replace(/&gt;/g, ">")
    .replace(/&amp;/g, "&")
    .replace(/&quot;/g, '"')
    .replace(/&#\d+;/g, "")
    .replace(/\s+/g, " ")
    .trim();
}

fs.writeFileSync(outFile, JSON.stringify(index, null, 0), "utf8");
console.log(`Search index: ${index.length} entries -> ${outFile}`);
