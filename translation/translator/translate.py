#!/usr/bin/env python

import os
import deepl
from enum import Enum

SPLITTING_TAGS = "h1,h2,h3,h4,table,th,td,column,column5,listnum,info,block,kw,title,ul1,ul2,ol1,ol2,ol3,ol4,ol5,footnote,list,parameter,m,dt,image,comment,code"
NON_SPLITTING_TAGS = "br,em,bracket"
IGNORE_TAGS = "hd,fn,imgref,tableref,chapref,listref,chap,code"

class TranslateLang(Enum):
    en = "EN-US"

class Translator:
    def __init__(self, target_lang: TranslateLang):
        api_key = os.environ.get("DEEPL_API_KEY")
        if not api_key:
            raise Exception("environment variable of 'DEEPL_API_KEY' is missing")

        self.target_lang = target_lang
        self.deepl_translator = deepl.Translator(api_key)

    def translate(self, lines: list[str]) -> list[str]:
        input_texts: list[str] = []
        translated_texts: list[str] = []
        for line in lines:
            if line.startswith("<h") and len(input_texts) > 300:
                # header行が来て、既に300行以上溜まっている場合はいったん翻訳する
                input_text = "".join(input_texts)
                translated_text = self._translate(input_text)
                if translated_text is None:
                    return
                translated_texts.append(translated_text)
                input_texts.clear()

            input_texts.append(line)

        input_text = "".join(input_texts)
        translated_text = self._translate(input_text)
        if translated_text is None:
            return
        translated_texts.append(translated_text)

        result = []
        for text in translated_texts:
            text = text.replace("\n", "") # まず指定していない位置の改行を削除
            text = text.replace("<br/>", "<br/>\n") # 次に指定した位置での改行タグを改行付きに置換
            result.extend(text.splitlines(keepends=True))
        return result

    def _translate(self, text: str) -> str:
        try:
            result = self.deepl_translator.translate_text(text,
                                                          target_lang=self.target_lang.value,
                                                          tag_handling="xml",
                                                          split_sentences="nonewlines",
                                                          splitting_tags=SPLITTING_TAGS,
                                                          non_splitting_tags=NON_SPLITTING_TAGS,
                                                          ignore_tags=IGNORE_TAGS)
            return result.text
        except deepl.DocumentTranslationException as error:
            # If an error occurs during document translation after the document was
            # already uploaded, a DocumentTranslationException is raised. The
            # document_handle property contains the document handle that may be used to
            # later retrieve the document from the server, or contact DeepL support.
            doc_id = error.document_handle.id
            doc_key = error.document_handle.key
            print(f"Error after uploading ${error}, id: ${doc_id} key: ${doc_key}")
        except deepl.DeepLException as error:
            # Errors during upload raise a DeepLException
            print(error)
