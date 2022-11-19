title: "Hot-reloading OCaml on Web, Desktop, and Android"
tags:
  - reason
  - ocaml
  - android
date: 2018-1-23
description: Documenting how I got hot-reloading going when developing Gravitron.
---

## General program setup

If you hot reload one thing, you'll need to reload everything that depends on it -- otherwise you get type errors.

So if we picture our program as a DAG, you'll need to reload a whole chunk. In the case of [reprocessing](https://github.com/Schmavery/reprocessing), the chunk that you reload is "all of the user's modules", while the library modules don't get reloaded.

![diagram of the chunk to hot reload](/images/hotreload.png)

Another important thing is that there needs to be a "hooking in point", where the newly loaded modules can make themselves known, and this point has to exist in the "not hot reloaded " portion of things. For reprocessing, that point is [`Reprocessing.run()`](https://github.com/Schmavery/reprocessing/blob/master/src/Reprocessing.rei#L33), which is the main starting point of the game. When the user's code gets reloaded, it calls `Reprocessing.run` again, and Reprocessing notices that this wasn't the first time, so it preserves state and swaps in the new `draw` function.

## Native desktop

This one is the most straightforward.

You rebuild the `.cmo`s for each module that you want to reload using `ocamlc -c`, and combine them into a `.cma` with `ocamlc -a`. Note that the order they're put into the .cma is important! They must be in dependency order, which you can get by running `ocamldep`.

Notify the running process that it should hot reload (either by having the process periodially check the `mtime` of the `.cma`, or via a socket, or whatever), and then do a `Dynlink.load("./path/to/the.cma")`.

And that's the size of it!

## Native Android

Android requires some more work, because you have to get the `.cma` file to your device.

I set up a [basic TCP server](https://github.com/jaredly/reprocessing-scripts/blob/799a610bb8439275155d44b5a51e9b53761787fd/src/HotServer.re) that watches the files for a change, and on successful rebuild makes a `.cma` for any waiting clients.

The client is [written in Java](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/android/src/main/java/com/jaredforsyth/reasongl/MyAssetManager.java#L46) because I didn't want to mess with getting the `Unix` library working on Android. It attaches to the TCP socket, sends over information about what file it wants hot reloaded & the current architecture, and then waits. When the server sends over a new `.cma`, it writes it to a file & then notifies the ocaml side so [it can run](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/src/Reasongl_Hotreload.re#L10) `Dynlink.load`.

## Web

JavaScript is quite different.

Bucklescript doesn't have `dynlink` built in, but I [built my own JavaScript bundler](https://github.com/jaredly/pack.re), and I implemented a feature similar in spirit to browserify's "externals".

The first time through, I do a full build, which creates a global `packRequire` along with a mapping of filenames to package IDs. Each successive time, I bundle with the flag `external-everything`, which only bundles local files, and defers to the global `packRequire` for all non-relative requires (e.g. requires that would be searched for in `node_modules`).

I then have [a simple script](https://github.com/jaredly/reprocessing/blob/7bab292d5d04ef9876777ca0a21e89982e3091fb/src/web/Reprocessing_Hotreload_Web.re#L12) that tries to fetch and evaluate `/hot/bundle.js` in an recursive loop. The server, when it gets a request there, keeps the client open & waiting until there is a change, at which point it creates a new bundle and fulfills the request with it.

## That's it!

I've had some musings about making a library that will take care of the bones of hot reloading for you automatically... we'll see where that ends up.

Catch me [on twitter](https://twitter.com/jaredforsyth) or the [reasonml discord channel](https://discord.gg/reasonml).