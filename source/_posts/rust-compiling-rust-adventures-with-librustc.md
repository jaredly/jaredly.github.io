title: "Rust compiling rust: adventures with librustc"
tags:
  - rust
  - compiler
  - notablemind
categories: []
date: 2014-11-22 18:17:00
---
For [the web-based everything notebook](http://jaredly.github.io/itreed/) that I'm working on, I've been writing backend hookups to various programming REPLs, including [IPython](http://ipython.org) and [Gorilla](http://gorilla-repl.org). I wanted to be able to evaluate rust code as well in this notebook environment, and so I set about writing a simple server that would compile up a string in rust when asked. This proved do have a couple of gotchas, so I thought I'd air my thoughts here.

<!-- more -->

For the impatient, [here's the code](https://github.com/jaredly/itreed/blob/5d0b0fdc544a18aab0fdcf6990beea9c15dce8d4/lib/kernels/rust/compile_inline.rs). Just beware of the caveat at the bottom of this post.

## Using `librustc`

The current `librustc` API doesn't have a nice, simple "here's the string, compile it please" method, so that's what I wrote. Here's the basic signature:

```rust
pub fn compile_string(input: String, output: Path, sysroot: Option<Path>)

// example usage:
let src = "fn main() { println!("Hello!") }".to_string();
let output = Path::new("/tmp/hello".to_string());
let sysroot = Path::new("/usr/local/");
compile_string(src, output, Some(sysroot));
// now `/tmp/hello` is ready to go!
```
The `sysroot` from the above definition has to do with the location of the compiled rust libraries (given that there's not a fool-proof way of automatically knowing where these would be). If the libraries are located in `/usr/local/lib/rustlib`, then sysroot is `/usr/local/`. Here are the headers we'll need:

```rust
extern crate rustc;
extern crate rustc_trans;
extern crate syntax;

use rustc_trans::driver::driver::{FileInput, StrInput, Input, compile_input};
use rustc_trans::session::config::{basic_options, build_configuration, OutputTypeExe};
use rustc_trans::session::{build_session, Session};
use syntax::diagnostics;```

There are several crates involved here; `librustc` is currently in the process of being refactored, so the locations of some of these items might change.

The function that is exposed by `librustc` is:

```rust
compile_input(sess: Session, cfg: CrateConfig,
              input: &Input, outdir: &Option<Path>, 
              output: &Option<Path>, addl_plugins: Option<Plugins>)
```

This requires the creation of a basic session and accompanying config, in addition to the input and the output that we provide.


```rust
/* Build a basic session object to output a compiled binary. */
fn basic_sess(sysroot: Path) -> Session {
    let mut opts = basic_options();
    // the only modifications that need to be made to the session options are
    // output_types (otherwise no binary is created),
    // and maybe_sysroot (otherwise the rust libs cannot be found)
    opts.output_types = vec![OutputTypeExe];
    opts.maybe_sysroot = Some(sysroot);

	// copied from librustc
    let descriptions = diagnostics::registry::Registry::new(&rustc::DIAGNOSTICS);
    let sess = build_session(opts, None, descriptions);
    sess
}```

And now the final pieces; a few helper functions to expose a simple interface:

```rust
/*
 * Compile a string to rust! Takes an input string of source code and an output
 * path to write to, and optionally a sysroot. If no sysroot is given, a default
 * of /usr/local/ is used.
 */
pub fn compile_string(input: String, output: Path, sysroot: Option<Path>) {
    compile_simple(StrInput(input), output, sysroot)
}

pub fn compile_file(input: Path, output: Path, sysroot: Option<Path>) {
    compile_simple(FileInput(input), output, sysroot)
}

fn compile_simple(input: Input, output: Path, sysroot: Option<Path>) {
    let sess = basic_sess(match sysroot {
        Some(path) => path,
        None => Path::new("/usr/local/")
    });
    let cfg = build_configuration(&sess);

    compile_input(sess,
            cfg,
            &input,
            &None, // output directory (unused when there's an output file)
            &Some(output), // output file
            None);
}```

Now you're all set to be compiling rust from rust!

### Traits

For bonus points, let's make this a little more rusty be taking advantage of `Trait`s. This will allow us to do away with the type-specialized functions `compile_string` and `compile_file`.

```rust
trait Compileable {
    fn to_input(self) -> Input;
}

impl Compileable for String {
    fn to_input(self) -> Input {
        StrInput(self)
    }
}

impl Compileable for Path {
    fn to_input(self) -> Input {
        FileInput(self)
    }
}

fn compile<T: Compileable>(input: T, output: Path, sysroot: Option<Path>) {
    compile_simple(&input.to_input(), output, sysroot)
}
```

This way we can call `compile(mystring, ...)` or `compile(mypath, ...)`, and the compiler will translate the calls to `compile::<String>(mystring, ...)` and `compile::<Path>(mypath, ...)`. Awesome!

## The Gotcha

As it happens, there's a bug in `librustc` that prevents your from compiling multiple files in a row -- there's some global state being stored somewhere, and I don't as yet know how to reset it ([github issue](https://github.com/rust-lang/rust/issues/19371)). So, for the moment, I've resorted to just creating a subprocess calling the binary `rustc`, which is far from ideal, but works.

Here's the code from this post all together in a single file: [compile_inline.rs](https://github.com/jaredly/itreed/blob/5d0b0fdc544a18aab0fdcf6990beea9c15dce8d4/lib/kernels/rust/compile_inline.rs).