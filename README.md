## VulkanTestbed
by Gabriel Nkole

## Description
Vulkan framework showcasing various graphical techniques.

## Libraries/Programs
- Vulkan 1.1 - Graphics API
- GLFW - Windowing & User Input
- DirectXMath - Linear Algebra
- PVR Tex Tools & Nvidia Texture Tools Exporter - Creating skyboxes/generating mipmaps/converting textures to ktx format
- Blender - 3D Models (exported as FBX)

## Screenshots/Video [captured on an Intel UHD 630]
![Screenshot_1](https://github.com/user-attachments/assets/d4020951-f881-4fef-b4a8-35da20c23671)
![Screenshot_2](https://github.com/user-attachments/assets/2893df5c-96eb-4324-94a4-d87befe8bcb3)
![Screenshot_3](https://github.com/user-attachments/assets/6cc1262a-2736-4ae3-b536-2efab6ddc323)
![Screenshot_4](https://github.com/user-attachments/assets/6f4559b4-ef32-4b0e-84e5-5f61924a7f02)

https://github.com/user-attachments/assets/41bfa4a0-30a4-4fd7-9e2b-c605e5491040

https://github.com/user-attachments/assets/7857529e-d5a2-47ef-9d3f-c004d25dec71

https://github.com/user-attachments/assets/8a80ab5f-2112-4089-9be7-918bf8930e0b

## Controls
- WASD - Forward, Left, Back, Right
- E - Ascend
- Q - Descend
- Mouse - Rotate Camera
- Escape - Exit Program

## Features
- 3D Rendering
- Texturing (Khronos Ktx library)
- Skybox
- Floor Grid with 1m x 1m squares
- Camera Controls
- Fullscreen
- Multi-Sampling
- Sample Shading
- Loading 3D Models (Assimp library)
- Lighting (Directional, Spot, Point)
- Shadows (Directional, Spot, Point)
- Rendering light objects (Cylinder - Directional, Cone - Spot, Sphere - Point)  

## Todo
- Implement GUI using the Imgui library (be able to alter light properties[position, direction, color, intensity, etc.] and turn shadows on/off in real-time)
- Framerate/Frametime Counters
- Improve Shadow Quality (Percentage Closer Filtering [PCF])
- Normal Mapping
- Post Processing
- Soft Shadows

## Assets
- Albedomaps - Futuregames, Freya Holmér - https://acegikmo.com/storage/futuregames/ShaderCourseAssets.unitypackage
- Skyboxes - rpgwhitelock (Standard Unity Asset Store EULA) - https://assetstore.unity.com/packages/2d/textures-materials/sky/allsky-free-10-sky-skybox-set-146014
- Viking Room - nigelgoh (CC BY 4.0) - https://sketchfab.com/nigelgoh

## Notes
- Because the cursor needs to be disabled instead of hidden for camera rotation to work properly, in the event of a runtime error, it may be necessary to sign out with Ctrl+Alt+Delete since the cursor will not be visible. Must fix.
- assimp-vc143-mt.dll needs to be in same directory as exe or program will not work.
