-- Diffuse textures
Material.new(":/textures/uv.png", "Material")
-- PBR materials
-- black tile floor
Material.new(":/textures/black-white-tile/albedo.png",
             ":/textures/black-white-tile/normal.png",
             ":/textures/black-white-tile/roughness.png",
             ":/textures/black-white-tile/metallic.png",
             ":/textures/black-white-tile/ao.png",
             "black-white-tile")
Material.new(":/textures/dragon-scales/albedo.png",
             ":/textures/dragon-scales/normal.png",
             ":/textures/dragon-scales/roughness.png",
             ":/textures/dragon-scales/metallic.png",
             ":/textures/dragon-scales/ao.png",
             "dragon-scales")
-- Diffuse colors
Material.new(vec3(1, 0, 0), "red")
