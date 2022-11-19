
title: What's cool about Unison?
featured: true
tags:
  - unison
  - programming languages
date: 2021-03-02 19:03:00
---

[Unison](https://unisonweb.org) is doing a lot of interesting things, and as I recently implemented an [alternative runtime in rust](https://github.com/jaredly/unison.rs), I have some insight into what's going on under the hood 😉. Here's my attempt to sum things up.

<!-- more -->

## Algebraic Effects (called "Abilities" in the unison world)

One reason you might be excited about unison is because it supports algebraic effects, which have mostly been an area of research up until now. They allow you to write your code in a "direct" style, calling functions as though they were interacting with the outside world (e.g. reading & writing to the file system), while in reality your code is "pure" unless/until you hook it up with a wrapper that performs the side-effects. This is a great step forward in functional programming, because it allows the purity of monadic / managed effects, with the clarity & straightforward nature of direct function calls.

It also goes a long way to addressing the issues described in ["What color is your function"](https://journal.stuffwithstuff.com/2015/02/01/what-color-is-your-function/); "async" calls become normal function calls, and the effectful wrapper can decide how to orchestrate the async calls. Also, mocking interaction with the outside world, a famously tricky part of unit testing, becomes very straightforward!

Eff is the toy/research language created to originally develop the ideas around algebraic effects. It's not viable for production usage, but [their tutorial paper](https://www.eff-lang.org/handlers-tutorial.pdf) is a great read for understanding what's going on. If you dive into that paper, it'll be important to note that unison's implementation is of "shallow handlers" (given passing reference on page 4 of that paper, and described in more depth in [this 2013 paper](https://doi.org/10.1145/2544174.2500590)), in contrast to Eff's "deep handlers".

OCaml has been teasing an implementation of algebraic effects (along with multicore support) for several years now, but it looks like it's probably still several years away from being merged in & usable.

React's Suspense is very much inspired by algebraic effects, as [Dan Abramov describes](https://overreacted.io/algebraic-effects-for-the-rest-of-us/). He ends with "I don’t know if they’re ever going to reach mainstream adoption. I think I’ll be disappointed if they don’t catch on in any mainstream language by 2025."

## Code is stored as a structured, type-checked tree in a database, not as text in files

Having the "source of truth" of our code be an unstructured list of ascii (or utf8) characters in files & folders has a number of downsides. For one, text files can contain all manner of incoherence, garbled merge conflicts, parser errors, etc. I remember [this blog post by Josh  Marinacci in 2012](https://joshondesign.com/2012/04/09/open-letter-language-designers) being pretty formative to my thinking around this area. The representation in our heads is very structured, and the representation the compiler expects is very structured, why do we go through unstructured text on the way from our brains to the compiler?

Here we are 9 years later, and at least we have [prettier](https://prettier.io/)! So autoformatting is mainstream, which feels like only a hop and a skip away from not storing things as raw text at all. When Reasonml hit the scene, I was excited about the prospect of the project supporting multiple syntaxes, but they've had plenty of other trails to blaze, and it doesn't seem likely to be a priority going forward.

At the moment, Unison is the most developed project I know of that's serious about not settling for raw text files for storing code. (Note that the current implementation uses a home-grown database that has some unfortunate inefficiencies; there's work underway to move to sqlite, which will be much faster and smaller.)

Early on in the project, Paul (the creator of unison) experimented with a structured editor, but it got set aside in favor of working out distributed programming and other core language features. I assume it's still likely that a structured editor is in unison's future, but at the moment you write syntax in a "scratch" text file, which gets processed & inserted into the database via their command line tool.

## Code chunks (functions, values) are stored by the hash of their contents, not by name

This is also a fascinating step above and beyond storing structured code, because it totally changes the dependency game. If you change a function's implementation, you *can't* unknowingly break users of the function, because your "changed" function is actually a "new" function. Terms once defined are immutable.

This has some very interesting implications. For one, semver is no longer an issue; a library changing its definitions can't unknowingly break you. But now there's a new problem: how do you update the downstream users of the function to use the new version? The unison folks have a "rebase" process built in to their cli tool `ucm`, where you can interactively traverse the dependency tree forward, choosing which call-sites to update automatically, which to edit manually, and which to leave alone. I haven't used this tool in anger yet, so I can't really pass judgement, but it's definitely a user experience that they need to get right in order for this to work.

This decision is also essential for unison's distributed programming use case, which is central to the whole project. Because functions are referenced by a hash of their contents, nodes have the potential to distribute computation very efficiently, only sending the hash of the function they want another node to run, instead of the whole implementation. If the other node doesn't have that definition locally, it can request it, and then at it to its database.

Another benefit of this approach is that compilation is by definition incremental. Each term is parsed & type-checked exactly once, and then stored in the database. This greatly reduces the need for blazing fast compilers that can process megabytes of code in milliseconds; optimization time can be spent elsewhere. Having code in a database also removes the need for expensive file watchers which have become common in the modern development workflow.

In a statically-typed language such as unison, this has the interesting effect of dramatically limiting the scope of type inference. Changes to one function cannot change the inferred types of other functions; once a function is processed and added to the database, its type is fixed (and is indeed part of the hash under which it is stored).

## hash() and compare() are built-in for all values, *including partially applied functions*

This one really surprised me. If you have a function `plus` that takes two numbers and adds them together, `plus 3 != plus 5`, but `plus 5 == plus 5`. This allows you to do some very interesting things, such as using a dictionary with partially applied functions as keys to memoize computation. It is also an important feature for distributed computing, where you are sending (sometimes partially applied) functions to other nodes on the network.

## Misc information that might be useful to know

Unison currently runs via a bytecode interpreter written in haskell, using haskell's own garbage collector, IO libraries, etc. There has been some investigation into targeting assembly directly for improved performance, but that's still quite a ways off.

The language is still quite alpha-quality, with a number of type-system bugs and incompleteness in the builtin types. For example, there's no solution as yet for ffi, so its not yet possible to interact with libraries written in other languages. That said, it's quite fun to play around with, and I've been very impressed with the progress they've made so far.
