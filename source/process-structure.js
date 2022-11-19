
const full = require(__dirname + '/posts/structure-editor-literature-review.nm.json');

const nodesById = {}
const nodeSize = {}

const walk = (node, fn) => {
  node.children.forEach(child => walk(child, fn))
  fn(node)
}

walk(full.root, node => {
  nodeSize[node.id] = node.children.reduce((sum, child) => sum + nodeSize[child.id], 1)
  nodesById[node.id] = node
})

// const types = {}
// walk(full.root, node => types[node.type] = true)
// console.log(Object.keys(types).join(','))
// types: undefined,code-playground,base,normal,symlink,image,table,todo,ordered_list,list

const renderContent = node => {
  switch (node.type) {
    case 'image':
      return `<div><img src="${node.imageSrc}" alt="${node.content}"/><p>${node.content}</p></div>`
    case undefined:
    case 'base':
    case 'normal':
    case 'todo':
      // TODO markdown render
      return `<p>${node.content}</p>`
    case 'symlink':
      return `<a href="#${node.content}">${renderContent(nodesById[node.content])}</a>`
    default:
      return `${node.type}<code><pre>${JSON.stringify({...node, children: undefined})}</pre></code>`
  }
}

const renderNode = node => {
  return `<div id="${node.id}" class="node">
    <div class="contents">
    ${nodeSize[node.id] > 1 ? nodeSize[node.id] : ''} ${renderContent(node)}
    </div>
    ${node.children.length 
    ? `<div class="children">
    ${node.children.map(renderNode).join('\n')}
    </div>` : ''}
  </div>`
};


const replaceSymlinks = node => {
  if (node.type === 'symlink') {
    node = nodesById[node.content]
  }
  return {...node, children: node.children.map(replaceSymlinks)}
}

const preamble = `
title: Structure Editors: A Literature Review
author: Jared Forsyth
featured: true
tags:
categories:
  - structured editors
description: All the editors
date: 2019-2-23
---

I've been meaning to publish this for a long time.

<!-- more -->

<style>
.children {
  border-left: 1px solid #aaa;
  padding-left: 24px;
  display: none;
}
.node.show > .contents {
  background-color: #eee;
}
.node.show > .children {
  display: block
}
.contents:hover {
  cursor: pointer;
  outline: 2px solid #faf;
}

</style>
<script>
document.body.onload = () => {

for (const node of document.querySelectorAll('.node')) {
  node.querySelector('.contents').onclick = (evt) => {
    node.classList.toggle('show')
    evt.stopPropagation()
  }
}
}
</script>

`


const fs = require('fs')
// const fixed = {...full, root: replaceSymlinks(full.root)}
fs.writeFileSync(__dirname + '/posts/structured-editors-a-literature-review.html', preamble + renderNode(full.root))
