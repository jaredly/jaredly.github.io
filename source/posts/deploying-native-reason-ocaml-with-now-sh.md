title: "Deploying Native Reason/OCaml with Zeit's now.sh"
tags:
  - reason
  - ocaml
  - native
  - server
date: 2018-9-7
description: Getting a native OCaml server in the cloud has never been easier.
---

I'm working on a new ReasonReact project that will need a stateful server, and I thought it might be nice to write that in Reason too :) but what's the best way to deploy it? And by "best" I mean "easiest, with a free-to-try option". After looking around a little, I settled on Zeit's [now.sh](http://now.sh).

What did I end up with? **A 10mb docker image** with a server inside!

<!-- more -->

For the server code, I'm just taking cohttp's [example server](https://github.com/mirage/ocaml-cohttp/blob/master/examples/doc/server_lwt.ml). The full source for this example is [here](https://github.com/jaredly/reason-docker-server).

## Making the Docker image

```javascript
## This is a two-stage docker file
# This first stage has opam & a ton of other things
# inside it. The full image is 1.4Gigs! Which is way
# too big to keep around.
FROM ocaml/opam:alpine as base

RUN sudo apk update
RUN sudo apk add m4
RUN sh -c "cd ~/opam-repository && git pull -q"
RUN opam update
# We'll need these two whatever we're building
RUN opam install dune reason > /dev/null 2>&1

# need these two for building tls, which is needed by cohttp
RUN opam depext conf-gmp.1
RUN opam depext conf-perl.1
RUN opam install tls > /dev/null 2>&1
# these are the dependencies for our server
RUN opam install lwt cohttp cohttp-lwt-unix > /dev/null 2>&1

# Now we copy in the source code which is in the current
# directory, and build it with dune
COPY --chown=opam:nogroup . /hello-reason
WORKDIR /hello-reason
RUN sh -c 'eval `opam config env` dune build bin/Server.exe'

## Here's the second, *much* leaner, stage
# It only contains the server binary! The reason we can do this
# is we statically linked the binary (with -ccopt -static)
FROM scratch
COPY --from=base /hello-reason/_build/default/bin/Server.exe /server
CMD ["/server"]
```

`docker build -t hello .` will take a minute, but at the end we get our 10mb image!

```javascript
REPOSITORY    TAG      IMAGE ID       CREATED       SIZE
hello         latest   15ebe8ca2a7d   8 hours ago   9.85MB
```

Now run the docker image locally to make sure it's working

```javascript
docker run -d -p 4321:8000 hello
```

If you open up `http://localhost:4321`, You should see something like this:

```javascript
Hello folks! This site has been visited 1 times

Uri: //0.0.0.0:32780/
Method: GET
Headers
Headers: accept: text/html
accept-encoding: gzip, deflate
connection: keep-alive
host: 0.0.0.0:32780
upgrade-insecure-requests: 1

Body: 
```

## Deploying with Now

This part is so easy it's almost not even worth mentioning. Once you've [installed `now`](https://zeit.co/download), you go to the directory with the `Dockerfile` and run `now` and it all just works! The first time you run it, the deploy will take a while because it's building the docker image for the first time. Subsequent builds will use the image cache, however, and will be much faster.

## That's all, folks!

With OCaml native deployment this easy, there's no excuse not to do it! Now all we need are some super-easy-to-use server libraries... 🤔 cohttp and httpaf are quite powerful, but are a far cry from express.js or rust's [rocket](https://rocket.rs/).
