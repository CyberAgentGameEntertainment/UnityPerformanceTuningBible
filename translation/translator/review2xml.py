#!/usr/bin/env python

import re
from enum import Enum
from xml.sax.saxutils import escape

ENABLE_BR = True
DISABLE_CLEARPAGE = True

if ENABLE_BR:
    BR = "<br/>"
else:
    BR = ""

class Mode(Enum):
    Inherit = 0
    Body = 1
    Table = 2
    List = 3
    Embed = 4

# xml escaping
pre_converters = [
    (re.compile(r"\&"), r"&amp;"), # escaping "&" first
    (re.compile(r"(?<!\@)\<"), r"&lt;"), # escaping "<" but not "@<"
    (re.compile(r"\>(?!\{)"), r"&gt;"), # escaping ">" but not ">{"
    (re.compile(r"\""), r"&quot;"), # escaping "
    (re.compile(r"\'"), r"&apos;"), # escaping '
]

converters = [
    (re.compile("^={(.+?)} (.+)$"), r"<h1 \1>\2</h1>", Mode.Body), # "={Foo} Bar" => "<h1 Foo>Bar</h1>"
    (re.compile("^=={(.+?)} (.+)$"), r"<h2 \1>\2</h2>", Mode.Body), # "=={Foo} Bar" => "<h2 Foo>Bar</h2>"
    (re.compile("^=== (.+)$"), r"<h3>\1</h3>", Mode.Body), # "=== Bar" => "<h3>Bar</h3>"
    (re.compile("^==={(.+?)} (.+)$"), r"<h3 \1>\2</h3>", Mode.Body), # "==={Foo} Bar" => "<h3 Foo>Bar</h3>"
    (re.compile("^==== (.+)$"), r"<h4>\1</h4>", Mode.Body), # "==== Bar" => "<h4>Bar</h4>"
    (re.compile("^===={(.+?)} (.+)$"), r"<h4 \1>\2</h4>", Mode.Body), # "===={Foo} Bar" => "<h4 Foo>Bar</h4>"
    (re.compile(r"^ \* (.+)$"), r"<ul1>\1</ul1>", Mode.Body), # " * Foo" => "<ul1>Foo</ul1>"
    (re.compile(r"^( )+\*\*\s(.+)$"), r"<ul2>\2</ul2>", Mode.Body), # "  ** Foo" => "<ul2>Foo</ul2>"
    (re.compile(r"^ ([0-9]+)\. (.+)$"), r"<ol \1>\2</ol>", Mode.Body), # "  ** Foo" => "<ul2>Foo</ul2>"
    (re.compile(r"^ ([0-9]+)\. (.+)$"), r"<ol \1>\2</ol>", Mode.Body), # "  ** Foo" => "<ul2>Foo</ul2>"
    (re.compile(r"^ : (.+)$"), r"<dt>\1</dt>", Mode.Body), # " : Foo" => "<dt>Foo</dt>"
    (re.compile(r"^====\[column\] (.+)$"), r"<column><title>\1</title>", Mode.Body), # "====[column] Bar" => "<column><title>Bar</title>"
    (re.compile(r"^====\[column\]{(.+?)} (.+)$"), r"<column \1><title>\2</title>", Mode.Body), # "====[column]{Foo} Bar" => "<column Foo><title>Bar</title>"
    (re.compile(r"^====\[\/column\]$"), r"</column>", Mode.Body), # "====[/column]" => "</column>"
    (re.compile(r"^=====\[column\] (.+)$"), r"<column5><title>\1</title>", Mode.Body), # "=====[column]{Foo} Bar" => "<column5 Foo><title>Bar</title>"
    (re.compile(r"^=====\[\/column\]$"), r"</column5>", Mode.Body), # "=====[/column]" => "</column5>"
    (re.compile(r"@<(img|table|list)>\{(.+?)\}"), r"<\1ref \2/>", Mode.Inherit), # "@<img>{Foo}" => "<imgref Foo/>"
    (re.compile(r"//image\[(.+?)\]\[(.+?)(?<!\\)\]"), r"<image \1>\2</image>", Mode.Body), # "//image[foo][bar]" => "<image foo>bar</image>"
    (re.compile(r"//list(num)?\[(.+?)\]\[(.*?)\]"), r"<list\1 \2>\3</list\1>", Mode.List), # "//list[Foo][Bar]" => "<list Foo>Bar</list>"
    (re.compile(r"//tsize\[(.+?)\]"), r"<tsize \1/>", Mode.Body), # "//tsize[Foo]" => "<tsize Foo/>"
    (re.compile(r"//table\[(.+?)\]\[(.+?)\]"), r"<table \1>\2</table>", Mode.Table), # "//table[Foo][Bar]" => "<table Foo>Bar</table>"
    (re.compile(r"//footnote\[(.+?)\]\[(.+?)\]"), r"<footnote \1>\2</footnote>", Mode.Body), # "//footnote[Foo][Bar]" => "<footnote Foo>Bar</footnote>"
    (re.compile(r"^//info"), r"<info/>", Mode.Body), # "//info" => "<info/>"
    # (re.compile(r"//embed\[latex\]"), r"<embed/>", Mode.Body),
    (re.compile(r"\[(.+?)\\\]"), r"<bracket>\1</bracket>", Mode.Inherit), # ex: "[DOTween\]" => "<bracket>DOTween</bracket>"
    (re.compile(r"\[(.+?)(?<!\\)\]"), r"<parameter \1/>", Mode.Inherit), # ex: "[csharp]" => "<parameter csharp/>"
]

