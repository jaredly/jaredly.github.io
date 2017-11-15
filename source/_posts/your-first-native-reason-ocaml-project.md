title: Your first native Reason/OCaml project
featured: false
tags:
  - reason
  - ocaml
  - tutorial
categories:
  - reason
date: 2017-6-17 19:15:00
description: A quick tutorial where we build a simple cli program
---

I just wrote my first Reason project that compiled to native, and you can too! Luckily for you, my first native project was a cli tool to help people get started with native Reason development 🙌.

<!-- more -->

> If you're more interested in targeting JavaScript, I wrote [two](http://jaredforsyth.com/2017/06/03/getting-started-with-reason-and-bucklescript/) [tutorials](http://jaredforsyth.com/2017/06/03/javascript-interop-with-reason-and-bucklescript/) about that to get you started.

What will we be doing?

- project setup
- building & running
- parsing cli arguments

## Project Setup

First, you'll need to install OCaml on your machine if you don't have it already. `brew install ocaml` on macos, `apt-get install ocaml` on debian/ubuntu, with instructions for more platforms [here](http://opam.ocaml.org/doc/Install.html).

Then we'll install [ohai](https://github.com/jaredly/ohai), the tool I made for configuring a new native reason project. You can think of it as the `npm init` or `cargo new` for Reason/OCaml.

```bash
opam pin add ohai git+https://github.com/jaredly/ohai#1.0.1
```

Finally, let's setup the project!

```bash
ohai init --bin my_cli
```

You'll see that a bunch of files have been created for you - go ahead and take a look in each to see what's there.

```bash
~$ tree
.
├── Makefile
├── bin
│   ├── jbuild
│   └── my_cli.re
├── jbuild-workspace.dev
├── lib
│   ├── Main.re
│   └── jbuild
├── test
│   ├── jbuild
│   └── test.re
└── my_cli.opam
```

So we've got 3 directories - `lib` is where our functionality lives, `bin` just has a single top-level call into `lib/Main.re`, and `test` will have any tests we write.

## Building & Running

If you run `make run` it will build your project and run the built executable.

```bash
~$ make run
jbuilder build @install
       refmt bin/my_cli.re.ml
    ocamldep bin/my_cli.depends.ocamldep-output
      ocamlc lib/lib.{cmi,cmo,cmt}
       refmt lib/Main.re.ml
    ocamldep lib/lib.depends.ocamldep-output
      ocamlc lib/lib__Main.{cmi,cmo,cmt}
      ocamlc bin/my_cli.{cmi,cmo,cmt}
    ocamlopt lib/lib.{cmx,o}
    ocamlopt lib/lib__Main.{cmx,o}
    ocamlopt lib/lib.{a,cmxa}
    ocamlopt bin/my_cli.{cmx,o}
    ocamlopt bin/my_cli.exe
jbuilder exec my_cli
Hello world
```

As you can see, the makefile first ran `jbuilder build @install`, which means "build everything you know about", and then `jbuilder exec my_cli`. The latter is shorthand for `./_build/default/bin/my_cli.exe`.

## Parsing CLI Arguments

> If you haven't already, look through the [docs for Reason](https://facebook.github.io/reason/) for a primer on the syntax. The documentation for the [OCaml standard library](https://caml.inria.fr/pub/docs/manual-ocaml/libref/index.html) can also be useful.

### Echoing Sys.argv

The default `Main.re` is very simple, and doesn't do anything with arguments.

```rust
let run () => {
  print_endline "Hello world";
};

let add2 x => x + 2;
```

To start out let's just print out all received arguments:

```rust
let run() => {
  print_endline (String.concat ", " (Array.to_list Sys.argv));
};
```

> Docs for `Sys.argv` are [here]()

Now, we rebuild with `make`, and this time we'll run the executable with some extra arguments:

```bash
~$ jbuilder exec my_cli -- -an-argument another argument
my_cli, -an-argument, another, argument
```

[`Sys.argv`](https://caml.inria.fr/pub/docs/manual-ocaml/libref/Sys.html) is an `array` (fixed-length, mutable) of strings, starting with the name of the program. We wanted to display them, and the simplest way was to convert it to a list so that we could `String.concat` them all together into a single string.

### Reacting to arguments

Let's do something more intersting in response to arguments. If you run `my_cli beep` we'll respond with "boop", and if you type `my_cli cowsay` followed by any text we'll mimic that beloved utility.

```rust
let run() => {
  /**
   * Let's convert the args to a list again, as lists
   * are generally nicer to work with in Reason,
   * especially when we want to "do something with the
   * first one, and then deal with the rest".
   * Arrays are better if we want mutation or random access.
   */
  let arglist = Array.to_list Sys.argv;
  switch arglist {
      /**
       * This initial "_" is a placeholder, meaning
       * "we don't care what's first in the list".
       * Sys.argv's first item is the name of the
       * executable, which we don't care about.
       */
      | [_, "beep"] => print_endline "boop"
      | [_, "cowsay", ...rest] => cowsay (String.concat " " rest)
      /**
       * This is the catchall case - any unrecognized
       * invocation will be met with our help text.
       */
      | _ => print_endline help_text;
  }
};
```

Now what does cowsay look like? We'll do a simplified version that puts everything on one line.

```rust
/* A multiline string! */
let cow = {|
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
|};
let cowsay text => {
  // `^` is the way to join strings together
  let message = " ( " ^ text ^ " )" ^ cow;
  // We could also do
  // Printf.printf " ( %s ) %s" text cow;
  print_endline message;
};
```

> In OCaml, unlike rust, javascript, golang, java, etc., all declarations must go before usage, even at the top level. This means that we'll put `help_text` and `cowsay()` *above* our `run()` function in the final code.

Come up with some helpful text, and we're all set!

```bash
let help_text = {|my_cli - a cli for all your needs!

Usage:
 - my_cli beep
 - my_cli cowsay some text here
|};

let cow = {|
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
|};
let cowsay text => {
  let message = " ( " ^ text ^ " )" ^ cow;
  print_endline message;
};

let run() => {
  let arglist = Array.to_list Sys.argv;
  switch arglist {
      | [_, "beep"] => print_endline "boop"
      | [_, "cowsay", ...rest] => cowsay (String.concat " " rest)
      | _ => print_endline help_text;
  }
};
```

## We're done!

```bash
~$ make
...
~$ ./_build/default/bin/my_cli.exe
my_cli - a cli for all your needs!

Usage:
 - my_cli beep
 - my_cli cowsay some text here

~$ ./_build/default/bin/my_cli.exe beep
boop
~$ ./_build/default/bin/my_cli.exe cowsay reason is awesome
 ( reason is awesome )
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
```

## What's next?
The [source code](https://github.com/jaredly/ohai) for `ohai` is pretty well documented, if you're interested in looking at something a little more complicated.

I'm also planning on writing a tutorial on making a simple web server with [cohttp](https://github.com/mirage/ocaml-cohttp) pretty soon.

Join us in our [Discord channel](https://discord.gg/reasonml), and catch me [on twitter](twitter.com/jaredforsyth) with your comments!
