# Translator for Unity Performance Tuning Bible

Python scripts to translate text written with [Re:VIEW](https://github.com/kmuto/review) format in Unity Performance Tuning Bible.
Translation is done by DeepL API, which mean it is machine translation.

## Requirements

- Python >= 3.9
- Poetry

## Usage

### Setup

Run poetry install command.

```shell
$ poetry install
```

### Translation

To translate whole articles to english, run following commands.

```shell
$ export DEEPL_API_KEY="YOUR_API_KEY"
$ poetry run python main.py en
```

Translated articles will be written to `text_en` folders.

#### Traslating to other langueage

Thanks to DeepL, translating to other language is easy.
Just add enum to `TranslateLang` defined in `translate.py` just like sample.

```python
class TranslateLang(Enum):
    en = "EN-US"
    zh = "ZH" # added chinese
```

Then run translation command, replacing last parameter with target language of translation.

## Developing

Since this book is written by [Re:VIEW format](https://github.com/kmuto/review/blob/master/doc/format.md), we need to exlude Re:VIEW tags from translation.
So we first convert Re:VIEW tags to original xml tags, because DeepL support handling xml tags.
Then we translate with DeepL, and convert back to Re:VIEW tags from xml tags.

If you have any trouble with translation because of tags, you need to maintain scripts since we only tested with English translation.

### Enabling debug output of translation

You can enable to output intermediate files to `tmp` folder under here by command:

```shell
DEBUG_OUTPUT=True poetry run main.py en
```

Before and after translated xml files will be written.

### Testing tag convert

#### Simple both side convert test

Just run command:

```shell
$ poetry run main.py test
```

Output will overwritten to `text_jp` folder.
You can compare diff by `git diff` command.

#### One side convert test

If you just want test Re:VIEW => xml convert or xml => Re:VIEW convert, run either command:

```shell
$ poetry run main.py convert_review # convert Re:VIEW files to xml files
$ poetry run main.py convert_xml # convert xml files to Re:VIEW files
```

Both commands will write files to `tmp` folder under here.
