title: Switching from Ghost to Hexo
date: 2014-07-26 23:45:18
tags:
---
My blog used to use Hyde, a python clone of the popular jekyll platform. When Ghost came out, I quickly switched over, due in main part to the great editor and beautiful themes. I used [buster] to serialize the blog so that I could still serve it as a static site. I had two main regrets, though. 1) using buster to scrape ghost was a hack. 2) My posts were locked in an sqlite db, where git could only do a binary diff.

Recently, I started looking around again for static blog generators, and found [Hexo](http://hexo.io), which looked reasonably capable of easily doing everything I wanted. I had my version-controlled markdown posts back, along with a non-hacky static site generation script.

