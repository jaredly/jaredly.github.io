title: Detecting unused styles in JavaScript with `babel-traverse`
featured: false
author: Jared Forsyth
date: 2017-04-08 15:47:36
tags:
  - javascript
  - babel
  - tooling
categories:
  - javascript
description: It's unexpectedly easy to do powerful analysis using the tools babel provides.
---

Last week, my coworker [Charlie](https://twitter.com/crm416) asked what it would take to automatically detect and purge unused [aphrodite](https://github.com/Khan/aphrodite) styles in our codebase.

If asked 2 years ago, I probably would have gone with a regex and a string-munging python script, but I'd just spent the past few nights messing with babel plugins, and figured I could probably get pretty far with relatively little work. As it happened, I was **impressed by how easy it was** using the tools that babel provides.

As a bonus, it also works with [React Native](https://github.com/facebook/react-native) because they have the same API, and it could probably be extended to other libraries without too much work.

<!-- more -->

## Here's what we're building

We're making the guts of the [stylecleanup](https://github.com/jaredly/stylecleanup) tool, which finds & deletes unused styles in your JavaScript, and works with both [aphrodite](https://github.com/Khan/aphrodite) and [React Native](https://github.com/facebook/react-native).

[![screencap of stylecleanup in action](https://github.com/jaredly/stylecleanup/blob/master/docs/screencap.gif?raw=true)](https://github.com/jaredly/stylecleanup)

For example, here's a React Native file

```js
import {StyleSheet, View} from 'react-native'
import {Component} from 'react'

export default class MyComponent extends Component {
  render() {
  	return <View style={styles.header} />
  }
}

const styles = StyleSheet.create({
  header: { backgroundColor: 'red' },
  awesome: { fontSize: 20 },
})
```

The style `awesome` is unused, and we'd like to automatically detect that.

## Here's how we do it

The rough steps are

- parse the JavaScript file
- find all the stylesheet declarations, e.g. `const myStyleSheet = StyleSheet.create({ .... })`
- get a list of the styles within that stylesheet (e.g. `"header"`, `"awesome"`)
- go through and find references to that stylesheet, e.g. `myStyleSheet.someStyle`
- from that, determine which styles are never referenced and can be safely deleted

### Parse the JavaScript file

We'll be using the libraries [babylon]() and [babel-traverse](). The [plugins section](https://github.com/thejameskyle/babel-handbook/blob/master/translations/en/plugin-handbook.md) of [@thejameskyle](https://twitter.com/thejameskyle)'s [babel-handbook](https://github.com/thejameskyle/babel-handbook) is also an excellent reference.

```js
const babylon = require('babylon')
const text = fs.readFileSync(file, 'utf8')
const ast = babylon.parse(text, {
  // this means that `import` and `export` are allowed
  sourceType: 'module',
  // we want to allow all the fancy syntax
  plugins: ['jsx', 'flow', 'objectRestSpread', 'classProperties'],
})
```

### Find the StyleSheet declarations

We'll look for the form `const myStyleSheet = StyleSheet.create({ .... })`

We're going to use [`babel-traverse`](https://github.com/babel/babel/tree/master/packages/babel-traverse), which makes walking through the tree super easy.

It gives us a function, `traverse`, that will walk through the whole AST, and call the visitors we specify corresponding to the type of a given node. To figure out what the types are (and what the shape of the AST is like), I lean on [astexplorer.net](https://astexplorer.net/) and [the Readme](https://github.com/babel/babel/blob/master/packages/babel-types/README.md) of [babel-types](https://github.com/babel/babel/tree/master/packages/babel-types).

```js
const styleSheets = []
traverse(ast, {
  CallExpression(path) {
    if (isStyleSheetCreate(path)) {
      const members = path.node.arguments[0].properties.filter(
        // Not gonna try to figure out spreads
        property => property.type === 'ObjectProperty'
      )
      const styleNames = members.map(member => member.key.name)
      styleSheets.push({id: path.parent.id, styleNames})
    }
  },
})
```

So I go through every [`CallExpression`](https://github.com/babel/babel/blob/master/packages/babel-types/README.md#callexpression), which has the form `something(arg1, arg2, ...)`, check if it looks like `StyleSheet.create`, and then process it. The function `isStyleSheetCreate` (that I'll describe in a second) determines whether the current node looks like `var myStyleSheet = StyleSheet.create({y: ... })`. I then grab all the members of the object that's being passed to `StyleSheet.create`, discarding any that happen to be [`ObjectMethod`](https://github.com/babel/babel/blob/master/packages/babel-types/README.md#objectmethod)s or [`computed`](https://github.com/babel/babel/blob/master/packages/babel-types/README.md#objectproperty) properties, and get the names that they're being identified by. In our example file, `styleNames` would end up being `["header", "awesome"]`.

`path.parent.id` refers to the variable name that this stylesheet is being bound to -- `myStyleSheet` in the `var myStyleSheet = StyleSheet.create({y: ...})` example.

So at the end of this I have a list of the stylesheets that got created and assigned to a variable, and the style names within each stylesheet.

#### Let's look at `isStyleSheetCreate` now.

```js
const isStyleSheetCreate = ({node, parent}) => {
  return node.callee.type === 'MemberExpression' &&
    node.callee.object.type === 'Identifier' &&
    node.callee.object.name === 'StyleSheet' &&
    node.callee.property.name === 'create' &&
    parent.type === 'VariableDeclarator' &&
    node.arguments.length === 1 &&
    node.arguments[0].type === 'ObjectExpression'
}
```

The argument I'm passing in is a [`NodePath`](https://github.com/babel/babel/blob/master/packages/babel-traverse/src/path/index.js#L14) that has many attributes, but the ones we care about are `node` and `parent`, which are both nodes in our AST (again, refer to the [babel-types Readme](https://github.com/babel/babel/blob/master/packages/babel-types/README.md) for more info).



These checks establish that

- The function being called is `StyleSheet.create`
- It's only being called with one argument, which is an object literal
- It's being assigned to a variable (indicated by the parent being a [`VariableDeclarator`](https://github.com/babel/babel/blob/master/packages/babel-types/README.md#variabledeclarator)

### Find all the references to styles

as in `myStyleSheet.someStyle`

`babel-traverse` actually makes this super easy for us, because it **already tracks all variable references**. We can just hang on to the [`Binding`](https://github.com/babel/babel/blob/master/packages/babel-traverse/src/scope/binding.js#L14) object that it uses to collect references, and then iterate through them after traversal.

```js
const styleSheets = []
traverse(ast, {
  CallExpression(path) {
    if (isStyleSheetCreate(path)) {
      const members = path.node.arguments[0].properties.filter(
        property => property.type === 'ObjectProperty' // Not gonna try to figure out spreads
      )
      const styleNames = members.map(member => member.key.name)
      styleSheets.push({
        id: path.parent.id,
        styleNames,
        // hang on to the binding object for "myStyleSheet"
        binding: path.scope.getBinding(path.parent.id.name)
      })
    }
  },
})
```

Now we iterate through our stylesheets, and take a look at the references.

```js
styleSheets.forEach(({id, styleNames, binding}) => {
  const referencedNames = binding.referencePaths
    .filter(ref => ref.parent.type === 'MemberExpression' && !ref.parent.computed)
    .map(ref => ref.parent.property.name)
})
```

In the example reference `<View style={styles.header} />`, the `referencePath` just points to the `styles` identifier, which is the reference to our stylesheet binding. We want to get the string `"header"`, so we

- determine that we're looking at a [`MemberExpression`](https://github.com/babel/babel/blob/master/packages/babel-types/readme.md#memberexpression) (e.g. `styles.something`)
- make sure it's not computed (we can't do much with `styles[someVariable]`)
- get the property name (e.g. `"header"`)

### Then use the list of references to determine which styles aren't being used at all.

```js
styleSheets.forEach(({id, styleNames, binding}) => {
  const referencedNames = binding.referencePaths
    .filter(ref => ref.parent.type === 'MemberExpression' && !ref.parent.computed)
    .map(ref => ref.parent.property.name)
  const unused = styleNames.filter(name => referencedNames.indexOf(name) === -1)
  // 🎉
})
```

Just for kicks, we can also get a list of styles that are referenced, but never defined!

```js
styleSheets.forEach(({id, styleNames, binding}) => {
  const referencedNames = binding.referencePaths
    .filter(ref => ref.parent.type === 'MemberExpression' && !ref.parent.computed)
    .map(ref => ref.parent.property.name)
  const unused = styleNames.filter(name => referencedNames.indexOf(name) === -1)
  // too easy
  const missing = referencedNames.filter(name => styleNames.indexOf(name) === -1)
})
```

## And we're done!

To see the code in context, look at [`analyzeFile.js`](https://github.com/jaredly/stylecleanup/blob/master/analyzeFile.js) in the [`stylecleanup`](https://github.com/jaredly/stylecleanup) project.

For a real tool, there's some more bookkeeping involved in

- showing the lines of code where e.g. a missing style is referenced
- being conservative about what styles are definitely unused vs styles that *might* be unused -- if the stylesheet variable is used in a way that we ignore (e.g. not in a [`MemberExpression`](https://github.com/babel/babel/blob/master/packages/babel-types/README.md#memberexpression), or with a computed lookup), then there could be references to styles that we can't track.
