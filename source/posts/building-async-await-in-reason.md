title: Building async/await in Reason
featured: true
tags:
  - reason
  - ocaml
  - await
categories:
  - reason
date: 2017-12-30 17:04:00
description: OCaml already has all the tools we need!
---

Lots of people have come into the discord channel asking about how to elegantly deal with async things. We've got `Promise.then_` and good old callbacks, but having a syntax like `async/await` can really make things nicer when you have a lot of async going on. So far in the web clients I've made, there hasn't been enough asynchrony to really feel that pain, but I thought it would be an interesting challenge to tackle anyway.

<!-- more -->

So here is how we can "recreate" javascript's `await` syntax by making a syntax transformation (basically a macro), known as a `ppx`. I won't be getting into the details of how to actually write the ppx here (maybe that will come in another post).

> Clone the [example project](https://github.com/jaredly/reason_async_example) to get started using this syntax right away! Or check out the source of the ppx [over here](https://github.com/jaredly/reason_async)

## Starting with JavaScript

First, let's imagine how we would implement async/await as a syntax transform in JavaScript.

We will ignore imperative things like `for/while` loops, because they complicate things rather dramatically.

Here's a simple async function:

```javascript
const myFunc = async (value) => {
  const something = await getFile(value);
  console.log('hi')
  const nextThing = await somePromisyThing(something);
  console.log('ho')
  return nextThing + 2 + something
}
```

Here's what it would look like with promises:

```javascript
const myFunc = (value) => {
  return Promise.resolve(getFile(value)).then(something => {
    console.log('hi')
    return somePromisyThing(something).then(nextThing => {
      console.log('ho')
      return nextThing + 2 + something
    })
  })
}
```

So in the simple case of `const x = await y`, if we have the following structure:

```javascript
const name = await expression;
/* the rest of the function body */
```

we can replace it with

```javascript
return Promise.resolve(expression).then(name => {
  /* the rest of the function body */
})
```

We could extend this to any arbitrary expression containing an `await` if we wanted to.

## Notes on JavaScript's promise weirdness

JavaScript promises have a couple of quirks that make them problematic when translating to a well-typed world.

The main thing is that they auto-collapse. So `Promise.resolve(Promise.resolve(2))` is semantically equal to `Promise.resolve(2)`. There's no way to tell them apart! A promise that holds a promise that holds a number acts as though it's just a promise holding a number.

In async/await land, this means that the following are all equivalent:

```javascript
async () => {
  // ...
  return x
}
async () => {
  // ...
  return await x
}
async () => {
  // ...
  return Promise.resolve(x)
}
```

It gets even weirder in the *way* that promises auto-collapse: they check for a `.then()` function, and call it if it exists. Which means we could add another version to our list:

```javascript
async () => {
  return {then: done => done(x)}
}
```

Weird, right?

In Reason, such type-fluid shenanigans won't fly -- `return x` will be different from `return Promise.resolve(x)`, and we'll have to account for that in our solution.

## What would this look like in Reason?

Taking advantage of OCaml's [extension points](https://caml.inria.fr/pub/docs/manual-ocaml/extn.html), we could imagine a syntax that looks like this:

```javascript
let myFunc = (value) => {
  [%await let something = getFile(value)];
  Js.log("hi");
  [%await let nextThing = somePromisyThing(something)];
  Js.log("ho");
  Promise.resolve(nextThing + 2 + something);
};
```

And this is what it will turn into:

```javascript
let myFunc = (value) => {
  Promise.then_(getFile(value), (something) => {
    Js.log("hi");
    Promise.then_(somePromisyThing(something), (nextThing) => {
      Js.log("ho");
      Promise.resolve(nextThing + 2 + something);
    })
  })
};
```

That doesn't look too bad, actually!

One thing that's a little annoying is that we have to wrap the final value in `Promise.resolve`. That's because the type signature of `Promise.then_` is `(promise('a), ('a) => promise('b)) => promise('b)`. The function we give it has to return something of type `promise` -- we can't do the "auto-collapsing" that javascript does.

There will be plenty of times when the final thing we come up with won't already be a promise, so let's make another keyword `awaitWrap` that indicates "the final thing won't be already wrapped in a promise, please wrap it for me". So we'd have

```javascript
let myFunc value => {
  [%await let something = getFile(value)];
  Js.log("hi");
  [%awaitWrap let nextThing = somePromisyThing(something)];
  Js.log("ho");
  nextThing + 2 + something;
};
```

```javascript
let myFunc = (value) => {
  Promise.then_(getFile(value), (something) => {
    Js.log("hi");
    Promise.thenWrap(somePromisyThing(something), (nextThing) => {
      Js.log("ho");
      nextThing + 2 + something;
    })
  })
};
```

And the type of `thenWrap` is `(promise('a), ('a) => 'b) => promise('b)`.

> At this point, the FP folks reading this will be thinking "oh, `then` and `thenWrap` are just the monadic `bind` and `map`". Shhhhh

Another thing you'll notice is that we didn't have to annotate the function declaration with `async` or anything. This also means we can use our await syntax in any block, not just in a function body. For example:

```javascript
let someValue = {
  [%await let x = fetch(y)];
  [%awaitWrap let z = convert(x)];
  z + 2
}
// someValue is of type `promise(int)`
```

🤔

I won't dig into how cool this is just now, but I hope to in a future post.

## Can we do this with observables?

Sure!

```javascript
let myFunc value => {
  [%await let something = value];
  Js.log("hi");
  [%awaitWrap let nextThing = someObservablyThing(something)];
  Js.log("ho");
  nextThing + 2 + something;
};
```

This would become

```javascript
let myFunc value => {
  value.flatMap(something => {
    Js.log("hi");
    someObservablyThing(something).map(nextThing => {
      Js.log("ho");
      nextThing + 2 + something;
    })
  })
};
```

Ok, that's kindof cool. We could rewrite it to be exactly the same as the promise one with just a different module name. Here's the module we'd need:

```javascript
let module Observable = {
  let then_ = (obs, fn) => obs##flatMap(fn);
  let thenWrap = (obs, fn) => obs##map(fn);
}
```

And now we can do:

```javascript
let myFunc = (value) => {
  Observable.then_(getFile(value), (something) => {
    Js.log("hi");
    Observable.thenWrap(someObservablyThing(something), (nextThing) => {
      Js.log("ho");
      nextThing + 2 + something;
    })
  })
};
```

## Making it general

How do we distinguish between when we want `await` to be for `Promises` vs `Observables`?

My macro is based heavily on [ppx_let](https://github.com/janestreet/ppx_let) from janestreet, and I copied their convention in this case. `[%await let x = y]` transforms into `Let_syntax.bind(y, x => ...`, so it assumes that you have defined what `Let_syntax` is. For Promises, you do `let module Let_syntax = Reason_async.Promise`, and it would work similarly for observables (I haven't implemented that yet).

## A potential alternate syntax

Another way to do this would be to use a `@decorator`, like so:

```javascript
let myFunc value => {
  [@await Observable] let something = value;
  Js.log("hi");
  [@awaitWrap] let nextThing = someObservablyThing(something);
  Js.log("ho");
  nextThing + 2 + something;
};
```

This has the advantage of allowing us to specify what module we need to use, if we don't want to specify `Let_syntax` every time.

Another potential modification would be to remove `awaitWrap` and just have a `%wrap` macro that we could put at the end -- so `[%wrap nextThing + 2 + something]` would turn into `Promise.resolve(nextThing + 2 + something)` in the `Promises` case.

This wouldn't work as well for observables -- there's a performance penalty in doing `.flatMap(x => Observable.just(x))` that we probably wouldn't want to pay. But it might be less confusing in the promises case.

## What's next?

If you want to use this syntax, [get it right here!](https://github.com/jaredly/reason_async) There's installation and usage instructions in the Readme, and if they're confusing, let me know.

There's been a proposal for Reason to adopt a much more concise monadic-bind syntax (inspired by f-sharp's `let!`), but I'm of the opinion that louder is much better in this case, given that it changes the runtime behavior of the program so dramatically.

Tell me what you think [on twitter](https://twitter.com/jaredforsyth) or in our [Discord channel](https://discord.gg/reasonml).

## Bonus: a full example

Here's the code in the [example repo](https://github.com/jaredly/reason_async_example):

```javascript
let getThing = () => Js.Promise.make((~resolve, ~reject) => [@bs]resolve(20));
let getOtherThing = () => Js.Promise.make((~resolve, ~reject) => [@bs]resolve(40));

let module Let_syntax = Reason_async.Promise;
let doSomething = () => {
  /* These two will be awaited concurrently (with Promise.all) */
  [%await let x = Js.Promise.resolve(10)
  and y = getThing()];

  [%awaitWrap let z = getOtherThing()];
  x + y + z + 3
};

/* Heyy look we have top-level await!
 * `consume` means "give me this promise, and have the result
 * of this whole expression be ()" */
{
  [%consume let result = doSomething()];
  Js.log(result)
};
```



