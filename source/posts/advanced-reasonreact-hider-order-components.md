title: "Advanced ReasonReact: Higher Order Components"
tags:
  - reason
  - react
date: 2017-11-12 22:01:00
updated: 2017-11-12 22:01:00
description: Looking into some more complex patterns in Reason+React
old: 6 months
old_message: This post was written very near the beginning of Reason's development, during a time when Reason's development was very fast-moving. Details have probably changed since then.
dead: 18 months
---

After jumping into ReasonReact, I soon came to the question "How do I do higher-order components?" I had some duplicated logic in several components regarding data fetching, and I wanted to use this familiar React tool to refactor.

To illustrate, we'll be making a "fetches something from the network" wrapper component. We'll start with a "mixed component" that we'll then try to refactor.

<!-- more -->

## An example mixed component

The example is a `UserCard` that, given a userId, loads up & displays information about that user.

```reason
let module UserCard = {
  type state =
    | Loading
    | Failed(Js.Promise.error)
    | Loaded(user);
  let component = ReasonReact.reducerComponent("UserCard");
  let make = (~greeting, ~userId, _children) => {
    ...component,
    initialState: () => Loading,
    didMount: ({state, reduce}) => {
      fetchUser(~userId)
      |> Js.Promise.then_((user) =>  {
        reduce(() => Loaded(user))();
        Js.Promise.resolve();
      }) |> Js.Promise.catch((error) => {
        reduce(() => Failed(error))();
        Js.Promise.resolve();
      }) |> ignore
    },
    render: ({state, reduce}) => {
      switch state {
        | Loading => <div>(str("loading..."))</div>
        | Failed(error) => <div>(str("unable to load user..."))</div>
        | Loaded(user) => <div>
          <div className="name">
            (str(greeting ++ " " ++ user##name))
          </div>
          <img src=user##profileImage className="profile-pic" />
        </div>
      }
    }
  }
}
```

Ok, so now we have a working component, but there's a ton of plumbing involved in getting & managing the state, that it would be nice to abstract away into a higher-order-component (especially if we make several of these components).

In JavaScript, we might do something like this

```javascript
const wrapWithFetch = (fetchFromProps, ChildComponent) => class WrappedComponent extends React.Component {
  /* set up the state, make the fetch, etc. */
  render() {
    if (this.state.loaded) {
      return <ChildComponent {...this.props} data={this.state.data} />
    }
  }
}
```

And so we have people come into the #react room of our [discord channel](https://discord.gg/reasonml) asking "how do I spread props in reason?" And the basic answer is "you can't."

## Why we can't spread props

First let's look at why you can spread props in JavaScript.

```javascript
<ChildComponent {...this.props} data={this.state.data} />
// translates to
React.createComponent(ChildComponent, {...this.props, data: this.state.data})
```

After the JSX transformation, `props` is a plain JavaScript object, and props spread becomes [object spread](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Spread_operator#Spread_in_object_literals). Reason's JSX trasnformation & corresponding API is a bit different:

```reason
<ChildComponent one=two three=four five />
/* translates to */
ReasonReact.createElement(ChildComponent.make(~one=two, ~three=four, ~five, [||]))
```

Named props translate directly to named arguments on the component module's `make` function, and there's no support in OCaml for "I have a bag of arguments please dump them into this function call".

One response here is "can we change ReasonReact's API to something more spreadable?" Even if we did, though, there's an issue of getting the types right. Higher Order Components work in JavaScript because, in spreading the props to the child, you can "pass through" any props that the HOC doesn't care about, but the child needs. OCaml's type system doesn't give us a good way to express "this component accepts any props the child does but also these other props". (there are some dances you can do, but I haven't found any that produces a good HOC api).

Fortunately, there's another pattern that accomplishes the same thing without any type system trickery!

## Render props!

You might have heard of a fancy new technique called a "render prop", which is what [the cool kids](https://cdb.reacttraining.com/use-a-render-prop-50de598f11ce) are calling the "function as child" pattern.

In JavaScript, we could rewrite our HOC example as:

```javascript
class FetcherComponent<T> extends React.Component {
  props: {
    fetch: () => Promise<T>,
    render: (data: T) => React$Component,
  }
  /* set up the state, make the fetch, etc. */
  render() {
    if (this.state.loaded) {
      return this.props.render(this.state.data)
    }
  }
}
```

Here's what it would look like in Reason

```reason
let module FetcherComponent (Config: {type data}) = {
  type state =
    | Loading
    | Failed(Js.Promise.error)
    | Loaded(Config.data);
  let component = ReasonReact.reducerComponent("FetcherComponent");
  let make = (~fetch, ~failedMessage, ~render, _children) => {
    ...component,
    initialState: () => Loading,
    didMount: ({state, reduce}) => {
      fetch()
      |> Js.Promise.then_((data) =>  {
        reduce(() => Loaded(data))();
        Js.Promise.resolve();
      }) |> Js.Promise.catch((error) => {
        reduce(() => Failed(error))();
        Js.Promise.resolve();
      }) |> ignore
    },
    render: ({state}) => {
      switch state {
        | Loading => <div>(str("loading..."))</div>
        | Failed(error) => <div>(str(failedMessage))</div>
        | Loaded(data) => render(data)
      }
    }
  }
}
```

And then we could use it like this:

```reason
let module UserFetcher = FetcherComponent({type data = user});
let module UserCard = {
  let make = (~greeting, ~userId, _children) => {
    UserFetcher.make(
      ~fetch=(() => fetchUser(~userId)),
      ~failedMessage="Unable to load user",
      ~render=((user) => {
        <div>
          <div className="name">
            (str(greeting ++ " " ++ user##name))
          </div>
          <img src=user##profileImage className="profile-pic" />
        </div>
      }),
      [||]
    )
  }
}
/* somewhere */
<UserCard greeting="Howdy" userId="root" />
```

There are a couple of interesting things going on here.

First, we're using a parameterized module (in OCaml called a "functor") so that our `FetcherComponent` can know what type of thing it's loading. (We *might* have been able to get away with relying on inference by using Variants, but this way the error messages will be much nicer.)

Second, there is no `UserCard` component! Our `UserCard` module's `make` function just instantiates a `UserFetcher` component after doing a transformation on the props. This render-prop pattern is just one of the cool things you can do with this setup 🙂.

And at the end of it, we've separated out the plumbing of "dealing with async" from the things that were actually unique to our UserCard component. I consider that success!

## More examples

I extracted this pattern from a [web app I'm working on](https://github.com/jaredly/f3d), and you can browse around for some real-code examples:

- Some generic "Fetcher" components ([code](https://github.com/jaredly/f3d/blob/818760290ae6f0ee243a9f405d2d313d32161dd3/packages/bs-firebase-react/src/FirebaseFetcher.re))
- A simple example of using a fetcher ([code](https://github.com/jaredly/f3d/blob/3f7642312a18f5699be25987f030f75c4df63607/src/Recipe/UserName.re))
- Using two Fetchers together ([code](https://github.com/jaredly/f3d/blob/3f7642312a18f5699be25987f030f75c4df63607/src/Recipe/ViewRecipe.re#L234))
- Combining a Fetcher with a curried function ([code](https://github.com/jaredly/f3d/blob/3f7642312a18f5699be25987f030f75c4df63607/src/Recipe/ViewMadeIts.re#L11))

Let me know [on twitter](https://twitter.com/jaredforsyth) or [discord](https://discord.gg/reasonml) if this was helpful or you have questions!
