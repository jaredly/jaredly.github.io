title: "// Commented: Comments done simple"
date: 2014-07-26 22:16:05
tags:
---
**// Commented** is inspired by Disqus and Medium, in an open source library that's beautiful and easy to use.

### What it does
- comments on the side or the bottom
- login with github, twitter, facebook, or google
- ❤ posts
- community moderation through flagging
- replies (two levels deep)

<!-- more -->

### How can I have this on my blog?

1. Create a [firebase app]
1. Create apps for facebook, twitter, github and/or google, and hook them into the firebase app
1. Include the required js and css, and the corresponding html snippet in your blog template

### How it's done
- ReactJS
- Firebase
- Browserify
- LessCSS
- FontAwesome

### Origin Story
Remember when Disqus came along and liberated us from the shackles of wordpress and drupal commenting systems, giving us unified login and beautiful comments for free?
Yeah, that was 5 years ago.
How has the world of commenting changed since then?
Actually, not a ton.

In my continuous quest to make my blog the best possible communication transmission system, I have been enviously eying the comment system on Medium, but I've been sceptical of its [content policy]. Several months ago, someone put together a demo using disqus to do side-comments, which was really cool, but felt a little clunky - you can't get around the Disqus UI, which has a lot of superfluous parts.
Then I saw the [side-comments] library two weeks ago, which was a really cool proof of concept, but it didn't provide a backend solution, and was missing a lot of features that I would like to have. So I built it myself.

### What remains to be done
- quote by selection (a la medium)
- other backends (hood.ie?)
- it would be super cool if someone wanted to make a business out of this, providing hosting centrally disqus-style