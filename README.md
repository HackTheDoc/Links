# Links

CMD application to store and get links to numerous websites.

## Table of Content

- [General info](#general-info)
- [Features](#features)
- [Technologies](#technologies)
- [Setup](#setup)
- [Authors](#authors)

## General info

This project is a database dedicated to the storage of websites links.

It only work on linux, as this is where I will be using it and it has been designed for personal use.
If you want to add new features or to use it on another plateform, feel free to contact me.

## Features

- register and remove links
- get link directly in your copyboard
- possibility to mark link (chatroom, forum, library, scam, wiki)
- list link depending on their affectation (see previous point)

## Technologies

Project is created with:

- CPP
- SQLITE 3
- xclip (<mark>necessary</mark>)

## Setup

### Requirements

The program use the `xclip` package to run, in order to install it run the following command (may require **root** mode) :

```bash
$ apt install xclip
```

### Installation

To run this project, install the file located in the `app` folder and run it in your terminal using:

```bash
$ cd path/to/app
$ ./links
```

To be able to run the program from everywhere in your terminal, you can choose one of the following options :

- Copy the compiled executable to a directory that is already included in the PATH, such as `/usr/local/bin` or `/usr/bin`.

- Add the directory containing your executable to the PATH.

### To do

- make a linux package

## Authors

HackTheDoc - <hackthedoc@gmail.com> *(deprecated)*
