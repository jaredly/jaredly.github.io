title: "Thinking in Rust: move, copy, and fighting the compiler"
date: 2014-11-22 18:12:06
tags:
---
Rust describes itself as a language that is pursuing the trifecta: fast, concurrent, safe.

Note that "easy to learn" is not among these.

Granted, there are many things about rust that are similar to what you have probably seen in other languages; it is imperative, with some classiness, some functional things, and lots that would be familiar. It is also to be expected that once you dive deep into a language, really exercizing all of its capacity, you will run into some things that have a steep learning curve.

What you will probably spend a fair amount of time banging your head up against, however, is the memory management system.