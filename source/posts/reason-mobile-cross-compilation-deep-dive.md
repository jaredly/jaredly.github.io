title: "Reason mobile cross-compilation deep dive"
tags:
  - reason
  - ocaml
  - ios
  - android
date: 2018-1-23
description: I figure it's good to document what I went through for future ocaml cross-compilation spelunkers.
---

I recently released [a tool that will manage all of this stuff](https://github.com/jaredly/reprocessing-scripts) (see the [accompanying blog post](https://jaredforsyth.com/posts/making-a-cross-platform-mobile-game-in-reason-ocaml/)), but I figure it's good to document what I went through for future ocaml cross-compilation spelunkers.

<!-- more -->

## Getting the cross compilers

To make the cross compilers, I piggy-backed off of the work done over in the [opam-cross-android](https://github.com/ocaml-cross/opam-cross-android) and [opam-cross-ios](https://github.com/ocaml-cross/opam-cross-ios) projects. You can download the prebuilt compilers [here](https://github.com/jaredly/ocaml-cross-mobile/releases/tag/2.0.0), or go the opam route.

## Relocation

One thing that stymied me for a bit is that the OCaml compiler contains a lot of hard-coded absolute paths, which makes it difficult to e.g. distribute a prebuilt compiler to people.

If you build the compiler, and then move the directory to a different one, for example, everything dies, because it's looking for various files at absolute paths, which is not where they are anymore.

This is even more of a problem with the android cross-compiler, because it hardcodes the paths to all of the ndk utilities.

So I had to patch the compiler to allow these hardcoded paths to be overridden at runtime via env variables.

Fortunately, all of the hardcoded things were in the same place, in `utils/config.mlp`, and so the patch was quite localized (you can look at it [here](https://github.com/jaredly/ocaml-cross-mobile/blob/d737fe0cf56934d65aa43b90996df8a08624d0bd/building/config_cross_402.patch)).

The compilers that I built were all based off `4.02.3` for compatability with the reason toolchain, but all of this applies to the latest OCaml version as well.

## A note about bytecode compilation

OCaml has two compilation modes built-in -- bytecode and assembly. The first is akin to Java or Python bytecode, which then gets run by the `ocamlrun` interpreter. This mode has much faster compile times, allows for dynamic loading of runtime plugins (via the `Dynlink` module), *and* produces bytecode that is cross-architecture compatible, theoretically removing the need for cross-compilation (I wasn't able to get this last part quite working). The assembly compiler takes longer and is less flexible, but produces much faster binaries as you would guess.

Bytecode compilation is the way that I got hot-reloading on android to work, and so I tried to get it going on the iOS side too, but I was unsuccessful. Something about bytecode compilation, together with trying to produce a statically-linked library, resulted in linking errors that I couldn't surmount.

If any of you know more about this, I'd love your help! Hot-reloading on iOS would be awesome.

## OCaml <-> C ffi

Now if we want our OCaml code to do anything useful, we're going to need to between OCaml and C. The documentation on this is [quite detailed](https://caml.inria.fr/pub/docs/manual-ocaml/intfc.html), and well worth a read if you're going to really get into this.

To call from OCaml into C, we'll need to define an `external some_name: string -> int = "some_c_name"` on the OCaml side, and make the corresponding function `CAMLprim int some_c_name(value string) { ... }` on the C side.

To call from C to OCaml, we use `Callback.register("some_name", someFn)` on the OCaml side, and call it with e.g. `caml_callback(*caml_named_value("some_name"), Val_int(42))`.

This isn't meant to be a tutorial on the C ffi, but [here's one](https://www.linux-nantes.org/~fmonnier/OCaml/ocaml-wrapping-c.html) if you want to know more.

## Building into an iOS app

We first need to initialize the OCaml runtime with `caml_main(argv)`, which I do [right before starting up](https://github.com/jaredly/reprocessing-example-cross-platform/blob/a9f4b2cd4b9c76445755e7e3ae31b7789157226b/ios/App/main.m#L16) the application.

Then at some point we'll want to call an OCaml function. In my case, I have a `reasonglMain` function that I call [in the `viewDidLoad`](https://github.com/jaredly/reasongl-ios/blob/626e82a79eb4ab66f71f6f7a94f633d3f288b2b5/ios/Reprocessing.m#L21) of my main `ViewController`. This function does a `caml_callback` to call a `main` function that I've registered on the OCaml side. In that ObjC file I declare `reasonglMain` as an external, indicating that it will be provided by the static library that I'll be building via OCaml and adding to the Xcode project.

To build that, it's basically a call to `ocamlopt -static -output-obj` with a bunch more flags [that you can check out here](https://github.com/jaredly/reprocessing-scripts/blob/799a610bb8439275155d44b5a51e9b53761787fd/src/Builder.re#L180), and you'll get a nice `.a` static library for a single architecture. I built that library for both `x86_64` (simulator) and `arm64` (device), and used [`lipo`](https://github.com/jaredly/reprocessing-scripts/blob/799a610bb8439275155d44b5a51e9b53761787fd/src/IOS.re#L87) to create a "fat library" that Xcode could use to build for either target.

Once you have that `.a`, you drag it into Xcode, and it should compile & run (I spent a long time slogging through it not working, figuring out the write flags to pass to ocamlopt 😓).

## Building into an Android app

Because android starts out in Java land, there's a further step of `C <-> Java` which took a bit of figuring out. Basically you define a C function like `com_namespace_of_TheJavaClass_someFunction(JNIEnv* env, jobject obj, jint thing)` and then in your `com/namespace/of/TheJavaClass.java` file you declare `public native void someFunction(int thing);`. [Here](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/src/CforOCaml.c) [are](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/src/CforJava.c) the [files](https://github.com/jaredly/reasongl-android/blob/master/android/src/main/java/com/jaredforsyth/reasongl/OCamlBindings.java) to look at if you want to know more.

Similar to the ios app, I jave a `reasonglMain` C-side function that [I call from Java](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/android/src/main/java/com/jaredforsyth/reasongl/ReasonGLView.java#L83) when the view is first loaded.

We also need to call `caml_startup` before doing any interfacing with OCaml, and I ended up just defining a [c constructor function](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/src/CforOCaml.c#L20) to get that out of the way.

Android external libraries are shared `.so` libraries, not static `.a`. So that means [a different invocation of `ocamlopt`](https://github.com/jaredly/reprocessing-scripts/blob/799a610bb8439275155d44b5a51e9b53761787fd/src/Builder.re#L151) (or `ocamlc` for bytecode compilation). The resulting `.so` we put in the `jniLibs` folder of our android app, for example `./app/src/main/jniLibs/armv7/libmything.so`. Then on the java side we need to indicate that we want to load this shared library [with a `System.loadLibrary("mything")` call](https://github.com/jaredly/reasongl-android/blob/64b32773b2960ccabc53abbc9845521b4c7d7ca2/android/src/main/java/com/jaredforsyth/reasongl/OCamlBindings.java#L19).

And then you're all set! If you build with `ocamlopt`, then android will get mad at you for having a shared library that [contains text relocations](https://github.com/ocaml-cross/opam-cross-android/issues/7#issuecomment-349545193), so you'll need to use an old version of the android sdk (I [use 21](https://github.com/jaredly/reprocessing-example-cross-platform/blob/master/android/app/build.gradle#L23)). In debug mode, this shows an annoying alert, but that alert doesn't appear in release mode, so it works 🤷‍.

## That's it!

Feel free to reach out to me [on twitter](https://twitter.com/jaredforsyth) or in our [discord channel](https://discord.gg/reasonml) if you hit any snags while trying this yourself -- I've probably run into them too 😅.