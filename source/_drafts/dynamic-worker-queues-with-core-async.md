title: Dynamic Worker Queues with Core.Async
date: 2015-12-13 21:46:14
tags:
---
I've got some work to do -- in this case, it's crawling a genealogical tree - starting from a root person, going to their parents, grandparents, etc. A twist -- I also want to collect *siblings* of their direct ancestors.

In javascript, I implemented this using a queue, and the queue knew how to dispatch a worker, and when it was exhausted, I knew I was done. This is probably still the most reasonable way to architect things... ?

Anyway, for some reason I wanted to try this out using core.async. b/c people keep saying it's the bomb. But sometimes what you want is a higher-level structure.

```
(defn queue [initial workers process on-empty]
  (let [arr (clj->js (vec initial))
        working (atom 0)
        on-done nil ; waiter
        on-add
        (fn [item]
          (if (< @working workers)
            (do
              (process item on-add on-done)
              (swap! working inc))
            (.push arr item))
        on-done
        (fn []
          (if (= (.-length arr) 0)
            (swap! working dec)
            (process (.shift arr) on-add on-done)
        ]
    (on-add initial)))
```