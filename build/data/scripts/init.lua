function vec2str(vec)
    return "("..vec.x.."; "..vec.y.."; "..vec.z..")"
end

-- materials
MaterialManager.loadImage(":/textures/uv.png", "Material")
-- models
ModelManager.loadModel(":/models/cube.glb", "cube")

dofile('data/scripts/fps_demo.lua')
-- dofile('data/scripts/colliseum.lua')

