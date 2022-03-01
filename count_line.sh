#!/bin/bash
cloc src/* --exclude-list-file=ignored_list.txt --exclude-dir=asio,boost
