
window.addEventListener('load', () => {
  let items = [].slice.call(document.querySelectorAll('.code-chunk.collapsible'))
  items.forEach(item => {
    item.addEventListener('click', () => {
      item.classList.toggle('collapsed')
    })
  })

  const node = document.createElement('div')
  document.body.appendChild(node)
  Object.assign(node.style, {
    position: 'fixed',
    backgroundColor: 'white',
    boxShadow: '0 1px 3px rgba(100, 100, 100, 0.1)',
    whiteSpace: 'pre',
    padding: '5px 10px',
    fontFamily: 'monospace',
    display: 'none',
    top: 0,
    left: 0,
  });

  const size = ({start, end}) => {
    return end.chars - start.chars
  }
  const before = ({line, col}, l2, c2) => line < l2 || (line === l2 && col <= c2)
  const after = ({line, col}, l2, c2) => line > l2 || (line === l2 && col >= c2)
  const findType = (types, lno, start, end) => {
    // console.log('looking', lno, start, end)
    // console.log(types.filter(t => t.start.line === lno + 1 && t.end.line === lno + 1).map(t => t.type))
    return types.filter(t => before(t.start, lno + 1, start ) && after(t.end, lno + 1, end ))
  }

  const xoff = 10
  const yoff = 20

  let showing = null



  const getMarker = (container, {line, col}) => {
    // console.log('looking for', line, col)
    return container.querySelector(`.marker[data-marker="${line - 1},${col}"]`)
  }

  let chunks = [].slice.call(document.querySelectorAll('.reason-source'))
  chunks.forEach(chunk => {
    const name = chunk.getAttribute('data-file-name')
    if (!name || name === 'null' || name === 'undefined') return
    if (!TYPES[name]) return console.warn('types not found for ' + name)

    const all_types = TYPES[name]
    all_types.sort((a, b) => {
      let d = size(a) - size(b)
      if (d == 0) return a.depth - b.depth
      return d
    })

    chunk.addEventListener('mousemove', evt => {
      node.style.left = evt.clientX + xoff + 'px'
      node.style.top = evt.clientY + yoff + 'px'
    })

    chunk.addEventListener('mouseout', evt => {
        node.style.display = 'none'
        if (showing) {
          showing.forEach(node => {
            node.style.display = 'none'
            node.classList.remove('left')
            node.classList.remove('right')
          })
          showing = null
        }
    })




    chunk.addEventListener('mouseover', evt => {
      const postxt = evt.target.getAttribute('data-pos')
      if (!postxt) return
      const [lno, start, end] = postxt.split(',').map(Number)
      const types = findType(all_types, lno, start, end)
      if (!types.length) {
        return
      }
      const t = types[0]

      node.style.display = 'flex'
      node.textContent = t.type
      node.style.left = evt.clientX + xoff + 'px'
      node.style.top = evt.clientY + yoff + 'px'

      showing = []
      const sstart = getMarker(chunk, t.start)
      const send = getMarker(chunk, t.end)
      if (sstart) {
        showing.push(sstart)
        sstart.classList.add('left')
      }
      if (send) {
        showing.push(send)
        send.classList.add('right')
      }
      showing.forEach(node => node.style.display = 'inline-block')
    })




  })
})
