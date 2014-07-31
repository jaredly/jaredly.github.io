title: First Impressions of Rust
tags:
  - rust
  - art
  - simulation
categories:
  - Tech
  - Programming
date: 2014-03-20 16:04:00
---
A while ago, I made a cellular automata simulator in Go, inspired by [this video](https://www.youtube.com/watch?v=M4cV0nCIZoc) about a "rock, paper, scissors" simulation, where there are three "species" of cells which consume each other. <!-- Here's [a post about that](). -->

Last week I rewrote this in Rust<!-- more -->, with a number of modifications. There are now four colors, which make the system far more stable, resulting in a much more pleasing simulation (imo).

Here's the result:

{% youtube lNFOnomruqk %}
<br/>
### Things that worked well

- I *really* like the `match` construct
- everything is an expression
- I like the module system, the syntax, etc. Feels comfortable
- the compiler is very helpful, telling you what you did wrong and what you should probably do to fix it
- runtime exceptions are reported well
- the type system, once I understood it, is *amazing*. There's a ton of potential there

### Things that didn't work as well
- the macro system wasn't quite powerful enough to do the things I wanted :( but it's still cool
- compilation process isn't blazing fast, but it's not too bad
- parallelism is currently limited to message passing, whereas I really wanted fork/join functionality for processing the arrays. That should be fixed soon
- the language is still higly volatile; breaking syntax changes happen all the time, which means that many libraries on github are broken
- it took a fair amount of experimentation before I understood the type system enough to make things happen. owner vs borrowed pointers, etc.
- there's no solution for package management. This should be fixed soon as well

### Conclusion
Rust is very promising, but to volatile for real work just yet. Once the syntax stabilizes, the standard library is fleshed out, and the community develops, I think it will be incredibly useful.