
Undo til you can't undo more

```
:for i in range(1, 100)
:  redo
:  exe "w change-" . i . ".txt"
:endfor
```

