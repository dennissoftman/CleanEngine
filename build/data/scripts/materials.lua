-- Diffuse textures
Material.createDiffuse(":/textures/uv.png", "Material")
-- PBR materials
local pbr_materials = {
    "aluminum-scuffed",
    "black-white-tile",
    "cheap-plywood1",
    "chipped-paint-metal3",
    "concrete1", "concrete2", "concrete3",
    "cracking-painted-asphalt1",
    "dashboard1",
    "dash-lined-metal1",
    "metal-ventilation1",
    "modern-brick1",
    "old-metal-slats1",
    "painted-metal-shed",
    "patchy-cement1",
    "pitted-metal",
    "pitted-rusted-metal1",
    "ribbed-chipped-metal1", "ribbed-chipped-metal2", "ribbed-chipped-metal3",
    "scuffed-plastic4", "scuffed-plastic5",
    "storage-container2",
    "woodframe1"
}

for k, v in pairs(pbr_materials) do
    Material.createPBR(":/textures/"..(v)..".pbr", v)
end

-- Material.createPBR(":/textures/dashboard1.pbr", "dashboard1")

-- Diffuse colors
Material.createDiffuse(vec3(1, 0, 0), "red")
Material.createDiffuse(vec3(0, 1, 0), "green")
Material.createDiffuse(vec3(0, 0, 1), "blue")
Material.createDiffuse(vec3(1), "white")
Material.createDiffuse(vec3(0), "black")

Material.createDiffuse(vec3(0.01, 0.01, 1.0), "_playerColor")


