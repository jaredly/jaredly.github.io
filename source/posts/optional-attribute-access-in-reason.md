title: Optional Attribute Access in Reason
featured: true
author: Jared Forsyth
tags:
  - reason
  - ocaml
  - graphql
categories:
  - reason
description: Introducing a syntax plugin to ease the pain of working with graphql responses.
date: 2018-11-13 09:24:00
---

One thing that you'll run into when interfacing with complex javascript objects in Reason, is that dealing with optional objects can be a huge pain. The most common place to run into this is when using the excellent [graphql-ppx](https://github.com/mhallin/graphql_ppx) syntax plugin (often together with [reason-apollo](https://github.com/apollographql/reason-apollo)), because the result of a graphql query is a large complex javascript object, with many parts of it that are optional.

<!-- more -->

## The problem

For example, say you have the following query:

```graphql
query MyAwesomeQuery {
  user {
    bestFriend {
      profilePicture {
        url
      }
    }
  }
}
```

Where everything except the final `url` is optional.

The response type then looks like:

```reason
type queryResponse = {.
  "user": option({.
    "bestFriend": option({.
      "profilePicture": option({.
        "url": string
      }),
    }),
  }),
};
```

If you want to get the best friend's profile URL, it's quite a bit of code:

```reason
let url = switch (response##user) {
| None => None
| Some(user) => switch (user##bestFriend) {
  | None => None
  | Some(bestFriend) => switch (bestFriend##profilePicture) {
    | None => None
    | Some(profilePicture) => Some(profilePicture##url)
    }
  }
}
```

And this might be OK to write once, but it really adds up if you're doing lots of graphql queries in your app.

## A solution!

[Gregiorevda](https://github.com/Gregoirevda), an active community member and core reason constributor, proposed a [modification to the reason parser](https://github.com/facebook/reason/pull/2142) that would add "safe call operators" to address exactly this use case. It's had a fair amount of discussion back and forth, and I personally was hesitant about making a syntax change like this without first thoroughly testing it out.

And so I [made a PPX](https://github.com/jaredly/get_in) to test it out! You can install `https://github.com/jaredly/get_in_ppx` and experience the magic today.

This ppx includes two operators, that are only valid within the `[%get_in ]` form.

- `#??` is to be used when *both sides* are optional. E.g. the object on the left is optional, and the attribute you're getting out is also optional. `option({. "attr": option(int)})`
- `#?` is to be used when *only the object* is optional, but the attribute you're getting out is not. e.g. `option({. "attr": int})`

For the example query above, you could do:

```reason
let url = [%get_in response##user#??bestFriend#??profilePicture#?url]
```

## Installation

- `npm install get_in_ppx`
- add this line to your bsconfig.json: `"ppx-flags": ["get_in_ppx/ppx"]`

And you should be good to go! If you run into any issues, leave me some feedback [on the github repo](https://github.com/jaredly/get_in/issues).