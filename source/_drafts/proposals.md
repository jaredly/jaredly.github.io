title: Proposals
date: 2016-03-04 18:42:17
tags:
---

Methods for dealing with Asynchrony:
- callbacks (continuation passing style)
- CSP concurrent sequential processes
- Promises
- async/await
- Observables

Debugging React Apps:
- the react sandbox
  - if you have your pure leaf views, this works great!
  - potential problems
    - if you have a lot of state, this will be hard to test
    - if you use relay, then it can also be super annoying
- the react debugger
- architect for success
  - pure components
  - your view logic should be embarrassingly easy
  - if you have anything complex that you feel like you want to test, make a pure function for it
  - if you end up having lots of complex stuff, then rethink your design
  - use flow types! will cover a multitude of sins

