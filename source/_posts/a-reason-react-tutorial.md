title: A ReasonReact Tutorial
author: Jared Forsyth
tags:
  - reason
  - ocaml
  - tutorial
  - react
categories:
  - reason
description: // TODO
date: 2017-07-01 5:24:31
---

Are you a big fan of React, and want to more about Reason/OCaml? I made this for you!

Reason is a project that adds a javascript-esque syntax and a bunch of tooling to OCaml, with the goal of bringing this awesome language, with its powerful type system and robust multi-platform compiler to a broader audience. It's backed by the good folks at Facebook who invented and built React, and so naturally having best-in-class React interop has been a high priority.

This tutorial aims to give you a nice introduction to the syntax and type system of Reason, through the ReasonReact library. We'll be building a simple Todo list application.

<!-- more -->
<p class="empty"><script src="/assets/hover.js"></script><script src="/assets/nm-tmp-name.js"></script><link rel=stylesheet href="/assets/nm.css"></p><div class="toc-wrapper"><div class="table-of-contents"><a href="#post-title">A ReasonReact Tutorial</a><ul><li><a href="#5ecd092bb1884">What are we building?</a></li><li><a href="#83bf3a7e5dd27">Setup</a><ul><li><a href="#3d5ee98cd71aa">bsconfig.json</a></li><li><a href="#e5658c98749a4">package.json</a></li><li><a href="#43d82fd946a28">webpack.config.js</a></li></ul></li><li><a href="#1fdc011376386">Building</a></li><li><a href="#b3cdff33c3c28">Step 0: The included code</a><ul><li><a href="#57ac83f2c130e">Inter-file dependencies</a></li><li><a href="#b475a65c859c">ReasonReact's JSX</a></li><li><a href="#c7a4dc5c56b6d">Defining a component</a></li></ul></li><li><a href="#10e9330786dc8">Step 1: Adding some state</a><ul><li><a href="#07a5b32c62e9a">Declaring types</a></li><li><a href="#9fae06419113f">Making a stateful component</a></li><li><a href="#3f501cd49c8fc">Reacting to events and changing state</a></li></ul></li><li><a href="#cd172cf41873">Step 2: Rendering items</a><ul><li><a href="#fb717d100ee41">The TodoItem component</a></li><li><a href="#30b3551dbe786">Rendering a list</a></li><li><a href="#d4bea3e1a7eb7">Tracking ids w/ a mutable `ref`</a></li></ul></li><li><a href="#603768d469eaf">Step 3: Full interactivity</a><ul><li><a href="#9a1a549458f4b">Checking off items</a></li><li><a href="#8ec4b7b812fa4">Text input</a></li><li><a href="#ad35ce4ca0bd7">Editable Todo items</a></li></ul></li></ul></div></div>

<h2 id=5ecd092bb1884>What are we building?</h2>

[screenshot of Todo List]

<h2 id=83bf3a7e5dd27>Setup</h2>

Clone [this starter repo] that has all of the config files ready for you. Here's what it contains out of the box: a signal

```bash
~$ tree
.
├── bsconfig.json
├── package.json
├── webpack.config.js
├── public
│   ├── index.html
│   └── styles.css
└── src
    ├── Main.re
    └── TodoList.re
```

<h3 id=3d5ee98cd71aa>bsconfig.json</h3>

`bsconfig.json`: information telling `bucklescript` how to compile your code. In it, we specify libraries we depend on (reason-react), that we want to use the new react-jsx transform, and that our files are in `src`.

```json
{
  "name" : "tic-tac-toe",
  "reason" : {"react-jsx" : 2},
  "bs-dependencies": ["reason-react"],
  "sources": "src"
}
```

> Here's some documentation on the [schema of bsconfig.json](http://bucklescript.github.io/bucklescript/docson/#build-schema.json). Note that source directories are **not** walked recursively. Subfolders have to be listed out.

<h3 id=e5658c98749a4>package.json</h3>

For our development dependencies we have `bs-platform` (which contains the bucklescript compiler) and `webpack` (for bundling the compiled js files together).

