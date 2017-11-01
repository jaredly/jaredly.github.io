title: Template-based macros in Reason/OCaml
featured: false
author: Jared Forsyth
date: 2017-06-20 20:00:35
tags:
  - reason
  - ocaml
  - macros
categories:
  - reason
description: Thoughts about a friendlier macro system.
---

A couple of people have shown up in [the discord channel](https://discord.gg/reasonml) asking whether Reason has macros, and the answer is "sort of." I think we can do better.

<!-- more -->

OCaml has a feature called "syntax extensions", which are well-defined syntax structures that are there specifically for some plugin to take & transform into something else. The plugin that you include (referred to as a `ppx`), then is passed the whole AST of your file, and it can do anything it wants to it. In general, it will look for these syntax extensions that it knows how to process, and limit transformations to just those nodes.

The best explanation of syntax extensions is from whitequark, and has a big "this is out of date" disclaimer at the top 😅 https://whitequark.org/blog/2014/04/16/a-guide-to-extension-points-in-ocaml/.

## Why do we need anything else?

I was looking at the source of [bs-glamor](https://github.com/poeschko/bs-glamor), which is a wrapper around the css-in-js library [glamor](https://github.com/threepointone/glamor), and came across a familiar pattern -- [tons of boilerplate](https://github.com/poeschko/bs-glamor/blob/master/src/glamor.re).

```rust
let azimouth = (v) => Property("azimouth", v);
let background = (v) => Property("background", v);
let backgroundAttachment = (v) => Property("backgroundAttachment", v);
/* repeat for like 100 more properties */
```

If this were javascript, we'd be able to just do

```javascript
let properties = ['azimouth', 'background', /* etc */];
let helpers = {};
properties.forEach(name => {
  helpers[name] = v => new Property(name, v)
})
```

But this isn't JavaScript, and you can't dynamically create function names. Function names (and the types & shape of modules) need to be known at compile time.

You could imagine a `ppx` that goes through and looks for syntax like this, and does the appropriate transformation:

```rust
[%%make_properties [
  "azimouth",
  "background",
  /* etc */
]]
```

But `ppx`s take a fair bit of setting up, and it wouldn't really feel worth it to go through and make one just to save you some typing. Might as well just use "multi-cursor mode" in your favorite editor and type it all out, and hope that you never have to change all of the declarations in the future.

## An idea from Rust

But there's got to be a better way! Rust has a cool macro system that's designed precisely for this kind of problem.

The best part is that you don't have to know much about the Rust internal AST format, because this macro definition is template-based. Here's an example from [this excellent post](http://words.steveklabnik.com/an-overview-of-macros-in-rust):

```rust
macro_rules! vec {
    ( $( $x:expr ),* ) => {
        {
            let mut temp_vec = Vec::new();
            $(
                temp_vec.push($x);
            )* // this means "repeat for each value of x"
            temp_vec
        }
    };
}
```

Then you can do `vec!(1, 2, 3)` and it will be transformed into code that creates a vector and pushes into it.

## Taking that to Reason/OCaml

> This rest of this post might be fairly unintelligible if you aren't familiar with OCaml, syntax extensions, and ppxs. Sorry!

I could imagine a `template_macros` ppx that lets you do this (I'm keeping the application parallel to rust, even though we already have list & array literals):

```rust
/* at the top level */
[@let_macro.vec(items: list (expr));
  let temp_vec = Vec.new();
  [%loop(items, item);
    vec_push(temp_vec, [%expr item]);
  ];
  temp_vec;
];

/* somewhere in a function */
[%vec [1, 2, 3]]
```

So how would this apply to our boilerplate earler?

```rust
[@let_macro.make_properties(names: list(string));
  [%%loop(names, name);
    /* all bound variables (such as `name`) can be used as
     * identifiers, and will be substituted. If the content
     * of the bound value is not an identifier
     * or a string, then an error is thrown.
     */
    let name = (v) => Property([%string name], v);
    /* results in `let color = (v) => Property("color", v);` */
  ];
];
```

And then we can use it the way we want to!

```rust
[%%make_properties [
  "azimouth",
  "background",
  /* etc */
]];
```

## Another example

What if we wanted to generate functors as well? It might look something like this:

```rust
[@let_macro.glamorous_factory(node_types: list((Ident, string)));
  [%%loop(divs, (FunctorName, text));
    module FunctorName = (Config: GlamorousConfig) => GlamorousFactory({
      let elementName = [%string text];
    }, Config);
  ]
];

[%%glamorous_factory [
  (Div, "div"),
  (List, "list"),
  (Awesome, "awesome")
]];
```

## Some weeds

We can't do the `regex`-type thing that rust does, because OCaml extension points have to contain syntactically valid ASTs. My thought for the "macro arguments" specification would be

```rust
macro_definition= [@let_macro.<macro_name>(<macro_argument>(, <macro_argument>)*));
  <macro_body>
]

macro_name= identifier

macro_argument= identifier: <argument_type>

argument_type=
  | string
  | int
  | expr
  | ident
  | structure
  | type
  | list(<argument_type>)
  | '('<argument_type>(',' <argument_type>)+')'; /* a tuple literal */
```

And then `macro_body` would allow subsitutions of the form

```rust
[%string arg_name] /* a string literal -- will convert an identifier if given */
[%expr arg_name] /* drop in as an expression */
[%type arg_name] /* drop in as a type */
[%loop(<list_arg>, <bound_name>);
  <loop_body>
]
[%range(<start_int_or_arg_name>, <end_...>, <bound_name>);
]
[@@@gensym(<bound_name>)]; /* create an identifier that's guaranteed not to conflict */
[@@@ident(<bound_name>, <string_concat>)]; where <string_concat> is like `"something_" ^ vbl_name`

```

And when invoking, the corresponding arguments would look like:

```rust
invocation= [%%macro_name(<arg_literal>(, <arg_literal>)*)];
string= "a literal string"
int= 23
expr= /* any expression literal */
ident= an_ident
structure= [%structure /* now we can put any structure items in here */]
type= [%type: /* now this is a type */]
list(int)= [1, 2, 3]
(int, int)= (1, 2) /* for the tuple literal */
```

We'd also need some distinction between "structure item" macros and "expression" macros I imagine. Would we want to make "signature" macros? I dunno.

## Comparison to existing solutions

.

### [CPPO](https://github.com/mjambon/cppo)

CPPO is for c-style "use this code if we're targetting iOS 9.2, use thise other code if we're targetting windows". Which is quite different from eliminating boilerplate.

### [Cinaps](https://github.com/janestreet/cinaps)

Cinaps is similar in goal, but quite different in design. Both are targetted at fixing boilerplate with something that's much less work than creating a ppx

Cinaps tackles the "make sure the code doesn't get too confusing" problem by actually writing the resulting code to disk, and that's the code you commit. Template-based macros tackle the problem by limiting the power of the tool -- you can't do arbitrary transformations; you can only use templates, which (theoretically) make it pretty easy to understand what the output would be. I'd also want a cli (or IDE) tool where you enter a file name & it shows you the transformed output. (For that matter, I'd love that for all ppxs).

### ppx_deriving

Making a ppx_deriving plugin is somewhat simpler than a full blown ppx, but it's still complex enough that you'd only go to the trouble if you had a generalized pattern that you use a ton. Template-based macros are for one-offs.

## What do you think?

Let me know on [discord](https://discord.gg/reasonml) @jaredly or twitter [@jaredforsyth](https://twitter.com/jaredforsyth)
