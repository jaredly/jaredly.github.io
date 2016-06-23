title: React Native Codegen
date: 2016-03-04 15:25:00
tags:
---
I've been thinking about writing a blog post about this react-native codegen stuff I've been doing.
But here's the thing: I don't know what to write.
How would I organize this?
- start by talking about our App, and KA
- maybe a little intro to react native, and how it works?
- oh I don't know.
- basically I'll type words to avoid losing my progress.
- things here are great.


- talk about the problem: I did an exploration into integrating RN into our app before, but it turned out that there was a lot of friction at the boundary -- marshaling data across the border ended up being a real pain, and we especially wanted everything to be type-checked and super safe
- so codegen!

What does it look like? (yeah maybe have this first)
- show some flow types
- also show the react native root component
- show a swift / java use case (like they can toggle between showing which code)
- and then show the generated code maybe? or not. That would probably be enough code to be getting on with
> talk a little bit about what type of data is supported, e.g.

- string, number, boolean (TODO get floats going)
- structs/autovalues
- functions that contain structs, base types, or functions =-> but no return values allowed.

How does it work?
- grab the flow ast! and parse it into a tree of types
- concat strings together! yay

hmmm would that be interesting/useful? idk.
definitely end w/ a gratuitous demo of hot reloading or something. I could even link to someone else's video of RN hot reloading.