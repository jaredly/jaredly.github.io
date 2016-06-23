title: Making a custom Clojure Parser
date: 2016-02-03 13:48:27
tags:
---
I recently ran across [Golem](http://shem.io/), and was blown away, and immediately wanted that kind of goodness for my everyday development :D. I've done a bit of research & experimentation with structural editing, but never got too far. Seeing Golem & how it was setup gave me the impetus I needed to dive into the problem again.

My current favorite language for new web projects is [ClojureScript](), which happily shares many characteristics with [Shem]() (the language paired with Golem), and so I decided to explore making a structural editor got ClojureScript.

But it's slower
- gclosure didn't make much difference
- something