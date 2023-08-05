# Persona

CMD application to store and get links to numerous websites.

## Table of Content

- [General info](#general-info)
- [Technologies](#technologies)
- [Setup](#setup)
- [Features](#features)
- [Authors](#authors)

## General info

This project is a database dedicated to the storage of websites links.
It alows you to store a link and mark them as scams or not.

The program won't be able to copy links to the clipboard if xclip isn't installed.

## Technologies

Project is created with:

- CPP
- SQLITE 3
- DOCOPT
- xclip (linux neccessary)

## Setup

To run this project, install locally the `app` folder and run it in your terminal using:

```bash
$ cd path/to/app
$ ./links
```

To be able to run the program from everywhere in your terminal, you can choose one of the following options :

- Copy the compiled executable to a directory that is already included in the PATH, such as `/usr/local/bin` or `/usr/bin`.

- Add the directory containing your executable to the PATH.

### Extra *(linux users only)*

To install `xclip`, you can run the following command:

```bash
$ apt install xclip
```

## Features

- get link directly in your copyboard
- register new link
- remove old link
- mark a link as a scam

### To do

- clipboard copy without xclip (linux)
- implement implement a version for windows and apple ? :)

## Authors

HackTheDoc - <hackthedoc@gmail.com> *(deprecated)*