Our runtime dependencies include both `reason-react` *and* the npm libraries that `reason-react` code depends on, `react`, and `react-dom`.

`npm start` will start the bucklescript compiler in watch mode, and `npm run build` will start our webpack bundler in watch mode. While developing, we'll have both these processes running.

```json
{
  "name": "reason-to-do",
  "scripts": {
    "start": "bsb -make-world -w",
    "build": "webpack -w",
    "clean": "bsb -clean-world"
  },
  "dependencies": {
    "react": "^15.4.2",
    "react-dom": "^15.4.2",
    "reason-react": ">=0.1.4"
  },
  "devDependencies": {
    "bs-platform": "^1.7.5",
    "webpack": "^3.0.0"
  }
}
```

<h3 id=43d82fd946a28>webpack.config.js</h3>

Webpack also needs some configuration, so it knows what to compile and where to put it. Notice that bucklescript puts our compiled javascript into `./lib/js/`, with parallel file structure to our `./src` directory.

```javascript
module.exports = {
  entry: './lib/js/src/main.js',
  output: {
    path: __dirname + '/public',
    filename: 'bundle.js',
  },
};
```

<h2 id=1fdc011376386>Building</h2>

Open two terminals, and `npm start` in one, `npm run build` in the other. The one with `npm start` is bucklescript, which you'll want to keep an eye on -- that's the one that's going to show you error messages if you e.g. have a type error in your code. The webpack one you can pretty much ignore.

Now open `public/index.html` in your favorite browser, and you should see this!

[screenshot]

<h2 id=b3cdff33c3c28>Step 0: The included code</h2>

[screenshot]

We've got two reason source files at the moment: `Main.re` and `TodoApp.re`.

```reason
/* Main.re */
ReactDOMRe.renderToElementWithId <TodoApp /> "root";
```

<h3 id=57ac83f2c130e>Inter-file dependencies</h3>

One thing you'll notice is that there's no `require()` or `import` statement indicating where `TodoApp` came from. In OCaml, inter-file (and indeed inter-package) dependencies are all *inferred* from your code. Basically, the compiler sees `TodoApp` isn't defined anywhere in this file, there must be a file `TodoApp.re` (or `.ml`) somewhere that this depends on.

> Currently, there is no distinction made between files in your own project and files in libraries you depend on -- meaning that if `ReasonReact` had a file called `Utils.re` inside of it, you wouldn't be able to have a file named `Utils.re` in your project. As you might imagine, this is something of a mess, and is being [actively worked on](https://github.com/BuckleScript/bucklescript/issues/1584).

<h3 id=b475a65c859c>ReasonReact's JSX</h3>

In React.js, we would have done `import TodoApp from './TodoApp.js'`, and `export default class TodoApp extends React.Component { ...`. What's going on here?

Let's look at what `<TodoApp />` desugars to in ReasonReact:

```javascript
TodoApp.make [||];
```

This means "call the `make` function in the `TodoApp` module with a single argument, an empty array".

If there had been some props and some children, it would desugar like this

```javascript
<TodoApp some="thing" other=12>child1 child2</TodoApp>
/* becomes */
TodoApp.make some::"thing" other::12 [|child1, child2|];
```

Some key points here

- Calling a function in Reason, like OCaml and Haskell, doesn't involve parenthesis or commas. `a b c` is akin to JavaScript's `a(b, c)`. There's an [open pull request](https://github.com/facebook/reason/pull/1299) to move to more js-like syntax.
- `[| val, val |]` is array literal syntax. An array is fixed-length & mutable, with O(1) random access, in comparison to a List, which is singly linked & immutable, with O(n) random access.
- prop values don't have the `{}` curly wrappers what we know from jsx, they are parsed as expressions. So `a=some_vbl_name` is perfectly fine.
- Children are also expressions - in contrast to JSX, where they are strings by default.

So we know that `TodoApp` needs to have a `make` function that takes an array of children. Let's take a look at it.

<h3 id=c7a4dc5c56b6d>Defining a component</h3>

