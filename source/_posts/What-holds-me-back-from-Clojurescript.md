title: What Holds Me Back From Clojurescript
featured: false
tags:
  - clojure
  - clojurescript
  - javascript
categories:
  - clojure
date: 2015-11-26 10:46:00
---
I've expressed [multiple](https://twitter.com/jaredforsyth/status/507253337310236672) [times](https://twitter.com/jaredforsyth/status/668947788021960704) that I really want to get into clojurescript, but I keep running into barriers.

My background: I'm an experienced JS dev, and for my personal projects I use the latest & greatest of [React](https://facebook.github.com/react), [Flow](https://flowtype.org), [Babel](https://babeljs.io), and [Webpack](https://webpack.github.io), which makes for a pretty rocking experience. Given my background, lots of my hesitation could just be the fact that staying in my comfort zone of javascript is *easy*, though it may be far less *simple* than the experience provided by clojurescript.

By the end of this post, I hope I will have convinced myself to face my fears and dive into clojurescript anyway :D

<!-- more -->

### Summary
What draws me to clojurescript?
- the community
- immutable all the data
- functional to the core
- om/next
- protocols
- macros

But what holds me back?
- startup time is a bear
- user-hostile error messages
- immature tooling
- no good story for type checking

### What draws me to clojurescript?

#### The community
This is really what keeps drawing me back to clojure(script): realy awesome talks from clojure conj & other clojurey conferences, both about difficult real-world problems and interesting/fun/playful problems. And I keep thinking "I want to be a part of that." Also, Rich Hickey's talks are fantastic.

From what I've seen, the community is still small enough to be pretty cohesive, and there are lots of helpful people on irc and [slack](https://clojurians.net).

##### vs Javascript
Javascript has a larger and necessarily much less cohesive community. This can be nice in that almost every error you encounter will have a Stack Overflow question + answer. That said, there are *so* many stackoverflow results about javascript (from so many different eras of js dev) that it can be really difficult to determine what is current best practice.

#### Immutable all the data
Having data be immutable frees up a lot of my complexity budget and enables great functional paradigms across the board.

##### vs Javascript
In javascript, everything is mutable, but w/ new es7 spread/rest syntax it is increasingly easy to just do immutable anyway. E.g. instead of `myobj.x = newx` you can do `myobj = {...myobj, x: newx}` and everything is immutable & persistant!

And for more data-heavy situations where that breaks down, I can always switch to `mori` or `immutable.js`, albeit with significant disadvantages from syntax & interop.

The fact remains however that javascript, at it's core, is designed around mutability, and clojurescript is designed around immutability. This has many design, mindset, and culture implications that are not to be underestimated.

#### Functional to the core
Clojurescript, as a lisp, gets a number of advantages automatically when it comes to handling data cleanly and concisely. Things like the threading macro, conj, partial application and more end up defining the feel of the language as well as idioms and best practices. From the brief stints that I've spent in clojure-land, I've come to really appreciate them & miss them when I'm in javascript or another imperative languages.

