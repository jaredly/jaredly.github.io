title: Teaching Javascript Fatigue
date: 2016-02-20 14:42:00
tags:
---
My little sister is an Art magor, and zin a Design class is learning some processing. She asks me for help every once in a while, and recently I was faced with an interesting situation.

She asked "how do I make ... whatever it was"

The immediately obvious solution to me was to use an array of colors, and math.random() to index into the array.
After trying to explain it to her, though, I realized that there were many new concepts involved, and it was way overkill for her current situation -- she just wanted to draw different colored circles. You know a much simpler way?

```
float x = random()
if (x < .3) color(100)
else if (x < .7) color(200)
else color(300)
```
b/c array indexing, all the types involved, etc.

This also comes up when teaching my little brother, who's currently in middle school, to program. When he comes with a question, the way that's most immediately obvious to me (and is of course Better™) tends to require much more learning before the solution begins to make sense, let along before he can replicate it himself & modify it to fit future requirements.

Yes, the solution I see is likely to scale better, be more general, perhaps side-step some language quirks, etc. But it's only Better™ from *my perspective*, with my experience etc. For him, it's actually Worse™.

I think this might be some of the cause of javascript fatigue.

Because what people want, is whatever they want at the moment. deep, I know.
They don't want to learn "this one cool trick that will make your life easier in the future once you invest time into learning it." Sometimes teaching the new thing doesn't get too much in the way of them completing their goal, but often times it's a distraction. Remember, **you** learned to program back before there were these awesome tools, you used the rookie methods until you learned better ones. And now that you're in the Big Legues™ where you have to make sure products you make are Web Scale™, it can be easy to forget that not everything needs to be Web Scale™, not everything needs to be maintainable by a team of a dozen over many months or years.

Sometimes the easy/naiive solution is the only solution that's needed, for now.

If webpack is the asprin, what's the headache? <--- talk to this more