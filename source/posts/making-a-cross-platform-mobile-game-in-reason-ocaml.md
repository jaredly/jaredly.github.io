title: Making a cross-platform mobile game in Reason/OCaml
featured: true
tags:
  - reason
  - game
  - ios
  - android
date: 2018-1-13 13:08:00
description: Targets web, native android, ios, and desktop!
---

I launched the first cross-platform mobile native game written in Reason a few weeks ago 🎉, to the [Android](https://play.google.com/store/apps/details?id=com.jaredforsyth.gravitron) and [iOS app stores](https://itunes.apple.com/us/app/gravitron-master-gravity/id1330043938), with a free [web version](https://gravitron.jaredforsyth.com) and a [macos desktop bundle](https://github.com/jaredly/gravitron/releases/tag/1.0.0) (source code [all on github](https://github.com/jaredly/gravitron)).

Getting there required lots of fiddling with cross-compilers and build systems, and so I made a tool that will take away much of the pain involved, so you can get started immediately and get your game into the world. **NB: This is still super experimental** but I'm really excited about it and want to get y'all in on the fun.

<!-- more -->

[![yay I made a thing](/images/gravitron-banner.png)](https://github.com/jaredly/gravitron)

> Platform note: I've only managed to set things up completely for macos so far -- I've started on prebuilt cross-compilers for linux, but some of the other puzzle pieces haven't quite fallen into place. If you want to dive in, drop me a line! And windows... is farther behind :(. The "compile to native" story for the reason ecosystem is in flux right now, so the particulars of how we install & configure cross-compilers will likely change in the next year or so.



The tool is called [reprocessing-scripts](https://github.com/jaredly/reprocessing-scripts), but the best way for you to get started is with [an example game](https://github.com/jaredly/reprocessing-example-cross-platform) I made for this post. The amazing cross-platform graphics library we're using is called [reprocessing](https://github.com/Schmavery/reprocessing), made by [bsansouci](https://twitter.com/bsansouci/) and [schmavery](https://twitter.com/_schmavery).

## Setup

You'll need ocaml compilers that are capable of building to android & ios targets. Normally this would take like 45 minutes to set up, but I've prebuilt some compilers for you.

Clone `https://github.com/jaredly/ocaml-cross-mobile` into `~/.ocaml-cross-mobile`, and run `./download.sh` to download the appropriate cross-compilers. You'll also need to grab the android ndk if you want to build for android, with `./get-ndk.sh`.

## The example project

Next, clone the example game from https://github.com/jaredly/reprocessing-example-cross-platform and put it somewhere.

Install the dependencies with `npm install` (I'm on npm 5.6.0, node 8.4.0). This will take a minute, but probably not more than 4 or 5.

## Build & Run!

`npm start` will compile the app to native desktop and start it up -- **with hot-reloading!** You'll see a window pop up with the game, ready to try out.

If you go into `src/Game.re` and edit something small (like a color or position), you should see it hot reload!

If the game crashes for some reason (as can happen if you change the type of `state`), the script will restart it for you.

![screenshot of the game running on desktop](/images/desktop-caml.png)

## Web target

`npm run serve` will compile your app to js, run [pack.re](https://github.com/jaredly/pack.re) on it, and serve it up **with hot reloading**.

![screenshot of the game running on the web](/images/web-caml.png)

## Android target

`npm run android:run` will build the shared library, build an apk, and install it on whatever device/emulator you have connected.

> OCaml currently compiles with "text relocations", which recent android versions are unhappy about. This means that debug apks have a little popup complaining about it. Fortunately, release apks (such as you send to the play store) do **not** have the popup, so it still works.

![screenshot of the game running in the android emulator](/images/android-caml.png)

🎁🎁 **Bonus:** 🎁🎁 android hot reloading!! `npm run android:hot` will compile, install on your emulator/device, and proceed to hot-reload as you make changes to the code 😍. This requires that your phone be on the same wifi network as your computer.

## iOS target

`npm run ios` will build the app into a static library that Xcode can then use in an iOS app, targeting the simulator or a device. `npm run ios:sim` will build & open a simulator, and `npm run ios:device` will build & run on your usb-connected device.

> If you're excited about getting hot reloading going on iOS, talk to me! I've gone a fair way down that road but got stuck, and I'd love some help!

![screenshot of the game running in the ios simulator](/images/ios-caml.png)

## Native Mac Bundle

`npm run bundle` does an optimised compilation for native desktop, and generates a `.app` bundle for you.

![screenshot of the app bundle](/images/caml-app.png)

## Profit!

And that's all you need to get your game running on 4 platforms! Now you just have to build it 😄

For reference, here are some games that have been written with reprocessing:

- https://github.com/jaredly/gravitron/
- https://github.com/bsansouci/reprocessing-example/tree/livestream-flappybird
- https://github.com/bsansouci/reprocessing-example/tree/2048
- https://github.com/bsansouci/ludum-dare-40
- https://github.com/illbexyz/repong

As always, if you have questions (or just want to chat) come join our [Discord channel](https://discord.gg/reasonml) or @ me [on twitter](https://twitter.com/jaredforsyth).
