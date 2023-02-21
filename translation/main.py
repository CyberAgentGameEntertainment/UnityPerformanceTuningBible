#!/usr/bin/env python

import os
import sys
import glob
import pathlib

from translator.review2xml import convert_xml
from translator.translate import Translator, TranslateLang
from translator.xml2review import convert_review

# Enabling file output of intermediates
ENABLE_DEBUG_OUTPUT = os.environ.get('DEBUG_OUTPUT', False) == "True"

SCRIPT_DIR = pathlib.Path(os.path.dirname(os.path.abspath(__file__)))
BASE_DIR = SCRIPT_DIR.parent
ARTICLE_DIR = BASE_DIR / "articles"
JP_DIR = ARTICLE_DIR / "text_jp"
TMP_DIR = SCRIPT_DIR / "tmp"

def read_lines(file: str) -> list[str]:
    with open(file, "r") as f:
        return f.readlines()

def write_lines(file: str, lines: list[str]):
    with open(file, "w") as f:
        f.writelines(lines)

def debug_write_lines(file: str, lines: list[str]):
    if ENABLE_DEBUG_OUTPUT:
        write_lines(file, lines)

def command_translate(target_lang: TranslateLang):
    """日本語Reviewファイルを翻訳します"""
    jp_files = glob.glob(str(JP_DIR) + f"/*.re")
    translator = Translator(target_lang)
    for file in jp_files:
        print(f"processing: {file}")
        path = pathlib.Path(file)
        lines = read_lines(file)

        # review => xml
        xml_lines = convert_xml(lines)
        debug_write_lines(TMP_DIR + path.with_suffix(".xml").name, xml_lines)

        # translate
        translated_lines = translator.translate(xml_lines)
        if translated_lines is None:
            continue
        debug_write_lines(TMP_DIR + path.with_suffix(".translated.xml").name, translated_lines)

        # xml => review
        review_lines = convert_review(translated_lines)
        EXPORT_DIR = ARTICLE_DIR / f"text_{target_lang.name}"
        output_file = EXPORT_DIR / path.name
        write_lines(output_file, review_lines)

def command_test():
    """翻訳無しで、日本語ReviewファイルをXML変換しつつ、再度Review変換して上書きします（テスト用）"""
    jp_files = glob.glob(str(JP_DIR) + f"/*.re")
    for file in jp_files:
        print(f"processing: {file}")
        lines = read_lines(file)
        xml_lines = convert_xml(lines)
        review_lines = convert_review(xml_lines)
        write_lines(file, review_lines)

def command_convert_review():
    """日本語ReviewファイルをXMLファイルに変換します（調整用）"""
    jp_files = glob.glob(str(JP_DIR) + f"/*.re")
    for file in jp_files:
        print(f"processing: {file}")
        path = pathlib.Path(file)
        lines = read_lines(file)
        xml_lines = convert_xml(lines)
        write_lines(TMP_DIR + path.with_suffix(".xml").name, xml_lines)

def command_convert_xml():
    """tmpフォルダにある翻訳済みXMLファイルをReviewファイルに変換します（調整用）"""
    translated_files = glob.glob(str(TMP_DIR) + f"/*.translated.xml")
    for file in translated_files:
        print(f"processing: {file}")
        path = pathlib.Path(file)
        lines = read_lines(file)
        review_lines = convert_review(lines)
        re_file = path.with_suffix("").with_suffix(".re")
        write_lines(re_file, review_lines)
            

if __name__=="__main__":
    command = sys.argv[1]

    if command == "test":
        command_test()
    elif command == "convert_review":
        command_convert_review()
    elif command == "convert_xml":
        command_convert_xml()
    else:
        target_lang = TranslateLang[command]
        command_translate(target_lang)
