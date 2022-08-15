%#!ptex2pdf -l -u -ot '-synctex=1' test-rejsbk
% Copyright (c) 2018-2021 Munehiro Yamamoto, Kenshi Muto.
%
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
%
% The above copyright notice and this permission notice shall be included in
% all copies or substantial portions of the Software.
%
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
% THE SOFTWARE.

\IfFileExists{plautopatch.sty}{\RequirePackage{plautopatch}}{}
\NeedsTeXFormat{pLaTeX2e}
\ProvidesClass{review-jsbook}
  [2021/01/12 v5.1 Re:VIEW pLaTeX class modified for jsbook.cls]

\def\recls@error{\ClassError{review-jsbook}}
\def\recls@warning{\ClassWarning{review-jsbook}}
\def\recls@warningnoline{\ClassWarningNoLine{review-jsbook}}
\def\recls@info{\ClassInfo{review-jsbook}}

%% hook at end of reviewmacro
\let\@endofreviewmacrohook\@empty
\def\AtEndOfReVIEWMacro{%
  \g@addto@macro\@endofreviewmacrohook}
\@onlypreamble\AtEndOfReVIEWMacro

%% fixes to LaTeX2e
\RequirePackage{fix-cm}%%\RequirePackage{fix-cm,exscale}
\IfFileExists{latexrelease.sty}{}{\RequirePackage{fixltx2e}}

%% graphicx: added nosetpagesize
\IfFileExists{platexrelease.sty}{%% is bundled in TL16 or higher release version
\PassOptionsToPackage{nosetpagesize}{graphicx}%%for TL16 or higher version
}{}

\RequirePackage{xkeyval,everypage}%%,etoolbox