```javascript
/* TodoApp.re */

let component = ReasonReact.statelessComponent "TodoApp";

let make children => {
  ...component,
  render: fun () self => {
    <div className="app">
      <div className="title">
        (ReasonReact.stringToElement "What to do")
      </div>
      <div className="items">
        (ReasonReact.stringToElement "Nothing")
      </div>
    </div>
  }
};
```

In the `make` function, we're taking a `children` argument (but ignoring it), and returning a component definition. `ReasonReact.statelessComponent` returns a default component definition (as a [record]), with various lifecycle methods & other properties that you can override with the `...record spread` syntax, which is similar to es6 object spread. In this case, we only want to override the `render` function.

> In Reason, like OCaml, Haskell, and Lisps in general, there is no explicit `return` statement for designating the result of a function. The value of any block is equal to the value of the last expression in it. In fact, a block is nothing more than a sequence of expressions where we ignore all but the last value. 

Our render function takes two arguments -- the value of the current state, and `self`, used for updating state. Because this is a stateless component, we use neither.

We return some virtual dom elements! Because `<div` starts with a lower-case letter, it is intepreted as a DOM element, and undergoes a different transform from custom components (it actually becomes a straight `React.createElement` call in the compiled JS).

`ReasonReact.stringToElement` is required to satisfy the type system -- we can't drop in React elements and strings into the same array - we have to wrap the strings with this function first. In my code I often have an alias at the top of the file `let se = ReasonReact.stringToElement` to make it less cumbersome.

<h2 id=10e9330786dc8>Step 1: Adding some state</h2>

<h3 id=07a5b32c62e9a>Declaring types</h3>

Our state will be just a list of Todo items.

```reason
type item = {
  title: string,
  completed: bool,
};
type state = {
  /* this is a type w/ a type argument,
   * similar to List<Item> in TypeScript,
   * Flow, or Java */
  items: list item,
};
```

If you're familiar with [flow] or [typescript] this syntax shouldn't look too foreign to you.

One important difference is that you can't nest type declarations like you can in flow or typescript. For example, this is illegal:

```javascript
type state = {
  /* won't compile! */
  items: list {
    title: string,
    completed: bool,
  }
}
```

Another important thing to note is that type names (and indeed variable names) *must* start with a lower-case letter. Variant (enum) cases and Module names must start with an upper-case letter.

<h3 id=9fae06419113f>Making a stateful component</h3>

We'll start out by changing `ReasonReact.statelessComponent` to `ReasonReact.statefulComponent`. Then our `make` function gets a little more interesting.

```reason
/* I've gone ahead and made a shortened name for converting strings to elements */
let se = ReasonReact.stringToElement;
let make children => {
  ...component,
  initialState: fun () => {
    items: [{
      title: "Write some things to do",
      completed: false,
    }],
  },
  render: fun {items} self => {
    let numItems = List.length items;
    <div className="app">
      <div className="title">
        (se "What to do")
      </div>
      <div className="items">
        (se "Nothing")
      </div>
      <div className="footer">
        (se (string_of_int numItems) ^ "items")
      </div>
    </div>
  }
};
```

`initialState` is what you'd expect, and now the first argument to our `render` function gets useful. The argument destructuring syntax is just like in JavaScript, where we get the `items` right out of the state argument.

<h3 id=3f501cd49c8fc>Reacting to events and changing state</h3>

Let's make a button that adds an item to the list.

```reason
// defined at the top level
let newItem () => {title: "Click a button", completed: true};

// now add a button to our render method
  render: fun {items} self => {
    <div className="app">
      <div className="title">
        (se "What to do")
        <button
          onClick=(fun evt => Js.alert "didn't add something")
        >
          Add something
        </button>
      </div>
      /* etc */
    </div>
  }
```

If this were JavaScript & React, this is where we'd call `this.setState`. In ReasonReact, we make an updater function that takes the current state and returns a new one.

```reason
<button
  onClick=(self.update (fun evt state self => {
    ReasonReact.Update {
      ...state,
      items: [newItem(), ...state.items]
    }
  }))
>
```

