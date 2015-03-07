title: "visualizing reactive streams: hot and cold"
date: 2015-03-06 16:21:18
tags:
---
Rx seems to be a pretty big thing lately. It's done [great things] for nexflix, and [microsoft research] can't stop talking about it. Unfortunately, there's a sizeable learning curve involved, while you try and get your head to think in streams instead of imperative sequential processes.

In order to greatly ease the mental burden involved, I've created a tool that allows you to **visualize the streams** in real time, removing the guesswork.

![this is what it looks like](/images/pasted-9.png)

## Some code for demo
/the setup/

My first real experience playing with reactive streams was by means of [@staltz](https://github.com/staltz)'s excellent [tutorial](https://gist.github.com/staltz/868e7e9bc2a7b8c1f754), in which you build a simple "Who to follow" box, similar to the one on twitter. 

![The who to follow box](/images/pasted-2.png)

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
> you can follow along in the [`rxvision` playground](http://jaredly.github.io/rxvision/examples/playground/)

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

So how does this play out in a somewhat less trivial example?

## Cold Multiplication

This is a screenshot of [rxvision] running on the original demo code, under the following user actions:

- load the page
- click `x` next to the first two people
- click `refresh`
- click `x` next to the third person
- click `refresh again`

Each light gray block represents an "async group" -- e.g., all of the events happened within a single tick of the js event loop.

![Original code - lots of duplicate events](/images/pasted-7.png)

Deduping the ajax requests
![Ajax deduped, but click handler still going crazy](/images/pasted-6.png)

This is the ideal
![Order has been restored](/images/pasted-16.png)