%% useful helpers
\newcommand\recls@get@p@[2]{%
  \edef#2{\expandafter\@recls@GET@P@\the#1}}
{\catcode`p=12\catcode`t=12\gdef\@recls@GET@P@#1pt{#1}}%

\long\def\recls@ifempty#1{%
  \expandafter\ifx\expandafter\relax\detokenize{#1}\relax\expandafter\@firstoftwo\else\expandafter\@secondoftwo\fi}
% \long\def\recls@ifempty#1{\recls@xifempty#1@@..\@nil}
% \long\def\recls@xifempty#1#2@#3#4#5\@nil{%
%   \ifx#3#4\expandafter\@firstoftwo\else\expandafter\@secondoftwo\fi}
\long\def\recls@ifnotempty#1{\recls@ifempty{#1}{}}

\newcommand*{\recls@DeclareOption}[2]{%
  \DeclareOptionX{#1}{%
    \recls@ifempty{##1}{}{%
      \ClassError{recls}{The option #1 should have no value}{\@ehc}}%
    #2}}

%% define/set specific paper(s) for review-jsbook.cls
\def\recls@define@paper#1#2{%
  %% \paper@<papername>=> <size>:<width>x<height>+<cutoffset>
  \@namedef{recls@paper@#1}{#1#2}%
}

\def\recls@set@paper#1{%
  \@ifundefined{recls@paper@#1}{%
    \recls@error{Not define such paper: #1}}\relax
  \expandafter\expandafter\expandafter
    \@recls@set@paper\expandafter\expandafter\expandafter
      {\csname recls@paper@#1\endcsname}}
\def\@recls@set@paper#1{%
  \edef\recls@set@js@paper{#1}%
  \PassOptionsToClass{\recls@set@js@paper}{jsbook}}

\recls@define@paper{a3}{paper}
\recls@define@paper{a4}{paper}
\recls@define@paper{a5}{paper}
\recls@define@paper{a6}{paper}
\recls@define@paper{b4}{paper}
\recls@define@paper{b5}{paper}
\recls@define@paper{b6}{paper}
\recls@define@paper{a4var}{}
\recls@define@paper{b5var}{}
\recls@define@paper{letter}{paper}
\recls@define@paper{legal}{paper}
\recls@define@paper{executive}{paper}

%% define/set specific fontsize
\def\recls@define@fontsize#1{%
  \@namedef{recls@fontsize@#1}{#1}}

\def\recls@set@fontsize#1{%
  \@ifundefined{recls@fontsize@#1}{%
    \recls@set@customfontsize{#1}}{\@recls@set@fontsize{#1}}}
\def\@recls@set@fontsize#1{%
  \expandafter\expandafter\expandafter
    \@@recls@set@fontsize\expandafter\expandafter\expandafter
      {\csname recls@fontsize@#1\endcsname}}
\def\@@recls@set@fontsize#1{%
  \edef\recls@jsfontsize{#1}%
  \ifdim\recls@jsfontsize=\recls@fontsize\else
    \recls@warning{jsbook.cls has no such fontsize '\recls@fontsize'.^^J
      pass through '\recls@jsfontsize' option to jsbook.cls}%
  \fi
  \PassOptionsToClass{\recls@jsfontsize}{jsbook}}

%% NOTE: カスタムフォントサイズの対応は、事実上、止めることにしました。
\def\recls@set@customfontsize#1{%
  \setlength{\@tempdima}{#1}%
  \ifdim\@tempdima<8.5pt\recls@set@fontsize{8pt}%
  \else\ifdim\@tempdima<9.5pt\recls@set@fontsize{9pt}%
  \else\ifdim\@tempdima<10.5pt\recls@set@fontsize{10pt}%
  \else\ifdim\@tempdima<11.5pt\recls@set@fontsize{11pt}%
  \else\ifdim\@tempdima<12.5pt\recls@set@fontsize{12pt}%
  \else\ifdim\@tempdima<13pt\recls@set@fontsize{14pt}%
  \else\ifdim\@tempdima<18.5pt\recls@set@fontsize{17pt}%
  \else\ifdim\@tempdima<20.5pt\recls@set@fontsize{20pt}%
  \else\ifdim\@tempdima<23pt\recls@set@fontsize{21pt}%
  \else\ifdim\@tempdima<27.5pt\recls@set@fontsize{25pt}%
  \else\ifdim\@tempdima<33pt\recls@set@fontsize{30pt}%
  \else\ifdim\@tempdima<39.5pt\recls@set@fontsize{36pt}%
  \else\recls@set@fontsize{43pt}%
  \fi\fi\fi\fi\fi\fi\fi\fi\fi\fi\fi\fi
}

\@for\recls@tmp:={%
  8pt,9pt,10pt,11pt,12pt,14pt,17pt,20pt,21pt,25pt,30pt,36pt,43pt,12Q,14Q,%
  10ptj,10.5ptj,11ptj,12ptj}\do{%
  \expandafter\recls@define@fontsize\expandafter{\recls@tmp}}

%% disable some options of jsbook.cls
\def\recls@disable@jsopt#1{%
  \recls@DeclareOption{#1}{\recls@error{option #1: not available}}}

\@for\recls@tmp:={%
  a4j,a5j,b4j,b5j,winjis,mingoth}\do{%
  \expandafter\recls@disable@jsopt\expandafter{\recls@tmp}}

%% \recls@set@tombowpaper{<papersize>}
\def\recls@set@tombowpaper#1{%
  \xdef#1{\ifx\recls@hiddenfolio\@empty tombo,\fi#1}}

%% \recls@set@hiddenfolio{<preset>}
%% <preset>: default, marusho-ink (丸正インキ), nikko-pc (日光企画), 
%%    shippo (ねこのしっぽ)
\def\recls@set@hiddenfolio#1{\ifx#1\@empty\else
  \@ifundefined{@makehiddenfolio@#1}{%
    \recls@error{Not define such hiddenfolio: #1}}\relax
  %% set hiddenfolio preset
  \expandafter\let\expandafter\@makehiddenfolio\csname @makehiddenfolio@#1\endcsname
  %% redefine to output \@makehiddenfolio for every page
  \settombowbanner{\hskip-5mm\smash{\hiddenfolio@font\@makehiddenfolio}}%
  \AddEverypageHook{\maketombowbox}%
\fi}

\def\hiddenfolio@font{\reset@font
  \scriptsize\sffamily\baselineskip.8\baselineskip}

%% hiddenfolio=default
\@namedef{@makehiddenfolio@default}{%
  \ifodd\c@page
    \llap{\thepage\hspace{\dimexpr\@tombowbleed}}%
  \else
    \rlap{\hspace{\dimexpr\paperwidth+\@tombowbleed}\thepage}%
  \fi}

%% hiddenfolio=marusho-ink
\@namedef{@makehiddenfolio@marusho-ink}{%
  \gdef\recls@tombobleed{5mm}%
  \@nameuse{@makehiddenfolio@nikko-pc}}

%% hiddenfolio=nikko-pc
\@namedef{@makehiddenfolio@nikko-pc}{%
  \def\recls@hiddfolio{%
    \edef\recls@tmp{\thepage}%
    \lower\dimexpr4pt+\@tombowbleed+.5\paperheight+5\p@\hbox{%
      \vbox{\expandafter\@tfor\expandafter\recls@x\expandafter:\expandafter=\recls@tmp\do{%
        \hbox to 1zw{\hss\recls@x\hss}}}}}%
  \ifodd\c@page
    \rlap{\recls@hiddfolio}%
  \else
    \llap{\recls@hiddfolio\hspace{-\paperwidth}}%
  \fi}

%% hiddenfolio=shippo
\@namedef{@makehiddenfolio@shippo}{%
  \@nameuse{@makehiddenfolio@nikko-pc}}


%% media=print,ebook,preview
\newif\if@cameraready \@camerareadyfalse
\newif\if@pdfhyperlink \@pdfhyperlinkfalse
\DeclareOptionX{cameraready}[print]{\gdef\recls@cameraready{#1}}
\DeclareOptionX{media}[print]{\gdef\recls@cameraready{#1}}

%% 用紙
\DeclareOptionX{paper}[a4]{\gdef\recls@paper{#1}}
\DeclareOptionX{tombopaper}{%
  \gdef\recls@tombowopts{}%%default: auto-detect
  \ifx#1\@empty\else\gdef\recls@tombowopts{tombow-#1}\fi}
\DeclareOptionX{bleed_margin}[3mm]{\gdef\recls@tombobleed{#1}}
%% 隠しノンブルプリセット
\DeclareOptionX{hiddenfolio}{\gdef\recls@hiddenfolio{#1}}%%default: (none)
%% カスタム用紙サイズ
\DeclareOptionX{paperwidth}{\gdef\recls@paperwidth{#1}}
\DeclareOptionX{paperheight}{\gdef\recls@paperheight{#1}}
%% 基本版面、天、ノド
\DeclareOptionX{fontsize}[10pt]{\gdef\recls@fontsize{#1}}
\DeclareOptionX{line_length}{\gdef\recls@line@length{#1}}%%ベタ組みになるように調整
\DeclareOptionX{number_of_lines}{\gdef\recls@number@of@lines{#1}}
\DeclareOptionX{baselineskip}{\def\recls@baselineskip{#1}}
\DeclareOptionX{linegap}{\def\recls@linegap{#1}}
\DeclareOptionX{head_space}{\gdef\recls@head@space{#1}}
\DeclareOptionX{gutter}{\gdef\recls@gutter{#1}}
%% headheight,headsep,footskip
\DeclareOptionX{headheight}{\gdef\recls@headheight{#1}}
\DeclareOptionX{headsep}{\gdef\recls@headsep{#1}}
\DeclareOptionX{footskip}{\gdef\recls@footskip{#1}}

%% 表紙・開始番号・通しノンブル
\newif\if@reclscover \@reclscovertrue
\newif\ifrecls@coverfitpage \recls@coverfitpagefalse
\newif\ifrecls@serialpage \recls@serialpagefalse
\DeclareOptionX{cover}[\@empty]{\gdef\recls@forcecover{#1}}
\DeclareOptionX{cover_fit_page}[false]{\csname recls@coverfitpage#1\endcsname}
\DeclareOptionX{startpage}[1]{\gdef\recls@startpage{\numexpr#1}}
\DeclareOptionX{serial_pagination}[false]{\csname recls@serialpage#1\endcsname}

\PassOptionsToClass{dvipdfmx,nomag}{jsbook}
\DeclareOptionX*{\PassOptionsToClass{\CurrentOption}{jsbook}}%
\ExecuteOptionsX{cameraready,media,hiddenfolio,%
  paper,tombopaper,bleed_margin,paperwidth,paperheight,%
  fontsize,line_length,number_of_lines,baselineskip,linegap,head_space,%
  gutter,headheight,headsep,footskip,%
  cover,startpage,serial_pagination}
\ProcessOptionsX\relax

%% set specific papersize, fontsize
\recls@set@paper{\recls@paper}
\recls@set@tombowpaper{\recls@tombowopts}
\recls@set@fontsize{\recls@fontsize}

%% camera-ready PDF file preparation for each print, ebook
\def\recls@tmp{preview}\ifx\recls@cameraready\recls@tmp
  \@camerareadyfalse\@pdfhyperlinkfalse\@reclscovertrue
  \PassOptionsToClass{papersize}{jsbook}%
\else\def\recls@tmp{print}\ifx\recls@cameraready\recls@tmp
  \@camerareadytrue\@pdfhyperlinkfalse\@reclscoverfalse
  \IfFileExists{gentombow.sty}{%
    \AtEndOfClass{%
      \RequirePackage[pdfbox,\recls@tombowopts]{gentombow}%
      \settombowbleed{\recls@tombobleed}%
      \recls@set@hiddenfolio{\recls@hiddenfolio}}%
  }{\recls@warning{%
      package gentombow: not installed^^J
      option tombopaper: not available}%
    \PassOptionsToClass{tombo}{jsbook}%
  }%
\else\def\recls@tmp{ebook}\ifx\recls@cameraready\recls@tmp
  \@camerareadytrue\@pdfhyperlinktrue\@reclscovertrue
  \PassOptionsToClass{papersize}{jsbook}%
\else
  \recls@error{No such value of media: \recls@cameraready}%
\fi\fi\fi

%% 内部Unicode動作の時だけupTeXとみなす
\newif\if@recls@uptex \@recls@uptexfalse
\ifx\ucs\@undefined\else \ifnum\ucs"3000="3000
  \@recls@uptextrue
  \PassOptionsToClass{uplatex}{jsbook}
  \PassOptionsToPackage{uplatex}{otf}
\fi\fi

\LoadClass{jsbook}

% \typeout{!!! magscale: \jsc@magscale}
% \typeout{!!! mag: \the\mag}%%=> 1000 -> OK

%% override papersize with custom papersize
\ifx\recls@paperwidth\@empty\else\ifx\recls@paperheight\@empty\else
  \setlength{\paperwidth}{\recls@paperwidth}
  \setlength{\paperheight}{\recls@paperheight}
  \def\recls@tmp{print}\ifx\recls@cameraready\recls@tmp\else
    \AtBeginDvi{\special{papersize=\the\paperwidth,\the\paperheight}}
  \fi
\fi\fi

%% baseline
\ifx\recls@linegap\@empty\else
  \setlength{\baselineskip}{\dimexpr\Cwd+\recls@linegap}
  \renewcommand{\normalsize}{%
    \jsc@setfontsize\normalsize\@xpt\baselineskip% \@xiipt
    \abovedisplayskip 11\jsc@mpt \@plus3\jsc@mpt \@minus4\jsc@mpt
    \abovedisplayshortskip \z@ \@plus3\jsc@mpt
    \belowdisplayskip 9\jsc@mpt \@plus3\jsc@mpt \@minus4\jsc@mpt
    \belowdisplayshortskip \belowdisplayskip
    \let\@listi\@listI}
  \normalsize
\fi
\ifx\recls@baselineskip\@empty\else
  \setlength{\baselineskip}{\recls@baselineskip}
  \renewcommand{\normalsize}{%
    \jsc@setfontsize\normalsize\@xpt\baselineskip% \@xiipt
    \abovedisplayskip 11\jsc@mpt \@plus3\jsc@mpt \@minus4\jsc@mpt
    \abovedisplayshortskip \z@ \@plus3\jsc@mpt
    \belowdisplayskip 9\jsc@mpt \@plus3\jsc@mpt \@minus4\jsc@mpt
    \belowdisplayshortskip \belowdisplayskip
    \let\@listi\@listI}
  \normalsize
\fi
\setlength{\Cvs}{\baselineskip}

%% headheight, headsep, footskip
% \setlength\topskip{\Cht}%%<= カスタムにしても、jsbook.clsのままにしとく
\ifx\recls@headheight\@empty\else\setlength\headheight{\recls@headheight}\fi
\ifx\recls@headsep\@empty\else\setlength\headsep{\recls@headsep}\fi
\ifx\recls@footskip\@empty\else\setlength\footskip{\recls@footskip}\fi
% \setlength\maxdepth{.5\topskip}%%<= カスタムにしても、jsbook.clsのままにしとく

%% 字詰め数、行数
\ifx\recls@line@length\@empty\else
  \@tempcnta\dimexpr\recls@line@length/\Cwd\relax
  \setlength\textwidth{\@tempcnta\Cwd}
  \setlength\fullwidth{\textwidth}
\fi
\ifx\recls@number@of@lines\@empty\else
  \setlength\textheight{\recls@number@of@lines\Cvs}
  \addtolength\textheight{-\Cvs}\addtolength\textheight{\Cwd}
  \addtolength\textheight{\dimexpr\topskip-\Cht}%%adjustment for jsbook.cls's \topskip
\fi

%% ノド、小口
%% gutterがあればそれに基づいて設定。
%% line_lengthが指定されていればtextwidth基準にして設定。
%% どちらも指定がなければjsbookをそのまま使用。
\ifx\recls@gutter\@empty
  \ifx\recls@line@length\@empty\else
    \setlength\oddsidemargin{\paperwidth}
    \addtolength\oddsidemargin{-\fullwidth}%%line_lengthを与えたとき\textwidth
    \setlength\oddsidemargin{.5\oddsidemargin}
    \addtolength\oddsidemargin{-1in}
    \setlength\evensidemargin{\oddsidemargin}
    \edef\recls@gutter{\evensidemargin}
  \fi
\else
  \setlength\oddsidemargin{\recls@gutter}%ノド
  \addtolength\oddsidemargin{-1in}
  \setlength\evensidemargin{\paperwidth}
  \addtolength\evensidemargin{-2in}
  \addtolength\evensidemargin{-\oddsidemargin}
  \addtolength\evensidemargin{-\textwidth}
\fi

%% 天、地
\ifx\recls@head@space\@empty
  \ifx\recls@paperwidth\@empty\else\ifx\recls@paperheight\@empty\else
    \setlength\topmargin\paperheight
    \addtolength\topmargin{-\textheight}
    \setlength\topmargin{.5\topmargin}
    \addtolength\topmargin{-1in}
    \addtolength\topmargin{-\headheight}\addtolength\topmargin{-\headsep}
  \fi\fi
  \ifx\recls@number@of@lines\@empty\else
    \setlength\topmargin\paperheight
    \addtolength\topmargin{-\textheight}
    \setlength\topmargin{.5\topmargin}
    \addtolength\topmargin{-1in}
    \addtolength\topmargin{-\headheight}\addtolength\topmargin{-\headsep}
  \fi
  \edef\recls@head@space{\dimexpr\topmargin+1in+\headheight+\headsep}
\else
  \setlength\topmargin{\recls@head@space}%天
  \addtolength\topmargin{-1in}
  \addtolength\topmargin{-\headheight}\addtolength\topmargin{-\headsep}
\fi

%% load hyperref package
\PassOptionsToPackage{hyphens}{url} % URLにハイフンが含まれる場合でも改行されるようにする
\RequirePackage[dvipdfmx, \if@pdfhyperlink\else draft,\fi
  bookmarks=true,
  bookmarksnumbered=true,
  hidelinks,
  setpagesize=false,
]{hyperref}
\RequirePackage[dvipdfmx]{pxjahyper}

%% more useful macros
%% ----------
%% include fullpage graphics
\let\grnchry@head\recls@head@space
\let\grnchry@gutter\recls@gutter
\newcommand*\includefullpagegraphics{%
  \clearpage
  \@ifstar
    {\@includefullpagegraphics}%
    {\thispagestyle{empty}\@includefullpagegraphics}
}

\newcommand*\@includefullpagegraphics[2][]{%
    \vbox to \textheight{%
      \vskip-\grnchry@head
      \vbox to \paperheight{\vss
        \hbox to \textwidth{%
          \ifodd\c@page
            \hskip-\dimexpr\oddsidemargin + 1in\relax
          \else
            \hskip-\dimexpr\evensidemargin + 1in\relax
          \fi
          \hbox to \paperwidth{\hss
            \includegraphics[#1]{#2}%
          \hss}%
        \hss}%
      \vss}%
    \vss}%
  \clearpage
}

%% 空ページ
\newcommand\oneblankpage{\clearpage\thispagestyle{empty}%
  \hbox{}\newpage\if@twocolumn\hbox{}\newpage\fi}

%% 横書き向けの、奇数ページまでの改丁(\cleardoublepage)・偶数ページまでの改丁(\clearoddpage)
\let\cleardoublepage@right\cleardoublepage
\def\cleardoublepage@left{\clearpage\if@twoside\ifodd\c@page
  \hbox{}\thispagestyle{empty}\newpage\if@twocolumn\hbox{}\newpage\fi\fi\fi}
\let\clearoddpage\cleardoublepage@left

%% 行のサンプル。\makelines{行数} で「■□■□…」からなる行を指定行数配置する
\def\makelines#1{%
  \@tempcnta\z@\relax
  \def\@makeline@f@size{\f@size}%
  \@whilenum\@tempcnta<#1\do{%
    \advance\@tempcnta\@ne\relax
    \noindent\rlap{\the\@tempcnta}\nobreak
    \makelines@neline\par}%
}
\def\makelines@unit@#10#2\relax{%
  \ifx!#2!\relax □\else\relax ■\fi}%
\newcounter{makelines@unit}
\def\makelines@neline{%
  \c@makelines@unit\@ne
  \@whilenum\c@makelines@unit<\dimexpr(\textwidth + \Cwd)/\Cwd\do{%
    \expandafter\makelines@unit@\the\c@makelines@unit0\relax
  \advance\c@makelines@unit\@ne}%
}

%% coverオプションによる表紙判定の上書き
\def\recls@tmp{true}\ifx\recls@forcecover\recls@tmp
\@reclscovertrue
\else\def\recls@tmp{false}\ifx\recls@forcecover\recls@tmp
\@reclscoverfalse
\else% それ以外の値は単に無視
\fi\fi

%% シンプルな通しノンブル
\ifrecls@serialpage
\def\pagenumbering#1{%
  \gdef\thepage{\csname @arabic\endcsname\c@page}}
\fi

%% 開始ページを変更
\g@addto@macro\frontmatter{\setcounter{page}{\the\recls@startpage}}

%% titlepageのsetcounterを使わない
\renewenvironment{titlepage}{%
    \clearpage
    \if@twoside\ifodd\c@page\else
      \hbox{}\thispagestyle{empty}\newpage
      \if@twocolumn\hbox{}\newpage\fi
    \fi\fi
    \if@twocolumn
      \@restonecoltrue\onecolumn
    \else
      \@restonecolfalse\newpage
    \fi
    \thispagestyle{empty}%
    \ifodd\c@page\relax%% \setcounter{page}\@ne
      \else\setcounter{page}\z@\fi %% 2017-02-24
  }%
  {\if@restonecol\twocolumn \else \newpage \fi
    \if@twoside\else
      %% \setcounter{page}\@ne
    \fi}

%% 表紙のノンブル
\def\coverpagezero#1{\expandafter\@coverpagezero\csname c@#1\endcsname}
\def\@coverpagezero#1{cover}

%% 脚注がページをまたいで泣き別れさせない
\interfootnotelinepenalty\@M

%% 代替定義
\def\reviewleftcurlybrace{\{}
\def\reviewrightcurlybrace{\}}

\listfiles
\endinput