##### vs Javascript
Something like [trine](https://github.com/jussi-kalliokoski/trine) might do quite nicely, along with some as-yet very experimental language features (that can nonetheless be used with abandon via babel).

On the other hand, all those little helpers land you with a long list of things to memorize, with their own little tricks and gotchas (I recently spent way too long bugfighting before realizing that `map`'s return value is a *lazy* sequence). But is it complex, or simply unfamiliar? I put together a bunch of [quizlet flashcards](https://quizlet.com/Jared_Forsyth/folders/clojure) to try and overcome this barrier.

#### Om/next
Om was pretty neat when it came out, but it looks like om/next has set out to solve all our problems forever :D, at least relating to client-side state management. David Nolen is a really clever guy, and I really want what om/next promises. We'll see how that pans out.

##### vs Javascript
Relay and Falcor are really interesting, but are focused on the "server communication" part of the puzzle, leaving the rest of the client-side state up to you. Redux maybe comes closest to om/next, although it just provides a small subset of the functionality. Another possibility is to (just) port om/next [to javascript](https://twitter.com/swannodette/status/672090146339188736), maybe as a layer on top of redux...

#### Protocols
I'm really intrigued by this approach to polymorphism, but I'll have to use it more to see how it compares. It feels a lot like `swift`'s protocols or `go`'s interfaces or `rust`'s traits, which I find really useful.

##### vs Javascript
These aren't really things you can bolt on post-hoc -- you really need language-level support for them. I can't imagine javascript having an equivalent, unless via some babel transform that did whole-program analysis + type inference...

#### Macros also look cool...
But I haven't used them enough to give a real review. In rust I mostly use macros to get around limitations (mine or the compiler's) surrounding types & lifetimes.

##### vs Javascript
[SweetJS](http://sweetjs.org/) looks really awesome, and there are plans to [integrate well with babel](https://github.com/mozilla/sweet.js/pull/485), but clojurescript still wins from a community + mindset perspective.

### But what holds me back?

#### Clojure Startup time
[Twitter tells me](https://twitter.com/borkdude/status/669422012759150592) that this doesn't end up being a huge problem, but it's always a huge drag when I try to get started.

```
$ time lein help
...
lein help  1.62s user 0.24s system 104% cpu 1.785 total
```

```
$ time boot help
...
boot help  11.56s user 0.70s system 278% cpu 4.395 total
```

```
$ time lein repl
> (ctrl-c as soon as prompt appears)
lein repl  5.65s user 0.44s system 78% cpu 7.801 total
```

Someone made an npm package with the bootstrapped clojurescript repl, which helps a lot:

```
$time cljs
> (ctrl-c as soon as prompt appears)
cljs  0.95s user 0.13s system 79% cpu 1.361 total
```

[Here's an in-depth look](http://blog.ndk.io/2014/02/25/clojure-bootstrapping.html) at clojure startup time. There's a [long leinigen wiki page](https://github.com/technomancy/leiningen/wiki/Faster) about ways to speed things up, which I haven't tried yet, and [drip](https://github.com/flatland/drip) also looks promising.

#### vs Javascript

```
$ time npm help
...
npm help  0.34s user 0.10s system 79% cpu 0.548 total
```

```
$ time node
> (ctrl-c as soon as prompt appears)
node  0.10s user 0.01s system 20% cpu 0.543 total
```

Again, some on twitter say "it's not really a problem" :/ We'll have to see how it pans out.

#### User-hostile error messages
In the little time I've spent with clojure(script), I've seen plenty of multiple-screen-long java tracebacks, which are completely bewildering. Certainly, if I had a background in java they wouldn't be nearly as foreign, but I don't think anyone would argue that they are 'simple'. I don't know what kind of effort is going into "nicer error messages" (elm is a [great example](http://elm-lang.org/blog/compilers-as-assistants) of this), but if I ever dig into the clojure core system that's the first thing I'd look into.

#### Immature tooling
Being compile-to-js in a js world means there's always going to be a bit of feature lag, but there are a lot of things I miss when trying to use clojurescript.

##### REPL
People talking about clojure(script) frequently shout from the rooftops about how awesome it is to have a repl, and I'm sure it's night & day when coming from java, c++, etc. But chrome devtools is way ahead of your cli repl (and even your editor-integrated repl) in a bunch of important ways, and you can't use clojurescript in the devtools console.

The same is true for babel/es7+, but the problem is much less because es7+ is just a (fairly) light sprinkle on top of js, and data structures are all the same, etc.
To call a clojurescript function from the console... I'm not even sure how to get a reference to a function when you're at a breakpoint (because names are so mangled).

Someone made [a chrome extension](https://chrome.google.com/webstore/detail/clojurescript-repl/lmjjlapjpjeodaadkljnmdfbjpfddchm?hl=en) to give you a tab that is a clojurescript repl, but it uses an external server for compilation and doesn't integrate into your project. If it integrated with figwheel (or could talk to an arbitrary nRepl) that would be a good start.

##### Debugging
It's a great start that clojurescript has full sourcemap support, so you can generally see where you are in what file when you hit a breakpoint or throw an error, but there's still a lot to be desired. It's rather difficult (for me) to tell how to step through a clojurescript program -- when to step in & out in order to get where I want.

Also, in javascript, there's mostly just one and rarely more than three break-able expressions on a given line (such that hitting "next" would leave you on the same line). In clojurescript there is rarely just one break-able expression on a line, and more often several. Every time I try to navigate this it is frustrating.

The other big debugging issue is with value display & repl integration. When I'm at a breakpoint, it's really difficult to introspect values and actually understand what a given map contains, for example. Someone has [a library that fixes this](https://github.com/binaryage/cljs-devtools) but it requires a chrome feature that's behind a flag in canary :/. I also want to be able to interact with the values at the console (see REPL section).

#### No good story for type checking
Core.typed generated a lot of excitement when it first showed up a few years ago, although circleci has since decided it [wasn't worth it](http://blog.circleci.com/why-were-no-longer-using-core-typed/). It's based on racket's optional type checker, which I highly respect, so I was hoping to try it out on an example clojurescript project, but unfortunately compatability hasn't been maintained and it's broken for the latest version of clojurescript. But [hopefully](https://twitter.com/jaredforsyth/status/669917598016868352) this will be fixed at some point.

The Google closure compiler (which cljs already uses extensively) has support for type checking, including inference and even generics, and there are some experiments around leveraging that in clojurescript. It remains to be seen whether it will prove a good experience. I'm a bit skeptical that it will be able to give really good error messages & hints when you have type errors though. It also doesn't support typing maps, which make up a large part of clojure data.

##### vs Javascript
I had the chance to use [Flow](https://flowtype.org) on a large-scale project (the [React devtools](https://github.com/facebook/react-devtools) rewrite) and I loved it! It can be added incrementally to a codebase, has great support for lots of javascript idioms, realy great inference, and a solid user experience (long-running server to fix wait times).

### Conclusion
Clojurescript really has a lot of things going for it, and a lot of the issues I have are actively being worked on. I'm going to try using clojurescript + om/next for a fairly large personal project pretty soon and we'll see if I get hooked :)
