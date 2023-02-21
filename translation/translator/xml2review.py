#!/usr/bin/env python

import re
from enum import Enum
from xml.sax.saxutils import escape

class Mode(Enum):
    Inherit = 0
    Body = 1
    Table = 2
    List = 3
    Embed = 4
    Definition = 5

pre_converters = [
    (re.compile(r"\<block\>"), r"{", Mode.Inherit),
    (re.compile(r"\</block\>"), r"//}", Mode.Body),
]

converters = [
    # header
    (re.compile(r"( )*\<h1 (.+?)\>( )?(.+?)\</h1\>"), r"={\2} \4", Mode.Body),
    (re.compile(r"( )*\<h2 (.+?)\>( )?(.+?)\</h2\>"), r"=={\2} \4", Mode.Body),
    (re.compile(r"( )*\<h3 (.+?)\>( )?(.+?)\</h3\>"), r"==={\2} \4", Mode.Body),
    (re.compile(r"( )*\<h3\>( )?(.+?)\</h3\>"), r"=== \3", Mode.Body),
    (re.compile(r"( )*\<h4 (.+?)\>( )?(.+?)\</h4\>"), r"===={\2} \4", Mode.Body),
    (re.compile(r"( )*\<h4\>( )?(.+?)\</h4\>"), r"==== \3", Mode.Body),

    (re.compile(r"\<ul1\>( )?(.+?)\</ul1\>"), r" * \2", Mode.Body),
    (re.compile(r"\<ul2\>( )?(.+?)\</ul2\>"), r" ** \2", Mode.Body),
    (re.compile(r"\<ol ([0-9]+)\>( )?(.+?)\</ol\>"), r" \1. \3", Mode.Body),
    (re.compile(r"\<dt\>( )?(.+?)\</dt\>"), r": \2", Mode.Definition), # 用語リスト前のスペースは最後に入れる

    (re.compile(r"^\s*\<column\>\<title\>( )?(.+?)\</title\>"), r"====[column] \2", Mode.Body),
    (re.compile(r"^\s*\<column (.+?)\>\<title\>( )?(.+?)\</title\>"), r"====[column]{\1} \3", Mode.Body),
    (re.compile(r"\</column\>"), r"====[/column]", Mode.Body),
    (re.compile(r"^\s*\<column5\>\<title\>( )?(.+?)\</title\>"), r"=====[column] \2", Mode.Body),
    (re.compile(r"\</column5\>"), r"=====[/column]", Mode.Body),
    
    (re.compile(r"\<(img|table|list)ref (.+?)/\>"), r"@<\1>{\2}", Mode.Body),
    (re.compile(r"^\s*\<image (.+?)\>( )?(.+?)\</image\>"), r"//image[\1][\3]", Mode.Body),
    (re.compile(r"\<list(|num) (.+?)\>( )?(.*?)\</list\1\>"), r"//list\1[\2][\4]", Mode.List),
    (re.compile(r"\<tsize (.+?)\/>"), r"//tsize[\1]", Mode.Body),
    (re.compile(r"\<table (.+?)\>( )?(.+?)\</table\>"), r"//table[\1][\3]", Mode.Table),
    (re.compile(r"\<footnote (.+?)\>(.+?)\</footnote\>"), r"//footnote[\1][\2]", Mode.Body),
    (re.compile(r"\<info/\>"), r"//info", Mode.Body),
    (re.compile(r"\<embed/\>"), r"//embed[latex]", Mode.Body),

    (re.compile(r"\<parameter (.+?)/\>"), r"[\1]", Mode.Inherit),
    (re.compile(r"\<bracket>(.+?)</bracket\>"), r"[\1\]", Mode.Inherit),

    # (re.compile(r"//embed\[latex\]"), r"<embed/>", Mode.Body),
    # (re.compile(r"\[(.+?)\]"), r"<parameter \1/>", Mode.Inherit),
]

list_converters = [
    (re.compile(r"\<comment\>(.*)\</comment\>"), r"\1"),
    (re.compile(r"\<code\>(.*)\</code\>"), r"\1"),
]

fixed_converters = [
    (re.compile(r"\<(.+?)\>( )?(.*?)\</\1\>"), r"@<\1>{\3}"),
]

post_converters = [
    ("&lt;", "<"),
    ("&gt;", ">"),
    ("&quot;", "\""),
    ("&apos;", "'"),
    ("&amp;", "&"), # 必ず最後
]

spacer = [
    (re.compile(r"([A-Za-z\.])@\<em\>"), r"\1 @<em>"), # emの前のスペースが消えているので追加
    (re.compile(r"([A-Za-z\.])@\<kw\>"), r"\1 @<kw>"), # kwの前のスペースが消えているので追加
    (re.compile(r"([A-Za-z\.])@\<code\>"), r"\1 @<code>"), # codeの前のスペースが消えているので追加
    (re.compile(r"(\})([A-Za-z])"), r"\1 \2"), # 閉じタグ後のスペースが消えているので追加
]

def convert_review(text_lines: list[str]) -> list[str]:
    if len(text_lines) == 0:
        return text_lines

    review_lines: list[str] = []
    current_mode = Mode.Body

    if len(text_lines) < 3:
        # one lined by DeepL
        text_lines = text_lines[0].split("<br/>")

    for l in text_lines:
        l = l.rstrip('\r\n')

        if l.endswith("<br/>"):
            l = l[0:-5]

        if l.startswith("<!--"):
            current_mode = Mode.Embed
            continue
        if l.startswith("-->"):
            current_mode = Mode.Body
            continue
        if current_mode == Mode.Embed:
            for pattern, replacer in post_converters:
                l = l.replace(pattern, replacer)
            review_lines.append(l + "\n")
            continue

        for pattern, replacer, new_mode in pre_converters:
            l, count = pattern.subn(replacer, l)
            if count > 0 and current_mode != new_mode and new_mode != Mode.Inherit:
                # print("new mode", new_mode)
                current_mode = new_mode
        
        if current_mode is Mode.Table:
            if not l.startswith("---") and not l == "//}":
                l = "\t".join([td.lstrip() for td in l.split("</td><td>")])[4:-5].lstrip()
                # l = "".join([f"<td>{s}</td>" for s in l.split("\t")])
                pass
        
        if current_mode is Mode.List:
            for pattern, replacer in list_converters:
                l = pattern.sub(replacer, l)
        
        if current_mode is not Mode.Table and current_mode is not Mode.List:
            for pattern, replacer, new_mode in converters:
                l, count = pattern.subn(replacer, l)
                if count > 0 and new_mode != Mode.Inherit:
                    # print("new mode", new_mode)
                    current_mode = new_mode
                    # 用語リストの前に空行を入れる
                    if new_mode == Mode.Definition:
                        review_lines.append("\n")

        for pattern, replacer in fixed_converters:
            l = pattern.sub(replacer, l)

        for pattern, replacer in post_converters:
            # l = pattern.sub(replacer, l)
            l = l.replace(pattern, replacer)

        for pattern, replacer in spacer:
            l = pattern.sub(replacer, l)
        
        if current_mode is Mode.Definition and len(l) > 0:
            l = " " + l

        l = l + "\n"
        review_lines.append(l)

    return review_lines
