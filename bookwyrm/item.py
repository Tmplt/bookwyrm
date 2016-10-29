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

from enum import Enum, unique
from fuzzywuzzy import fuzz
from collections import namedtuple
import argparse
import itertools

FUZZ_RATIO_DEF = 75

# A namedtuple, but with optional arguments.
# Credits:
# <https://stackoverflow.com/questions/11351032/named-tuple-and-optional-keyword-arguments/16721002#16721002>
class Exacts(namedtuple('Exacts', ['year', 'lang', 'edition', 'doi', 'ext'])):
    __slots__ = ()
    def __new__(cls, year=None, lang=None, edition=None, doi=None, ext=None):
        return super(Exacts, cls).__new__(cls, year, lang, edition, doi, ext)

Data = namedtuple('Data', 'authors title publisher isbns mirrors exacts')

class Item:
    """A class to hold all data for a book or paper."""

    def __init__(self, *args):
        if len(args) == 1 and isinstance(args[0], argparse.Namespace):
                self.init_from_argparse(args[0])
        elif len(args) == 1 and isinstance(args[0], Data): # check type
            self.data = args[0]

    def init_from_argparse(self, args):
        # NOTE: change this so exacts is nested within data,
        # would look so much better.

        self.data = Data(
            authors = args.author,
            title = args.title,
            publisher = args.publisher,
            isbns = args.isbn,
            mirrors = None,

            exacts = Exacts(
                year = args.year,
                lang = args.language,
                edition = args.edition,
                doi = args.doi,
                ext = args.extension
            )
        )

    def __getattr__(self, key):
        return getattr(object.__getattribute__(self, 'data'), key)

    def matches(self, wanted):

        # Parallell iteration over the two tuples of exact values.
        for val, requested in zip(self.exacts, wanted.exacts):
            if requested is not None:
                return val == requested

        if wanted.isbns:
            try:
                if not set(wanted.isbns) & set(self.isbns):
                    return False
            except TypeError:
                return False

        # partial_ratio, useful for course literature which can have some
        # crazy long titles.
        if wanted.title:
            ratio = fuzz.partial_ratio(self.data.title, wanted.data.title)
            if ratio < FUZZ_RATIO_DEF:
                return False

        # token_set seems to work best here, both when only
        # the last name is given but also when something like
        # "J. Doe" is given.
        if wanted.authors:
            ratio_thus_far = 0
            for comb in itertools.product(self.data.authors, wanted.data.authors):
                fuzz_ratio = fuzz.token_set_ratio(comb[0], comb[1])
                ratio_thus_far = max(fuzz_ratio, ratio_thus_far)

            if ratio_thus_far < FUZZ_RATIO_DEF:
                return False

        # We use partial ratio here since some sources may not use the
        # publisher's full name.
        if wanted.publisher:
            ratio = fuzz.partial_ratio(self.data.publisher, wanted.data.publisher)
            if ratio < FUZZ_RATIO_DEF:
                return False

        return True

