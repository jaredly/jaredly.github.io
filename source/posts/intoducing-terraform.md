title: Terraform: generate 3-d models of geographic terrain
featured: true
article_image: /images/terraform/splash.png
tags:
  - rust
categories:
  - projects
date: 2019-05-16 10:00:00
---

I just spent the past two weeks building a GUI Rust app to help you generate a 3-d model of terrain from around the world, that can then be used for 3-d printing, rendering, or whatever you like.

<!-- more -->

## Where did this come from?

I saw a project by an old acquiantance on facebook where he was [3d-printing mountain ranges](https://www.facebook.com/Cartographed/), and it looked really cool, and I thought "I wonder if I could do that?"

I had a couple of false starts & abandoned it a few times, but a couple weeks ago I got the bug again, and set about seeing what could be done.

## Finding a Data Source

There are a number of free, public data sources out there with relatively high-resolution elevation data for various parts of the world. The one that's easiest to use, and conveniently doesn't require you to register, is the USGS National Map (this only works if the area you're interested in is within the US, however). It provides "1/3 arc-second resolution" -- e.g. an elevation reading for every 10 meters. [Here's a link](https://viewer.nationalmap.gov/basic/?basemap=b1&category=ned&q=&zoom=11&bbox=-111.99426681,40.14528109,-111.23003036,40.59204857&preview=&avail=&refpoly=) to the map viewer zoomed to Mount Timpanogos in Utah.

<iframe width="560" height="315" src="https://www.youtube.com/embed/cXmrv8sWsxg" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

The other data source I looked at was the "ALOS Global Digital Surface Model", also referred to as the "AW3D30", which is provided by the Japanese government, and has "1 arc-second resolution" data (every 30 meters), for the whole world. You have to register with them to access it, but it's free & fairly painless to make an account. You register [at this page](https://www.eorc.jaxa.jp/ALOS/en/aw3d30/registration.htm), and then they email you a password to use [on the map viewer page](https://www.eorc.jaxa.jp/ALOS/en/aw3d30/data/index.htm).

## Initial attempt

My first attempt at parsing & processing the data was in Python. It's a language I'm very familiar with, and it's broadly used by the scientific community, which generally means that it will have the best libraries available for any given scientific task. I used the [python-gdal](https://pypi.org/project/GDAL/) library, which made short work of loading the data from either data source.

I then used [numpy-stl](https://pypi.org/project/numpy-stl/) to create and export a 3d mesh of the given terrain, and imported it into Blender for viewing & further processing. It required a little more back & forth as I inspected the rendered mesh in blender, and then determined the coordinates that I should "zoom in on" in my python script -- this quickly became a tedious process.

The final issue, however, that led me to take more drastic measures (sinking multiple weeks into making a bespoke rust program) came when I tried to "cut" the mesh into a hexagon in Blender. 

You see, the end goal of all this is to make a custom art installation for my wall with renderings of various places I've hiked -- Mt Timpanogos, the Grand Canyon, Mt Manadnock, and others. I want the terrain to be displayed on a hexagonal block, so that they can fit together on the wall.

Anyway, the only way I could determine to do such a "cut" in blender was to create a hexagonal prism, and do a "binary intersection" of it and the rendered terrain. This worked initially, when I was testing with lower resolutions, but when I switched to higher resolutions, Blender gave out entirely, consuming tens of gigabytes of RAM before I force-quit.

So I determined to build an app in rust that would bring the whole process together -- allowing you to visually select the portion of terrain to "cut" into a hexagon, and export as an STL file.

For posterity, [here's the Jupyter notebook I used](https://nbviewer.jupyter.org/github/jaredly/terraform/blob/fc5e427788da498b32c7ca648f546afdeffd15df/Python%20GIS.ipynb), feel free to use it as inspiration if you're looking to go the python route.

## The Rust version

I chose rust instead of reasonml (my current favorite language) because I knew there would be some intense 3d work needed (rendering 10 million triangles is no mean feat) and I wanted as little memory & processing overhead as could be reasonably achieved. I've been very impressed by the quality and availability of rust community packages in the past, and I was not disappointed: I found [kiss3d](https://github.com/sebcrozet/kiss3d) for 3-d rendering, [conrod](https://docs.rs/conrod/0.61.1/conrod/) for on-screen UI controls, [ncollide3d](https://ncollide.org/) and [nalgebra](https://nalgebra.org) for the math primitives, [rust-gdal](https://github.com/georust/gdal) for loading the elevation data, and [rust-stl](https://github.com/eholk/rust-stl) for saving it out to a mesh, allowing me to mostly focus on building my app. (I did end up forking kiss3d and rust-stl to add functionality I needed, but they were great foundations to work from).

If you're on macos, you can download [the .app bundle here](https://github.com/jaredly/terraform/releases/tag/1.0.0), and on Windows or Linux you'll need to build it yourself (which shouldn't be too painful I hope). You will need to install the `gdal` native dependency, whether you're using the prebuilt bundle or you're building it yourself (I tried to figure out static linking and it got complicated real fast).

## How to use it

Here's what it looks like to use this for yourself.

<iframe width="560" height="315" src="https://www.youtube.com/embed/7WnlRNaqinI" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Open an Elevation Data File

If you got the data from USGS National Map, you'll need to unzip the archive, and find the `.adf` file that's the largest (it will probably look something like `x001001.adf`).

If you got the data from ALOS AW3D30, you'll want to open the `.tif` file that ends in `_DSM.tif`.

![opening the data file](/images/terraform/step_1.png)

### Crop to the section you care about

Cropping to a smaller rectangle than the full initial file allows Terraform to render at a higher resolution & show you more detail for the final cut.

![cropping](/images/terraform/step_2.png)

### Cut to a hexagon

Currently terraform only supports hexagons for the final cut, but I could imagine supporting cylindars, rectangles and triangles in the future.

Before you make the cut, you might want to tweak the resolution to reduce the number of triangles & the output  file size, depending on where you want to use the final model, and how large & detailed it needs to be. The status text at the bottom indicates the triangle count and the file size for the current selection & resolution.

![cutting](/images/terraform/step_3.png)

![cut finished](/images/terraform/step_4.png)

### Export!

This will generate a `.stl` mesh file, which can be imported into Blender, AutoCAD, FreeCAD, etc. for 3-d printing or (maybe) cnc routing!

![exporting](/images/terraform/step_5.png)

## That's all, folks

Feel free to check out [the github project](https://github.com/jaredly/terraform) if you're interested, contributions are welcome. I'm planning to write a post soon that goes into my experience writing a moderate amount of rust, what I miss from reasonml, etc. so stay tuned 😀
