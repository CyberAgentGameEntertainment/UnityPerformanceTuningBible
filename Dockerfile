FROM ruby:3.3-bookworm

# TeX Live + Japanese support for Re:VIEW PDF builds
RUN apt-get update && apt-get install -y --no-install-recommends \
    texlive-luatex \
    texlive-lang-japanese \
    texlive-lang-cjk \
    texlive-fonts-recommended \
    texlive-fonts-extra \
    texlive-latex-recommended \
    texlive-latex-extra \
    texlive-plain-generic \
    texlive-extra-utils \
    lmodern \
    pandoc \
    nodejs \
    npm \
    ghostscript \
    && rm -rf /var/lib/apt/lists/*

RUN gem install bundler:4.0.3

WORKDIR /book
