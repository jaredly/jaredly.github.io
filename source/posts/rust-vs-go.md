title: Rust vs Go
tags:
  - rust
  - golang
categories:
  - Programming
date: 2014-03-22 15:57:00
---
Go and Rust seem like natural competitors. They are both trying the role of a C-like low-level language with modern affordances, safety, and nice, clean concurrency. And they're each backed by a major player in the browser race - go by Google, rust by Mozilla.

<!-- more -->

I've made a few things in [go](http://golang.org), and I made [my first (non-trivial) program](https://github.com/jaredly/rusty-automata) in [rust](http://rust-lang.org) this past week, which was a port of [a program I wrote in go](https://github.com/jaredly/rocks) a few months before. There are many differences between rust and go that I haven't included here because they haven't effected me yet.

## Where Go Wins

### Stability
Go 1.0 was released 2 years ago, along with some guarantees about backwards compatability going forward. Because of this stability, a community has been able to grow around it, with lots of packages, examples, and tutorials available.

Rust just released 0.9 in Janurary, which had a number of breaking syntax, type system, and std library changes from 0.8. The roadmap to 1.0 looks like there will be several more breaking changes of various natures as the team nails down how they want the library to behave. As a result of this volatility, many community modules and tutorials are hopelessly out of date, making it more difficult for a community to form.

### Community
Go has a very active community, a growing number of community modules, and lots of tutorials and examples out there to get you started. There are also some high-profile projects using Go (like Docker), and a number of startups and consulting companies that are all about Go. The go runtime comes with a package manager built in (`go get`) that makes dependency management simple.

Rust's community, on the other hand, suffers from the volatility described above. Additionally, the Rust package manager was just deprecated, so there is currently no supported solution for package management -- you have to clone and install dependencies by hand or with custom makefiles. Fortunately, this will change in the near future; Yehuda Katz and Carl Lerche, who built Ruby's  "Bundler", [have been recruited](https://mail.mozilla.org/pipermail/rust-dev/2014-March/009090.html) to make an awesome package manager "Cargo" for Rust.

### Standard Library

Go has a [really solid standard library](http://static.rust-lang.org/doc/master/std/index.html), making everything from image en/decoding to building a web server to cryptography simple and straightforward.

Rust's [standard library](http://static.rust-lang.org/doc/master/std/index.html) leaves a lot to be desired. Again, this can be blamed on the general immaturity of the language (it would be a pain to rewrite a huge std library every time the syntax changes).

## Where Rust Wins

### The Type System
Go did some neat things with their type system, like the duck-typing traits and lots of great inference, but nothing that really broke the mold -- it felt like C, minus much of the headache.

One thing that Go doesn't have is generics. This is an oft-requested feature in the mailing list, but the response from the core team is "we don't think it's that important." Rust has generics.

Rust's type system is really breaking the mold in ways that I find really exciting.

- mutability, not just of data variables but also of pointers, must be declared explicitly
- there are 3 types of pointers:
  - Managed pointers are reference counted by a thread-local GC.
  - Owned pointers are allocated and freed *automatically by the compiler*.
  - Borrowed pointers "borrow" the reference from an owned pointer, with the type-system-enforced promise that they won't to anything to let the reference escape.

I won't try to fully explain the type system here; because it's so much more powerful than many of us procedural folks are used to, it can take some time to wrap your head around it. **Don't be discouraged**, though. It's really awesome once you get to know it.
In sum, the type system allows the compiler to make some really neat guarentees about safety and parallelizability. And did I mention the **automatic malloc & free**? No segfaults. It's like GC (easy) but without the expense (pausing, reference counting, etc.)!

### Memory Management
In Go, anything that's not on the stack is garbage collected. If you want more control, you have to resort to unsafe code.

Rust has GC if you want it, but the type system's automatic manual memory management is so poweful that reference counted pointers are rarely needed.

### Functional Affordances
There are a number of things in Rust that are inspired by functional programming languages.

##### Everything is an expression
You can, for example, assign the result of an if/else block to a variable.

```
let x = if something {
  3
} else {
  calculateX()
};
```

##### Variable destructuring
This example might not be terribly comprehensible if you've never seen rust before. Trust me it's cool :)
```
struct Point(int, int); // this is a named tuple type
let p1 = Point(23, 24);
let Point(x, y) = p1;
if x > 20 && y < 10 {
  // etc.
}
```

##### Match expressions
These are amazingly useful. It's like a switch on steroids.

```
enum Shape {
  Circle(Point, f64),
  Rectangle(Point, Point)
}
fn area(sh: Shape) -> f64 {
  match sh {
    // special case: if the radius
    // is less than 10, return the product of x and y
    Circle(Point{x, y}, r) if r < 10 => x * y,
    Circle(_, radius) => PI * radius * radius,
    Rectangle(Point{x, y}, Point{x: x2, y: y2}) => {
      let a = (x2 - x) * (y2 - y);
      if a < 0 { -a } else { a }
  }
}
```
You can also `match` on lists.

## Conclusion
**I'm betting on Rust.** Not that this has to be a competition; both languages might well grow to maturity and achieve popularity. However, it does seem that they are both trying to fill a similar role; a C-like low-level language with modern affordances, safety, and nice, clean concurrency.

From my experience, the places that Go wins are less to do with intrinsic language features and more due to its first-mover advantage. Of course, a language that no one speaks is dead, so I'm not going to say community doesn't matter. I just think that once Rust settles down and matures a bit its superior design will shine through and it will become really popular.
