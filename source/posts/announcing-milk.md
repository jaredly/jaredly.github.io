title: Automatic, well-typed JSON serialization in Reason/OCaml with Milk 🥛
categories:
  - reason
  - ocaml
date: 2019-05-28 08:40:16
description: Forget boilerplate -- milk will generate all of that code for you, in a future-proof way including automatic, type-safe migrations.
article_image: /images/cowsay-top.png
draft: false
featured: true
author: Jared Forsyth
---

One of the things that keeps coming up as a pain point in Reason development is the boilerplate involved in parsing & generating JSON. Whether you're using JSON for a REST api, or an on-disk file format, JSON is ubiquitous, and if you're coming from JavaScript, you're used to it also being painless! Because the data objects you're working with are immediately serializable to JSON and back.

[`Milk` 🥛](https://github.com/jaredly/milk) is a new tool I developed that generates serialization and deserialization code for your Reason/OCaml types, including types from arbitrary external packages, *and* it manages migration when the types change.

<!-- more -->

## Things milk supports

- Serializing Reason/OCaml data into JSON
    - no function types, and if a type is abstract you have to provide the serializer/deserializer functions
    - no recursive data (it will loop indefinitely), although recursive types are supported just fine.
    - > Note that you don't have to own the types that you want to serialize (in contrast to e.g. ppx-deriving). As long as the types are public (not abstract), milk can automatically generate code for them.
    - At this time, objects & classes are not supported, but records, tuples, variants, and polymorphic variants are, along with builtin types.
- Deserializing that type back from JSON
- Handling migrations and versioning, so that when you change your types, you can still deserialize old JSON
- Ezjsonm, Yojson, Bs.Json, and RexJson as backends
- Both Bucklescript and native OCaml

## Things milk does not support

- Generating type definitions from an existing JSON schema (although if someone made a tool for that, milk would be able to work with the resulting types)
- Generating JSON that looks very different from the shape of the data

## How ready is milk?

I've been using it for several months in various projects, and have stress-tested it quite a bit. That said, I'm calling it "alpha" for now until other people have tried it out.

## How does this compare to X?

- [ppx_deriving](https://github.com/ocaml-ppx/ppx_deriving) & friends: a pure ppx route differs in a couple of ways: 1) you need to annotate every type that you want to serialize. 2) ppx_deriving doesn't handle type migration.
- [marshal](https://caml.inria.fr/pub/docs/manual-ocaml/libref/Marshal.html): marshal is unsafe, and doesn't handle migration, and only targets a custom binary format. However, it does handle circular references in data, and requires no setup.
- [atdgen](https://atd.readthedocs.io/en/latest/index.html): with atdgen, the types are defined in a separate language, and then ocaml types are generated from that. Because of this, you can't serialize types from e.g. third-party libraries. It also doesn't handle migration. Atdgen does support generating code for multiple languages, which is cool.
- [graphql_ppx](https://github.com/mhallin/graphql_ppx): generates type defitions from a graphql schema, fills a similar role to atdgen.
- If you know of a serialization solution that's not on this list, let me know and I'll take a look!

## Installation

- Go to the latest release of milk on [the github releases page](https://github.com/jaredly/milk/releases).
- Download & extract the zip corresponding to your platform
- put the `milk` binary somewhere on your path
- You're ready to go!

## An example: serializing a config file for a CLI tool

To demonstrate what it's like to use milk, let's make a little CLI tool -- and what classic unix tool could be more appropriate than `cowsay` for this job 🐮!

In our example, `cowsay` is going to read configuration from a local file to influence the output.

Here are the types for our config that we want to read from the file:

```
// src/Config.re
type color = Red | Blue | Green;

type language = Spanish | English | German;

type config = {
  color,
  languages: array(language),
  defaultGreeting: option(string),
}
```

### Initial setup

To generate serialization & deserialization code for this config, we first need to make a `types.json` file, and `milk --init` will create the following default `types.json` for you:

```
{
  // This is the version of your types. Increment this when you make
  // a change to the types
  "version": 1,
  // Here you specify what "engines" you want to generate code for.
  // Supported engines are:
  // - Js.Json (bucklescript)
  // - ezjsonm
  // - yojson
  // - rex-json
  "engines": {
    "Js.Json": {
      // This is where the serialization/deserialization (serde) code will be written.
      // If the extension is `.re` it will generate reason code, if it's
      // `ml` it will generate OCaml syntax.
      "output": "src/TypeSerde.re"
    }
  },
  // For each entry, milk will generate a `serializeX` and `deserializeX`
  "entries": [
    {
      "file": "src/Types.re",
      "type": "config"
    }
  ],
  "custom": [],
  // This config file format is also versioned! So this just specifies
  // which version of the milk config format specification we're using.
  "milkSchemaVersion": 1
}
```

The default entry that is created assumes the existence of a `src/Types.re`, so let's change that to `src/Config.re`. Let's also change the engine from `Js.Json` (which is for bucklescript targets) to `ezjsonm`, a native-side json library.

```
{
  "version": 1,
  "engines": {
    "ezjsonm": {
      "output": "src/TypeSerde.re"
    }
  },
  "entries": [
    {
      "file": "src/Config.re",
      "type": "config"
    }
  ],
  "custom": [],
  "milkSchemaVersion": 1
}
```

Now we can run `milk`, and it will generate `src/TypeSerde.re`, which exports `serializeConfig: config => Ezjsonm.t` and `deserializeConfig: Ezjsonm.t => result(config, list(string))`.

> milk also generates a `types.lock.json`, which you should check into your git repo as well. This file is the way milk keeps track of the type definitions of all previous versions, in order to be able to generate deserializers & migrators for them.

And now we're all set to load that config file in our main executable module ([full source here](https://github.com/jaredly/cowsay/blob/0f5204c9fe6aa1216f82546e483668281f910766/src/Cowsay.re)):

```re
// src/Cowsay.re

let main = () => {
  let data = Stdio.In_channel.read_all("./config.json") |> Ezjsonm.from_string;
  let config = TypeSerde.deserializeConfig(data);
  switch config {
    | Ok(config) =>
      // We now have the loaded config!
      let color = switch config.color {
```

Once we make a `config.json` that contains `{"color": "red", "languages": ["English", "German"], "$schemaVersion": 1}`, we can run `esy dune exec cowsay` and we're off to the races:

> `$schemaVersion` corresponds to the `version:` field of our `types.json`

![screenshot](/images/cowsay-1.png)

That might be a cool enough trick on its own, but the real magic happens when you want to make a change to the types.

### Automatic migrations

After releasing our new & improved cowsay into the world, we decide that we want to be able to specify a different color per language output. The new config type looks like this:

```diff
  type config = {
-   color,
-   languages: array(language),
+   languages: array((language, color)),
    defaultGreeting: option(string),
  }
```

Now that we've made a change to a type that's managed by `milk`, our app will fail to compile -- `TypeSerde.deserializeConfig` is producing the old config type! If we run `milk` again, we get the following error:

`Types do not match lockfile! You must increment the version number in your types.json`

So we increment the `"version"` field in the lockfile, run `milk` again, and now we have a new error!

`Must provide migrator. Cannot migrate automatically: config`

Milk doesn't know how to turn the old config into the new config, and so we need to provide a custom migrator, as a decorator on the type declaration. Here's what this looks like:

```re
[@migrate oldConfig => {
  languages: oldConfig.languages |> List.map(lang => (lang, oldConfig.color)),
  defaultGreeting: oldConfig.defaultGreeting,
}]
type config = {
  languages: array((language, color)),
  defaultGreeting: option(string),
}
```

And in this case, the only field that can't be automatically migrated is `languages` -- so we can simplify it to this:

```re
// `migrate.languages` indicates that we're just providing a migrator for that field.
[@migrate.languages oldConfig =>
  oldConfig.languages |> List.map(lang => (lang, oldConfig.color))]
type config = {
  languages: array((language, color)),
  defaultGreeting: option(string),
}
```

The `color` field can be automatically migrated because it's been removed, and the `defaultGreeting` hasn't changed at all.

Now we can run `milk` and it succeeds!

And once we convert the `Cowsay.re` to use the new config file format, we can run `esy dune exec cowsay`, and it produces the same output as before, **without us modifying config.json**.

Milk saw that `config.json` had `"$schemaVersion": 1`, deserialized it with the previous `config` type, migrated the data, and then gave us the config file in the new format!

> If/when you go to serialize, it will produce only the latest format -- there is no support for reverse-migration.

### A second migration

But what happens when you change the types again? `Milk` saves the migration functions into the `types.lock.json`, so you only ever specify the most recent migration information for a given type. For example, if we want to add a field to config, and remove a color from `color`, here's what that looks like ([full source here](https://github.com/jaredly/cowsay/blob/b2408fda38f286c84031fc2bb924bc9000adca46/src/Config.re)):

```re
// Any value that was Blue becomes Green
[@migrate.Blue (Blue) => Green]
type color = Red | Green;

// Previously the only direction the cow could face was left,
// so that makes sense as the migration default.
[@migrate.cowDirection (_) => `Left]
type config = {
  languages: array((language, color)),
  defaultGreeting: option(string),
  cowDirection: `Left | `Right,
}
```

> If `Blue` had an argument, the migrator function would include that in the function argument, e.g. `[@migrate.Blue (Blue(arg)) => ...]`

And now the `config` that we load from config.json will get migrated twice -- once to move color from a toplevel property into the languages array, and once to convert the `Blue`s into `Green`s and add the `cowDirection`.

![screenshot of cowsay with green text](/images/cowsay-2.png)

At this point, I recommend you take a look at the generated serializers, deserializers, and migrator code in [TypeSerde.re](https://github.com/jaredly/cowsay/blob/b2408fda38f286c84031fc2bb924bc9000adca46/src/TypeSerde.re). The great thing about this approach is there's much less magic -- you can manually inspect the result and ensure it matches your expectations.

## And that's it!

There's a lot more complexity we could go into, but hopefully this gave you a taste of what milk is capable of.

If you want a deep dive of how milk does what it does, check out [the Readme](https://github.com/jaredly/milk/blob/master/Readme.md).

I'll be publishing another post soon about using Milk to make an isomorphic REST application w/ a bucklescript frontend and a native backend, so stay tuned!