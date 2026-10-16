## Introduction

![my artwork](https://i.pinimg.com/736x/fa/6a/eb/fa6aeb92c7dc2abe683b90f82cad8324.jpg)

While preparing for a game jam, I opened [Blender](https://www.blender.org/) for the second time in my life. Since then, 3D has become one of my favourite hobbies. This work perfectly scratches my itch for doing something creative, that is difficult, looks pretty, is related to photography and involves heavy use of computers.  
Even though my passion lies in prorgamming, I have always had massive respect and love for making art.

However, spending all this time as an artist, I saw that there is usually a massively overlooked understanding gap between how these softwares operate internally and how the artists are using them. Having a deeper knowledge about the them can be very beneficial and helps reduce unnecessary iterations that could have been avoided from the beginning by taking a little peek under the hood.  
In my profession as a rendering programmer, the aim has always been to give maximum control to the designers and artists of our project and we design the tech with their needs and preferences in mind. But that is not possible in a public software like blender where millions of users have their own style of using. Even when the development team actively listens to their community's feedback and we all love the direction in which our favorite software is going, it will never be possible for them to design the tech that is centered around you and only you. This is where us, as users, need to learn and tweak our working preferences slightly to save time, save resources, avoid unnecessary headache and ultimately make the art that we love.  
For every feature we are provided with hundereds of thousands of properties to tweak and almost always, we all just follow a tutorial online and then do a *trial and error* until we achieve our desired look. Instead the correct approach should be to really understand the working of each property. Doing so, saves a lot of iteration time and by giving the optimal values to the software we can save a ton of computations.

Rendering is one such area where I personally have been very curious about each of the properties and settings. Mostly because this has been my domain for years but also because I have a very shitty PC and my scenes take forever to render. The tutorials that I found online always blasted me with *"set property_x to this and set property_y to that"* but following them was never a guaranteed save in my render times. We need to understand that all scenes are different and no one can ever give a generic solution that works for all. So I always wanted to dig into this so that I can get my renders quickly without compromising the quality or the creative vision of the artwork.

## Blender Cycles Renderer
> [Cycles](https://www.cycles-renderer.org/) is a physically based production renderer developed by the [Blender project](https://www.blender.org/). It is a path tracing renderer focused on interactivity and ease of use, while supporting many [production features](https://www.cycles-renderer.org/features/).

Cycles has been my primary choice when rendering my scenesd. Since it is a *path tracing renderer* it allows very complex light simulations and can deliver impressive results. But what is path tracing, why does it allow complex light simulations and why is it slower compared to other renderers such as [EEVEE](https://docs.blender.org/manual/en/latest/render/eevee/index.html), what can we do to get our results in reasonable time, what does each of the setting do, what does the code look like, how to make it render fast, and how to tweak the values optimally. These are all the questions that we will attempt to answer in this book.  
While we do take cycles as our primary example, if you do not use cycles, the core concepts discussed here remain the same across all the path tracing renderers and this book will work as a base and will provide a direction for you to further explore your renderer of choice.

Each chapter focuses on all the settings that are understood by a common concept. And by the end, you can expect to have a good understanding of path tracing and how to get quality results in minimum possible time. This book is for both artists who renders their own scenes and programmers who are interested in ray tracing and GPU programming. From sharing my artwork to writing custom C++ experiments, we are going to dive as much as we need.

As artist, you will get to understand the internals of a renderer. This book does include some C++ code, it is not at all expected from you to understand any of it. I provide visual diagram of the flow of the code and its there (and is needed) to completely understand a renderer. I will also mention the parts that you can skip over if your focus is to only achieve faster render times.

As rendering prorgammer, you will get a detailed look at one of the most popular rendering engines, Cycles. It will be very fruitful if you clone the [cycles repository](https://projects.blender.org/blender/cycles) and take a look at code yourself as the book advances. I would also recommend that you take a look at [Ray Tracing in One Weekend - The Book Series](https://raytracing.github.io/) before you start reading this book.

A personal aim for writing this book is that I would love to share my artwork with you all! All the artwork and examples showcased in this book are made by yours truly :)  
If you wish to see more, please drop a follow on [instagram @0xArt3D](https://www.instagram.com/0xart3d/) and on [artstation @JayNakum](https://www.artstation.com/jaynakum).

## For Programmers
As I mentioned earlier, I have also written custom C++ experiments throughout the book, I think it would be best to start by explaining the framework that I have designed to allow for such experiments. Artists can completely skip over this section.  
I am calling this render engine Hex. It is written in C++ and uses [Vulkan](https://vulkan.org/) as the primary graphics API. For vulkan I am using [Vulkan Profiles](https://docs.vulkan.org/guide/latest/vulkan_profiles.html) to easily manage the features and extensions. And I am using [Volk](https://github.com/zeux/volk) for dynamically load entrypoints without linking vulkan loaders.  
The rendering architecture is similar to my [d3d renderer Harmony](https://jaynakum.github.io/blog/5/GerstnerWaves#a-bit-about-harmony) with [memory management](https://jaynakum.github.io/blog/7/GPUMemoryManagement) and [shader input management](https://jaynakum.github.io/blog/7/GPUMemoryManagement) systems. However, since Vulkan is much more explicit by design, additional controls are provided to utilize it fully. This engine is designed with vulkan in mind only and hence does not include a separate API abstraction layer unlike Harmony. For example here, the command context directly makes vulkan calls. Following this simplified the code a little bit and allowed me to jump into ray tracing directly.  
For shaders, I am using [Slang](https://shader-slang.org/) and currently the engine is setup to have one compute pass for ray tracing and one present pass that displays our image on swapchain.

I don't feel the need to share the fully working projects because this book is not a C++ path tracer tutorial but the aim is to understand the core concepts of path tracing with a good example by our side so that we can explore other projects and when needed, write our own renderer without spoon feeding tutorials.

## Ray Tracing
[![Ray Tracing In One Weekend Image](https://raytracing.github.io/images/img-1.23-book1-final.jpg)](https://raytracing.github.io/books/RayTracingInOneWeekend.html)

> [Ray tracing](https://developer.nvidia.com/discover/ray-tracing) is a rendering technique that can realistically simulate the lighting of a scene and its objects by rendering physically accurate reflections, refractions, shadows, and indirect lighting.

In real world, the way a human eye sees, when the light rays from all the various sources bounces around the world objects and eventually reach our eye. Similarly, ray tracing is a physics accurate computer simulation of the same. In ray tracing, have *"n number of rays"* casted in *"some direction"* and we use them to calculate colors. 

Imagine a scenario where there is a point light placed in your blender scene. A [point light](https://docs.blender.org/manual/en/latest/render/lights/light_object.html#point-light) is a point radiating the same amount of light in all directions.  
Here you can image a strong point light placed outside the frame at top left such that it is casting these tree shadows.

![a tree scene with a point light](https://cdna.artstation.com/p/assets/images/images/100/658/932/large/jay-nakum-test3.webp?1783613626)

Now if we were to take our real world simulation where the rays originate from light sources and hit reach the eye (camera in blender), we would quickly realise that most of the lightrays emitted from the source will go in the opposite direction and are highly unlikely to bounce such that they reach the camera and contribute to the image color.  
Hence, in computer simulation, instead of casting rays from light sources to eye, we do the reverse, and we cast them from the camera. Doing so, we avoid a lot of unnecessary computation. But our results become ***highly dependent on the camera angle***.

**#TODO: MOVE THIS TO THE LIGHT BOUNCES CHAPTER**  
Which is the reason why sometimes we see rendering artifacts. This usually happen when the number of light bounces are configured to a lower number which makes it such that from a certain camera angle depending on your scene the ray might never reach the light source with fewer bounces. 

![pinhole camera](https://thumb.wikimedia.org/wikipedia/commons/thumb/3/3b/Pinhole-camera.svg/1920px-Pinhole-camera.svg.png)

The light ray fundamentals are better understood via a [pinhole camera](https://en.wikipedia.org/wiki/Pinhole_camera). In a pinhole camera, the light rays passes through a tiny hole and we get an upside down image at the back where we would have placed a photosensitive material. Similar fundamentals are applied in our computer simulation, however since its all just code, instead of waiting for the rays to pass through a tiny hole and get an upside down image, we just capture the ray color ahead of the point. It will give the same results and we won't have to flip the image right-side-up.

[![tracing rays from camera to light](https://d29g4g2dyqv443.cloudfront.net/sites/default/files/pictures/2018/RayTracing/ray-tracing-image-1.jpg)](https://developer.nvidia.com/discover/ray-tracing)

This process of casting rays is one of the most important fundamentals that we need to clear up before we take a look at all the cycles rendering parameters one by one. The details about the math and how we compute the image color are trivial at this point and we will explore them one by one throughout the book, the only process that you need to know is that the camera casts rays in the direction of pixels of our image and whatever color that the calculation results in, will be the image.
