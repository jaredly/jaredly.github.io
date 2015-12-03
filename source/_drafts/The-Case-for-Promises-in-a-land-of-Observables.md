title: The Case for Promises in a land of Observables
date: 2015-11-28 13:05:37
tags:
---


intro, getting into objc @ ka, etc. we use reactiveCocoa! Which is an observables framework, related to RxJava, RxJS, Bacon, Kefir, etc. I've had a bit of experience w/ RxJS, Bacon, Kefir.

One thing I've been missing, though, are Promises. "Why would you need promises?" you ask. "Observables are basically Promises++! Anything you can do with Promises, you can do with Observables, but better!"

And yet.

I find myself (and my coworkers) using Observables (the contract for which is `may send 0<=n<=infinity values followed by error|completed|interrupted`, in places where the implicit contract is `expects a value|error|interrupted`. For example, network requests.

Now, things are all fine & dandy & completely indistinguishable between promises and observables *as long as everything is pure*, but in the end you want to have some kind of effect, which often involves touching some stateful UIKit stuff.

And there are state machine invariants that you want to maintain, which often assume that you will never, for example, get multiple values out of your network request.
And so maybe you have an `assert` in the subscriber code, making sure you are in the proper state before transitioning, which, by the way, will only be the case if your observable is acting like a promise.
So why not encode that expectation in a type?

`Promise` could easily be a subtype of `Observable`, so going from `Observable` to `Promise` would be painless and no subscribers would really have to care. But going from `Promise` behavior to `Observable` behavior will probably violate some implicit invariants in your state machine, and wouldn't it be nice for the type system to tell you all the places where those contracts might be violated?

----
copied from the other draft

Recently I've been diving head-first into objc/swift/xcode land (sometimes it feels like drowning), and one big paradigm we use at Khan Academy in our mobile apps is `Observables`.

Currently, there's no way to communicate the implicit contract that "this thing will only emit once", even though in many cases the receiver does depend on that behavior.