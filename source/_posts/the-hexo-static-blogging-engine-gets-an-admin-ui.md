title: The Hexo static blogging engine gets an admin UI
tags:
  - javascript
  - react
  - announce
  - blog
categories:
  - Tech
  - Programming
date: 2014-07-26 23:21:00
fetured: true
---
I [recently switched](/2014/07/26/switching-from-ghost-to-hexo/) from [ghost](http://ghost.org) to [hexo](http://hexo.io), and the biggest thing missing for me was the editor interface. So I made one. Currently, it's mostly a clone of the Ghost interface, but I have some ideas for making it even more awesome.

<!-- more -->

Get the [source code](https://github.com/jaredly/hexo-admin-plugin), or `npm install --save hexo-admin-plugin` to get this goodness for your hexo blog. **Note:** There is no security, so don't use this plugin on a public-facing server. Access @ http://localhost:4000/admin/

#### Posts overview
![posts view](/images/pasted-0.png)
#### Editor view
![editor view](/images/pasted-1.png)

**Features:**
- side-by-side markdown editing
- auto-save posts
- paste to insert an image
- publish/unpublish

**Missing features (which you can still do just fine w/ the raw files):**
- edit categories & tags
- edit publish date/time
- edit slug

**Future awesome:**
- drag & drop to insert a picture or file
- autocomplete for linking to other posts
- git commit from the UI?
