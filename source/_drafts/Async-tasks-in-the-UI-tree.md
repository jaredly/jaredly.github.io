title: Async tasks in the UI tree
date: 2015-11-26 10:49:12
tags:
---
In the React world, lots of interesting work has been happening around managing server concerns in a clean, composable way. [Falcor], [Relay], [Om next].

All of these solutions have data needs directly tied into the UI tree -- which, most of the time, is where you want them.

The context of this discussion is in developing a iOS app in Swift, we'd love to have a similar framework to make data-fetching & other async tasks make more sense. I was talking to Andy Matushak about this, pointing to Relay as a good example of "solving this problem", and he expressed the following concern:

(something like) that's a great start, but there are also times when it doesn't make sense to have the data-fetching tied into the UI tree -- you want a separate hierarchy.
why?
if we want to preload things (this takes too long to load - can we optimistically load it before the user navigates?); or do image processing or other longish tasks *before* the component gets rendered into the UI tree. Basically, the component lifecycle is insufficient for managing our async processing needs.