title: Switching from Ghost to Hexo
tags:
  - blog
categories:
  - Tech
article_image: images/ghost-to-hexo.png
date: 2014-07-26 15:45:00
---
My blog used to use [Hyde](http://hyde.github.io/), a python clone of the popular jekyll platform. When [Ghost](http://ghost.org) came out, I quickly switched over, due in main part to the great editor and beautiful themes. I used [buster](https://github.com/axitkhurana/buster) to serialize the blog so that I could still serve it as a static site. I had two main regrets, though. 1) using buster to scrape ghost was a hack. 2) My posts were locked in an sqlite db, where git could only do a binary diff.

<!-- more -->

So I started looking around again for static blog generators, and found [Hexo](http://hexo.io), which looked reasonably capable of doing everything I wanted. I now have my version-controlled markdown posts back and built-in static side generation.

Now I'm working on building an editor interface :)