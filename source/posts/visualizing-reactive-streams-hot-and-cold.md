title: "Visualizing Reactive Streams: Hot and Cold Observables"
tags:
  - javascript
  - rxjs
  - react
categories:
  - Tech
  - Programming
date: 2015-03-06 16:21:00
---
Reactive Programming is getting a lot of attention these days, and it promises to reduce frustration, bugs, and greenhouse gas emissions. Unfortunately, there's a sizeable learning curve involved while you try and get your head to think in streams instead of imperative sequential processes.

<!-- more -->

> In order to greatly ease the mental burden involved, I've created a tool that allows you to **visualize the streams** in real time, removing the guesswork. It's called [RxVision](https://jaredly.github.io/rxvision) and you should check it out.

One of the things that caught me by suprise is the difference between `hot` and `cold` observables, and how they interact with multiple observers.

**In this article, I will:**

- [give some background & describe the cold-induced bug](#Learning_Rx_and_Duplicate_Ajax_Calls)
- [give & explain a trivial example demonstrating this bug](#Stone_cold)
- [explain the bug in the larger example](#Cold_Multiplication)
- [show how to fix the bug in the larger example](#Cleaning_up_with_a_little_heat)

All with lots of visuals. Enjoy!

## Learning Rx and Duplicate Ajax Calls

My first real experience playing with reactive streams was by means of [@staltz](https://github.com/staltz)'s excellent [tutorial](https://gist.github.com/staltz/868e7e9bc2a7b8c1f754), in which you build a simple "Who to follow" box, similar to the one on twitter.

![The who to follow box](/images/pasted-17.png)

Just to make sure I really understood what was going on, I [rewrote the demo in clojurescript](https://gist.github.com/jaredly/fee1bd6346ea95144d27).

Essentially, what the box does is get the `users.json` list from github (at a random offset), and randomly pick 3 of the 100 in that list to display to you. Clicking `x` next to one of the users replaces that line with a new user (drawn from the 100). Clicking `refresh` triggers a request to the `users.json` api endpoint again, this time with a different offset.

While debugging my clojurescript version, I saw in devtools that when refresh was clicked, there were **3 ajax requests instead of one**. Confused, I searched through the article, and eventually found in the comments section my answer: the issue was `hot` vs `cold` observables.

## Stone cold
Cold observables essentially replicate themselves for each new observer - and this works **retroactively** up the chain of observables.

For a somewhat trivial example:
```js
let btn = $('button')[0]
let clicks = Rx.Observable.fromEvent(btn, 'click')
clicks.subscribe(value => console.log('clicked!'))

let values = clicks.map(() => Math.floor(Math.random() * 10 + 2))
let less1 = values.map(value => value - 1)
let times2 = less1.map(value => value*2)

times2.subscribe(value => console.log('i got a value', value))
times2.subscribe(value => console.log('also subscribing', value))
values.subscribe(value => console.log('the original was', value))
```
> you can follow along in the [RxVision playground](http://jaredly.github.io/rxvision/examples/playground/)

You would expect that the two `times2` subscriptions would return the same number, right? *they don't*. Take a look at the flow of values here:

![one flow for each subscriber](/images/pasted-14.png)

The "click" event is duplicated *four times*, once for each subscriber. The first `map`, which generates a random number, therefore generates **3 different numbers**, one for each subscriber down the chain.

To fix that obvious bug, we have to make the random mapper `hot`, by adding `.share()` at the end. Line 5 now looks like:

```js
let values = clicks.map(() => Math.floor(Math.random() * 10 + 2)).share()
```

This makes our `console.log`s give the right values, but the flow diagram still shows some duplication:

![still some duplication.](/images/pasted-15.png)

To fully deduplicate, we need to add `.share()` to every observable that is observed more than once (in this case, line 2 and line 7).

![doesn't that look so much cleaner?](/images/pasted-18.png)

So how does this play out in a somewhat less trivial example?

## Cold Multiplication

To demonstrate the issue, I ran the original demo code under the following user actions:

- load the page
- click `x` next to the first two people
- click `refresh`
- click `x` next to the third person
- click `refresh again`

This is a screenshot of [RxVision](https://jaredly.github.io/rxvision) which visualizes the flow of values between streams.

> Here, too you can follow along on [the demo page I made](http://jaredly.github.io/rxvision/examples/gh-follow/). The code there represents the fully deduplicated version.

![Original code - lots of duplicate events](/images/pasted-7.png)

> Each light gray block represents an "async group" -- e.g., all of the events happened within a single tick of the js event loop.

There are lots of things going on here, so let's dissect it:

- those blue `create` streams each represent an individual Ajax request. Within the first tick, 3 requests get fired off. You can see the `startWith` observable that initiates this pushes out the same value 3 times -- this is definitely a `hot` vs `cold` problem.
- the refresh button click (the very top stream) fires off **6 times** when it is clicked once. Three of those times are to clear each UI list item, and then 3 other times for our duplicated ajax calls.

## Cleaning up with a little heat

As with the first example, the way to fix duplication is the `.share()` method of an observable. To stop the duplicate requesting, we just `.share()` the `responseStream`

```js
var responseStream = requestStream.flatMap(ajaxGet).share();
```

That was easy. Now it looks like this:

![Ajax deduped, but click handler still going crazy](/images/pasted-6.png)

Note that there are now only 3 ajax requests (the `create` streams), one for the initial and two more for the times we clicked `refresh`. However, the refresh button click handler is still duplicating, so we need to `share()` that too:

```js
var refreshClickStream = Rx.Observable.fromEvent(refreshButton, 'click').share();
```

The data flow chart now looks much cleaner, and duplication has been eliminated.
![Order has been restored](/images/pasted-16.png)

## What have we learned?

- It's easier to debug something you can look at.
- Whenever an observable is subscribed to more than once, make it hot with `.share()` to make all subscribers see the same thing.

Thanks for your time, and if you check out [RxVision](https://jaredly.github.io/rxvision), let me know what you think!
