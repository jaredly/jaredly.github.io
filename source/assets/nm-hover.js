
window.addEventListener('load', () => {
  let items = [].slice.call(document.querySelectorAll('.code-chunk.collapsible'))
  items.forEach(item => {
    item.addEventListener('click', () => {
      item.classList.toggle('collapsed')
    })
  })
})