If we determined that we didn't want to update the state (and avoid a re-render), we could return `ReasonReact.NoUpdate`.

> `Update` and `NoUpdate` are examples of variant values in Reason (kinda like TypeScript enums but much better), which will be familiar if you've used Swift or Haskell. In Flow, we approximate these with [tagged unions].

Now when we click the button, the count at the bottom goes up!

<h2 id=cd172cf41873>Step 2: Rendering items</h2>

<h3 id=fb717d100ee41>The TodoItem component</h3>

We're going to want to have a component for rendering the items, so let's make one. To start out, we won't have it be its own file -- we'll use a nested module.

```reason
/* at the top of TodoApp.re */
type item = {
  title: string,
  completed: bool,
};

let module TodoItem = {
  let component = ReasonReact.statelessComponent "TodoItem";
  let make ::item children => {
    <div className="item">
      <input
        _type="checkbox"
        value=(string_of_bool item.completed)
        /* TODO make interactive */
      />
      (se item.title)
    </div>
  };
};
```

So this is another stateless component, except this one accepts a property: `item`. The `::argname` syntax means "this function takes a labeled argument which is known as `text` both externally and internally". Swift and Objective C also allow you have labeled arguments, with an external name that is optionally different from the internal one. If you wanted them to be different, you would write e.g. `fun externalFacingName::internalFacingName => `. `children` is an unnamed argument.

> In OCaml, named arguments can be given in any order, but unnamed arguments cannot. So if you had a function `let myfn = fun ::a ::b c d => {}` where `c` was an `int` and `d` was a `string`, you could call it `myfn b::2 a::1 3 "hi"` or `myfn a::3 3 "hi" b::1` but not `myfn a::2 b::3 "hi" 4`.

<h3 id=30b3551dbe786>Rendering a list</h3>

Now that we've got a `TodoItem` component, let's use it! We'll replace the section that's currently just `(se "Nothing")` with this:

```reason
<div className="items">
  (ReasonReact.arrayToElement
    (Array.of_list
      (List.map (fun item => <TodoItem item />) items)
    )
  )
</div>
```

In the center of all this you can see the function that takes our data and renders a react element.

```reason
fun item => <TodoItem item />
```

Another difference from JSX is that an attribute without a value is "punned", meaning that `<TodoItem item />` is the same as `<TodoItem item=item />`. In JSX, lone attributes are interpreted as `<TodoItem item={true} />`.

```reason
React.arrayToElement (Array.of_list (List.map /*...*/ items))
```

And now we've got the nuts and bolts of calling that function for each item and appeasing the type system. Another way to write the above is

```reason
List.map /*...*/ items |> Array.of_list |> React.arrayToElement
```

The pipe `|>` is a left-associative binary operator that's defined as `a |> b == b a`. It can be quite nice when you've got some data and you just need to pipe it through a list of conversions.

<h3 id=d4bea3e1a7eb7>Tracking ids w/ a mutable `ref`</h3>

Now, if you're familiar with React, you'll know that we really ought to be using a [`key`] to uniquely identify each rendered `TodoItem`, and in fact we'll want unique keys once we get around to modifying the items as well.

Let's add an `id` property to our `item` type, and add an `id` of `0` to our initialState item.

But then, what do we do for the `newItem` function? We want to make sure that each item created has a unique id, and one way to do this is just have a variable that we increment by one each time we create a new item.

```reason
let lastId = 0;
let newItem () => {
  let lastId = lastId + 1;
  {id: !lastId, title: "Click a button", completed: true};
};

```

Of course this won't work -- we're just defining a new variable that's only scoped to the `newItem` function. At the top level, `lastId` remains `0`. In order to simulate having a let binding be mutable, we'll use a `ref`.

<h2 id=603768d469eaf>Step 3: Full interactivity</h2>

<h3 id=9a1a549458f4b>Checking off items</h3>


<h3 id=8ec4b7b812fa4>Text input</h3>



<h3 id=ad35ce4ca0bd7>Editable Todo items</h3>



