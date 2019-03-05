# ⚛️ Computer Graphics 🖥
> By Jeroen Overschie and Indy Roode

## 1. Shaders

### Normal shading

Normal shading using no matrix transformation. Visible is the fact that the colors don't adjust according to the newly rotated/transformed angles.

![Normal Shading](./Screenshots/normal_shading_no-matrix.gif)

Normal shading with matrix:

![Normal Shading](./Screenshots/normal_shading.gif)

A clear difference in color is visible between the mapped color values in the fragment shader and the non-mapped values. Not mapped:

![Not mapped values](./Screenshots/normal_shading_range-not-mapped.png)


Color values mapped. No data is lost, colors are brighter.

![Mapped values](./Screenshots/normal_shading_range-mapped.png)

### Selecting shaders

Darkening color for Gouraud and Phong shader programs:

![switching-shader-programs](./Screenshots/switching-shader-programs.gif)

### Gouraud shader

Gouraud shading. Calculation in vertex shader.

![switching-shader-programs](./Screenshots/gouraud-shading.gif)

### Phong shader

For the Phong shader, we are using exactly the same calculation as in Gouraud's shader, but then in the Fragment shader instead of the vertex shader. We pass on required variables on from the vertex to the Fragment shader, and other via an uniform variable.

![switching-shader-programs](./Screenshots/phong-shading.gif)


## 2. Texture mapping

Set the parameters to:

```
GL_TEXTURE_WRAP_S = GL_REPEAT
GL_TEXTURE_WRAP_T = GL_REPEAT
GL_TEXTURE_MIN_FILTER = GL_NEAREST
GL_TEXTURE_MAG_FILTER = GL_NEAREST
```

### Texturized Phong:

![texturized_phong](./Screenshots/texturized_phong.gif)

Using:

```
GL_TEXTURE_MIN_FILTER = GL_NEAREST
GL_TEXTURE_MAG_FILTER = GL_NEAREST
```

![texturized_gouraud](./Screenshots/texturized_phong-gl-nearest.gif)

Using:

```
GL_TEXTURE_MIN_FILTER = GL_LINEAR
GL_TEXTURE_MAG_FILTER = GL_LINEAR
```

![texturized_gouraud](./Screenshots/texturized_phong-gl-linear.gif)

### Texturized Gouraud:

![texturized_gouraud](./Screenshots/texturized_gouraud.gif)

