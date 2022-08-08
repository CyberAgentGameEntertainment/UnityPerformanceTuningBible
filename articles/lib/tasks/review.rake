# Copyright (c) 2006-2021 Minero Aoki, Kenshi Muto, Masayoshi Takahashi, Masanori Kado.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

require 'fileutils'
require 'rake/clean'

BOOK = ENV['REVIEW_BOOK'] || 'ReVIEW-Template'
BOOK_PDF = BOOK + '.pdf'
BOOK_EPUB = BOOK + '.epub'
CONFIG_FILE = ENV['REVIEW_CONFIG_FILE'] || 'config.yml'
CATALOG_FILE = ENV['REVIEW_CATALOG_FILE'] || 'catalog.yml'
WEBROOT = ENV['REVIEW_WEBROOT'] || 'webroot'
TEXTROOT = BOOK + '-text'
TOPROOT = BOOK + '-text'
IDGXMLROOT = BOOK + '-idgxml'
PDF_OPTIONS = ENV['REVIEW_PDF_OPTIONS'] || ''
EPUB_OPTIONS = ENV['REVIEW_EPUB_OPTIONS'] || ''
WEB_OPTIONS = ENV['REVIEW_WEB_OPTIONS'] || ''
IDGXML_OPTIONS = ENV['REVIEW_IDGXML_OPTIONS'] || ''
TEXT_OPTIONS = ENV['REVIEW_TEXT_OPTIONS'] || ''

REVIEW_VSCLI = ENV['REVIEW_VSCLI'] || 'vivliostyle'
REVIEW_VSCLI_USESANDBOX = ENV['REVIEW_VSCLI_USESANDBOX'] ? '' : '--no-sandbox'
REVIEW_VSCLI_PDF = ENV['REVIEW_VSCLI_PDF'] || BOOK_PDF
REVIEW_VSCLI_OPTIONS = ENV['REVIEW_VSCLI_OPTIONS'] || ''

def build(mode, chapter)
  sh("review-compile --target=#{mode} --footnotetext --stylesheet=style.css #{chapter} > tmp")
  mode_ext = { 'html' => 'html', 'latex' => 'tex', 'idgxml' => 'xml', 'top' => 'txt', 'plaintext' => 'txt' }
  FileUtils.mv('tmp', chapter.gsub(/re\z/, mode_ext[mode]))
end

def build_all(mode)
  sh("review-compile --target=#{mode} --footnotetext --stylesheet=style.css")
end

task default: :html_all

desc 'build html (Usage: rake build re=target.re)'
task :html do
  if ENV['re'].nil?
    puts 'Usage: rake build re=target.re'
    exit
  end
  build('html', ENV['re'])
end

desc 'build all html'
task :html_all do
  build_all('html')
end

desc 'preproc all'
task :preproc do
  Dir.glob('*.re').each do |file|
    sh "review-preproc --replace #{file}"
  end
end

desc 'generate PDF and EPUB file'
task all: %i[pdf epub]

desc 'generate PDF file'
task pdf: BOOK_PDF

desc 'generate static HTML file for web'
task web: WEBROOT

desc 'generate text file (without decoration)'
task plaintext: TEXTROOT do
  sh "review-textmaker #{TEXT_OPTIONS} -n #{CONFIG_FILE}"
end

desc 'generate (decorated) text file'
task text: TOPROOT do
  sh "review-textmaker #{TEXT_OPTIONS} #{CONFIG_FILE}"
end

desc 'generate IDGXML file'
task idgxml: IDGXMLROOT do
  sh "review-idgxmlmaker #{IDGXML_OPTIONS} #{CONFIG_FILE}"
end

desc 'generate EPUB file'
task epub: BOOK_EPUB

IMAGES = FileList['images/**/*']
OTHERS = ENV['REVIEW_DEPS'] || []
SRC = FileList['./**/*.re', '*.rb'] + [CONFIG_FILE, CATALOG_FILE] + IMAGES + FileList[OTHERS]
SRC_EPUB = FileList['*.css']
SRC_PDF = FileList['layouts/*.erb', 'sty/**/*.sty']

file BOOK_PDF => SRC + SRC_PDF do
  FileUtils.rm_rf([BOOK_PDF, BOOK, BOOK + '-pdf'])
  sh "review-pdfmaker #{PDF_OPTIONS} #{CONFIG_FILE}"
end

file BOOK_EPUB => SRC + SRC_EPUB do
  FileUtils.rm_rf([BOOK_EPUB, BOOK, BOOK + '-epub'])
  sh "review-epubmaker #{EPUB_OPTIONS} #{CONFIG_FILE}"
end

file WEBROOT => SRC do
  FileUtils.rm_rf([WEBROOT])
  sh "review-webmaker #{WEB_OPTIONS} #{CONFIG_FILE}"
end

file TEXTROOT => SRC do
  FileUtils.rm_rf([TEXTROOT])
end

file IDGXMLROOT => SRC do
  FileUtils.rm_rf([IDGXMLROOT])
end

desc 'run vivliostyle'
task 'vivliostyle:preview': BOOK_EPUB do
  sh "#{REVIEW_VSCLI} preview #{REVIEW_VSCLI_USESANDBOX} #{REVIEW_VSCLI_OPTIONS} #{BOOK_EPUB}"
end

task 'vivliostyle:build': BOOK_EPUB do
  sh "#{REVIEW_VSCLI} build #{REVIEW_VSCLI_USESANDBOX} #{REVIEW_VSCLI_OPTIONS} -o #{REVIEW_VSCLI_PDF} #{BOOK_EPUB}"
end

task vivliostyle: 'vivliostyle:build'

CLEAN.include([BOOK, BOOK_PDF, BOOK_EPUB, BOOK + '-pdf', BOOK + '-epub', WEBROOT, 'images/_review_math', 'images/_review_math_text', TEXTROOT, IDGXMLROOT])
