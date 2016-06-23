title: My Local Reverse Proxy Setup
date: 2015-12-02 14:18:51
tags:
---
I do a lot of local web development, and have a variety of pet projects that I'm running at any given time, each on different ports.
Mostly, I try to remember "now which port was that running on?", and it's super annoying.

**BREAKING NEWS**
I should use this instead: http://passingcuriosity.com/2013/dnsmasq-dev-osx/
`dnsmasq`, you can configure a particular tld (.dev for example) to be dns'd be something else. and it doesn't require root for modification!

<!-- more -->

Currently, I have:

- a notablemind instance
- a hexo instance, for blog editing
- a familyfound instance
- a stories.familyfound instance

And in the past I've change /etc/hosts to map `familyfound.local` to localhost, and that helped chrome autocomplete, but I still needed to use an explicit port (b/c port 80 is root only), so it would be `familyfound.local:4000`.

And so there's more management than I want.

But I found a great tutorial [here](https://gist.github.com/kujohn/7209628), and now 127.0.0.2:80 -> 127.0.0.1:{some high port}, which I can then listen to from a normal-user program.

And the program I'm using (for the moment) is `drunken-hipster`, which is a reverse-proxy golang program, with the following config:

```
[frontend main]
bind = 127.0.0.1:{the same port as above}
hosts = familyfound.local stories.local hexo.local

[host familyfound.local]
backends = familyfound

[host stories.local]
backends = stories

[host hexo.local]
backends = hexo

[backend familyfound]
connect = 127.0.0.1:4001

[backend stories]
connect = 127.0.0.1:7200

[backend hexo]
connect = 127.0.0.1:9003
```

And then in `/etc/hosts` I have a line:
```
127.0.0.2      familyfound.local stories.local hexo.local
```

So now I get my urls and there's not too much to change when I want to start up something new.

## BUT my dream interface would be:

Probably written in rust? Or maybe go, but I like rust much better... 
[A reverse proxy thing in rust](https://gist.github.com/infinityb/600c22ae549cecf43244)

Going to `proxy.local` gives you a dashboard, w/ the status of all your running programs.

And you could go ahead and run
```
npm start --port=`proxify mything.local`
```

And it would output the corresponding port for your app to listen on.

If `mything.local` wasn't setup yet, it would ask you to `sudo proxify -a mything.locla`, so that it can modify `/etc/hosts`, and it would pick a port for your new thing to use.

Basically, I want my own little local PAAS. Kindof. B/c it would also be cool, for example, for it to be able to manage some things completely. Now for apps that I'm actively developing, I want full control over the processes, but for things like my hexo blog, I don't need it running all the time, but it would be awesome if, when I go to `hexo.local`, `proxify` would start up the process for me, and kill it after some timeout of inactivity (maybe a day or more?).