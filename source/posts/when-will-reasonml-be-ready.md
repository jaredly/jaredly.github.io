title: When will ReasonML be ready?
featured: false
author: Jared Forsyth
tags:
  - reason
  - ocaml
categories:
  - reason
description: Reason is super exciting and also very new -- there's lots to be done before it's stable.
date: 2017-06-23 22:36:00
---

Someone came into our [discord channel](https://discord.gg/reasonml) a few days ago asking "Is Reason suitable for migrating a large production JavaScript codebase incrementally?" I answered "Yes, but wait 6 months."

What's the current status? At the moment, we're mostly in the realm of "enthusiasts who are OK with being on the bleeding edge, and want to help build out the foundation in their spare time."

<!-- more -->

> Btw if this sounds like you, we'll provide you with the support you need! Our [discord channel](https://discord.gg/reasonml) is very friendly and helpful 😀

I've gotten really excited about Reason recently because I think it has the potential to be the best language + ecosystem I've worked in, and that it has a real shot at gaining wide adoption. However, I'd like to set some expectations first to avoid potential disappointment/disillusionment as people come up against its rough edges (I know of one person who snuck some reason code into their codebase at work and their coworkers were none too happy when they found out).

![](/images/reason_stability.png)

The end goal, as [Jordan](https://twitter.com/jordwalke) puts it, is to be able to use this great language at our day jobs, instead of just on the weekends as a respite from our day jobs.

> It's also worth noting that Reason is more than just a couple of developers hacking around in their spare time. Facebook is using it to power substantial portions of messenger.com and other products -- they're definitely invested in its future.

## What needs work?

### Syntax

The Reason syntax is up for some [big changes](https://github.com/facebook/reason/pull/1299) in the near future. Fortunately, we have `refmt` which will be able to auto-upgrade all of your code to the new style. However, you won't want to train a large team on the current syntax only to relearn it in a few months.

### Build system

What we have now is a ton better than what we had 6 months ago, but it's still far from polished. I'm guessing that in 3-6 months we'll have most of the kinks ironed out.

### JavaScript ecosystem

There are hardly any Reason libraries out there, and what is there is super new and far from stable. We do have the benefit of being able to use all of `npm` though - you just have to write the type definitions and you're good to go.

### Native ecosystem

The OCaml community has some pretty fundamental fragmentations (there are three incompatible versions of the standard library, two incompatible ways to handle async), and documentation is generally pretty sparse. We're hoping to establish good "batteries included" standard library (that will work for both js and native), but it will take some time.

### Documentation

[Real World OCaml](https://realworldocaml.org/) is the best and nearly the only resource out there for getting started. The OCaml core documentation can be really hard to get into, and is really lacking in examples. There are a couple of efforts at creating some beginner-friendly documentation for Reason, but there's lots to do!

### IDE Integration

There are some really nice plugins to get type hinting, syntax highlighting, refactoring & more in VSCode, Atom, Vim, etc. Unfortunatly, they can be pretty finnicky to set up - lots more work needs to be done to make the install & setup process bullet proof.

### Community

The community is pretty small, but very enthusiastic! If you show up in our [discord channel](https://discord.gg/reasonml) with a question, you'll find lots of helpful people who are happy to answer it. [StackOverflow](https://stackoverflow.com/questions/tagged/reason) doesn't have very many Questions & Answers for you to browse through (there are 14 as of this writing), and googling an error you come across might not get you anywhere, but if you ask a question on SO you'll get a good answer -- especially if you ping us on discord 🙂.

## So what now?

Despite the rough edges, I find Reason really delightful to write compared to the languages I use at work (JavaScript/Flow, Java, ObjC, Swift), and it's even helped me think of solutions to problems I've faced at work.

If you're excited & have the free time, please jump in! And if you want to document your progress (write tutorials or documentation about what you're learning), all the better!
