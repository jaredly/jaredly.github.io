title: Make a cross-platform mobile game in Reason/OCaml
featured: true
tags:
  - reason
  - ocaml
  - game
categories:
  - reason
date: 2018-1-6 18:29:00
description: Target web, android, ios, and desktop!
---

I launched the first cross-platform mobile native game written in Reason a few weeks ago 🎉, to the [Android](https://play.google.com/store/apps/details?id=com.jaredforsyth.gravitron) and [iOS app stores](https://itunes.apple.com/us/app/gravitron-master-gravity/id1330043938), with a free [web version](https://gravitron.jaredforsyth.com) and a [macos desktop bundle](https://github.com/jaredly/gravitron/releases/tag/1.0.0) (source code [all on github](https://github.com/jaredly/gravitron)).

Getting there required lots of fiddling with cross-compilers and build systems, and so I made a tool that will take away much of the pain involved, so you can get started immediately and get your game into the world.

[![yay I made a thing](/images/gravitron-banner.png)](https://github.com/jaredly/gravitron)

<!-- more -->

> Platform note: I've only managed to set things up completely for macos so far -- I've got prebuilt cross-compilers for linux, but some of the other puzzle pieces haven't quite fallen into place. If you want to dive in, drop me a line! And windows... is farther behind :(. The "compile to native" story for the reason ecosystem is a bit in flux right now, so the particulars of how we install & configure cross-compilers will likely change in the next year or so.



The tool is called [reprocessing-scripts](https://github.com/jaredly/reprocessing-scripts), but the best way for you to get started is with [an example game](https://github.com/jaredly/reprocessing-example-cross-platform) I made for this post.

## Setup

You'll need ocaml compilers that are capable of building to android & ios targets. Normally this would take like 45 minutes to set up, but I've prebuilt some compilers for you.

Clone `https://github.com/jaredly/ocaml-cross-mobile` into `~/.ocaml-cross-mobile`, and run `./download.sh` to download the appropriate cross-compilers. You'll also need to grab the android ndk (see [the readme](https://github.com/jaredly/ocaml-cross-mobile)).

## The example project

Next, clone the example game from https://github.com/jaredly/reprocessing-example-cross-platform and put it somewhere.

Install the dependencies with `npm install` (I'm on npm 5.6.0, node 8.4.0). This will take a minute, but probably not more than 4 or 5.

## Build & Run!

`npm start` will compile the app to native desktop and start it up -- with hot-reloading! You'll see a window pop up with the game, ready to try out.

If you go into `src/Game.re` and edit something small (like a color or position), you should see it hot reload!

If the game crashes for some reason (as can happen if you change the type of `state`), the script will restart it for you.

![screenshot of the game running on desktop](/images/desktop-caml.png)

## Android target

`npm run android:run` will build the shared library, build an apk, and install it on whatever device/emulator you have connected.

> OCaml currently compiles with "text relocations", which recent android versions are unhappy about. This means that debug apks have a little popup complaining about it. Fortunately, release apks (such as you send to the play store) do **not** have the popup, so it still works.

![screenshot of the game running in the android emulator](/images/android-caml.png)

## iOS target

`npm run ios` will build the app into a static library that Xcode can then use in an iOS app, targeting the simulator or a device.

Once you've built it, you have to open up `ios/App.xcodeproj` in Xcode to build & run in a simulator or on a device.

![screenshot of the game running in the ios simulator](/images/ios-caml.png)

## Web target

`npm run serve` will compile your app to js, run webpack on it, and start a static server so you can view it in a browser. There's no hot-reloading on the web yet, but it will recompile on file change, so when you refresh the page you get the latest code.

![screenshot of the game running on the web](/images/web-caml.png)

## Profit!

And that's all you need to get your game running on 4 platforms! Now you just have to build it 😄

For reference, here are some games that have been written with reprocessing:

- https://github.com/jaredly/gravitron/
- https://github.com/bsansouci/reprocessing-example/tree/livestream-flappybird
- https://github.com/bsansouci/reprocessing-example/tree/2048
- https://github.com/bsansouci/ludum-dare-40
- https://github.com/illbexyz/repong

As always, if you have questions (or just want to chat) come join our [Discord channel](https://discord.gg/reasonml) or @ me [on twitter](https://twitter.com/jaredforsyth).