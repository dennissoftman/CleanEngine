ModelManager.loadModel("data/models/cube.obj", "cube")
ModelManager.loadModel("data/models/sphere.obj", "sphere")
ModelManager.setMaterial("sphere", "uv")
ModelManager.loadModel("data/models/suzanne.obj", "monkey")
ModelManager.setMaterial("monkey", "uv")

MaterialManager.loadImage("data/textures/slab.png", "slab")
ModelManager.setMaterial("cube", "slab")
