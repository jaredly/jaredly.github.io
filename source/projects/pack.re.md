title: Pack.re
description: A reason-native javascript bundler 🏎
status: alpha
github: pack.re
tags: tool, javascript, reason
---
date: 2018-1-12

I did some benchmarking, compiling [gravitron](/projects/gravitron/), and pack.re was pretty fast 😄

- webpack (no pre/post-processing, simplest config) - 1.25s
- parcel (no cache, no minify) - 3.0s
- pack.re - 0.2s 🚀

That's 6-10x faster! (Note: webpack & parcel have tons more features, and pack.re probably still has some bugs)

Also of note:
- `time node -e 'console.log("hi")'` - 0.3s 🙃

https://twitter.com/jaredforsyth/status/951971685795876865

---
date: 2018-1-11

I was working on [reprocessing-scripts](/projects/reprocessing-scripts), and I didn't want to have to depend on some huuge javascript bundler w/ all of their dependencies (webpack's `node_modules` is 50megs, parcel's `node_modules` is 150megs), so I figured I'd try making a reason-native bundler.

I repurposed flow's [javascript parser](https://github.com/facebook/flow/tree/master/src/parser), inspired by [jeason](https://github.com/chenglou/jeason), and figured out the node resolution algorithm, and it wasn't too much work after that.

I also managed to get hot-reloading working! I added a `--external-everything` flag which creates a bundle that only contains the files in your project, and no external dependencies. It expects them to already have been loaded on the page by a previous `pack.re` bundle.

https://twitter.com/jaredforsyth/status/951857741320073216