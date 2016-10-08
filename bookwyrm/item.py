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
import argparse

entities = (
    'author',
    'title',
    'year',
    'language',
    'size',
    'ext',
    'isbn',
    'doi'
)

# Use <https://docs.python.org/3/library/enum.html#orderedenum>
# for specified priority?
@unique
class ext(Enum):
    pdf = 1
    epub = 2
    djvu = 3

class item(object):
    """A class to hold att data for a book or paper."""

    def __init__(self, args):
        if not isinstance(args, argparse.Namespace):
            raise NotImplementedError('items can only be initialized from argparse.Namespace')

        self.author = args.author
        self.title = args.title
        self.publisher = args.publisher
        self.year = args.year
        self.lang = args.language
        self.isbn = args.isbn
        self.doi = args.doi
        self.ext = args.extension

#    def __str__(self):
#        return self.author
        #return "%s, %s, %s" % (self.author, self.title, self.year)