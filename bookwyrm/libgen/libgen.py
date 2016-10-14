# coding: utf-8
# This file is part of bookwyrm.
# Copyright 2016, Tmplt.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.

"""
This file contains functions for parsing search results of
Library Genesis and its mirrors.
    Fortunately, libgen gives us a html table, so we fetch each row,
zoom into the right column (with the help of _get_column and the column enums)
and extract the data we want in their respective _get_* function.

Common variable descriptions:
    soup: a BeautifulSoup.Resultset which we are gonna parse.
    html_row: a soup, but that of a html table row from libgen.
"""

from bs4 import BeautifulSoup as bs
from enum import IntEnum
import utils
import requests
import urllib
import re
import bs4

from item import Item

mirrors = (
    'libgen.io',
    'gen.lib.rus.ec'
)

class column(IntEnum):
    id = 0
    author = 1
    title = 2
    isbn = 2
    edition = 2
    publisher = 3
    year = 4
    # skip page count column,
    lang = 6
    # and file size.
    ext = 8
    mirrors_start = 9
    mirrors_end = 12

class _fetcher(object):
    results = []

    # Note that any dictionary keys whose value is None
    # will not be added to the URL's query string.
    def __init__(self, query):
        r = None
        url = "http://%s/search.php"

        for mirror in mirrors:
            r = requests.get(url % mirror, params=query)
            if r.status_code == requests.codes.ok:
                # Don't bother with the other mirrors if
                # we already have what we wanted.
                break

        # If not 200, raises requests.exceptions.HTTPError
        # (or somethig else?)
        # TODO: catch this!
        r.raise_for_status()

        soup = bs(r.text, 'html.parser')

        for row in soup.find_all('tr'):
            # The search result table gives every item an integer ID,
            # so we only want those rows.
            if row.find('td').text.isdigit():
                self.results.append(row)

    def __iter__(self):
        self.current = 0
        return self

    def __next__(self):
        # NOTE: superfluous code, since self.results is a list already.
        result = [r for n, r in enumerate(self.results) if n == self.current]
        if not result:
            raise StopIteration
        self.current += 1
        return result[0]

def _get_column(row, column):
    return row.find_all('td')[column]

def _get_author(row):
    soup = _get_column(row, column.author)

    author = soup.text.strip()
    return author if author else None

def _get_title(row):
    soup = _get_column(row, column.title)

    def inner_isdigit(value):
        try:
            return value.isdigit()
        except AttributeError:
            return False

    # A book which is part of a serie will have the same row index
    # for the title as a book which isn't part of a series.
    # What the two books share, however, is that the <a> tag has the
    # book's ID (an integer) in the "id" attribute.
    # NOTE: can this be simplified by using BeautifulSoup's attributes?
    soup = soup.find(id=inner_isdigit)

    # This will affect the top-most object,
    # and since this strips ISBNs, we should probably extract those,
    # before running this. Course, we could probably deepcopy it,
    # but may not be necessary.
    rmtags = ['br', 'font']
    for tag in soup(rmtags):
        tag.decompose()

    title = soup.text.strip()
    return title if title else None

def _get_isbn(row):
    soup = _get_column(row, column.isbn)
    try:
        soup = soup.find('br').find('i')

        isbn = soup.text.split(', ')
        return isbn if isbn else None
    except AttributeError: # not all entries will have ISBN numbers
        return None

def _get_edition(row):
    soup = _get_column(row, column.edition)
    soups = soup.find_all('i')

    for soup in soups:
        try:
            edition = soup.text
        except AttributeError:
            return None

        print(edition)

        # Item editions are always incased in brackets,
        # e.g. '[6ed.]', '[7th Revised Edition]',
        if edition[0] != '[':
            continue

        # We could use substring to get the edition number,
        # but in the case that the number is more than one digit,
        # we regex it instead.
        try:
            edition = int(re.findall(r'\d+', edition)[0])
            return edition
        except (TypeError, ValueError):
            return None

def _get_publisher(row):
    soup = _get_column(row, column.publisher)

    publisher = soup.text.strip()
    return publisher if publisher else None

def _get_year(row):
    soup = _get_column(row, column.year)

    year = soup.text.strip()
    return year if year else None

def _get_lang(row):
    soup = _get_column(row, column.lang)

    lang = soup.text.strip()
    return lang if lang else None

def _get_ext(row):
    soup = _get_column(row, column.ext)

    ext = soup.text.strip()
    return ext if ext else None

def _get_mirrors(row):
    """Parse all mirrors and return URIs which can be downloaded
    with a single request."""

    # NOTE: URLs and URIs are kinda similar, so URLs are not exclusively
    # used for http(s) locations. Make up a better variable name.

    urls = []
    for col in range(column.mirrors_start, column.mirrors_end):
        soup = _get_column(row, col)
        url = soup.find('a')['href']
        urls.append(url)

    uris = []
    for url in urls:
        if url.startswith('http'):
            r = requests.get(url)
            soup = bs(r.text, 'html.parser')

            if "golibgen" in url:
                # golibgen gives us a form with three <input>-tags.
                # Of these, we just want the ones named 'hidden' and 'hidden0',
                # which contains the uid and file name, respectively.
                # NOTE: there must be a better way to do this..
                action = soup.find('form', attrs={'name': 'receive'})
                action = action['action']

                inputs = soup.find('input', attrs={'name': 'hidden'})
                uid = inputs['value'] # of first tag

                child = next(c for c in inputs.children if isinstance(c, bs4.element.Tag))
                filename = child['value']

                params = {
                    'hidden': uid,
                    'hidden0': filename
                }
                params = urllib.parse.urlencode(params)

                # NOTE: HTTP refer(r)er "http://golibgen.io/" required to GET this.
                url = ('http://golibgen.io/%s?' % action) + params
                uris.append(url)

                continue

        elif url.startswith("/ads"):
            # The query contains an identifier we can use to
            # get the .torrent-file directly; skipping a request.
            o = urllib.parse.urlparse(url)
            ident_attr = o.query # includes attribute name

            torrent_url = "http://libgen.io/book/index.php?%s&oftorrent=" % ident_attr
            r = requests.get(torrent_url)
            magnet = utils.magnet_from_torrent(r.content)
            uris.append(magnet)

            continue

    return uris

def get_results(query):
    # debugging and testing
    query = {'req': query.title}
    results = _fetcher(query)

    items = []
    for result in results:
        item = Item()

        # Existing in the same column as the title -- for which
        # we must decompose all <i>-tags -- these must be extracted first.
        item.edition = _get_edition(result)
        item.isbns = _get_isbn(result)

        item.author = _get_author(result)
        item.title = _get_title(result)
        item.publisher = _get_publisher(result)
        item.year = _get_year(result)
        item.lang = _get_lang(result)
        item.ext = _get_ext(result)

        item.mirrors = _get_mirrors(result)[1]

        items.append(item)

    return items

