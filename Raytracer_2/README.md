# Raytracer 2

> By Jeroen Overschie and Indy Roode

All exercises are implemented. Ray tracer can still read all old files because we incrementally added the features by reading additional data in the json files. We found a formula for doing rotation on [https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula](https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula).

### Optical Laws

Using shadow.

`scene01-shadows.png`

![pic](./Scenes/scene01-shadows.png)

`scene01-lights-shadows.png`

![pic](./Scenes/scene01-lights-shadows.png)

`scene01-reflect-lights-shadows.png`

![pic](./Scenes/scene01-reflect-lights-shadows.png)

### Anti-Aliasing

Super sampling. 

`scene01-ss.png`

![pic](./Scenes/scene01-ss.png)

### Textures

Using the provided `earthmap1k.png` image. Is also rotated:

```
    "rotation": [0,1,0.7]
```

`scene01-texture-ss-reflect-lights-shadows.png`

![pic](./Scenes/scene01-texture-ss-reflect-lights-shadows.png)

Cheers.