post_converters = [
    (re.compile(r"@<(.+?)>\{(.*?)\}"), r"<\1>\2</\1>", Mode.Inherit), # "@<Foo>{Bar}" => "<Foo>Bar</Foo>"
    (re.compile(r"(?<=\>)\{$"), r"<block>", Mode.Inherit), # "{" => "<block>"
    (re.compile(r"^\/\/\}"), r"</block>", Mode.Body), # "}}" => "</block>"
]

# comment inside code block
comment_converter = (re.compile(r"^(.*\/\/ )(.*)$"), r"<code>\1</code><comment>\2</comment>") # "int i; // Bar" => "<code>int i; // </code><comment>Bar</comment>"

def convert_xml(text_lines: list[str]) -> list[str]:
    xml_lines = []
    current_mode = Mode.Body

    for l in text_lines:
        l = l.rstrip('\r\n')

        # skip comments
        if l.startswith("#@#"):
            continue

        if l.startswith("//embed"):
            current_mode = Mode.Embed
            if DISABLE_CLEARPAGE:
                continue
            xml_lines.append(f"<!-- {BR}\n")
        if current_mode == Mode.Embed:
            if not DISABLE_CLEARPAGE:
                xml_lines.append(f"{l}{BR}\n")
            if l == "//}":
                if not DISABLE_CLEARPAGE:
                    xml_lines.append(f"-->{BR}\n")
                current_mode = Mode.Body
            continue

        for pattern, replacer in pre_converters:
            l = pattern.sub(replacer, l)

        
        if current_mode is Mode.Table:
            if not l.startswith("---") and not l == "//}":
                l = "".join([f"<td>{s}</td>" for s in l.split("\t")])

        if current_mode is Mode.List and not l.startswith("//}"):
            l, count = comment_converter[0].subn(comment_converter[1], l)
            if count == 0:
                l = f"<code>{l}</code>"
        
        if current_mode is not Mode.Table and current_mode is not Mode.List:
            for pattern, replacer, new_mode in converters:
                l, count = pattern.subn(replacer, l)
                if count > 0 and current_mode != new_mode and new_mode != Mode.Inherit:
                    # print("new mode", new_mode)
                    current_mode = new_mode

        for pattern, replacer, new_mode in post_converters:
            l, count = pattern.subn(replacer, l)
            if count > 0 and current_mode != new_mode and new_mode != Mode.Inherit:
                # print("new mode", new_mode)
                current_mode = new_mode

        l = f"{l}{BR}\n"
        xml_lines.append(l)

    return xml_lines
