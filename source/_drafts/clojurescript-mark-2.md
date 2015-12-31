title: 'ClojureScript, mark 2'
date: 2015-12-09 11:07:45
tags:
---
Importing woes -- if you import in the wrong order, functions end up `nil`, and the first you hear of it is `can't call X as a function`. And the "things" that get clobbered aren't even always at the top level -- so some functions from a required namespace will be there, and others won't.

Circular imports crash and burn.

Error message: https://gist.github.com/jaredly/9365d1f3fe08a39ed134

Also, I really miss my debugger :// chrome devtools is next to useless

BUT! cljs-devtools **Really helps**

BUT awesome things:

- re-frame & reagent are super awesome. For my server-less SPA, it's exactly what I wanted, and the ergonomics is fantastic. om/next isn't quite as ergonomic, but I still plan to try and get something going.
- working in a data-first language is so great!
- having a repl in your editor is really cool.
- macros are super